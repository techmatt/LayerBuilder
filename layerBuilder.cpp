
#include "main.h"

SupervoxelLayerSet LayerBuilder::buildSupervoxelLayers(const AppParameters &parameters, const Video &v, const Vector<VideoCoordinate> &supervoxels, NearestNeighborSearchANN<float> &supervoxelSearch, Palette palette, Vector<SupervoxelLayerConstraint> explicitConstraints)
{
	//
	// Compute the energy terms
	//
	Console::log("Computing energy terms");
	ComputeNeighborWeights(parameters, v, supervoxels, supervoxelSearch);
	ComputeWeightMatrix(parameters);
	ComputeEnergyMatrix(parameters, supervoxels, palette, explicitConstraints);

	UINT negativeSuppressionIters = parameters.negativeSuppressionIters;
	UINT numSupervoxels = (UINT)supervoxels.size();
	UINT numLayers = (UINT)palette.size();
	
	//
	// Do the layer solve
	//
	Console::log("Solving for layers");
	Vector<double> negativeSuppressionWeights(numSupervoxels*numLayers, 0);

	Vector<double> layerWeights = extractSupervoxelLayers(parameters, negativeSuppressionWeights);

	//
	// Negative suppression
	//
	for (UINT i=0; i<negativeSuppressionIters; i++)
	{
		Console::log("Negative suppression iteration " + Convert::toString(i));
		for (UINT contributionIndex=0; contributionIndex < layerWeights.size(); contributionIndex++)
			if (layerWeights[contributionIndex] < 0)
				negativeSuppressionWeights[contributionIndex] += parameters.negativeSuppressionWeight; 

		layerWeights = extractSupervoxelLayers(parameters, negativeSuppressionWeights);
	}

	//
	// Return the result
	//
	SupervoxelLayerSet result;
	for (UINT layerIndex=0; layerIndex < numLayers; layerIndex++)
	{
		SupervoxelLayer layer;
		for (UINT supervoxelIndex=0; supervoxelIndex < numSupervoxels; supervoxelIndex++)
			layer.supervoxelWeights.pushBack(layerWeights[layerIndex*numSupervoxels + supervoxelIndex]);
		result.layers.pushBack(layer);
	}

	return result;

}


Vector<double> LayerBuilder::extractSupervoxelLayers(const AppParameters &parameters, const Vector<double> &negativeSuppressionWeights)
{
	//
	// Update the energy matrix with the negative suppression weights (TODO: do this in eigen?)
	//
	SparseMatrix<double> updatedA(_A);
	for (UINT contributionIndex = 0; contributionIndex < negativeSuppressionWeights.size(); contributionIndex++)
		if (negativeSuppressionWeights[contributionIndex] > 0)
			updatedA(contributionIndex, contributionIndex) += sqrt(negativeSuppressionWeights[contributionIndex]);

	LinearSolverConjugateGradient<double> solver;

	return solver.solveLeastSquares(updatedA, _b);
}

void LayerBuilder::ComputeNeighborWeights(const AppParameters &parameters, const Video &v, const Vector<VideoCoordinate> &supervoxels, NearestNeighborSearchANN<float> &supervoxelSearch)
{
	_neighborhoods.resize(supervoxels.size(), SupervoxelNeighborhood());

	for (UINT supervoxelIndex=0; supervoxelIndex < supervoxels.size(); supervoxelIndex++)
	{
		Vector<UINT> neighborIndices = supervoxelSearch.kNearest(supervoxels[supervoxelIndex].features, parameters.supervoxelNeighborCount+1, 0.0f);
		if (neighborIndices.contains(supervoxelIndex))
			neighborIndices.removeSwap(neighborIndices.findFirstIndex(supervoxelIndex));
		else
			neighborIndices.popBack();

		_neighborhoods[supervoxelIndex].indices = neighborIndices;
		_neighborhoods[supervoxelIndex].embeddingWeights = MathHelper::linearEmbedding(parameters, supervoxels, neighborIndices, supervoxels[supervoxelIndex].features);

		const bool testLinearEmbedding = false;
		if(testLinearEmbedding)
		{
			Vector<double> reconstruction(3, 0.0);
			for(UINT neighborIndex = 0; neighborIndex < (UINT)neighborIndices.size(); neighborIndex++)
			{
				for(UINT dimension = 0; dimension < 3; dimension++)
				{
					reconstruction[dimension] += _neighborhoods[supervoxelIndex].embeddingWeights[neighborIndex] * supervoxels[neighborIndices[neighborIndex]].features[dimension];
				}
			}
			double maxDelta = 0.0;
			for(UINT dimension = 0; dimension < 3; dimension++)
				maxDelta = Math::max(maxDelta, fabs(reconstruction[dimension] - (double)supervoxels[supervoxelIndex].features[dimension]));
			Console::log(Convert::toString(maxDelta));
		}
	}

}

void LayerBuilder::ComputeWeightMatrix(const AppParameters &parameters) 
{
	_W = SparseMatrix<double>((UINT)_neighborhoods.size(), (UINT)_neighborhoods.size());
	for (UINT supervoxelIndex=0; supervoxelIndex < _neighborhoods.size(); supervoxelIndex++)
	{
		const SupervoxelNeighborhood &neighborhood = _neighborhoods[supervoxelIndex];
		for (UINT neighborIndex=0; neighborIndex<neighborhood.indices.size(); neighborIndex++)
		{
			_W(supervoxelIndex, neighborhood.indices[neighborIndex]) = neighborhood.embeddingWeights[neighborIndex];
		}
	}
}

void LayerBuilder::ComputeEnergyMatrix(const AppParameters &parameters, const Vector<VideoCoordinate> &supervoxels, const Palette &palette, const Vector<SupervoxelLayerConstraint> &explicitConstraints) 
{
	UINT numSupervoxels = (UINT)supervoxels.size();
	UINT numLayers = (UINT)palette.size();
	UINT numContributions = numSupervoxels*numLayers;

	//
	// Solve for the A and b matrices of the energy equation
	//
	// In block form, A = [E M R U]
	// E = explicit constraints
	// M = manifold consistency matrix
	// R = image reconstruction matrix
	// U = unity matrix
	//
	// weighted appropriately with sqrt(term weights)
	//
	// We're then finding the least squares solution, which is equivalent to solving for 
	// A^tA x = A^tb
	//

	UINT ERows = numContributions; 
	UINT MRows = numContributions;
	UINT RRows = 3*numSupervoxels;
	UINT URows = numSupervoxels; 

	_A = SparseMatrix<double>(ERows+MRows+RRows+URows, numContributions);
	_b = Vector<double>(ERows+MRows+RRows+URows, 0.0);
	

	//
	// Explicit constraints
	// Save room for the negative suppression constraints
	//
	UINT termIndex = 0;
	double sqrtExplicitConstraintWeight = sqrt(parameters.voxelConstraintWeight);
	for (UINT constraintIndex=0; constraintIndex < explicitConstraints.size(); constraintIndex++)
	{
		const SupervoxelLayerConstraint &constraint = explicitConstraints[constraintIndex];
		int contributionIndex = constraint.index * constraint.layerIndex;
		_A(contributionIndex, contributionIndex) = sqrtExplicitConstraintWeight;
		_b[contributionIndex] = sqrtExplicitConstraintWeight*constraint.target;
	}
	termIndex += ERows;

	//
	// Manifold consistency 
	//
	double sqrtManifoldWeight = 1;
	for (UINT layerIndex=0; layerIndex < numLayers; layerIndex++)
	{	
		// copy I - W along the block diagonal
		UINT blockIndex = layerIndex*numSupervoxels;
		for (UINT supervoxelIndex=0; supervoxelIndex < numSupervoxels; supervoxelIndex++)
		{
			_A(termIndex+blockIndex+supervoxelIndex, blockIndex+supervoxelIndex) = 1*sqrtManifoldWeight;

			const SparseRow<double> &neighbors = _W.sparseRow(supervoxelIndex);
			for (UINT neighborEntryIndex = 0; neighborEntryIndex < neighbors.entries.size(); neighborEntryIndex++)
				_A(termIndex+blockIndex+supervoxelIndex, blockIndex+neighbors.entries[neighborEntryIndex].col) = -neighbors.entries[neighborEntryIndex].val*sqrtManifoldWeight;
			
		}
	}
	termIndex += MRows;

	//
	// Image reconstruction
	//
	double sqrtReconstructionWeight = sqrt(parameters.reconstructionWeight);
	for (UINT supervoxelIndex=0; supervoxelIndex < numSupervoxels; supervoxelIndex++)
	{		
		vec3f voxelColor = vec3f(supervoxels[supervoxelIndex].color);
		for (UINT channelIndex=0; channelIndex < 3; channelIndex++)
		{
			UINT rowIndex = termIndex + channelIndex * numSupervoxels + supervoxelIndex;
			for (UINT layerIndex=0; layerIndex < numLayers; layerIndex++)
				_A(rowIndex, numSupervoxels*layerIndex+supervoxelIndex) = palette[layerIndex][channelIndex] * sqrtReconstructionWeight;
			_b[rowIndex] = voxelColor[channelIndex] * sqrtReconstructionWeight;
		}
	}
	termIndex += RRows;

	//
	// Unity
	//
	double sqrtUnityWeight = sqrt(parameters.sumToOneWeight);
	for (UINT supervoxelIndex=0; supervoxelIndex < numSupervoxels; supervoxelIndex++)
	{
		UINT rowIndex = termIndex + supervoxelIndex;
		for (UINT layerIndex = 0; layerIndex < numLayers; layerIndex++)
			_A(rowIndex, layerIndex*numSupervoxels+supervoxelIndex) = sqrtUnityWeight;
		_b[rowIndex] = sqrtUnityWeight;
	}


}




class MathHelper
{
public:
	static Vector<double> linearEmbedding(const AppParameters &parameters, const Vector<VideoCoordinate> &supervoxels, const Vector<UINT> &supervoxelIndices, const float *pixelFeatures)
	{
		//
		// This implementation of linear embedding is based off a CMU lecture:
		// Nonlinear Dimensionality Reduction I: Local Linear Embedding
		// http://www.stat.cmu.edu/~cshalizi/350/lectures/14/lecture-14.pdf
		//

		const UINT k = (UINT)supervoxelIndices.size();

		//
		// This is the slow way of computing G, but this function is called so frequently we should try to inline G without
		// explicitly constructing z.
		//
		// We don't have dense matrices yet, so sparse matrices are used instead so it compiles.
		//
		DenseMatrix<double> z(k, 3);
		for(UINT neighborIndex = 0; neighborIndex < k; neighborIndex++)
		{
			const VideoCoordinate &neighbor = supervoxels[supervoxelIndices[neighborIndex]];
			for(UINT dimensionIndex = 0; dimensionIndex < 3; dimensionIndex++)
				z(neighborIndex, dimensionIndex) = neighbor.features[dimensionIndex] - pixelFeatures[dimensionIndex];
		}

		DenseMatrix<double> G = z * z.transpose();

		//
		// Add weight regularization term
		//
		for(UINT neighborIndex = 0; neighborIndex < k; neighborIndex++)
			G(neighborIndex, neighborIndex) += parameters.linearEmbeddingRegularizationTerm;
		
		G.invertInPlace();

		Vector<double> columnVector(k, 1.0);
		Vector<double> result = DenseMatrix<double>::multiply(G, columnVector);

		result.scale(1.0 / result.sum());

		return result;
	}
};

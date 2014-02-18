
#include "main.h"

PixelLayerSet::PixelLayerSet(const AppParameters &parameters, const Video &v, UINT frameIndex, const Vector<VideoCoordinate> &supervoxels, const NearestNeighborSearchANN<float> &supervoxelSearch, const SupervoxelLayerSet &supervoxelLayers)
{
	//
	// this is where we implement equation 10 (X_j = Q * L_j)
	//

	const UINT layerCount = (UINT)supervoxelLayers.layers.size();
	layers.allocate(layerCount);
	for(UINT layerIndex = 0; layerIndex < layerCount; layerIndex++)
	{
		PixelLayer &layer = layers[layerIndex];
		layer.color = supervoxelLayers.layers[layerIndex].color;
		layer.pixelWeights.allocate(v.height, v.width);
	}

#pragma omp parallel for
	for(int y = 0; y < (int)v.height; y++)
		for(int x = 0; x < (int)v.width; x++)
		{
			VideoCoordinate pixelCoordinate(parameters, v.frames[frameIndex](y, x), vec2i(x, y), frameIndex, v.width, v.height);
			
			Vector<UINT> indices = supervoxelSearch.kNearest(pixelCoordinate.features, parameters.pixelNeighborCount, 0.0f);

			Vector<double> weights = MathHelper::linearEmbedding(parameters, supervoxels, indices, pixelCoordinate.features);
			
			for(UINT layerIndex = 0; layerIndex < layerCount; layerIndex++)
			{
				PixelLayer &pixelLayer = layers[layerIndex];
				const SupervoxelLayer &supervoxelLayer = supervoxelLayers.layers[layerIndex];

				double sum = 0.0;
				for(UINT neighborIndex = 0; neighborIndex < parameters.pixelNeighborCount; neighborIndex++)
					sum += supervoxelLayer.supervoxelWeights[indices[neighborIndex]] * weights[neighborIndex];

				pixelLayer.pixelWeights(y, x) = (float)sum;
			}
		}
}

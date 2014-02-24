
struct AppParameters
{
	AppParameters(const String &frameIndexilename)
	{
		pixelNeighborCount = 20;
		supervoxelNeighborCount = 20;
		spatialToColorScale = 0.1f;
		temporalToColorScale = 0.1f;

		negativeSuppressionIters = 3;

		supervoxelCount = 2000;
		regionGrowingIterations = 5;

		visualizeIntermediateSupervoxels = true;
		visualizeFinalSupervoxels = true;
		supervoxelVisualizationFrameCount = 5;

		voxelConstraintWeight = 0.1;
		sumToOneWeight = 0.1;
		reconstructionWeight = 0.5;
		negativeSuppressionWeight = 0.01;

		linearEmbeddingRegularizationTerm = 0.000001; 
	}

	double voxelConstraintWeight;
	double sumToOneWeight;
	double reconstructionWeight;
	double regularizationWeight;
	double negativeSuppressionWeight;
	double maniframeIndexoldWeight;

	int negativeSuppressionIters;


	//
	// linear embedding neighborhood sizes
	//
	UINT pixelNeighborCount;
	UINT supervoxelNeighborCount;

	//
	// linear embedding constants
	//
	double linearEmbeddingRegularizationTerm;

	//
	// spatial, temporal, and color scales
	//
	float spatialToColorScale;
	float temporalToColorScale;

	//
	// supervoxel constants
	//
	UINT supervoxelCount;
	UINT regionGrowingIterations;

	//
	// supervoxel visualization constants
	//
	bool visualizeIntermediateSupervoxels;
	bool visualizeFinalSupervoxels;
	UINT supervoxelVisualizationFrameCount;
};

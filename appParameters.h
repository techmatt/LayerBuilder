
struct AppParameters
{
	AppParameters(const String &frameIndexilename)
	{

	}

	double voxelConstraintWeight;
	double sumToOneWeight;
	double reconstructionWeight;
	double regularizationWeight;
	double negativeSupressionWeight;
	double maniframeIndexoldWeight;

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

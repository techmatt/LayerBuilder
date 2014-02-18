
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

	float spatialToColorScale;
	float temporalToColorScale;

	UINT supervoxelCount;
	UINT regionGrowingIterations;
};

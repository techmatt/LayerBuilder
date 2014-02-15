
struct AppParameters
{
	AppParameters(const String &filename)
	{

	}

	double pixelConstraintWeight;
	double sumToOneWeight;
	double reconstructionWeight;
	double regularizationWeight;
	double negativeSupressionWeight;
	double manifoldWeight;
};

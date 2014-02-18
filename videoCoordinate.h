
struct VideoCoordinate
{
	VideoCoordinate()
	{

	}
	VideoCoordinate(const AppParameters &parameters, RGBColor _color, vec2i _coord, UINT _frame, int width, int height)
	{
		color = _color;
		coord = _coord;
		frame = _frame;
		makeFeatureVector(parameters, width, height);
	}
	RGBColor color;
	vec2i coord;
	UINT frame;
	float features[6];

private:
	void makeFeatureVector(const AppParameters &parameters, int width, int height)
	{
		features[0] = color.r / 255.0f;
		features[1] = color.g / 255.0f;
		features[2] = color.b / 255.0f;
		features[3] = coord.x/(float)width * parameters.spatialToColorScale;
		features[4] = coord.y/(float)height * parameters.spatialToColorScale;
		features[5] = frame * parameters.temporalToColorScale;
	}
};

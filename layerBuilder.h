struct SupervoxelLayerConstraint
{
    SupervoxelLayerConstraint()
    {

    }
    SupervoxelLayerConstraint(UINT _index, UINT _layerIndex, double _target, double _weight)
    {
        index = _index;
        layerIndex = _layerIndex;
        target = _target;
		weight = _weight;
    }

    UINT index;
    UINT layerIndex;
    double target;
	double weight;
};


struct SupervoxelNeighborhood
{
    Vector<UINT> indices;
    Vector<double> embeddingWeights;
};

struct Palette
{
	Palette()
	{
	}

	Palette(const std::string &filename)
	{
		Vector<std::string> lines = Utility::getFileLines(filename);
		if (lines.size() == 0)
			return;
		Vector<std::string> colorStrings = StringUtil::split(lines[0], " ");
		for (UINT colorIndex=0; colorIndex < colorStrings.size(); colorIndex++)
		{
			Vector<std::string> fields = StringUtil::split(colorStrings[colorIndex], ",");
			colors.pushBack(vec3f( Convert::toInt(fields[0])/255.0f, Convert::toInt(fields[1])/255.0f, Convert::toInt(fields[2])/255.0f));
		}
	}

	vec3f& operator[](UINT index)
	{
		return colors[index];
	}

	const vec3f& operator[](UINT index) const
	{
		return colors[index];
	}

	UINT size() const
	{
		return (UINT)colors.size();
	}

	Vector<vec3f> colors;
};

class LayerBuilder
{
public:
	SupervoxelLayerSet buildSupervoxelLayers(const AppParameters &parameters, const Video &v, const Vector<VideoCoordinate> &supervoxels, NearestNeighborSearchANN<float> &supervoxelSearch, Palette palette, Vector<SupervoxelLayerConstraint> explicitConstraints);
	
private:
	Vector<double> extractSupervoxelLayers(const AppParameters &parameters, const Vector<double> &negativeSuppressionWeights);
	void ComputeNeighborWeights(const AppParameters &parameters, const Video &v, const Vector<VideoCoordinate> &supervoxels,  NearestNeighborSearchANN<float> &supervoxelSearch);
	void ComputeWeightMatrix(const AppParameters &parameters);
	void ComputeEnergyMatrix(const AppParameters &parameters, const Vector<VideoCoordinate> &supervoxels, const Palette &palette, const Vector<SupervoxelLayerConstraint> &explicitConstraints);


	Vector<SupervoxelNeighborhood> _neighborhoods;
	SparseMatrix<double> _W;
	SparseMatrix<double> _A;
	Vector<double> _b;


};

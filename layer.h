
//
// A supervoxel layer stores the per-supervoxel weights
//
struct SupervoxelLayer
{
	vec3f color;
	Vector<double> supervoxelWeights;
};

struct SupervoxelLayerSet
{
	SupervoxelLayerSet() {}
	SupervoxelLayerSet(SupervoxelLayerSet &&s)
	{
		layers = std::move(s.layers);
	}
	void operator=(SupervoxelLayerSet &&s)
	{
		layers = std::move(s.layers);
	}

	Vector<SupervoxelLayer> layers;
};

//
// A pixel layer stores the per-pixel weights *for a single frame*
// This is because we might not be able to store all the layers
// for a long video at once, so we want to generate and save
// them frame-by-frame
//
struct PixelLayer
{
	vec3f color;
	Grid<float> pixelWeights;
};

struct PixelLayerSet
{
	PixelLayerSet(const AppParameters &parameters, const Video &v, UINT frameIndex, const Vector<VideoCoordinate> &supervoxels, const NearestNeighborSearchANN<float> &supervoxelSearch, const SupervoxelLayerSet &supervoxelLayers);

	//
	// unfortunately C++11 does not support implicit move constructors
	//
	PixelLayerSet(PixelLayerSet &&s)
	{
		layers = std::move(s.layers);
	}
	void operator=(PixelLayerSet &&s)
	{
		layers = std::move(s.layers);
	}

	void savePNG(const String baseName);
	

	Vector<PixelLayer> layers;
};

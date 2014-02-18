
class LayerBuilder
{
public:
	SupervoxelLayerSet buildSupervoxelLayers(const AppParameters &parameters, const Video &v, const Vector<VideoCoordinate> &supervoxels, NearestNeighborSearchANN<float> &supervoxelSearch);
	
private:

};


#include <queue>

class SupervoxelGenerator
{
public:
	virtual Vector<VideoCoordinate> extract(const AppParameters &parameters, const Video &v) = 0;
};

class SupervoxelGeneratorRandom : public SupervoxelGenerator
{
public:
	Vector<VideoCoordinate> extract(const AppParameters &parameters, const Video &v);
};

struct Supervoxel
{
	void reset(const Video &v, const vec3i &seed);

	vec3i massCentroid() const;
	double assignmentError(const Video &v, const vec3i &coord) const;

	void resetColor(const RGBColor &_color);
	void computeColor(const Video &v);

	vec3f color;
	Vector<vec3i> voxels;
	vec3i seed;
};

class SupervoxelGeneratorRegionGrowing : public SupervoxelGenerator
{
public:
	struct QueueEntry
	{
		double priority;
		vec3i coord;
		UINT supervoxelIndex;
	};

	Vector<VideoCoordinate> extract(const AppParameters &parameters, const Video &v);
    void extract(const AppParameters &parameters, const Video &v, Vector<Supervoxel> &supervoxelsOut, Vector< Grid<UINT> > &assignmentsOut);

private:
    void initializeSupervoxels(const AppParameters &parameters, const Video &v);
    void assignVoxel(const AppParameters &parameters, const Video &v, const vec3i &coord, UINT clusterIndex);
    void growSupervoxels(const AppParameters &parameters, const Video &v);
    void recenterSupervoxels(const AppParameters &parameters, const Video &v);
    
    void drawSupervoxelIDs(Video &v, int startFrameIndex, int frameCount);
    void drawSupervoxelColors(const Video &inputVid, Video &outputVid, int startFrameIndex, int frameCount);

    Vector<Supervoxel> _supervoxels;
    std::priority_queue<QueueEntry> _queue;
    Vector< Grid<UINT> > _assignments;
    vec3i _dimensions;
};

inline bool operator < (const SupervoxelGeneratorRegionGrowing::QueueEntry &a, const SupervoxelGeneratorRegionGrowing::QueueEntry &b)
{
    return (a.priority < b.priority);
}

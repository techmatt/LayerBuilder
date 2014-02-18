
#include "Main.h"

Vector<VideoCoordinate> SupervoxelGeneratorRandom::extract(const AppParameters &parameters, const Video &video)
{
	Vector<VideoCoordinate> result;
	for(UINT supervoxelIndex = 0; supervoxelIndex < parameters.supervoxelCount; supervoxelIndex++)
	{
		UINT frame = rand() % video.frames.size();
		UINT x = rand() % video.width;
		UINT y = rand() % video.height;
		VideoCoordinate coord(parameters, video.frames[frame](y, x), vec2i(x, y), frame, video.width, video.height);
		result.pushBack(coord);
	}
	return result;
}

void Supervoxel::reset(const Video &v, const vec3i &_seed)
{
	voxels.deleteMemory();
	seed = _seed;
	voxels.pushBack(seed);
}

vec3i Supervoxel::massCentroid() const
{
	vec3f center = vec3f::origin;
	for(const vec3i &p : voxels) center += vec3f(p);
	center /= voxels.size();

	vec3i bestCoord = voxels[0];
	float bestDistSq = std::numeric_limits<float>::max();
	for(const vec3i &p : voxels)
	{
		float curDistSq = vec3f::distSq(vec3f(p), center);
		if(curDistSq < bestDistSq)
		{
			bestDistSq = curDistSq;
			bestCoord = p;
		}
	}
	return bestCoord;
}

double Supervoxel::assignmentError(const Video &v, const vec3i &coord) const
{
	return vec3f::distSq(color, vec3f(v.frames[coord.z](coord.y, coord.x)));
}

void Supervoxel::resetColor( const RGBColor &_color )
{
	color = vec3f(_color);
}

void Supervoxel::computeColor( const Video &v )
{
	color = vec3f::origin;
	for(vec3i p : voxels) color += vec3f(v.frames[p.z](p.y, p.x));
	color /= (float)voxels.size();
}

Vector<VideoCoordinate> SupervoxelGeneratorRegionGrowing::extract(const AppParameters &parameters, const Video &v)
{
	Vector<Supervoxel> supervoxelsOut;
	Vector< Grid<UINT> > assignmentsOut;
	extract(parameters, v, supervoxelsOut, assignmentsOut);

	Vector<VideoCoordinate> result;
	for(const Supervoxel &p : supervoxelsOut)
	{
		result.pushBack(VideoCoordinate(parameters, RGBColor(p.color), vec2i(p.seed.x, p.seed.y), p.seed.z, v.width, v.height));
	}
	return result;
}

void SupervoxelGeneratorRegionGrowing::extract(const AppParameters &parameters, const Video &v, Vector<Supervoxel> &supervoxelsOut, Vector< Grid<UINT> > &assignmentsOut)
{
	ComponentTimer timer( "segmenting video: " + String(v.width) + "x" + String(v.height) + ", " + String(v.frames.size()) + " frames" );

	_dimensions = vec3i(v.width, v.height, (int)v.frames.size());
	_assignments.allocate(v.frames.size());
	for (UINT i = 0; i < v.frames.size(); i++)
		_assignments[i].allocate(_dimensions.y, _dimensions.x);
	_supervoxels.allocate(parameters.supervoxelCount);

	initializeSupervoxels(parameters, v);

	const UINT vizFrameCount = Math::min(parameters.supervoxelVisualizationFrameCount, (UINT)v.frames.size());

	auto vizHelper = [vizFrameCount](int iteration, int frameIndex, const String &descriptor)
	{
		String iterationDesc = "_i" + String(iteration);
		if(iteration == -1) iterationDesc = "Final";

		String frameDesc = "_f" + String(frameIndex);
		if(vizFrameCount == 1) frameDesc = "";
		
		return "supervoxel" + descriptor + iterationDesc + frameDesc + ".png";
	};

	for(UINT iterationIndex = 0; iterationIndex < parameters.regionGrowingIterations; iterationIndex++)
	{
		Console::log("starting supervoxel iteration " + String(iterationIndex));
		//ComponentTimer timer( "Iteration " + String(iterationIndex) );
		growSupervoxels(parameters, v);

		if(parameters.visualizeIntermediateSupervoxels)
		{
			Video clusterVid0, clusterVid1;
			drawSupervoxelIDs(clusterVid0, 0, vizFrameCount);
			drawSupervoxelColors(v, clusterVid1, 0, vizFrameCount);
			for (UINT frameIndex = 0; frameIndex < vizFrameCount; frameIndex++)
			{
				LodePNG::save(clusterVid0.frames[frameIndex], vizHelper(iterationIndex, frameIndex, "Clusters"));
				LodePNG::save(clusterVid1.frames[frameIndex], vizHelper(iterationIndex, frameIndex, "Colors"));
			}
		}

		recenterSupervoxels(parameters, v);
	}

	growSupervoxels(parameters, v);
	for(Supervoxel &p : _supervoxels)
		p.computeColor(v);

	if(parameters.visualizeFinalSupervoxels)
	{
		Video clusterVid0, clusterVid1;
		drawSupervoxelIDs(clusterVid0, 0, vizFrameCount);
		drawSupervoxelColors(v, clusterVid1, 0, vizFrameCount);
		for (UINT frameIndex = 0; frameIndex < vizFrameCount; frameIndex++)
		{
			LodePNG::save(clusterVid0.frames[frameIndex], vizHelper(-1, frameIndex, "Clusters"));
			LodePNG::save(clusterVid1.frames[frameIndex], vizHelper(-1, frameIndex, "Colors"));
		}
	}

	supervoxelsOut = std::move(_supervoxels);
	assignmentsOut = std::move(_assignments);
}

void SupervoxelGeneratorRegionGrowing::initializeSupervoxels(const AppParameters &parameters, const Video &v)
{
	Vector<vec3i> seeds;
	for(Supervoxel &p : _supervoxels)
	{
		vec3i randomSeed(rand() % _dimensions.x, rand() % _dimensions.y, rand() % _dimensions.z);
		while(seeds.contains(randomSeed))
			randomSeed = vec3i(rand() % _dimensions.x, rand() % _dimensions.y, rand() % _dimensions.z);

		p.resetColor(v.frames[randomSeed.z](randomSeed.y, randomSeed.x));
		p.reset(v, randomSeed);
		seeds.pushBack(randomSeed);
	}
}

void SupervoxelGeneratorRegionGrowing::assignVoxel(const AppParameters &parameters, const Video &v, const vec3i &coord, UINT supervoxelIndex)
{
	if(_assignments[coord.z](coord.y, coord.x) != 0xFFFFFFFF)
		return;

	_assignments[coord.z](coord.y, coord.x) = supervoxelIndex;
	_supervoxels[supervoxelIndex].voxels.pushBack(coord);

	const UINT neighborCount = 6;
	const int XOffsets[neighborCount] = {-1, 1, 0, 0,  0, 0};
	const int YOffsets[neighborCount] = {0, 0, -1, 1,  0, 0};
	const int FOffsets[neighborCount] = {0, 0,  0, 0, -1, 1};
	for(UINT neighborIndex = 0; neighborIndex < neighborCount; neighborIndex++)
	{
		vec3i finalCoord(coord.x + XOffsets[neighborIndex], coord.y + YOffsets[neighborIndex], coord.z + FOffsets[neighborIndex]);
		if(finalCoord.z >= 0 && finalCoord.z < (int)_assignments.size() && _assignments[finalCoord.z].validCoordinates(finalCoord.y, finalCoord.x) && 
			_assignments[finalCoord.z](finalCoord.y, finalCoord.x) == 0xFFFFFFFF)
		{
			QueueEntry e;
			e.supervoxelIndex = supervoxelIndex;
			e.coord = finalCoord;
			e.priority = 1.0 - _supervoxels[supervoxelIndex].assignmentError(v, finalCoord);
			_queue.push(e);
		}
	}
}

void SupervoxelGeneratorRegionGrowing::growSupervoxels(const AppParameters &parameters, const Video &v)
{
	for (UINT i = 0; i < _assignments.size(); i++)
		_assignments[i].clear(0xFFFFFFFF);

	//
	// Insert all seeds
	//
	for(UINT supervoxelIndex = 0; supervoxelIndex < _supervoxels.size(); supervoxelIndex++)
		assignVoxel(parameters, v, _supervoxels[supervoxelIndex].seed, supervoxelIndex);

	while(!_queue.empty())
	{
		QueueEntry curEntry = _queue.top();
		_queue.pop();
		assignVoxel(parameters, v, curEntry.coord, curEntry.supervoxelIndex);
	}
}

void SupervoxelGeneratorRegionGrowing::recenterSupervoxels(const AppParameters &parameters, const Video &v)
{
	const UINT clusterMinSizeCutoff = 20;

	UINT teleportCount = 0;
	Vector<vec3i> seeds;
	for(Supervoxel &p : _supervoxels)
	{
		vec3i newSeed;
		if(p.voxels.size() < clusterMinSizeCutoff)
		{
			newSeed = vec3i(rand() % _dimensions.x, rand() % _dimensions.y, rand() % _dimensions.z);
			while(seeds.contains(newSeed))
				newSeed = vec3i(rand() % _dimensions.x, rand() % _dimensions.y, rand() % _dimensions.z);
			
			teleportCount++;
			p.resetColor(v.frames[newSeed.z](newSeed.y, newSeed.x));
		}
		else
		{
			newSeed = p.massCentroid();
			p.computeColor(v);
		}
		p.reset(v, newSeed);
		seeds.pushBack(newSeed);
	}
}

void SupervoxelGeneratorRegionGrowing::drawSupervoxelIDs(Video &v, int startFrameIndex, int frameCount)
{
	UINT supervoxelMaxValue = 0;
	for (UINT i = 0; i < _assignments.size(); i++)
		supervoxelMaxValue = Math::max(supervoxelMaxValue, _assignments[i].maxValue());

	const UINT clusterCount = supervoxelMaxValue + 1;
	Vector<RGBColor> colors(clusterCount);
	for(RGBColor &c : colors) c = RGBColor::randomColor();
	
	v.frames.allocate(frameCount);
	const UINT height = _assignments[0].rows();
	const UINT width = _assignments[0].cols();
	for (int frameIndex = startFrameIndex; frameIndex < startFrameIndex + frameCount; frameIndex++)
	{
		v.frames[frameIndex - startFrameIndex].allocate(width, height);

		for(UINT y = 0; y < height; y++)
			for(UINT x = 0; x < width; x++)
				v.frames[frameIndex - startFrameIndex](y, x) = colors[_assignments[frameIndex](y, x)];
	}
}

void SupervoxelGeneratorRegionGrowing::drawSupervoxelColors(const Video &inputVid, Video &outputVid, int startFrameIndex, int frameCount)
{
	const UINT height = _assignments[0].rows();
	const UINT width = _assignments[0].cols();
	outputVid.frames.allocate(frameCount);
	for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
	{
		outputVid.frames[frameIndex].allocate(width, height);
		for(UINT y = 0; y < height; y++)
			for(UINT x = 0; x < width; x++)
			{
				const Supervoxel &p =  _supervoxels[_assignments[frameIndex](y, x)];
				outputVid.frames[frameIndex](y, x) = RGBColor(p.color);
			}
	}
}

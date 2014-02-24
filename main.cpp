
#include "main.h"

class App
{
public:
	void go(const CommandLineReader &commandLine);
};

void App::go(const CommandLineReader &commandLine)
{
	const String inputFile = commandLine.arg(0);
	const String outputFile = commandLine.arg(1);
	AppParameters parameters(commandLine.arg(2));
	const String paletteFile = commandLine.argWithPrefix("palette=");
	const String seedsFile = commandLine.argWithPrefix("seeds=");

	Video v;
	if(inputFile.endsWith(".png"))
		v.loadFromImage(inputFile);
	else
		v.loadFromDirectory(inputFile);

	SupervoxelGeneratorRegionGrowing generator;
	//SupervoxelGeneratorRandom generator;

	//
	// Generate the supervoxels
	//
	Vector<VideoCoordinate> supervoxels = generator.extract(parameters, v);

	//
	// Generate the supervoxel search structure
	//
	NearestNeighborSearchANN<float> supervoxelSearch;

	Vector<const float*> supervoxelCoordinates;
	for(const VideoCoordinate &superpixel : supervoxels)
		supervoxelCoordinates.pushBack(superpixel.features);

	supervoxelSearch.init(supervoxelCoordinates, 6, Math::max(parameters.pixelNeighborCount, parameters.supervoxelNeighborCount) + 1);

	//
	// Process palette and constraints
	//
	Palette palette(paletteFile);

	Vector<SupervoxelLayerConstraint> explicitConstraints;

	//
	// Build the layers
	//
	LayerBuilder builder;
	SupervoxelLayerSet supervoxelLayers = builder.buildSupervoxelLayers(parameters, v, supervoxels, supervoxelSearch, palette, explicitConstraints);

	//
	// Generate the pixel layers and immediately save them to a binary file
	//
	for(UINT frameIndex = 0; frameIndex < v.frames.size(); frameIndex++)
	{
		PixelLayerSet frameLayers = PixelLayerSet(parameters, v, frameIndex, supervoxels, supervoxelSearch, supervoxelLayers);
		//frameLayers.save("");
		frameLayers.savePNG("frame-"+String(frameIndex)+"-layer");
	}
}

int main(int argc, char* argv[])
{
	Console::openLogFile("console.txt");

	const String &usage = String("image usage: layerBuilder input.png output.layers parameters.txt <palette=palette.txt> <seeds=seeds.txt>\n") +
						  String("video usage: layerBuilder videos/inputVid/ output.layers parameters.txt <palette=palette.txt> <seeds=seeds.txt>");

	//CommandLineReader commandLine(usage, argc, argv);
	CommandLineReader commandLine(usage, "../testData/angel.png angel.layers parameters.txt palette=../testData/angelPalette.txt seeds=faceSeeds.txt");
	//CommandLineReader commandLine(usage, "../testData/bigbuckbunny/ angel.layers parameters.txt palette=facePalette.txt seeds=faceSeeds.txt");

	App a;
	a.go(commandLine);

	Console::log("Press any key to continue...");
	std::cin.get();

	return 0;
}

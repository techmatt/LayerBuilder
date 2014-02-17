
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
	AppParameters params(commandLine.arg(2));
	const String paletteFile = commandLine.argWithPrefix("palette=");
	const String seedsFile = commandLine.argWithPrefix("seeds=");

	Bitmap stuff;
}

void main(int argc, char* argv[])
{
	Console::openLogFile("console.txt");

	const String &usage = "layerBuilder input.png output.layers parameters.txt <palette=palette.txt> <seeds=seeds.txt>";

	//CommandLineReader commandLine(usage, argc, argv);
	CommandLineReader commandLine(usage, "face.png face.layers parameters.txt palette=facePalette.txt seeds=faceSeeds.txt");

	App a;
	a.go(commandLine);

	Console::log("Press any key to continue...");
	std::cin.get();
}

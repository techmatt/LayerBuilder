
#include "main.h"

class App
{
public:
	void go(const CommandLineReader &commandLine);
};

void App::go(const CommandLineReader &commandLine)
{
	AppParameters params(commandLine.arg(2));

	Console::log("logging!");
	std::cin.get();
}

void main(int argc, char* argv[])
{
	Console::openLogFile("console.txt");

	const String &usage = "layerBuilder input.png output.layers parameters.txt <palette=palette.txt> <seeds=seeds.txt>";

	//CommandLineReader commandLine(usage, argc, argv);
	CommandLineReader commandLine(usage, "face.png face.layers parameters.txt palette=facePalette.txt seeds=faceSeeds.txt");

	App a;
	a.go(commandLine);
}

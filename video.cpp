
#include "main.h"

void Video::loadFromImage(const std::string &filename)
{
	frames.resize(1);
	frames[0] = LodePNG::load(filename);
	width = frames[0].cols();
	height = frames[0].rows();
}

void Video::loadFromDirectory(const std::string &directory)
{
	Directory dir(directory);
	Vector<std::string> sortedFiles = dir.filesWithSuffix(".png");
	sortedFiles.sort();
	for(const std::string &filename : sortedFiles)
		frames.pushBack(LodePNG::load(dir.path() + filename));
	width = frames[0].cols();
	height = frames[0].rows();
}


#include "main.h"

void Video::loadFromImage(const String &filename)
{
	frames.resize(1);
	frames[0] = LodePNG::load(filename);
	width = frames[0].cols();
	height = frames[0].rows();
}

void Video::loadFromDirectory(const String &directory)
{
	Directory dir(directory);
	Vector<String> sortedFiles = dir.filesWithSuffix(".png");
	sortedFiles.sort();
	for(const String &filename : sortedFiles)
		frames.pushBack(LodePNG::load(dir.path() + filename));
	width = frames[0].cols();
	height = frames[0].rows();
}

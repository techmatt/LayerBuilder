
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
	for(const String &filename : dir.filesWithSuffix(".png"))
		frames.pushBack(LodePNG::load(filename));
	width = frames[0].cols();
	height = frames[0].rows();
}

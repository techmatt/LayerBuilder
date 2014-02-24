#TODO

* Make a new commandline exe for recoloring - Matt
* Recoloring UI that calls the exes on the commandline - Sharon
* DenseMatrix class - Matt
* Binary writing/reading classes
* Binary writing/reading files (using old Output/InputDataStreams for now) - Sharon
* Caching superpixels - Matt
* Determine palette and seeds from png - Sharon


There was a significant bug in the video-extraction code in LocallyLinearEmbedding, LayerExtractorVideo.cpp:
 -- tree.BuildTree(points, 5, Math::Max(parameters.pixelNeighborCount, parameters.superpixelNeighborCount) + 1);
 5 should be 6.  We'll fix it in this version, but if we notice significant differences in video performance we should revisit this bug.
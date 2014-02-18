#TODO

* Supervoxel solve (video) - Matt

* Layer solve (and make it QR friendly) 
	* Create the Layer, Palette, and other related classes - Sharon

* Make mLib cross-platform - Angela

* Multi-threading the pixel layer solve and the supervoxel solve

There was a significant bug in the video-extraction code in LocallyLinearEmbedding, LayerExtractorVideo.cpp:
 -- tree.BuildTree(points, 5, Math::Max(parameters.pixelNeighborCount, parameters.superpixelNeighborCount) + 1);
 5 should be 6.  We'll fix it in this version, but if we notice significant differences in video performance we should revisit this bug.
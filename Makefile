NAME=layerbuilder

CCSRCS=main.cpp layerBuilder.cpp video.cpp supervoxelGenerator.cpp mLibSource.cpp 

USER_CFLAGS=-I../../mLib/include -I../../mLib/external/include

include ../../mLib/Makefile.app
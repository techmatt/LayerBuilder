
//
// mLib config options
//

#define MLIB_ERROR_CHECK
#define MLIB_BOUNDS_CHECK

//
// mLib includes
//

#include "mLibCore.h"
#include "mLibANN.h"
#include "mLibLodePNG.h"

//
// It is often best to selectively include eigen to keep the
// pre-compiled header to a reasonable size.
//
//#include "mLibEigen.h"
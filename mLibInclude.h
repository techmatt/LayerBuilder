
//
// mLib config options
//

#ifdef DEBUG
#define MLIB_ERROR_CHECK
#endif

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
#include "mLibEigen.h"
// This is dbsk2d/dbsk2d_cassert.h

//- The <cassert> header does not
//- have inclusion guards. The
//- C and C++ standards say so.
//-- modelled after vcl_cassert

//:
// \file
// \author Amir Tamrakar

#include "vcl_compiler.h"

//##########################################################
// SPECIAL DEBUG MODE
//**********************************************************
//uncomment the following line for special debug mode.
//
//#define _DBSK2D_SPECIAL_DEBUG 
//
//##########################################################

// Win32: you can't set a bp on abort
#ifdef _WIN32

#undef dbsk2d_assert
#ifdef NDEBUG
# define dbsk2d_assert(x) ((void) 0)
#else 
#ifdef _DBSK2D_SPECIAL_DEBUG
extern void dbsk2d_assert_failure(char const *, int, char const *);
# define dbsk2d_assert(x) do { if (!(x)) dbsk2d_assert_failure(__FILE__, __LINE__, #x); } while (false)
#else
# define dbsk2d_assert(x) ((void) 0) //also do nothing  (do I need this?)
#endif
#endif

#ifdef VCL_METRO_WERKS
// for some reason, MW's <cassert> doesn't have its own printf() and abort() declarations.
# include <vcl_cstdio.h>
# include <vcl_cstdlib.h>
#endif

#else // i.e., not _WIN32

#if !VCL_CXX_HAS_HEADER_CASSERT
# include <assert.h>
#else
# include "iso/vcl_cassert.h"
#endif

//don't know how to handle this properly yet
//just define it as the standard assert

#undef dbsk2d_assert
#ifdef _DBSK2D_SPECIAL_DEBUG
# define dbsk2d_assert(x) (assert(x))
#else
# define dbsk2d_assert(x) ((void) 0)
#endif

#endif

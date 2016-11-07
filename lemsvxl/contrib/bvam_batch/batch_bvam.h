// This is brl/bpro/bppy/batch_bvam.h
#ifndef batch_bvam_h_
#define batch_bvam_h_
//:
// \file
// \brief External function declarations
// \author J.L. Mundy 
// \date February 5, 2008
//
//
// \verbatim
//  Modifications
// \endverbatim
#include "Python.h"


extern PyObject *
register_processes(PyObject *self, PyObject *args);

extern PyObject *
register_datatypes(PyObject *self, PyObject *args);

#endif //batch

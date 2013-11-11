//this is /contrib/bm/dsm_batch/reg_dsm.h
#ifndef REG_DSM_H_
#define REG_DSM_H_

#include<bpro/bpro_batch/bpro_batch.h>

static PyObject *register_processes(PyObject *self, PyObject *args);
static PyObject *register_datatypes(PyObject *self, PyObject *args);
static PyMethodDef dsm_batch_methods[METHOD_NUM+2];

#endif //REG_DSM_H_
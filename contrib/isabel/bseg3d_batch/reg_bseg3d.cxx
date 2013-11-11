#include "reg_bseg3d.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <bvxm/pro/bvxm_register.h>
#include <bseg3d/pro/bseg3d_register.h>
#include <bseg3d/algo/pro/bseg3d_algo_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  vil_register::register_process();
  vpgl_register::register_process();
  bvxm_register::register_process();
  bseg3d_register::register_process();
  bseg3d_algo_register::register_process();
  
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vpgl_register::register_datatype();
  bvxm_register::register_datatype();
  bseg3d_register::register_datatype();
  bseg3d_algo_register::register_datatype();

  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initbseg3d_batch(void)
{
  PyMethodDef reg_pro;
  reg_pro.ml_name = "register_processes";
  reg_pro.ml_meth = register_processes;
  reg_pro.ml_doc = "register_processes() create instances of each defined process";
  reg_pro.ml_flags = METH_VARARGS;


  PyMethodDef reg_data;
  reg_data.ml_name = "register_datatypes";
  reg_data.ml_meth = register_datatypes;
  reg_data.ml_doc = "register_datatypes() insert tables in the database for each type";
  reg_data.ml_flags = METH_VARARGS;

  bseg3d_batch_methods[0]=reg_pro;
  bseg3d_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    bseg3d_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("bseg3d_batch", bseg3d_batch_methods);
}

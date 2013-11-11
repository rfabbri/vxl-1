#include "reg_psm.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

// processes
#include <vil_pro/vil_register.h>
#include <vpgl_pro/vpgl_register.h>
#include <psm/pro/psm_register.h>
#include <psm/algo/pro/psm_algo_register.h>
#include <psm_opt/pro/psm_opt_register.h>
#include <psm_camera_opt/pro/psm_camera_opt_register.h>
#include <psm_mfa/pro/psm_mfa_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  vil_register::register_process();
  vpgl_register::register_process();
  psm_register::register_process();
  psm_algo_register::register_process();
  psm_opt_register::register_process();
  psm_camera_opt_register::register_process();
  psm_mfa_register::register_process();
  
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  vil_register::register_datatype();
  vpgl_register::register_datatype();
  psm_register::register_datatype();
  psm_algo_register::register_datatype();
  psm_opt_register::register_datatype();
  psm_camera_opt_register::register_datatype();
  psm_mfa_register::register_datatype();

  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initpsm_batch(void)
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

  psm_batch_methods[0]=reg_pro;
  psm_batch_methods[1]=reg_data;

  for (int i=0; i<METHOD_NUM; ++i) {
    psm_batch_methods[i+2]=batch_methods[i];
  }

  Py_InitModule("psm_batch", psm_batch_methods);
}

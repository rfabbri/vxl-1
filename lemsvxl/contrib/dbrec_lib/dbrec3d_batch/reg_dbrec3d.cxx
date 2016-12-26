#include "reg_dbrec3d.h"
#include <bprb/bprb_macros.h>
#include <bprb/bprb_batch_process_manager.h>

// processes
//#include <dbrec3d/pro/dbrec3d_register.h>
#include <bvpl/kernels/pro/bvpl_kernels_register.h>
#include <bvpl/bvpl_octree/pro/bvpl_octree_register.h>
#include <boxm/pro/boxm_register.h>
#include <boxm/algo/pro/boxm_algo_register.h>
#include <bvxm/grid/pro/bvxm_grid_register.h>
#include <bvxm/algo/pro/bvxm_algo_register.h>
#include <dbrec3d/bof/pro/bof_register.h>
#include <dbrec3d/pcl/pro/pcl_register.h>

PyObject *
register_processes(PyObject *self, PyObject *args)
{
  //dbrec3d_register::register_process();
  bvpl_kernels_register::register_process();
  bvpl_octree_register::register_process();
  boxm_register::register_process();
  boxm_algo_register::register_process();
  //bvxm_grid_register::register_process();
  //bvxm_algo_register::register_process();
  bof_register::register_process();
  pcl_register::register_process();
  Py_INCREF(Py_None);
  return Py_None;
}

PyObject *
register_datatypes(PyObject *self, PyObject *args)
{
  register_basic_datatypes();
  //dbrec3d_register::register_datatype();
  bvpl_kernels_register::register_datatype();
  bvpl_octree_register::register_datatype();
  boxm_register::register_datatype();
  boxm_algo_register::register_datatype();
  //bvxm_grid_register::register_datatype();
  //bvxm_algo_register::register_datatype();
  bof_register::register_datatype();
  pcl_register::register_datatype();
  Py_INCREF(Py_None);
  return Py_None;
}

PyMODINIT_FUNC
initdbrec3d_batch(void)
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
  
  dbrec3d_batch_methods[0]=reg_pro;
  dbrec3d_batch_methods[1]=reg_data;
  
  for (int i=0; i<METHOD_NUM; ++i) {
    dbrec3d_batch_methods[i+2]=batch_methods[i];
  }
  
  Py_InitModule("dbrec3d_batch", dbrec3d_batch_methods);
}

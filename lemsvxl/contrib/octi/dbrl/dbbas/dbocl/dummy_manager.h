#ifndef dummy_manager_h_
#define dummy_manager_h_

#include "bcl_cl.h"
#include "opencl_manager.h"

// T is the type of the data stored in the tree
template <class T>
class dummy_manager : public opencl_manager<dummy_manager<T> >
{
  
  cl_command_queue command_queue_;
  cl_program program_;
  cl_kernel kernel_;
    //private methods

 public:

  // static dummy_manager*  instance();

  dummy_manager()
    {}

  ~dummy_manager();


  };

#endif // boxm_ray_trace_manager_h_

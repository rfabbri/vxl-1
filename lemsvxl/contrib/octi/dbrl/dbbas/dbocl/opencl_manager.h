// This is dbrl/dbbas/dbocl/opencl_manager.h
#ifndef opencl_manager_h_
#define opencl_manager_h_
//:
// \file
// \brief
//  A parent class for singleton opencl managers
// \author J. Mundy
// \date November 13, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <vector>
#include <map>
#include <iostream>
#include <string>
#include "bcl_cl.h"
#include <cstddef>
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
#define GROUP_SIZE 64
#define VECTOR_SIZE 4


template <class T>
class opencl_manager
{
 protected:
  std::size_t number_devices_;
  std::size_t max_work_group_size_;   //!< Max allowed work-items in a group
  cl_uint max_dimensions_;           //!< Max group dimensions allowed
  std::size_t * max_work_item_sizes_; //!< Max work-items sizes in each dimension
  cl_ulong total_local_memory_;      //!< Max local memory allowed
  cl_ulong total_global_memory_;     //!< Max global memory allowed
  cl_uint max_compute_units_;        //!< Max compute units
  cl_uint vector_width_short_;       //!< Ideal short vector size
  cl_uint vector_width_float_;       //!< Ideal float vector size
  cl_uint max_clock_freq_;           //!< Maximum clock frequency
  cl_bool image_support_;            //!< image support
  cl_context context_;               //!< CL context
  cl_device_id *devices_;            //!< CL device list

 public:

  //: Destructor
  virtual ~opencl_manager();

  //: Use this instead of constructor
  static T* instance();

  //: Initialise the opencl environment
  void clear_cl();

  //: Initialise the opencl environment
  bool initialize_cl();

  //: Check for error returns
  int check_val(cl_int status, cl_int result, std::string message) {
    if (status != result) {
      std::cout << message << '\n';
      return 0;
    }
    return 1;
  }
  std::size_t group_size() const {return GROUP_SIZE;}
  cl_ulong total_local_memory() const {return total_local_memory_;}
  cl_context context() {return context_;}
  cl_device_id * devices() {return devices_;}

 protected:

  //: Constructor
  opencl_manager() : devices_(0),max_work_item_sizes_(0) {}

  static T* instance_;
};

#endif // opencl_manager_h_

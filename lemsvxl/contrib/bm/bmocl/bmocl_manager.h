// This is bm_ocl.h
#ifndef bm_ocl_manager_h_
#define bm_ocl_manager_h_
//:
// \file
// \brief  Alternative OpenCl manager
// \author Brandon Mayer bm@lems.brown.edu
// \date  November 1, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include<bocl/bocl_cl.h>
#include<bocl/bocl_cl_gl.h>
#include<bocl/bocl_utils.h>

#include<vbl/vbl_ref_count.h>
#include <vector>
#include <map>
#include <iostream>
#include <string>
//#include "bocl_cl.h"
#include"bmocl_cl.h"
#include <cstddef>
#if !defined(__APPLE__)
#include <malloc.h>
#endif
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
//#define GROUP_SIZE 64
#define VECTOR_SIZE 4


class bmocl_manager: public vbl_ref_count
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
    cl_device_id *devices_;            //!< CL device list
    std::size_t image2d_max_width_;       //!< Ideal float vector size
    std::size_t image2d_max_height_;       //!< Ideal float vector size
    cl_char extensions_supported_[1000];
    char platform_name[100];
public:

    std::size_t number_devices(){return number_devices_;}
    std::size_t max_work_group_size(){return max_work_group_size_;}
    cl_uint max_dimensions(){return max_dimensions_;}
    std::size_t* max_work_item_sizes(){return max_work_item_sizes_;}
    cl_ulong total_local_memory(){return total_local_memory_;}
    cl_ulong total_global_memory(){return total_global_memory_;}
    cl_uint vector_width_short(){return vector_width_short_;}
    cl_uint vector_width_float(){return vector_width_float_;}
    cl_uint max_clock_freq(){return max_clock_freq_;}
    //cl_bool image_support(){return image_support_;}
    //cl_device_id* devices(){return devices_;}
    std::size_t image2d_max_width(){return image2d_max_width_;}
    std::size_t image2d_max_height(){return image2d_max_height_;}
    cl_char* extensions_supported(){return extensions_supported_;}
    char* platformName(){return platform_name;}
	cl_device_id device(unsigned const& indx){ return devices_[indx]; }


    cl_context context_;               //!< CL context

    //: Destructor
    virtual ~bmocl_manager();

    //: Use this instead of constructor
    static bmocl_manager* instance();

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

    std::size_t group_size() const {return max_work_group_size_;}
    cl_ulong total_local_memory() const {return total_local_memory_;}
    cl_context context() {return context_;}
    cl_device_id * devices() {return devices_;}

    //: Allocate host memory for use with clCreateBuffer (aligned if necessary)
    void* allocate_host_mem(std::size_t size);
    bool load_kernel_source(std::string const& path);
    bool append_process_kernels(std::string const& path);
    bool write_program(std::string const& path);
    std::string program_source() const {return prog_;}

    //build kernel program: 
    int build_kernel_program(cl_program & program, std::string options);

    cl_bool image_support(){return image_support_;}

protected:

    //: Constructor
    bmocl_manager() : max_work_item_sizes_(0), devices_(0) {}

    static bmocl_manager* instance_;

    std::string prog_;
};

#endif //bm_ocl_manager_h_
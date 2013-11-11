#include <dbocl/opencl_manager.txx>
#include <dboxm/opt/open_cl/boxm_ray_trace_manager.h>
#include <dboxm/boxm_apm_traits.h>



typedef boxm_ray_trace_manager<float > boxm_ray_trace_manager_float;

OPENCL_MANAGER_INSTANTIATE(boxm_ray_trace_manager_float );

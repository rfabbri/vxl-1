// This is brl/bseg/bvpl/kernels/pro/processes/bvpl_create_edge2d_kernel_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for creating a planar edge kernel
// \author Isabel Restrepo
// \date June 23, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvpl/kernels/bvpl_edge2d_kernel_factory.h>


namespace bvpl_create_edge2d_kernel_process_globals
{
  //the specs of this kernel are specified as parameters
  constexpr unsigned n_inputs_ = 0;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_create_edge2d_kernel_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_create_edge2d_kernel_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvpl_kernel_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_create_edge2d_kernel_process(bprb_func_process& pro)
{
  using namespace bvpl_create_edge2d_kernel_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned int height = 5;
  pro.parameters()->get_value("height", height);
  unsigned int width = 5;
  pro.parameters()->get_value("width", width);
  float axis_x = 1.0f;
  pro.parameters()->get_value("axis_x", axis_x);
  float axis_y = 0.0f;
  pro.parameters()->get_value("axis_y", axis_y);
  float axis_z = 0.0f;
  pro.parameters()->get_value("axis_z", axis_z);
  float angle= 0.0f;
  pro.parameters()->get_value("angle", angle);

  vnl_float_3 axis(axis_x,axis_y, axis_z);

  //Create the factory
  bvpl_edge2d_kernel_factory factory(height, width);
  factory.set_rotation_axis(axis);
  factory.set_angle(angle);

  bvpl_kernel_sptr kernel_sptr = new bvpl_kernel(factory.create());
  kernel_sptr->print();
  pro.set_output_val<bvpl_kernel_sptr>(0, kernel_sptr);

  return true;
}

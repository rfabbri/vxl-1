//:
// \brief
// \file
// \author Isabel Restrepo
// \date 12-Aug-2010

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <dbrec3d/dbrec3d_models.h>
#include <dbrec3d/dbrec3d_pairwise_compositor.h>

#include <bsta/bsta_gauss_id3.h>

#include <boxm/algo/boxm_init_scene.h>

//:global variables
namespace dbrec3d_find_pairs_process_globals 
{
  const unsigned n_inputs_ = 15 ;
  const unsigned n_outputs_ = 1;
}


//:sets input and output types
bool dbrec3d_find_pairs_process_cons(bprb_func_process& pro)
{
  using namespace dbrec3d_find_pairs_process_globals ;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "int"; //primitive id
  input_types_[i++] = "int"; //primitive id

  //the following inputs shouldn't be here - kluge!
  input_types_[i++] = "double"; //mean 1-d
  input_types_[i++] = "double"; //mean 2-d
  input_types_[i++] = "double"; //mean 3-d
  input_types_[i++] = "double"; //var 1-d
  input_types_[i++] = "double"; //var 2-d
  input_types_[i++] = "double"; //var 3-d
  
  input_types_[i++] = "int"; //search-box xmin
  input_types_[i++] = "int"; //search-box ymin
  input_types_[i++] = "int"; //search-box zmin
  input_types_[i++] = "int"; //search-box xmax
  input_types_[i++] = "int"; //search-box ymax
  input_types_[i++] = "int"; //search-box zmax
  input_types_[i++] = "vcl_string"; //composition scene path
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "int"; //composition-context id
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//:the process
bool dbrec3d_find_pairs_process(bprb_func_process& pro)
{
  using namespace dbrec3d_find_pairs_process_globals;
  
  //get inputs
  unsigned i = 0;
  int part_id1 = pro.get_input<int>(i++);
  int part_id2 = pro.get_input<int>(i++);
  double mean1 =  pro.get_input<double>(i++);
  double mean2 =  pro.get_input<double>(i++);
  double mean3 =  pro.get_input<double>(i++);
  double var1 =  pro.get_input<double>(i++);
  double var2 =  pro.get_input<double>(i++);
  double var3 =  pro.get_input<double>(i++);
  int xmin = pro.get_input<int>(i++);
  int ymin = pro.get_input<int>(i++);
  int zmin = pro.get_input<int>(i++);
  int xmax = pro.get_input<int>(i++);
  int ymax = pro.get_input<int>(i++);
  int zmax = pro.get_input<int>(i++);
  vcl_string composition_scene_path = pro.get_input<vcl_string>(i++);
  
  vcl_cout << "mean: " << mean1 <<',' << mean2 <<',' << mean3 <<"\n"
  << "var: " << var1 << ',' << var2 << ',' << var3 << "\n"
  << "box-min: " << xmin <<',' << ymin <<',' << zmin << "\n"
  << "box-max: " << xmax <<',' << ymax<<',' << zmax << vcl_endl;
  
  //composition model
  bsta_gauss_id3 dist_model(vnl_vector_fixed<double,3>(mean1, mean2, mean3),vnl_vector_fixed<double,3>(var1,var2,var3));
  dbrec3d_pairwise_gaussian_model gauss_model(dist_model);
  
  //retrieve the primitive parts associated with each context.
  dbrec3d_context_sptr context1 = CONTEXT_MANAGER->get_context(part_id1);
  dbrec3d_context_sptr context2 = CONTEXT_MANAGER->get_context(part_id2);
  
  //unloads all active blocks
  context1->clean_memory();
  context2->clean_memory();
  
  dbrec3d_part_sptr part1 = context1->part();
  dbrec3d_part_sptr part2 = context2->part();
  
  //compositor  
  typedef dbrec3d_pairwise_compositor<dbrec3d_pairwise_gaussian_model> T_comp;
  T_comp compositor(gauss_model);
  vnl_float_3 aux_axis;
  vgl_rotation_3d<float> pair_geometry = compositor.compute_rotation(part1->axis(), part1->aux_axis(), part2->axis(), part2->aux_axis(), aux_axis);
  
  vcl_vector<int> children;
  children.push_back(part1->type_id());
  children.push_back(part2->type_id());
  float max_azimuthal; float min_azimuthal; float max_polar; float min_polar;
  
  //create a composite part
  int composition_id = PARTS_MANAGER->new_composite_part(compositor, pair_geometry.axis(),aux_axis,  pair_geometry.angle(), vnl_float_3(0,0,0), true, false, 
                                                         children, vnl_float_3(1, 2, 2),max_azimuthal, min_azimuthal, max_polar, min_polar);
  
  
  //the search box is defined assuming center cell location is at 0,0,0. this is in unit cell coordinates
  vgl_box_3d<double> search_box(xmin, ymin, zmin, xmax, ymax, zmax);
  
  //define the outupt scene
  //define the outupt scene
  unsigned max_level = 10;
  unsigned init_level = 1;
  boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > > *composition_scene = new boxm_scene<boct_tree<short, dbrec3d_pair_composite_instance > >(context1->lvcs(), context1->origin(), 
                                                                                                                                                           context1->block_dim(), context1->world_dim(),
                                                                                                                                                           context1->max_level(), init_level);

  composition_scene->set_paths(composition_scene_path, "composition_scene");
  composition_scene->set_appearance_model(DBREC3D_PAIR_COMPOSITE_INSTANCE);
  composition_scene->write_scene("/dbrec3d_composition_scene.xml");
  boxm_init_scene(*composition_scene);
  composition_scene->write_active_block();
  
  if(!composition_scene)
  {
    vcl_cout << "Null composition scene\n";
    return false;
  }
  
#ifdef DEBUG
  vcl_cout << " Composition Scene has:\n"
  << "Max level: " << composition_scene->max_level()
  << "Init Level: " << composition_scene->init_level() << vcl_endl;
  boxm_init_scene(*composition_scene);
  vcl_cout << " Composition Scene has:\n"
  << "Finest level: " <<composition_scene->finest_level()
  << "Max level: " << composition_scene->max_level()
  << "Init Level: " << composition_scene->init_level() << vcl_endl;
#endif
  
  //detect instances of compositions
  int cc_id =compositor.detect_instances<dbrec3d_part_instance>(composition_id, part1, part2, composition_scene, search_box);
 
  //store output
  pro.set_output_val<int>(0, cc_id);
  
  return true;
}
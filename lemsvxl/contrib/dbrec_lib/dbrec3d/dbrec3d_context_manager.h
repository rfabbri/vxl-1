// This is dbrec3d_context_manager.h
#ifndef dbrec3d_context_manager_h
#define dbrec3d_context_manager_h

//:
// \file
// \brief A class to register different part contexts in the database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  8-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_octree_context.h"
#include "dbrec3d_part_instance.h"
#include "dbrec3d_parts_manager.h"
#include "dbrec3d_models.h"

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>


#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_tuple.h>
#include <bvpl/bvpl_octree/sample/bvpl_octree_sample.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <boxm/boxm_scene.h>

// Forward declarations
class dbrec3d_context_manager;

typedef vbl_smart_ptr<dbrec3d_context_manager> dbrec3d_context_manager_sptr;

#define CONTEXT_MANAGER dbrec3d_context_manager::instance() 


//: At the time of creation this manager registers dbrec3d_octree_contexts,
//  this class should be templated for a more general design
//  The contexts are kept in a database (brdb) to be search by different fields
class dbrec3d_context_manager : public vbl_ref_count
{

public:
  //: Destructor
  ~dbrec3d_context_manager(){}
  
  //: Use instead of contructor
  static dbrec3d_context_manager_sptr instance()
  {
    if (!instance_){
      instance_ = new dbrec3d_context_manager();
      //instance_->initialize();
    }
    return instance_;
  }
  
  // Get the context with specified context id
  dbrec3d_context_sptr get_context(int context_id);
  
  // Get the context at specified level - assumes one context per level
  dbrec3d_context_sptr get_context_by_level(int hierachy_level);
  
  //: Returns all contexts registered in the database
  vcl_vector<dbrec3d_context_sptr> get_all_contexts();
  
  //: Number of registered contexts
  int size(){ return current_context_id_; }
  
  //: Create an octree context from a scene, and a part_id
  template<class T_instance>
  int register_empty_context( boxm_scene<boct_tree<short, T_instance > > *scene,
                              int part_id = -1)
  {
    //make sure the part_id exists in the database
    if (!PARTS_MANAGER->exists(part_id))
      return -1;
    
    return this->reg(*scene, true, part_id);
    
  }
  
  //: Create an octree context (and returns the context id) from a scene of kernels and a kernel- this function also registers the kernel as a part in the database
  template<class T_instance, class T_data, class T_model>
  int register_context( boxm_scene<boct_tree<short, T_data > > *scene_in,
                        T_model primitive_model,
                        bvpl_kernel_sptr kernel, boxm_scene<boct_tree<short, T_instance > > *parts_scene,
                        int part_id= -1)
  {
    //register the vector of kernels, retieve a map <kernel_id, part_id>
    part_id = PARTS_MANAGER->register_kernel(kernel);
    
    this->convert_response_to_parts_scene(scene_in, primitive_model, part_id, parts_scene, kernel->voxel_length());
    
    return this->reg(*parts_scene, true, part_id);
    
  }
  
  //: Create an octree context from a scene of kernels and a vector of kernels- this function also registers the kernels as parts in the database
  template<class T_instance, class T_data, class T_model>
  int register_context( boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > > *scene_in,
                        T_model primitive_model,
                        bvpl_kernel_vector_sptr kernel_vector,boxm_scene<boct_tree<short, T_instance > > *parts_scene,
                        vcl_map<int,int> &parts_id_map)
  {
    //register the vector of kernels, retieve a map <kernel_id, part_id>
    parts_id_map = PARTS_MANAGER->register_kernels(kernel_vector);
    
    vcl_map<int,int>::iterator map_it;
    for(map_it=parts_id_map.begin(); map_it!=parts_id_map.end(); map_it++)
    {
      vcl_cout << (*map_it).first << ',' << (*map_it).second << vcl_endl;
      
    }
    
    this->convert_response_to_parts_scene(scene_in, primitive_model,parts_id_map, parts_scene);
    
    for(map_it=parts_id_map.begin(); map_it!=parts_id_map.end(); map_it++)
    {
      vcl_cout << (*map_it).first << ',' << (*map_it).second << vcl_endl;
      
    }
    
    return this->reg(*parts_scene, false);
    
  }
  
  bool clear_contexts();
  
  // : write all contexts and parts associated with them to as an xml file
  bool xml_write(vcl_string name) const;
  
  // : Parse an xml file, registering all contexts and parts into the database
  bool xml_parse(const vcl_string &name);
  
  template<class T_instance>
  boxm_scene<boct_tree<short,float> >* response_scene(int context_id);
  

protected:

  //: Create an octree context from a scene of part instances and add it to the database
  template <class T_instance > 
  int reg(boxm_scene<boct_tree<short, T_instance > > &scene, bool exclusive, int part_id = -1, int level=0);
  
  template<class T_instance, class T_data, class T_model>
  void convert_response_to_parts_scene(boxm_scene<boct_tree<short, T_data > > *scene_in,
                                       T_model primitive_model,
                                       const int id, 
                                       boxm_scene<boct_tree<short, T_instance > > *scene_out,
                                       double voxel_length);
  
  template<class T_instance, class T_data, class T_model>
  void convert_response_to_parts_scene(boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > > *scene_in,
                                       T_model primitive_model,
                                       const vcl_map<int, int> &id_map, 
                                       boxm_scene<boct_tree<short, T_instance > > *scene_out);

protected:
  
  //: Constructor - use instance() instead
  dbrec3d_context_manager();
  
  //: Add a context to the database
  int add_context(const dbrec3d_context_sptr);
  
  //: Static instance of this manager
  static dbrec3d_context_manager_sptr instance_;  
  
  //: The name of database table where contexts are stored
  vcl_string context_table_name_;
  
  //: Vector to hold names for "columns" of context's table
  vcl_vector<vcl_string> names_;

  //: Variable to keep a count of contexts
  unsigned current_context_id_;

};


//: Create an octree context from a scene of part instances and add it to the database
template <class T_instance>
int dbrec3d_context_manager::reg( boxm_scene<boct_tree<short, T_instance > > &scene, bool exclusive, int part_id, int level)
{
  //sanity check
  if(level<0){
    vcl_cerr << " Error in dbrec3d_context_manager,  context at negative level" << vcl_cout;
    return NULL;
  }
  
  
  // Create context
  dbrec3d_part_sptr this_part;
  
  if(exclusive && part_id >= 0)
    this_part = PARTS_MANAGER->get_part(part_id);
  else 
    this_part = NULL;
    
  
  dbrec3d_context_sptr context_sptr = new dbrec3d_octree_context<T_instance>(scene, exclusive,this_part , level);
  
  unsigned db_id = brdb_database_manager::id();
  
  brdb_tuple_sptr new_tuple = new brdb_tuple(db_id, current_context_id_, level, context_sptr);
  
  if(DATABASE->add_tuple(context_table_name_, new_tuple))
    current_context_id_++;
  else {
    vcl_cerr << "in dbrec3d_context_manager :could not add context to database\n";
    return -1;
  }
  
  return (current_context_id_ - 1);
  
}


//: Converts the response octree obtained using bvpl_scene_vector_operator_process to a dbrec3d_octree_context
template <class T_instance, class T_data, class T_model>
void dbrec3d_context_manager::convert_response_to_parts_scene(boxm_scene<boct_tree<short, T_data > > *scene_in,
                                                              T_model primitive_model,
                                                              const int id,
                                                              boxm_scene<boct_tree<short, T_instance > > *scene_out,
                                                              double voxel_length)
{
  // Main Goal: To traverse the input octree conveting bvpl_octree_samples to dbrec3d_part_instances
  
  // Iterate through the scene
  short finest_level = scene_in->finest_level();
  
  boxm_block_iterator<boct_tree<short, T_data > > iter_in = scene_in->iterator();
  boxm_block_iterator<boct_tree<short, dbrec3d_part_instance > > iter_out = scene_out->iterator();
  iter_in.begin();
  iter_out.begin();
  
  for (; !iter_in.end(); iter_in++, iter_out++) {
    scene_in->load_block(iter_in.index());
    scene_out->load_block(iter_out.index());
    boct_tree<short, T_data >  *tree_in= (*iter_in)->get_tree();
    boct_tree<short, dbrec3d_part_instance >  *tree_out = tree_in ->template clone_to_type<dbrec3d_part_instance >();
    tree_out->init_cells(dbrec3d_part_instance());
    
    //at each tree, iterate through all cells, converting them to dbrec3d_primitives
    vcl_vector<boct_tree_cell<short,T_data >* > cells_in = tree_in->leaf_cells_at_level(finest_level);
    vcl_vector<boct_tree_cell<short,dbrec3d_part_instance >* > cells_out = tree_out->leaf_cells_at_level(finest_level);
    
    
    for (unsigned i = 0; i <  cells_in.size();i++)
    {
      
      boct_tree_cell<short,T_data >* cell_in = cells_in[i];
      float posterior  = primitive_model.compute_posterior(cell_in->data());
      vgl_point_3d<double> this_cell_origin = tree_in->global_origin(cell_in);
      vgl_point_3d<double> this_cell_center(this_cell_origin.x() + (voxel_length/2.0), this_cell_origin.y() + (voxel_length/2.0),this_cell_origin.z() + (voxel_length/2.0));
      dbrec3d_part_instance this_part(id, posterior, this_cell_center);
      cells_out[i]->set_data(this_part);
      
    }
    
    
    (*iter_out)->init_tree(tree_out);
    scene_out->write_active_block();
  }
  
}


//: Converts the response octree obtained using bvpl_scene_vector_operator_process to a dbrec3d_octree_context
template <class T_instance, class T_data, class T_model>
void dbrec3d_context_manager::convert_response_to_parts_scene(boxm_scene<boct_tree<short, bvpl_octree_sample<T_data> > > *scene_in,
                                                              T_model primitive_model,
                                                              const vcl_map<int, int> &id_map, 
                                                              boxm_scene<boct_tree<short, T_instance > > *scene_out)
{
  // Main Goal: To traverse the input octree conveting bvpl_octree_samples to dbrec3d_part_instances
  
  // Iterate through the scene
  boxm_block_iterator<boct_tree<short, bvpl_octree_sample<T_data> > > iter_in = scene_in->iterator();
  boxm_block_iterator<boct_tree<short, dbrec3d_part_instance > > iter_out = scene_out->iterator();
  vcl_map<int, int>::const_iterator map_it;
  iter_in.begin();
  iter_out.begin();
  
  for (; !iter_in.end(); iter_in++, iter_out++) {
    scene_in->load_block(iter_in.index());
    scene_out->load_block(iter_out.index());
    boct_tree<short, bvpl_octree_sample<T_data> >  *tree_in= (*iter_in)->get_tree();
    boct_tree<short, dbrec3d_part_instance >  *tree_out = tree_in ->template clone_to_type<dbrec3d_part_instance >();
    tree_out->init_cells(dbrec3d_part_instance());
    
    //at each tree, iterate through all cells, converting them to dbrec3d_primitives
    short level = 0;
    vcl_vector<boct_tree_cell<short,bvpl_octree_sample<T_data> >* > cells_in = tree_in->leaf_cells_at_level(level);
    vcl_vector<boct_tree_cell<short,dbrec3d_part_instance >* > cells_out = tree_out->leaf_cells_at_level(level);
    
    
    for (unsigned i = 0; i <  cells_in.size();i++)
    {
      
      boct_tree_cell<short,bvpl_octree_sample<T_data> >* cell_in = cells_in[i];
      bvpl_octree_sample<T_data> octree_sample = cell_in->data();
      T_data response = octree_sample.response_;
      vgl_point_3d<double> this_cell_origin = tree_in->global_origin(cell_in);
      int part_id = -1;
      map_it = id_map.find(octree_sample.id_);
      if(map_it!= id_map.end())
        part_id = map_it->second;
      float posterior = primitive_model.compute_posterior(octree_sample.response_);
      dbrec3d_part_instance this_part(part_id,posterior,this_cell_origin);
      cells_out[i]->set_data(this_part);
      
    }
    
    
    (*iter_out)->init_tree(tree_out);
    scene_out->write_active_block();
  }
  
}

template<class T_instance>
boxm_scene<boct_tree<short, float> >* dbrec3d_context_manager::response_scene(int context_id)
{
  
  typedef boct_tree<short,T_instance > tree_type;
  
  dbrec3d_context_sptr base_context= get_context(context_id);
  
  dbrec3d_octree_context<T_instance > *context = dynamic_cast<dbrec3d_octree_context<T_instance>* > (base_context.as_pointer());

  if(!(context))
    return NULL;
 
  
  boxm_scene<boct_tree<short, float> > *response_scene = new boxm_scene<boct_tree<short, float> >(context->parts_scene_.lvcs(), context->parts_scene_.origin(), context->parts_scene_.block_dim(), context->parts_scene_.world_dim());
  
  response_scene->set_paths(context->parts_scene_.path(), "weight_scene");
  response_scene->write_scene("/dbrec3d_weight_scene.xml");
  
  
  // Iterate through the scene
  boxm_block_iterator<boct_tree<short, float> > res_it = response_scene->iterator();
  boxm_block_iterator<tree_type > it= context->parts_scene_.iterator();
  res_it.begin();
  it.begin();
  
  for (; !it.end(); it++, res_it++) {
    response_scene->load_block(res_it.index());
    context->parts_scene_.load_block(it.index());
    tree_type  *tree = (*it)->get_tree();
    boct_tree<short,float> *res_tree= tree->template clone_to_type<float>();
    
    res_tree->init_cells(0.0f);
    
    //at each tree, iterate through all cells, converting them to dbrec3d_primitives
    short level = 0;
    vcl_vector<boct_tree_cell<short,float>* > res_cells = res_tree->leaf_cells_at_level(level);
    vcl_vector<boct_tree_cell<short,T_instance > * > cells = tree->leaf_cells_at_level(level);
    
    
    for (unsigned i = 0; i <  cells.size();i++){
      //T_instance pi = ;
      res_cells[i]->set_data(cells[i]->data().posterior());
    }
    
    
    (*res_it)->init_tree(res_tree);
    response_scene->write_active_block();
  }
  
  return response_scene;
}


/********* IO - NOT Implemented *******************/
void vsl_b_write(vsl_b_ostream & os, dbrec3d_context_manager const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec3d_context_manager &ph);
void vsl_b_read(vsl_b_istream& is, dbrec3d_context_manager* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec3d_context_manager* &ph);

#endif



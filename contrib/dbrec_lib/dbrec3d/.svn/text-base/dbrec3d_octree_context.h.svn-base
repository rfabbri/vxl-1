//This is dbrec3d_lib/dbrec3d/dbrec3d_part_context.h
#ifndef dbrec3d_octree_context_h_
#define dbrec3d_octree_context_h_
//:
// \file
// \brief classes to represent part contexts
//
//  Octree-based context to hold the extrinsic state of parts, e.g. their locations 
//  
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
// \verbatim
//  Modifications
//   Feb 23, 2010, Isabel Restrepo: dbrec3d_part_context is a modification of dbrec3d_part_context to 3D.
// \endverbatim
//
//

#include "dbrec3d_part_instance.h"
#include "dbrec3d_part.h"
#include "dbrec3d_primitive_kernel_part.h"
#include "dbrec3d_composite_part.h"
#include "dbrec3d_kernel_max_functor.h"

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_box_3d.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
//#include <bvpl/bvpl_octree/bvpl_scene_neighborhood_operator.h>
#include <bvpl/bvpl_octree/bvpl_scene_kernel_operator.h>

#include <bxml/bxml_document.h>
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>


// Forward declaration
class dbrec3d_context_manager;

//: Base class for contexts, it is used to save the contexts in the database
//  ISA: A more efficient implementation may save the contexts as different columns in the context relation in database
//       this may requiere to register types for that table. In that case we wouldn't need a base class
class dbrec3d_context : public vbl_ref_count
{
public:
  virtual void local_non_maxima_suppression()=0;
  virtual void clean_memory()=0;
  virtual dbrec3d_part_sptr part()=0;
  virtual bxml_data_sptr xml_element() const=0;
  //: The level of the hierarchy associated with this context
  virtual int hierarchy_level()=0;
  // Geographic properties // if this was trueli a genric context (not associated with a boxm_scene, some of teh properties should be moved to children)
  virtual bgeo_lvcs lvcs() const=0;
  virtual vgl_point_3d<double> origin() const=0;
  virtual vgl_vector_3d<double> block_dim() const =0;
  virtual unsigned max_level() const=0;
  virtual vgl_vector_3d<unsigned> world_dim() const =0;
  virtual double finest_cell_length()=0;
  virtual void save_to_disk()=0;
protected:
  virtual ~dbrec3d_context(){};
};

// Forward declaration
typedef vbl_smart_ptr<dbrec3d_context> dbrec3d_context_sptr;
dbrec3d_context_sptr dbrec3d_context_parse_xml_element(bxml_data_sptr d);

///: The context is basically an octree for fast access during location-based queries, (e.g. give me all the parts around this location)
template <class T_instance>
class dbrec3d_octree_context : public dbrec3d_context 
{
  
public:
  typedef boct_tree<short, T_instance > tree_type;
  
  //: A friend class that manages all contexts in the database
  friend class dbrec3d_context_manager;
  
  //: Request the part associated with this context --ISA: Some of the member variables should be moved to base class
  virtual dbrec3d_part_sptr part(){return part_;}
  
  
protected:
  
  //: Constructor - use CONTEXT_MANAGER to create contexts
  dbrec3d_octree_context() : parts_scene_(boxm_scene<tree_type>()), iterator_(NULL), part_(NULL), exclusive_(false), hierarchy_level_(-1){}
  
  //: Constructor from scene - use CONTEXT_MANAGER to create contexts
  dbrec3d_octree_context(boxm_scene<tree_type> &scene, bool exclusive, dbrec3d_part_sptr part = NULL, int level = -1)
  {
    parts_scene_ = scene;
    part_ = part;
    exclusive_ = exclusive;
    hierarchy_level_ = level;
    iterator_ = new boxm_cell_iterator<tree_type >(parts_scene_.iterator(), &boxm_scene<tree_type>::load_block);
  }
  
  //: Desctructor - should it be made protected?
  virtual ~dbrec3d_octree_context(){}

  
public:
  
  // Geographic properties
  virtual inline bgeo_lvcs lvcs() const { return parts_scene_.lvcs(); }
  virtual inline vgl_point_3d<double> origin() const { return parts_scene_.origin(); }
  virtual inline vgl_vector_3d<double> block_dim() const { return parts_scene_.block_dim(); }
  virtual inline unsigned max_level() const  { return parts_scene_.max_level(); }
  virtual inline vgl_vector_3d<unsigned> world_dim() const {return parts_scene_.world_dim(); }
  virtual inline double finest_cell_length() {return parts_scene_.finest_cell_length(); }
  virtual inline void save_to_disk() {parts_scene_.write_active_block(); } 

  //: The level of the hierarchy associated with this context
  int hierarchy_level(){ return hierarchy_level_; }
  
  //: Applies a particular bvpl_functor using the receptive field of the part associated with this context
  virtual void local_non_maxima_suppression()
  {
#if 0 // this is non-maxima suppression using a region as oposed to a kernel. Region finding seems too slow.
    vcl_cout << " Appling local non-maxima supression with radius: " << part_->radius() << vcl_endl;
    bvpl_scene_neighborhood_operator neighborhood_operator;
    dbrec3d_local_max_functor<T_instance> functor;
    vgl_box_3d<double> receptive_field(vgl_point_3d<double>(0.0,0.0,0.0), part_->radius()[0], part_->radius()[1], part_->radius()[2], vgl_box_3d<double>::centre);
    neighborhood_operator.local_non_maxima_suppression(parts_scene_,functor, receptive_field);
#endif
    dbrec3d_kernel_max_functor<T_instance> functor;
    bvpl_scene_kernel_operator oper;
    dbrec3d_primitive_kernel_part *part = dynamic_cast<dbrec3d_primitive_kernel_part*> (part_.as_pointer());
    oper.local_non_maxima_suppression(parts_scene_, functor, part->kernel());
    
    
    //clean memory
    parts_scene_.unload_active_blocks();
    
  }
  
  //: Clean in-memory blocks
  virtual void clean_memory() { parts_scene_.unload_active_blocks(); }
  
  //: Requests all part instances within a region
  void query(vgl_box_3d<double> const &search_box, vcl_vector<T_instance> &parts);

  vgl_box_3d<double> bounding_box() { return parts_scene_.get_world_bbox(); }
  
  //: Iterator to the first part instance
  T_instance first()
  {
    if (!iterator_)
      vcl_cerr<<"Error in dbrec3d_octree_context::first(): iterator_ is NULL" << vcl_endl;
    
    iterator_->begin();
    
    if (iterator_->end())
      return T_instance();
    
    return (*(*iterator_))->data();
  }
  
  //: Iterator to  part instances
  T_instance next()
  {
    if (!iterator_)
      vcl_cerr<<"Error in dbrec3d_octree_context::next(): iterator_ is NULL" << vcl_endl;
    
    ++(*iterator_);
    
    if (iterator_->end())
      return T_instance();
    
    return (*(*iterator_))->data();
  }
  
  bool end()
  {
    bool end = iterator_->end();
    
    if(end)
      parts_scene_.write_active_block();
    
    return end;
  }
  
  //: Add a part to the context
  //  Parts are added at the cell associated with the part location, parts are added such that there is only one part per cell
  void add_part_instance( T_instance const &pi);
  
  //: Write itself to as an xml element
  virtual bxml_data_sptr xml_element() const;
  
  //: XML read
  friend dbrec3d_context_sptr dbrec3d_context_parse_xml_element(bxml_data_sptr d);
  
protected:
 
  boxm_scene<tree_type> parts_scene_;
  
  boxm_cell_iterator<tree_type> *iterator_;
  
  //: Part associated with this context, if more than one part then this pointer should be NULL
  dbrec3d_part_sptr part_;
  
  //: Boolean indicating whether this context contains only one kind of part, if so set this boolean to true 
  bool exclusive_;
  
  //  //: The level of the hierarchy associated with this context
  int hierarchy_level_;
    
};

//: Requests all part instances within a region. The region must be specified in global-scene coordinates
template<class T_instance>
void dbrec3d_octree_context<T_instance>::query(const vgl_box_3d<double>  &search_box, vcl_vector<T_instance> &parts)
{
  // get all leaf cells in a region
  vcl_vector<boct_tree_cell<short, T_instance>* > cells;
  parts_scene_.leaves_in_region(search_box,cells);
  
  // get the parts  
  for(unsigned i = 0; i < cells.size(); i++)
  {
    T_instance part = cells[i]->data();
    if(part.type_id() < 0)
      continue;
    parts.push_back(cells[i]->data());
  }
  return;
  
}

//  Parts are added at the cell associated with the part location, parts are added such that there is only one part per cell
template<class T_instance>
void dbrec3d_octree_context<T_instance>::add_part_instance( T_instance const &pi)
{
  //find what block should contain this part
  vgl_point_3d<double> location = pi.location();
  vgl_point_3d<int> index;
  parts_scene_.get_block_index(location, index);
  
  //If block is not in memory, write current block and load 
  if (!(parts_scene_.active_block() == index))
  {
    parts_scene_.write_active_block();
    if(!parts_scene_.load_block(index))
    {
      vcl_cerr<< "Error on dbrec3d_octree_context::add_part_instance failed to load block" << vcl_endl;
      return;
    }
  }
   
  boct_tree<short, T_instance > *tree = parts_scene_.get_active_block()->get_tree();
 
  // locate root cell for new part instance
  boct_tree_cell<short, T_instance > *cell = tree->locate_point_global(location);
  
  //check if the cell contains a valid part. if part in file is not valid assign the current part to it. if there is a valid part in this cell, split so that there is one part per cell
  T_instance file_pi = cell->data();
  while(file_pi.type_id() >= 0)
  {
    T_instance invalid_pi;
    
    //split the cell and intitilize children with invalid parts
    //if minumum level has been reached, leave the part with larger porterior
    if(!cell->split(invalid_pi))
    {
      if(pi.posterior()>file_pi.posterior())
        cell->set_data(pi);
      
      return;
    };
    
    //place the part in file in correct child
    boct_tree_cell<short, T_instance > *file_pi_cell = tree->locate_point_global(file_pi.location());
    file_pi_cell->set_data(file_pi);
    
    //find the cell for new pi, if there is a part in it, recur
    cell =tree->locate_point_global(location);
    file_pi = cell->data();
  }
  
  cell->set_data(pi);  
  
}

//: Write itself to as an xml element
template<class T_instance>
bxml_data_sptr dbrec3d_octree_context<T_instance>::xml_element() const
{
  bxml_element *data = new bxml_element("dbrec3d_octree_context");
  data->append_text("\n");
  data->set_attribute("scene_file", this->parts_scene_.filename());
  data->set_attribute("part_id", part_->type_id());
  data->set_attribute("exclusive", exclusive_);
  data->set_attribute("hierarchy_level", hierarchy_level_);
  return data;
}

/************* Binary IO ***********************************/
//I/O

void vsl_b_write(vsl_b_ostream & os, dbrec3d_context const &c);


void vsl_b_write(vsl_b_ostream & os, dbrec3d_context const * &c);


void vsl_b_read(vsl_b_istream & is, dbrec3d_context &c);

void vsl_b_read(vsl_b_istream & is, dbrec3d_context *c);


vcl_ostream& operator << (vcl_ostream& os, const dbrec3d_context &c);

#endif  //_dbrec3d_part_context_h_

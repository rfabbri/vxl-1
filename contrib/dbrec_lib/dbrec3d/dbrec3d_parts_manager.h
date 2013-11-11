// This is dbrec3d_parts_manager.h
#ifndef dbrec3d_parts_manager_h
#define dbrec3d_parts_manager_h

//:
// \file
// \brief Singleton to register parts in database
// \author Isabel Restrepo mir@lems.brown.edu
// \date  10-Jun-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_part.h"
#include "dbrec3d_composite_part.h"

#include <vbl/vbl_ref_count.h>

#include <bvpl/kernels/bvpl_kernel.h>

#include <vcl_map.h>

#include <vsl/vsl_binary_io.h>

#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>

// Forward declarations
class dbrec3d_parts_manager;

typedef vbl_smart_ptr<dbrec3d_parts_manager> dbrec3d_parts_manager_sptr;

#define PARTS_MANAGER dbrec3d_parts_manager::instance() 

class dbrec3d_parts_manager : public vbl_ref_count
{
  
public:
  //: Destructor
  ~dbrec3d_parts_manager(){}
  
  //: Returns 'the' instance of this singleton class - use instead of contructor
  static dbrec3d_parts_manager_sptr instance()
  {
    if (!instance_){
      instance_ = new dbrec3d_parts_manager();
      //instance_->initialize();
    }
    return instance_;
  }
  
  //: Create parts from a vector of bvpl_kernels and return an map of <kernel_id , part_id>
  vcl_map<int,int> register_kernels( bvpl_kernel_vector_sptr kernels);
  
  //: Create a part from a bvpl_kernels and return its part id
  int register_kernel(bvpl_kernel_sptr kernel);
  
  //: Creates a new part and returns the part id
  template<class T_compositor>
  int new_composite_part(const T_compositor &compositor, vnl_float_3 axis, vnl_float_3 aux_axis, float angle, vnl_float_3 scale, bool symmetric, bool rot_invar, 
                         const vcl_vector<int>& children, vnl_float_3 radius, float max_azimuthal, float min_azimuthal, float max_polar, float min_polar); 
  
  //: Get part by type_id
  dbrec3d_part_sptr get_part(int part_id);
  
  //: Returns true if the part exists - this function could be more rigorous by checking if the id is in the database
  bool exists(int part_id) {return ((part_id >=0) && (part_id < current_part_id_)); } 
  
  //: Returns all parts registered in the database
  vcl_vector<dbrec3d_part_sptr> get_all_parts();
  
  //: The xml parser - needs to be a friend to register parts with pre-existing ids
  friend class dbrec3d_parse_xml_visitor;
  
protected:
  //: Contructor - Singleton class, use instance() instead
  dbrec3d_parts_manager();
  
  //: This class' instances
  static dbrec3d_parts_manager_sptr instance_;
  
  //: Current part id
  unsigned current_part_id_;
  
  //: Variable to hold name of database table where parts are stored
  vcl_string parts_table_name_;
  
  //: Vector to hold names for "columns" of parts' table
  vcl_vector<vcl_string> names_; 
  
private:
  //: A function to register a general part.
  // Be careful when using this function - user must take care of updatind current_part_id to a valid id!
  bool register_part(dbrec3d_part_sptr part);

  
};


//: Creates a new part and returns the part id
template<class T_compositor>
int dbrec3d_parts_manager::new_composite_part(const T_compositor &compositor, vnl_float_3 axis, vnl_float_3 aux_axis, float angle, vnl_float_3 scale, bool symmetric, bool rot_invar, 
                                              const vcl_vector<int>& children, vnl_float_3 radius,
                                              float max_azimuthal, float min_azimuthal, float max_polar, float min_polar) 
{
  //new database id
  unsigned db_id = brdb_database_manager::id();
  
  dbrec3d_part_sptr p = new dbrec3d_composite_part<T_compositor>(compositor, current_part_id_, axis, aux_axis, angle, scale, radius, symmetric, rot_invar, children); 
  
  
  //add part to the database
  brdb_tuple_sptr new_tuple = new brdb_tuple(db_id, current_part_id_, p);
  if(DATABASE->add_tuple(parts_table_name_, new_tuple)){
    current_part_id_++;
  }
  else 
    vcl_cerr << "in dbrec3d_parts_manager :could not add part to database\n";
  
 
  return current_part_id_ - 1;
    
}

/********* IO - NOT Implemented *******************/
void vsl_b_write(vsl_b_ostream & os, dbrec3d_parts_manager const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec3d_parts_manager &ph);
void vsl_b_read(vsl_b_istream& is, dbrec3d_parts_manager* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec3d_parts_manager* &ph);

#endif

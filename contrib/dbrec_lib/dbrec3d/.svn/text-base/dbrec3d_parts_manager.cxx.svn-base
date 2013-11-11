#include "dbrec3d_parts_manager.h"
//:
// \file
// \author Isabel Restrepo
// \date 10-Jun-2010

#include "dbrec3d_primitive_kernel_part.h"

dbrec3d_parts_manager_sptr dbrec3d_parts_manager::instance_ = NULL;

//: Constructor - Adds appropiate relations(tables) to the database
dbrec3d_parts_manager::dbrec3d_parts_manager():names_(3)
{  
  //initialize static variables
  current_part_id_ = 0;
  parts_table_name_ = "dbrec3d_parts";
  
  //create database table to hold the contexts
  if(!DATABASE->exists(parts_table_name_))
  {
    //define table
    vcl_vector<vcl_string> types(3);
    
    names_[0] = "db_id";
    names_[1] = "part_id";
    names_[2] = "part_sptr";
    //names_[2] = "name"; - This may need to be added
   
    
    types[0] = brdb_value_t<unsigned>::type();
    types[1] = brdb_value_t<unsigned>::type();
    types[2] = brdb_value_t<dbrec3d_part_sptr>::type();
    
    //add table
    brdb_relation_sptr parts_table= new brdb_relation(names_,types); 
    DATABASE->add_relation(parts_table_name_, parts_table);
  }
}

//: Create a part from a bvpl_kernels and return its part id
int dbrec3d_parts_manager::register_kernel( bvpl_kernel_sptr kernel)
{
  
  //TO DO : the following values are temporary
  vcl_cerr << "Warning: in dbrec3d_parts_manager::register_kernels the values of class_prior, symmetric, rot_invar are hardcoded, this should be changed \n"; 
  float class_prior = 1.0f;
  bool symmetric = true;
  bool rot_invar = false;
  
  
  unsigned db_id = brdb_database_manager::id();
  
  dbrec3d_part_sptr p = new dbrec3d_primitive_kernel_part(current_part_id_, class_prior, kernel, symmetric, rot_invar);
  
  //add part to the database
  brdb_tuple_sptr new_tuple = new brdb_tuple(db_id, current_part_id_, p);
  if(DATABASE->add_tuple(parts_table_name_, new_tuple)){
    current_part_id_++;
  }
  else 
    vcl_cerr << "in dbrec3d_parts_manager :could not add part to database\n";
   
  return current_part_id_ - 1;
  
}


//: Create parts from a vector of bvpl_kernels and return an map of <kernel_id , part_id>
vcl_map<int,int> 
dbrec3d_parts_manager::register_kernels( bvpl_kernel_vector_sptr kernels)
{
  
  //TO DO : the following values are temporary
  vcl_cerr << "Warning: in dbrec3d_parts_manager::register_kernels the values of class_prior, symmetric, rot_invar are hardcoded, this should be changed \n"; 
  float class_prior = 1.0f;
  bool symmetric = true;
  bool rot_invar = false;
  
  vcl_map<int,int> id_map;
  
  //itarate trhough vector of kernels, create parts and add them to the database
  for (unsigned i = 0; i< kernels->kernels_.size(); i++) {
    
    unsigned db_id = brdb_database_manager::id();
    
    dbrec3d_part_sptr p = new dbrec3d_primitive_kernel_part(current_part_id_, class_prior, kernels->kernels_[i], symmetric, rot_invar);
    
    //add part to the database
    brdb_tuple_sptr new_tuple = new brdb_tuple(db_id, current_part_id_, p);
    if(DATABASE->add_tuple(parts_table_name_, new_tuple)){
      id_map.insert(vcl_pair<int, int>(i, current_part_id_));
      current_part_id_++;
    }
    else 
      vcl_cerr << "in dbrec3d_parts_manager :could not add part to database\n";
    
  }
 
  return id_map;
  
}

//: Get part by type_id
dbrec3d_part_sptr dbrec3d_parts_manager::get_part(int part_id)
{
  //sanity check
  if(part_id<0){
    vcl_cerr << " Warning in dbrec3d_parts_manager, requesting part with negative id\n";
    return NULL;
  }
  
  //query to get the data by hierarchy level
  brdb_query_aptr Q = brdb_query_comp_new(names_[1], brdb_query::EQ, unsigned(part_id));
  
  brdb_selection_sptr selec = DATABASE->select(parts_table_name_, Q);
  if (selec->size()!=1) {
    vcl_cout << "in dbrec3d_parts_manager :number selections is not 1\n";
    return NULL;
  }
  //the result from a selection is a tuple/row, now get the desired value within the row
  brdb_value_sptr value;
  if (!selec->get_value(vcl_string("part_sptr"), value)) {
    vcl_cout << "in dbrec3d_context_manager didn't get value\n";
    return false;
  }
  if (!value) {
    vcl_cout << "indbrec3d_context_manager null value\n";
    return false;
  }
  
  dbrec3d_part_sptr part_sptr = value->val<dbrec3d_part_sptr>();
  
  return part_sptr;
  
}

//: Returns all parts registered in the database
vcl_vector<dbrec3d_part_sptr> dbrec3d_parts_manager::get_all_parts()
{
  // query to get all contexts
  brdb_query_aptr Q = brdb_query_comp_new(names_[1], brdb_query::ALL, -1);
  
  brdb_selection_sptr selec = DATABASE->select(parts_table_name_, Q);
  if (!selec->empty()) 
  {
    vcl_vector<dbrec3d_part_sptr> all_parts(selec->size());
    
    for(unsigned int i=0; i<selec->size(); i++)
    {
      dbrec3d_part_sptr part_sptr;
      selec->get(names_[2], i, part_sptr);
      all_parts[i] = part_sptr;
    }
    return all_parts;
  }  
  
  vcl_cerr << "in dbrec3d_context_manager :no selections - returning empty vector\n";
  return vcl_vector<dbrec3d_part_sptr>(0);
}


//: A function to register a general part.
// Be careful when using this function - user must take care of updatind current_part_id to a valid id!
bool dbrec3d_parts_manager::register_part(dbrec3d_part_sptr part)
{
  //add part to the database
  if(part->type_id() < 0 )
    return false;
  
  brdb_tuple_sptr new_tuple = new brdb_tuple(brdb_database_manager::id(), unsigned(part->type_id()), part);
  if(DATABASE->add_tuple(parts_table_name_, new_tuple)){
    if(part->type_id()>current_part_id_){
      current_part_id_ = part->type_id()+1;
      return true;
    }
  }
  else{ 
    vcl_cerr << "in dbrec3d_parts_manager :could not add part to database\n";
    return false;
  }
}

/********* IO - NOT Implemented *******************/
void vsl_b_write(vsl_b_ostream & os, dbrec3d_parts_manager const &pm){}
void vsl_b_read(vsl_b_istream & is, dbrec3d_parts_manager &pm){}

void vsl_b_write(vsl_b_ostream& os, const dbrec3d_parts_manager* &pm){}
void vsl_b_read(vsl_b_istream& is, dbrec3d_parts_manager* pm){}

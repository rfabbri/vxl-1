#include "dbrec3d_context_manager.h"
//:
// \file
// \author Isabel Restrepo
// \date 9-Jun-2010


#include <brdb/brdb_database_manager.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_selection.h>
#include <brdb/brdb_query.h>

#include <bxml/bxml_write.h>

dbrec3d_context_manager_sptr dbrec3d_context_manager::instance_ = NULL;


//: Constructor - Adds appropiate relations(tables) to the database
dbrec3d_context_manager::dbrec3d_context_manager() : names_(4)
{  
  //initialize static variables
  current_context_id_ = 0;
  context_table_name_ = "dbrec3d_contexts";
  
  //create database table to hold the contexts
   if(!DATABASE->exists(context_table_name_))
  {
    //define table
    vcl_vector<vcl_string> types(4);
    
    names_[0] = "db_id";
    names_[1] = "context_id";
    names_[2] = "hierachy_level";
    names_[3] = "context_sptr";
    
    
    types[0] = brdb_value_t<unsigned>::type();
    types[1] = brdb_value_t<unsigned>::type();
    types[2] = brdb_value_t<int>::type();
    types[3] = brdb_value_t<dbrec3d_context_sptr>::type();
    
    //add table
    brdb_relation_sptr context_table= new brdb_relation(names_,types); 
    DATABASE->add_relation(context_table_name_, context_table);
  }
}

//: Query the context by context_id
dbrec3d_context_sptr 
dbrec3d_context_manager::get_context(int context_id)
{
  //sanity check
  if(context_id<0){
    vcl_cerr << " Error in dbrec3d_context_manager, requesting context with negative id" << vcl_cout;
    return NULL;
  }
  
  //query to get the data by hierarchy level
  brdb_query_aptr Q = brdb_query_comp_new(names_[1], brdb_query::EQ, unsigned(context_id));
  
  brdb_selection_sptr selec = DATABASE->select(context_table_name_, Q);
  if (selec->size()!=1) {
    vcl_cout << "in dbrec3d_context_manager :number selections is not 1\n";
    return NULL;
  }
  brdb_value_sptr value;
  if (!selec->get_value(vcl_string("context_sptr"), value)) {
    vcl_cout << "in dbrec3d_context_manager didn't get value\n";
    return false;
  }
  if (!value) {
    vcl_cout << "indbrec3d_context_manager null value\n";
    return false;
  }
  
  dbrec3d_context_sptr context_sptr = value->val<dbrec3d_context_sptr>();
  
  return context_sptr;
}


//: Query the context at a particular level
dbrec3d_context_sptr 
dbrec3d_context_manager::get_context_by_level(int hierachy_level)
{
  //sanity check
  if(hierachy_level<0){
    vcl_cerr << " Error in dbrec3d_context_manager, requesting context at negative level" << vcl_cout;
    return NULL;
  }
  
  //query to get the data by hierarchy level
  brdb_query_aptr Q = brdb_query_comp_new(names_[2], brdb_query::EQ, unsigned(hierachy_level));
  
  brdb_selection_sptr selec = DATABASE->select(context_table_name_, Q);
  if (selec->size()!=1) {
    vcl_cout << "in dbrec3d_context_manager :number selections is not 1\n";
    return NULL;
  }
  brdb_value_sptr value;
  if (!selec->get_value(vcl_string("value"), value)) {
    vcl_cout << "in dbrec3d_context_manager didn't get value\n";
    return false;
  }
  if (!value) {
    vcl_cout << "indbrec3d_context_manager null value\n";
    return false;
  }
  
  dbrec3d_context_sptr context_sptr = value->val<dbrec3d_context_sptr>();
  
  return context_sptr;
}




vcl_vector<dbrec3d_context_sptr> 
dbrec3d_context_manager::get_all_contexts()
{
 
  // query to get all contexts
  brdb_query_aptr Q = brdb_query_comp_new(names_[1], brdb_query::ALL, -1);
  
  brdb_selection_sptr selec = DATABASE->select(context_table_name_, Q);
  if (!selec->empty()) 
  {
    vcl_vector<dbrec3d_context_sptr> all_contexts(selec->size());
    
    for(unsigned int i=0; i<selec->size(); i++)
    {
      dbrec3d_context_sptr context_sptr;
      selec->get(names_[2], i, context_sptr);
      all_contexts[i] = context_sptr;
    }
    return all_contexts;
  }  
  
  vcl_cerr << "in dbrec3d_context_manager :no selections - returning empty vector\n";
  return vcl_vector<dbrec3d_context_sptr>(0);
}
   

int dbrec3d_context_manager::add_context(const dbrec3d_context_sptr context_sptr)
{
  unsigned db_id = brdb_database_manager::id();
  brdb_tuple_sptr new_tuple = new brdb_tuple(db_id, current_context_id_, context_sptr->hierarchy_level(), context_sptr);
  if(DATABASE->add_tuple(context_table_name_, new_tuple))
    current_context_id_++;
  else {
    vcl_cerr << "in dbrec3d_context_manager :could not add context to database\n";
    return -1;
  }

  return (current_context_id_ - 1);
 
}

bool dbrec3d_context_manager::clear_contexts()
{
  if (DATABASE->clear_relation(context_table_name_)){
    current_context_id_ = 0;
    return true;
  }
  vcl_cerr << "in dbrec3d_context_manager :could not clear contexts\n";
  return false;

}

bool dbrec3d_context_manager::xml_write(vcl_string name) const
{
  bxml_document doc;
  bxml_element * root = new bxml_element("contexts");
  doc.set_root_element(root);
  root->append_text("\n");
  
  // query to get all contexts
  brdb_query_aptr Q = brdb_query_comp_new(names_[1], brdb_query::ALL, -1);
  
  brdb_selection_sptr selec = DATABASE->select(context_table_name_, Q);
  if (!selec->empty()) 
  {
    for(unsigned int i=0; i<selec->size(); i++)
    {
      dbrec3d_context_sptr context_sptr;
      selec->get(names_[3], i, context_sptr);
      root->append_data(context_sptr->xml_element());
      root->append_text("\n");
    }
    
    vcl_ofstream os(name.c_str());
    bxml_write(os,doc);
    return true;
  }  
  
  return false;
}


bool dbrec3d_context_manager::xml_parse(const vcl_string &name)
{
  vcl_ifstream is(name.c_str());
  if (!is) return false;
  bxml_document doc = bxml_read(is);
  bxml_element query("contexts");
  bxml_data_sptr contexts_data = bxml_find_by_name(doc.root_element(), query);
  bxml_element* contexts_elm = dynamic_cast<bxml_element *> (contexts_data.as_pointer());
  if (!contexts_elm) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name dbrec3d_contexts\n";
    return false;
  }
  
  for(bxml_element::const_data_iterator d_it = contexts_elm->data_begin(); d_it!=contexts_elm->data_end(); d_it++)
  {
    dbrec3d_context_sptr context= dbrec3d_context_parse_xml_element(*d_it);
    if(context.as_pointer())
      add_context(context);
  }
  return true;
  
}


/********* IO - NOT Implemented *******************/
void vsl_b_write(vsl_b_ostream & os, dbrec3d_context_manager const &cm){}
void vsl_b_read(vsl_b_istream & is, dbrec3d_context_manager &cm){}

void vsl_b_write(vsl_b_ostream& os, const dbrec3d_context_manager* &cm){}
void vsl_b_read(vsl_b_istream& is, dbrec3d_context_manager* cm){}

    

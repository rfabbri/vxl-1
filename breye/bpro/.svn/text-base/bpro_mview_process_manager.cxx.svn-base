// This is brl/bpro/bpro_mview_process_manager.cxx

//:
// \file

#include "bpro_mview_process_manager.h"

#include <vcl_cassert.h>
#include <vcl_utility.h>
#include <vul/vul_arg.h>

#include <brdb/brdb_database_manager.h>
#include <bpro/bpro_mview_dbutils.h>
#include <brdb/brdb_relation.h>
#include <brdb/brdb_query.h>
#include <brdb/brdb_tuple.h>

#include <bpro/bpro_storage_registry.h>
#include <bpro/bpro_storage.h>
#include <bpro/bpro_process.h>

//: Constructor
bpro_mview_process_manager::bpro_mview_process_manager()
{
  brdb_database_manager::instance()->clear();
  bpro_mview_dbutils::create_new_database();
  output_view_ids_.push_back(0);
}

//: Destructor
bpro_mview_process_manager::~bpro_mview_process_manager()
{
  process_queue.clear();
}

//: Run a process on the current frame
bool bpro_mview_process_manager::
run_process( const bpro_process_sptr& process,
             vcl_set<bpro_storage_sptr>* modified)
{
  bool to_return = false;
  if(!process)
    return false;
  process->clear_input();
  process->clear_output();
  unsigned n_input_views = static_cast<unsigned>(process->input_frames());
  unsigned n_output_views = static_cast<unsigned>(process->output_frames());
  if(n_input_views>input_view_ids_.size())
    return false;
  if(n_output_views>output_view_ids_.size())
    return false;
  // SET INPUTS ////////////////////////////////////////////

  vcl_vector< vcl_string > input_type_list = process->get_input_type();
  vcl_vector< vcl_string > input_names = process->input_names();

  for( unsigned int i = 0 ;
      i < input_names.size();
      i++ ) {
    for(unsigned int index = 0; index<n_input_views; index++)
    {
      bpro_storage_sptr input_storage_sptr; 
      bpro_mview_dbutils::get_view_data_by_name_at( input_names[i],
                                                     input_view_ids_[index] );
      assert(input_type_list[i] == input_storage_sptr->type());
      process->add_input(input_storage_sptr, input_view_ids_[index]);
    }
  }

  //////////////////////////////////////////////////////////
  // number of output frames
  int num_outputs = process->output_frames();


  // EXECUTE ///////////////////////////////////////////////
  to_return = process->execute();
  //////////////////////////////////////////////////////////

  // GET OUTPUTS ///////////////////////////////////////////

  if( to_return ) {
    for( int index = 0 ; index < num_outputs;  index++ )
      {
        vcl_vector < bpro_storage_sptr > output_storage_classes =
          process->get_output(index);
        vcl_vector< vcl_string > output_names = process->output_names();

      // Any outputs beyond those specified with output_names() are 
      // added to the modified set but not added to the repository
      // it is assumed that these are preexisting but modified data
      for( unsigned int i = 0 ;
          i < output_storage_classes.size();
          i++ )
      {
        if(!output_storage_classes[i])
          continue;
        if(i < output_names.size()){
          if(output_storage_classes[i]->name()=="defaultname")
            //assign the chosen names for the output classes
            output_storage_classes[i]->set_name( output_names[i]);
          to_return = to_return&&
            bpro_mview_dbutils::store_data_at( output_storage_classes[i], 
                                                output_view_ids_[index]);
          if (modified)
            modified->insert(output_storage_classes[i]);
        }
      }
    }
  }

  //Amir: The processes should not keep the input and output storage classes
  //      once the process has been executed. This is preventing those storage
  //      classes from being destructed.
  process->clear_input();
  process->clear_output();

  // RETURN VALUE //////////////////////////////////////////
  return to_return;
  //////////////////////////////////////////////////////////
}



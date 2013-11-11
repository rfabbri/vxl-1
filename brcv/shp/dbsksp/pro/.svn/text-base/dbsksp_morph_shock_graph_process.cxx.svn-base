// This is dbsksp/pro/dbsksp_morph_shock_graph_process.cxx

//:
// \file

#include "dbsksp_morph_shock_graph_process.h"

#include <bpro1/bpro1_parameters.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage_sptr.h>
#include <dbsksp/algo/dbsksp_morph_shock_graph_same_topology.h>




// ----------------------------------------------------------------------------
//: Constructor
dbsksp_morph_shock_graph_process::
dbsksp_morph_shock_graph_process()
{
  this->morph_type_descriptions_.push_back("same topology");
  if( 
    !parameters()->add("Morph type: " , "-morph_type" , 
      this->morph_type_descriptions_, 0) ||
    !parameters()->add("Number of sequences : " , "-num_sequences", 
      (int) 1) ||
    !parameters()->add("Number of output frames per sequence (>=2): " , "-num_frames", 
      (int) 5)
  )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

// ----------------------------------------------------------------------------
//: Destructor
dbsksp_morph_shock_graph_process::
~dbsksp_morph_shock_graph_process()
{
  
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_morph_shock_graph_process::
clone() const
{
  return new dbsksp_morph_shock_graph_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_morph_shock_graph_process::
name()
{ 
  return "Morph Shock Graph"; 
}


// ----------------------------------------------------------------------------
void dbsksp_morph_shock_graph_process::
clear_output(int resize)
{
  this->set_num_frames(0);
  bpro1_process::clear_output(resize);
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_morph_shock_graph_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  to_return.push_back("dbsksp_shock");
  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_morph_shock_graph_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbsksp_shock" );  
  return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_morph_shock_graph_process::
input_frames()
{
  int num_sequences = 0;
  parameters()->get_value( "-num_sequences" , num_sequences);

  return num_sequences;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_morph_shock_graph_process::
output_frames()
{
  return this->num_frames();
}

//: Execute this process
bool dbsksp_morph_shock_graph_process::
execute()
{
  unsigned int morph_type = 0;
  parameters()->get_value( "-morph_type" , morph_type);
  vcl_string mode = this->morph_type_descriptions_[morph_type];


  int num_sequences = 0;
  parameters()->get_value( "-num_sequences" , num_sequences);
  

  int num_frames = 0;
  parameters()->get_value( "-num_frames" , num_frames);
  this->set_num_frames(num_frames * num_sequences);

  // do the obvious
  if (num_frames < 2)
    return false;

  // handle each morphing mode separately
  if (mode == "same topology")
  {
    if ( this->input_data_.size() != num_sequences ){
      vcl_cerr << "ERROR: executing " __FILE__ "not exactly " 
        << num_sequences << " input frame.\n";
      return false;
    }

    vcl_vector<vcl_vector<bpro1_storage_sptr > > frame_storage_list;

    // Need to go backward on the sequence order
    for (int sequence = (num_sequences-1); sequence >= 0; --sequence)
    {
      dbsksp_shock_storage_sptr shock_storage_0;
      dbsksp_shock_storage_sptr shock_storage_1;
      shock_storage_0.vertical_cast(input_data_[sequence][0]);
      shock_storage_1.vertical_cast(input_data_[sequence][1]);

      dbsksp_shock_graph_sptr source_graph = shock_storage_0->shock_graph();
      dbsksp_shock_graph_sptr target_graph = shock_storage_1->shock_graph();

      dbsksp_morph_shock_graph_same_topology morpher;
      morpher.set_source_graph(source_graph);
      morpher.set_target_graph(target_graph);

      bool success = morpher.compute_correspondence();
      if (! success)
      {
        vcl_cout << "ERROR: could not establish correspondence between two shock graphs.\n";
        return false;
      }
      else
      {
        vcl_cout << "Computing correspondence succeeded.\n";
        //morpher.print(vcl_cout);

        morpher.morph();

        // create the output storage class
        // going backward
        for (int frame=0; frame < num_frames; ++frame)
        {
          // new shock to put in the storage
          double t = double(frame) / (num_frames-1);
          dbsksp_shock_graph_sptr graph_t = morpher.get_intermediate_graph(t);

          // new shock storage
          dbsksp_shock_storage_sptr output_shock = dbsksp_shock_storage_new();
          output_shock->set_shock_graph(graph_t);

          // put the shock storage in a frame
          vcl_vector<bpro1_storage_sptr > frame_storage;
          frame_storage.push_back(output_shock);

          // save the frame to the frame list
          frame_storage_list.push_back(frame_storage);
        }
      }
    }

    // Save the frame list to the repository
    for (vcl_vector<vcl_vector<bpro1_storage_sptr > >::reverse_iterator itr =
      frame_storage_list.rbegin(); itr != frame_storage_list.rend(); ++itr)
    {
      output_data_.push_back(*itr);
    }
   
  }
  return true;
}

bool dbsksp_morph_shock_graph_process::
finish()
{
  return true;
}






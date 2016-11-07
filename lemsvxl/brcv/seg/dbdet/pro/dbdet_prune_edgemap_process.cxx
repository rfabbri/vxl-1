// This is brcv/seg/dbdet/pro/dbdet_prune_edgemap_process.cxx

//:
// \file

#include "dbdet_prune_edgemap_process.h"
#include <vcl_vector.h>
#include <vcl_string.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

//: Constructor
dbdet_prune_edgemap_process::dbdet_prune_edgemap_process()
{
  if( !parameters()->add( "Edge Strength Threshold", "-strength_thresh",  4.0 ) ||
      !parameters()->add( "d2f Threshold", "-d2f thresh",  1.0 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
dbdet_prune_edgemap_process::~dbdet_prune_edgemap_process()
{
}


//: Clone the process
bpro1_process*
dbdet_prune_edgemap_process::clone() const
{
  return new dbdet_prune_edgemap_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_prune_edgemap_process::name()
{
  return "Prune Edgemap";
}


//: Return the number of input frame for this process
int
dbdet_prune_edgemap_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_prune_edgemap_process::output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_prune_edgemap_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_prune_edgemap_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Execute the process
bool
dbdet_prune_edgemap_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbdet_prune_edgemap_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  //get the input storage class
  dbdet_edgemap_storage_sptr input_edgemap;
  input_edgemap.vertical_cast(input_data_[0][0]);
  dbdet_edgemap_sptr old_edge_map = input_edgemap->get_edgemap();

  //get the parameters
  double strength_thresh, d2f_thresh;
  parameters()->get_value( "-strength_thresh", strength_thresh);
  parameters()->get_value( "-d2f_thresh", d2f_thresh);

  //create a new edgemap
  dbdet_edgemap_sptr new_edge_map = new dbdet_edgemap(old_edge_map->width(), old_edge_map->height());

  for (unsigned x = 0; x < old_edge_map->width(); x++){
    for (unsigned y = 0; y < old_edge_map->height(); y++)
    {
      vcl_vector<dbdet_edgel*> cur_cell = old_edge_map->cell(x,y);

      for (unsigned i=0; i<cur_cell.size(); i++){
        if (cur_cell[i]->strength > strength_thresh &&
            vcl_fabs(cur_cell[i]->deriv) > d2f_thresh)
        {
          dbdet_edgel* e = new dbdet_edgel(*cur_cell[i]);
          new_edge_map->insert(e);
        }
      }
    }
  }

  // create the output storage class
  dbdet_edgemap_storage_sptr output_edgemap = dbdet_edgemap_storage_new();
  output_edgemap->set_edgemap(new_edge_map);
  output_data_[0].push_back(output_edgemap);

  vcl_cout << "#edgels remaining = " << new_edge_map->num_edgels() << vcl_endl;

  return true;
}

bool
dbdet_prune_edgemap_process::finish()
{
  return true;
}


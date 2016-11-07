// This is pro/correlate_edge_maps_process.cxx

//:
// \file

#include "correlate_edge_maps_process.h"
#include <vcl_vector.h>
#include <vcl_string.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>

#include <edge_det/pro/edge_corr_storage_sptr.h>
#include <edge_det/pro/edge_corr_storage.h>

#include <edge_det/csa/match_edge_maps.h>

//: Constructor
correlate_edge_maps_process::correlate_edge_maps_process()
{
  if( !parameters()->add( "Radius of Sloppiness", "-radius",  3.0 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

//: Destructor
correlate_edge_maps_process::~correlate_edge_maps_process()
{
}


//: Clone the process
bpro1_process*
correlate_edge_maps_process::clone() const
{
  return new correlate_edge_maps_process(*this);
}


//: Return the name of this process
vcl_string
correlate_edge_maps_process::name()
{
  return "Correlate Edgemaps";
}


//: Return the number of input frame for this process
int
correlate_edge_maps_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
correlate_edge_maps_process::output_frames()
{
  return 1;
}

//: Provide a vector of required input types
vcl_vector< vcl_string > correlate_edge_maps_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edge_map" );
  to_return.push_back( "edge_map" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > correlate_edge_maps_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "edge_map_corr" );
  return to_return;
}


//: Execute the process
bool
correlate_edge_maps_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In correlate_edge_maps_process::execute() - not exactly one"
             << " input images \n";
    return false;
  }
  clear_output();

  //get the input storage classes
  dbdet_edgemap_storage_sptr edgemap1, edgemap2;
  edgemap1.vertical_cast(input_data_[0][0]);
  edgemap2.vertical_cast(input_data_[0][1]);

  dbdet_edgemap_sptr edge_map1 = edgemap1->get_edgemap();
  dbdet_edgemap_sptr edge_map2 = edgemap2->get_edgemap();

  // create the output storage class
  edge_corr_storage_sptr output_edgecorr = edge_corr_storage_new();
  output_edgecorr->edge_map1_ = edge_map1;
  output_edgecorr->edge_map2_ = edge_map2;
  output_edgecorr->match1.resize(edge_map1->num_edgels(), -1);
  output_edgecorr->match2.resize(edge_map2->num_edgels(), -1);

  //get the parameters
  double radius;
  parameters()->get_value( "-radius", radius);
  
  
  static const double maxDistDefault = 0.0075;
  static const double outlierCostDefault = 100;
  double idiag = vcl_sqrt( (double) edge_map1->height()*edge_map1->height() +
                          edge_map1->width()*edge_map1->width());

  
  //radius of sloppiness can be a function of the image size
  //radius = maxDistDefault*idiag;
  //double outlier_cost = outlierCostDefault*maxDistDefault*idiag;
  double outlier_cost = outlierCostDefault*radius;

  //call the edge correlation computation
  double cost = matchEdgeMaps( edge_map1, edge_map2, 
                               radius, outlier_cost,
                               output_edgecorr->match1, output_edgecorr->match2);

  //output the edge correlation storage class
  output_data_[0].push_back(output_edgecorr);

  return true;
}

bool
correlate_edge_maps_process::finish()
{
  return true;
}


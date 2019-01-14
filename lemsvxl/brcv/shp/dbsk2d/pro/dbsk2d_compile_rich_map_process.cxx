// This is brcv/shp/dbsk2d/pro/dbsk2d_compile_rich_map_process.cxx

//:
// \file

#include "dbsk2d_compile_rich_map_process.h"
#include "dbsk2d_shock_storage.h"
#include "dbsk2d_shock_storage_sptr.h"

#include <dbsk2d/dbsk2d_rich_map.h>
#include <dbsk2d/dbsk2d_rich_map_sptr.h>

#include <vsol/vsol_box_2d_sptr.h>

dbsk2d_compile_rich_map_process::dbsk2d_compile_rich_map_process()
{
}

dbsk2d_compile_rich_map_process::~dbsk2d_compile_rich_map_process()
{
}


//: Clone the process
bpro1_process* dbsk2d_compile_rich_map_process::clone() const
{
  return new dbsk2d_compile_rich_map_process(*this);
}


std::string dbsk2d_compile_rich_map_process::name()
{
  return "Compile Rich Map";
}

std::vector< std::string > dbsk2d_compile_rich_map_process::get_input_type()
{
  std::vector< std::string > to_return;
  to_return.push_back( "shock" );
  return to_return;
}

std::vector< std::string > dbsk2d_compile_rich_map_process::get_output_type()
{
  std::vector< std::string > to_return;
  return to_return;
}

int
dbsk2d_compile_rich_map_process::input_frames()
{
  return 1;
}

int
dbsk2d_compile_rich_map_process::output_frames()
{
  return 0;
}

bool
dbsk2d_compile_rich_map_process::execute()
{
  // get input storage class
  dbsk2d_shock_storage_sptr shock;
  shock.vertical_cast(input_data_[0][0]);

  //compile the rich map from the shock graph
  if (shock->get_shock_graph()){
    vsol_box_2d_sptr bounding_box = shock->get_boundary()->get_bounding_box();

    int width  = (int)(std::floor(bounding_box->get_max_x()) - 
                       std::floor(bounding_box->get_min_x()) + 
                       10);

    int height = (int)(std::floor(bounding_box->get_max_y()) - 
                       std::floor(bounding_box->get_min_y()) + 
                       10);

    shock->set_rich_map(new dbsk2d_rich_map(shock->get_shock_graph(), 
                                            width, height,
                                            (int)(std::floor(bounding_box->get_min_x())-5.0), 
                                            (int)(std::floor(bounding_box->get_min_y())-5.0)));
  }

  return true;
}

bool
dbsk2d_compile_rich_map_process::finish()
{
  return true;
}



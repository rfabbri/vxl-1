// This is brcv/shp/dbskfg/dbskfg_shock_node.cxx

//:
// \file

#include <dbskfg/dbskfg_shock_node.h>

//: Constructor
dbskfg_shock_node::dbskfg_shock_node(unsigned int id,
                                     vgl_point_2d<double> location,
                                     double radius) 
    : dbskfg_composite_node(location,dbskfg_composite_node::SHOCK_NODE,id),
      shock_node_type_(dbsk2d_shock_node::A3),
      radius_(radius)
{
}

//: Destructor
dbskfg_shock_node::~dbskfg_shock_node() 
{
  
}

void dbskfg_shock_node::print(vcl_ostream& os)
{
    this->dbskfg_composite_node::print(os);
    os<<"Radius: "<<radius_<<vcl_endl;
}


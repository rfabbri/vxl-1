// This is brcv/shp/dbskfg/dbskfg_composite_node.cxx

//:
// \file

#include <dbskfg/dbskfg_composite_node.h>
#include <vcl_string.h>


dbskfg_composite_node::dbskfg_composite_node
( 

    vgl_point_2d<double> location, 
    dbskfg_composite_node::NodeType node_type,
    unsigned int id,
    bool virtual_flag

):dbgrl_vertex<dbskfg_composite_link>(),
  id_(id),
  node_type_(node_type),
  pt_(location),
  virtual_node_(virtual_flag),
  composite_degree_(0)
{

}

void dbskfg_composite_node::print(vcl_ostream& os)
{
    vcl_string type = (node_type_ == dbskfg_composite_node::SHOCK_NODE)
        ?"SHOCK_NODE":"CONTOUR_NODE";

    os<<"Node Id: "<<id_<<" Type "<< type << " Degree "<<composite_degree_
      <<vcl_endl;
    os<<"Location: "<<pt_<<vcl_endl;
    os<<vcl_endl;

}

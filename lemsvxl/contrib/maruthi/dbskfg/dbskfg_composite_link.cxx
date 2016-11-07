// This is brcv/shp/dbskfg/dbskfg_composite_link.cxx

//:
// \file

#include <dbskfg/dbskfg_composite_link.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <vcl_string.h>

dbskfg_composite_link::dbskfg_composite_link
(dbskfg_composite_node_sptr source,
 dbskfg_composite_node_sptr target,
 dbskfg_composite_link::LinkType link_type,
 unsigned int id
)
    :dbgrl_edge<dbskfg_composite_node>(source,target),
     link_type_(link_type),
     id_(id)
{
}


void dbskfg_composite_link::print(vcl_ostream& os)
{
    vcl_string type;
    type=(link_type_ == CONTOUR_LINK)?"CONTOUR":"SHOCK";
    os<<"Link Type: "<< type <<" Id: "<<id_<<vcl_endl;

}

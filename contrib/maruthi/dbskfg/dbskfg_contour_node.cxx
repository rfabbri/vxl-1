// This is brcv/shp/dbskfg/dbskfg_contour_node.cxx

//:
// \file

#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_link.h>

//: Constructor
dbskfg_contour_node::dbskfg_contour_node(unsigned int id, 
                                         vgl_point_2d<double> location,
                                         int contour_id) 
    : dbskfg_composite_node(location,dbskfg_composite_node::CONTOUR_NODE,id),
      contour_node_type_(dbskfg_contour_node::I_JUNCTION),
      contour_id_(contour_id),
      tangent_(0.0)
{

}

//: Destructor
dbskfg_contour_node::~dbskfg_contour_node() 
{
  
}


void dbskfg_contour_node::shock_links_affected(dbskfg_shock_link* shock_link)
{
    bool add_link=true;
    vcl_vector<dbskfg_shock_link*>::iterator it;
    for ( it = shocks_affected_.begin() ; it != shocks_affected_.end(); ++it)
    {
        if ( (*it)->id() == shock_link->id() )
        {
            add_link=false;
            break;
            
        }

    }
    
    if ( add_link)
    {
        shocks_affected_.push_back(shock_link);
    }

        
}


void dbskfg_contour_node::delete_shock(unsigned int id)
{
    vcl_vector<dbskfg_shock_link*>::iterator it;
    for ( it = shocks_affected_.begin() ; it != shocks_affected_.end(); ++it)
    {
        if ( (*it)->id() == id )
        {
            (*it)=0;
            shocks_affected_.erase(it);
            break;
            
        }

    }
    

        
}

dbskfg_composite_link_sptr dbskfg_contour_node::degenerate_shock()
{


    dbskfg_composite_node::edge_iterator srit;
    for ( srit = this->out_edges_begin() ; srit != this->out_edges_end() 
              ; ++srit)
    {
        if ( (*srit)->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            return (*srit);
        }


    }

    return 0;


}
void dbskfg_contour_node::print(vcl_ostream& os)
{
    this->dbskfg_composite_node::print(os);
    os<<"Original Contour Id: "<<contour_id_<<vcl_endl;
    os<<"Tangent: "
      << this->get_tangent()<<vcl_endl;
    os<<"Shocks Affected: ";
    for (unsigned int i=0; i < shocks_affected_.size(); ++i)
    {
        os<<shocks_affected_[i]->id()<<" ";

    }
    os<<vcl_endl;
    os<<vcl_endl;


}

// This is brcv/shp/dbskfg/dbskfg_contour_link.cxx

//:
// \file

#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_shock_link.h>

//: Constructor
dbskfg_contour_link::dbskfg_contour_link(dbskfg_composite_node_sptr source,
                                         dbskfg_composite_node_sptr target,
                                         unsigned int id) 
    : dbskfg_composite_link(source,target,
                            dbskfg_composite_link::CONTOUR_LINK,id),
      contour_id_(0)

{
}

//: Destructor
dbskfg_contour_link::~dbskfg_contour_link() 
{
    shocks_affected_.clear();
    
}

void dbskfg_contour_link::delete_shock(unsigned int id)
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

void dbskfg_contour_link::shock_links_affected(dbskfg_shock_link* shock_link)
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

void dbskfg_contour_link::print(vcl_ostream& os)
{
    this->dbskfg_composite_link::print(os);
    os<<"Source Id: "
            << this->source()->id() 
            << " Target id: "
            << this->target()->id()
            << vcl_endl;
    dbskfg_contour_node* cnode=dynamic_cast<dbskfg_contour_node*>
        (&(*this->source()));
    os<<"Original Contour Id: "<<contour_id_<<vcl_endl;
    os<<"Shocks Affected: ";
    for (unsigned int i=0; i < shocks_affected_.size(); ++i)
    {
        os<<shocks_affected_[i]->id()<<" ";

    }
    os<<vcl_endl;
    os<<vcl_endl;


}

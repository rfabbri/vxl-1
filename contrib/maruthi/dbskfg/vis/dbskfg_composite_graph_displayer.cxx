// This is brcv/shpdbskfg/vis/dbskfg_composite_graph_displayer.cxx

//:
// \file


#include <dbskfg/vis/dbskfg_composite_graph_displayer.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/vis/dbskfg_composite_graph_tableau.h>
#include <dbskfg/dbskfg_composite_graph.h>

//------------------------------------------------------------------------------
//: Create a tableau if the storage object is of type dbskfg_composite_graph
vgui_tableau_sptr
dbskfg_composite_graph_displayer::make_tableau( bpro1_storage_sptr storage) 
const
{
    // Return 0 tableau if the types don't match
    if( storage->type() != this->type())
    {
        return 0;
    }
    
    // Cast the storage object into a dbskfg_skgraph storage object
    dbskfg_composite_graph_storage_sptr composite_graph_storage;
    composite_graph_storage.vertical_cast(storage);

    // create a new dbskfg_composite_graph tableau
    dbskfg_composite_graph_tableau_sptr composite_graph_tab = 
        dbskfg_composite_graph_tableau_new();
    if ( this->update_composite_graph_tableau(composite_graph_tab,
                                              composite_graph_storage))
    {

        return composite_graph_tab;

    }
    
    return 0;

}


//------------------------------------------------------------------------------
//: Update a tableau with a the storage object if both are of type 
// composite graph
bool dbskfg_composite_graph_displayer::
update_tableau(vgui_tableau_sptr tab, const bpro1_storage_sptr& storage ) 
const
{
    // Return a NULL tableau if the types don't match
    if( storage->type() != this->type() )
    {
        return false;
    }

    // Cast to composite graph tableau
    dbskfg_composite_graph_tableau_sptr composite_graph_tab = 
        dynamic_cast<dbskfg_composite_graph_tableau* >(tab.ptr());
    
    if(!composite_graph_tab)
    {
        return false;
    }

    // Cast the storage object into an storage object
    dbskfg_composite_graph_storage_sptr composite_graph_storage;
    composite_graph_storage.vertical_cast(storage);

    return this->update_composite_graph_tableau
        (composite_graph_tab, composite_graph_storage);  
}
 

//------------------------------------------------------------------------------
//: Perform the update once types are known
bool dbskfg_composite_graph_displayer::
update_composite_graph_tableau(
    dbskfg_composite_graph_tableau_sptr composite_graph_tab, 
    const dbskfg_composite_graph_storage_sptr& composite_graph_storage) const
{

    if (!composite_graph_tab || !composite_graph_storage)
    {
        return false;
    }

    // Clear all soviews
    composite_graph_tab->clear();

    // Set graphs
    composite_graph_tab->
        set_composite_graph(composite_graph_storage->get_composite_graph());
    composite_graph_tab->
        set_rag_graph(composite_graph_storage->get_rag_graph());
    composite_graph_tab->
        set_transforms(composite_graph_storage->get_transforms());

    // Add the soview elements
    composite_graph_tab->add_composite_graph_elms_to_display();

    // Add transforms
    composite_graph_tab->add_transforms_to_display();
    return true;

}



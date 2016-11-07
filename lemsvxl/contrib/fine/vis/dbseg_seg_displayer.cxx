// This is contrib/fine/gui/dbseg_seg_displayer.cxx

#include "dbseg_seg_displayer.h"

//: Create a tableau if the storage object is of type vsol2D
vgui_tableau_sptr
dbseg_seg_displayer::make_tableau( bpro1_storage_sptr storage) const {
    // Return a NULL tableau if the types don't match
    if( storage->type() != this->type() )
        return NULL;

    // Cast the storage object into an vsol2D storage object
    dbseg_seg_storage_sptr seg_storage;
    seg_storage.vertical_cast(storage);
    // Create a new vsol2D tableau
    dbseg_seg_tableau_sptr seg_tab;
    seg_tab = dbseg_seg_tableau_new(static_cast<dbseg_seg_object<vxl_byte>* >( seg_storage->get_object() ));

    return seg_tab;
}
  



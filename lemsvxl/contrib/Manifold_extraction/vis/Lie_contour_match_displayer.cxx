#include "Lie_contour_match_displayer.h"
#include <manifold_extraction/pro/Lie_contour_match_storage.h>
#include "Lie_contour_match_tableau.h"

//: Create a tableau if the storage object is of type curvematch
vgui_tableau_sptr
Lie_contour_match_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return NULL;

  // Cast the storage object into an curvematch storage object
  Lie_contour_match_storage_sptr curvematch_storage;
  curvematch_storage.vertical_cast(storage);

  // Create a new image tableau
  Lie_contour_match_tableau_sptr curvematch_tab = Lie_contour_match_tableau_new();
  curvematch_tab->set_curvematch( curvematch_storage->get_curvematch() );
  curvematch_tab->set_curve_cor( curvematch_storage->get_curve_cor() );

  return curvematch_tab;
}


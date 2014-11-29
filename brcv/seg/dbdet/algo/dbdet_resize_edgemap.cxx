// This is brcv/seg/dbdet/algo/dbdet_resize_edgemap.cxx

//:
// \file


#include "dbdet_resize_edgemap.h"

#include <dbdet/edge/dbdet_edgemap.h>

#include <vcl_string.h>



//: Re-scale an edgemap
bool dbdet_resize_edgemap(const dbdet_edgemap_sptr& edgemap, double scale_up_factor)
{
  if (scale_up_factor <= 0)
    return false;

  if (scale_up_factor == 1)
    return true;

  // new size of the cell array
  int new_cols = vnl_math::ceil( edgemap->edge_cells.cols() * scale_up_factor);
  int new_rows = vnl_math::ceil( edgemap->edge_cells.rows() * scale_up_factor);

  // duplicate the edgel list
  vcl_vector<dbdet_edgel* > edgels = edgemap->edgels;

  // clear the existing edgemap
  edgemap->edge_cells.clear();
  edgemap->edgels.clear();

  // resize
  edgemap->edge_cells.resize(new_rows, new_cols);

  // re-insert the edgels back to the edgemap
  for (vcl_vector<dbdet_edgel* >::iterator it = edgels.begin(); it != edgels.end(); ++it)
  {
    dbdet_edgel* edgel = *it;
    edgel->pt.set(scale_up_factor * edgel->pt.x(), scale_up_factor * edgel->pt.y());
    edgemap->insert(edgel);
  }
  return true;
}


//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_bucket_3d.h
//  MingChing Chang
//  Apr 01, 2007.

#ifndef dbmsh3d_vis_bucket_3d_h_
#define dbmsh3d_vis_bucket_3d_h_

#include <dbmsh3d/algo/dbmsh3d_pt_set_bucket.h>

#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/SbColor.h>
#include <Inventor/nodes/SoSeparator.h>

class SoGroup;
class SoSeparator;

SoSeparator* draw_bktstr (const dbmsh3d_pt_bktstr* BktStruct, 
                          const float& width,
                          const SbColor& color);

SoSeparator* draw_bktstr_reduced (const dbmsh3d_pt_bktstr* BktStruct, 
                                  const double& reduc_dist,
                                  const float& width, const SbColor& color);

#endif


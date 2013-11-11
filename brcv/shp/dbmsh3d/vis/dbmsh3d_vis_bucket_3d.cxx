//: This is lemsvxlsrc/brcv/shp/dbmsh3d/vis/dbmsh3d_vis_bucket_3d.cxx
//  MingChing Chang
//  May 03, 2005.

#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgl/dbgl_dist.h>
#include <dbmsh3d/vis/dbmsh3d_vis_bucket_3d.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>

#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoSeparator.h>


SoSeparator* draw_bktstr (const dbmsh3d_pt_bktstr* BktStruct, 
                          const float& width, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Loop through each box and draw it
  vgl_box_3d<double> box;
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    const dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      const dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        const dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());

        root->addChild (draw_box (box, width, color));
      }
    }
  }

  return root;
}

SoSeparator* draw_bktstr_reduced (const dbmsh3d_pt_bktstr* BktStruct, 
                                  const double& reduc_dist,
                                  const float& width, const SbColor& color)
{
  SoSeparator* root = new SoSeparator;

  //Loop through each box and draw it
  vgl_box_3d<double> box, reduc_box;
  for (unsigned int s=0; s<BktStruct->slice_list().size(); s++) {
    const dbmsh3d_pt_slice* S = BktStruct->slice_list(s);
    box.set_min_z (S->min_z());
    box.set_max_z (S->max_z());
    for (unsigned int r=0; r<S->row_list().size(); r++) {
      const dbmsh3d_pt_row* R = S->row_list(r);
      box.set_min_y (R->min_y());
      box.set_max_y (R->max_y());
      for (unsigned int b=0; b<R->bucket_list().size(); b++) {
        const dbmsh3d_pt_bucket* B = R->bucket_list(b);
        box.set_min_x (B->min_x());
        box.set_max_x (B->max_x());

        reduc_box = dbgl_reduce_box (box, reduc_dist);

        root->addChild (draw_box (reduc_box, width, color));
      }
    }
  }

  return root;
}



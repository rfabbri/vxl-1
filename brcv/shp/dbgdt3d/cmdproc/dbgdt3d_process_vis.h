//: This is lemsvxlsrc/brcv/shp/dbgdt3d/vis/dbgdt3d_process_vis.h
//  Creation: Dec 24, 2005   Ming-Ching Chang

#ifndef dbgdt3d_process_vis_h_
#define dbgdt3d_process_vis_h_

#include <dbgdt3d/pro/dbgdt3d_process.h>
#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>

#include <Inventor/SbColor.h>

class SoSeparator;
class SoSwitch;

//###### Visualization parameters ######

class dbgdt3d_pro_vis : public dbgdt3d_pro, public dbmsh3d_vis_base
{
public:
public:
  //###### Constructor and Destructor ######
  dbgdt3d_pro_vis() : dbgdt3d_pro(), dbmsh3d_vis_base() {
  }

  virtual ~dbgdt3d_pro_vis () 
  {
  }

  //###### Visualization Functions ######
  virtual SoSeparator* vis_ptset (const int colorcode = COLOR_NOT_SPECIFIED, 
                                  const bool b_draw_idv = false);
  virtual SoSeparator* vis_ptset_idv ();
  virtual SoSeparator* vis_ptset_color ();

  virtual SoSeparator* vis_mesh (const bool b_draw_individual, 
                                 const bool b_shape_hints = false,
                                 const int colorcode = 0,
                                 const bool b_draw_isolated_pts = true);
  virtual SoSeparator* vis_mesh_edges (const int colorcode, const float width = 1.0f, 
                                       const bool draw_idv = false);
  virtual SoSeparator* vis_mesh_pts_edges (const int colorcode = COLOR_NOT_SPECIFIED,
                                           const bool draw_idv = false,
                                           const float pt_size = 3.0f);

  virtual SoSeparator* vis_mesh_bndcurve (const int colorcode, const float width);
  virtual SoSeparator* vis_mesh_color ();
  virtual SoSeparator* vis_mesh_options (int option, const bool draw_idv = false, const bool showid = false);
  virtual SoSeparator* vis_mesh_anim (const int nF_batch);

  //###### Visualization Functions in dbgdt3d_pro_vis ######
};


#endif




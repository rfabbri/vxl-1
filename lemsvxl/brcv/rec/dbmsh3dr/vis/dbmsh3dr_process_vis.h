//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_process_vis.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

#ifndef dbmsh3dr_process_vis_h_
#define dbmsh3dr_process_vis_h_

#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>
#include <dbmsh3dr/pro/dbmsh3dr_process.h>
#include <Inventor/SbColor.h>

class SoSeparator;

class dbmsh3dr_pro_vis : public dbmsh3dr_pro
{
public:
  //====== Constructor/Destructor ======
  dbmsh3dr_pro_vis (dbmsh3d_pro_vis* meshpv, dbmsh3d_pro_vis* meshpv2);
  ~dbmsh3dr_pro_vis ();

  //====== Data access functions ======
  //: Virtual function to access the 2 dbmsh3d_pro_vis objects
  dbmsh3d_pro_vis* pv0();
  dbmsh3d_pro_vis* pv1();
  dbmsh3d_pro_vis* pv (const unsigned int i); 

public:
  //: ====== Processing with Visualization ======
  
  //option 0: random color, 1: silver color
  SoSeparator* vis_meshes (const int vis_option = 0);
  
  SoSeparator* vis_conn ();

  SoSeparator* vis_oripts ();
  SoSeparator* vis_oripts2 ();

  SoSeparator* vis_min_dist_pt_color_SoPoint (const float size,
                                              const bool show_line);
  SoSeparator* vis_min_dist_pt_color_SoCube (const float size);

  SoSeparator* vis_matching_vector ();
};

#endif

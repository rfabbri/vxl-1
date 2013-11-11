//: This is lemsvxlsrc/brcv/shp/dbsk3d/vis/dbsk3d_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#ifndef dbsk3d_cmdproc_h_
#define dbsk3d_cmdproc_h_

#include <dbmsh3d/cmdproc/dbmsh3d_process_vis.h>
#include <dbsk3d/vis/dbsk3d_process_vis.h>
#include <Inventor/nodes/SoSeparator.h>

//#####################################################################
// dbsk3d command-line process execution.

bool dbsk3d_check_cmdproc (dbsk3d_pro_vis* bndshkpv, int argc, char **argv);

void dbsk3d_setup_provis_parameters (dbsk3d_pro_vis* bndshkpv);

SoSeparator* dbsk3d_cmdproc_execute (dbsk3d_pro_vis* bndshkpv);

#endif




//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_cmdproc.h
//  Creation: Feb 27, 2007   Ming-Ching Chang

#ifndef dbmsh3dr_cmdproc_h_
#define dbmsh3dr_cmdproc_h_

#include <dbmsh3dr/vis/dbmsh3dr_process_vis.h>
#include <Inventor/nodes/SoSeparator.h>

//#####################################################################
// dbmsh3dr command-line process execution.

bool dbmsh3dr_check_cmdproc (dbmsh3dr_pro_vis* meshpvr, int argc, char **argv);

void dbmsh3dr_setup_provis_parameters (dbmsh3dr_pro_vis* meshpvr);

SoSeparator* dbmsh3dr_cmdproc_execute (dbmsh3dr_pro_vis* meshpvr);

#endif




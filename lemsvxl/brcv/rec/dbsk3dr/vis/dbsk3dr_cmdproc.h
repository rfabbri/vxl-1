//: This is lemsvxlsrc/brcv/rec/dbsk3dr/vis/dbsk3dr_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#ifndef dbsk3dr_cmdproc_h_
#define dbsk3dr_cmdproc_h_

#include <dbsk3dr/vis/dbsk3dr_process_vis.h>
#include <Inventor/nodes/SoSeparator.h>

//#####################################################################
// dbsk3d command-line process execution.

bool dbsk3dr_check_cmdproc (dbsk3dr_pro_vis* spvr, int argc, char **argv);

void dbsk3dr_setup_provis_parameters (dbsk3dr_pro_vis* spvr);

SoSeparator* dbsk3dr_cmdproc_execute (dbsk3dr_pro_vis* spvr);



#endif




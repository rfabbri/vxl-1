//: This is gdt/gdt_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#ifndef dbgdt3d_cmdproc_h_
#define dbgdt3d_cmdproc_h_

#include <dbgdt3d/cmdproc/dbgdt3d_process_vis.h>
#include <Inventor/nodes/SoSeparator.h>

//#####################################################################
// dbmsh3d command-line process execution.

bool dbgdt3d_check_cmdproc (dbgdt3d_pro_vis* gpv, int argc, char **argv);

void dbgdt3d_setup_provis_parameters (dbgdt3d_pro_vis* gpv);

SoSeparator* dbgdt3d_cmdproc_execute (dbgdt3d_pro_vis* gpv);

#endif




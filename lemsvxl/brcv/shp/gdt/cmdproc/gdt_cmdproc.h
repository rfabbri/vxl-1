//: This is gdt/gdt_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#ifndef gdt_cmdproc_h_
#define gdt_cmdproc_h_

#include <dbmsh3d/vis/dbmsh3d_process_vis.h>
#include <Inventor/nodes/SoSeparator.h>

//#####################################################################
// dbmsh3d command-line process execution.

bool gdt_check_cmdproc (gdt_pro_vis* mpv0, int argc, char **argv);

void gdt_setup_provis_parameters_0 (gdt_pro_vis* mpv0);

SoSeparator* gdt_cmdproc_execute (gdt_pro_vis* mpv0);

#endif




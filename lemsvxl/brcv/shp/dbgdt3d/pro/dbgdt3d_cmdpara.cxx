//: This is lemsvxlsrc/brcv/shp/dbgdt3d/pro/dbgdt3d_cmdpara.cxx
//  Creation: Dec 14, 2007   Ming-Ching Chang

#include <dbgdt3d/pro/dbgdt3d_cmdpara.h>

//#####################################################################
//: dbmsh3d_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default  

//Shortest path (graph) on mesh.
vul_arg<int>          dbgdt3d_cmd_shp ("-shp", "Compute (graph) shortest path on mesh.", 0);

//Geodesic distance transform options
vul_arg<int>          dbgdt3d_cmd_gdt ("-gdt", "Compute exact geodesic distance transform\n\
         1: Surazhsky & Kirsanov's interval based method,\n\
         2: Our face-based method, 3: Our face-based + shock propagation,\n\
         4: Our simutaneous discrete wavefront and shock propagation,\n\
         -f (prefix) -s (source index) -n (iter)", 0);

//#####################################################################





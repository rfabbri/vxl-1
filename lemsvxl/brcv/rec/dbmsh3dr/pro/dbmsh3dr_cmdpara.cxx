//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/vis/dbmsh3dr_cmdproc.cxx
//  Creation: Feb 27, 2007   Ming-Ching Chang

#include <dbmsh3dr/pro/dbmsh3dr_cmdpara.h>

//#####################################################################
//: dbmsh3dr_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default  

//List File
vul_arg<char const*>  dbmsh3dr_cmd_listfile ("-list", "Input list file (.txt)");

vul_arg<int>          dbmsh3dr_cmd_scalem ("-scalem", "Perform scaling to match a specifed object", 0);

//Mesh Matching
vul_arg<int>          dbmsh3dr_cmd_icp ("-icp", "1: pt-pt, 2: pt-plane, 3: RGRL ICP. -n for iter.", 0);
vul_arg<float>        dbmsh3dr_cmd_icpcv ("-icpcv", "ICP convergence (RMS of error of H 3x3 to I).", 0.0001f);
vul_arg<float>        dbmsh3dr_cmd_icpthr ("-icpthr", "ICP dist. threshold ratio (th = r * avg_samp_dist)", 5.0f);
vul_arg<float>        dbmsh3dr_cmd_dthr ("-dthr", "Dist. estim. threshold ratio (th = r * avg_samp_dist)", 2.0f);
vul_arg<int>          dbmsh3dr_cmd_verr ("-verr", "Visualize the error between two data sets", 0);
vul_arg<int>          dbmsh3dr_cmd_debug_verr ("-dbverr", "Debug the point-mesh distance computation", 0);

vul_arg<int>          dbmsh3dr_cmd_rsnv ("-rsnv", "Reduce surface normal variance", 0);





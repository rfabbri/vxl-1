//: This is lemsvxlsrc/brcv/rec/dbsk3dr/vis/dbsk3dr_cmdproc.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <dbsk3dr/pro/dbsk3dr_pro_cmdpara.h>

//#####################################################################
//: dbmsh3d_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default  

//Shock Matching
vul_arg<int>          dbsk3dr_cmd_fuse ("-fuse", "Smooth. + Fusing Scan Data.", 0);
vul_arg<int>          dbsk3dr_cmd_smrd ("-smrd", "Smooth. + Mesh. + Register. + Dist. Estim.", 0);
vul_arg<int>          dbsk3dr_cmd_smre ("-smre", "Smooth. + Mesh. + Register. + Estimat. Error", 0);
vul_arg<int>          dbsk3dr_cmd_smreo ("-smreo", "SMRE mega process option.", 0);
vul_arg<int>          dbsk3dr_cmd_smreb ("-smreb", "Batch run of Smooth. + Mesh. + Register. + Estimat. Error", 0);
vul_arg<int>          dbsk3dr_cmd_smatch ("-smatch", "Medial scaffold graph/hypergraph match\n\
         -f1 -f2 -icp 1 -reg 1", 0);
vul_arg<int>          dbsk3dr_cmd_smatchc ("-smatchc", "Medial scaffold curve match\n\
         -f1 -f2 -n1 -n2", 0);
vul_arg<int>         dbsk3dr_cmd_regrd ("-regrd", "Register by rigid transformation", 1);
vul_arg<int>         dbsk3dr_cmd_smicp ("-smicp", "Run ICP after MS matching (specify iterations).", 0);

vul_arg<int>          dbsk3dr_cmd_subsm ("-subsm", "Test matching a shock to a sub-sampled version.", 0);





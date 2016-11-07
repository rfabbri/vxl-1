//: This is lemsvxlsrc/brcv/rec/dbmsh3dr/pro/dbmsh3dr_cmdpara.h
//  Creation: Feb 12, 2007   Ming-Ching Chang

#ifndef _dbmsh3dr_cmdpara_h_
#define _dbmsh3dr_cmdpara_h_

#include <vul/vul_arg.h>

//#####################################################################
//: dbmsh3dr_cmdproc command-line arguments.

//List File
extern vul_arg<char const*>  dbmsh3dr_cmd_listfile;

extern vul_arg<int>          dbmsh3dr_cmd_scalem;

//Mesh Matching
extern vul_arg<int>          dbmsh3dr_cmd_icp;
extern vul_arg<float>        dbmsh3dr_cmd_icpcv;
extern vul_arg<float>        dbmsh3dr_cmd_icpthr;
extern vul_arg<float>        dbmsh3dr_cmd_dthr;
extern vul_arg<int>          dbmsh3dr_cmd_verr;
extern vul_arg<int>          dbmsh3dr_cmd_debug_verr;

extern vul_arg<int>          dbmsh3dr_cmd_rsnv;

#endif




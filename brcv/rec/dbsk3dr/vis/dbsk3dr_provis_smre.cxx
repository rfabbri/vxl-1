//: This is dbsk3dr_provis_smre.cxx
//  Ming-Ching Chang
//  Apr 08, 2007

#include <vcl_string.h>
#include <dbsk3dr/vis/dbsk3dr_process_vis.h>

//: Generate run file for smooth.+meshing+registeration+error estim.
void gen_smre_run_files (const vcl_string& f1, const vcl_string& f2,
                         const int smre, const int smreo)
{
  gen_view_f1_f2_xyz_bat (f1, f2);

  if (smreo==1) { //Pt-Pt base
    if (smre==2)
      gen_view_f1_f2_gsm_xyz_bat (f1, f2);
    else if (smre==3)
      gen_view_f1_f2_dcs_xyz_bat (f1, f2);
  }
  if (smreo==2) { //Pt-Mesh base
    if (smre==2) {
      gen_view_f1_f2_gsm_ply_bat (f1, f2);
      gen_view_f1_f2_shift_bat (f1, f2, "gsm");
    }
    else if (smre==3) {
      gen_view_f1_f2_dcs_ply_bat (f1, f2);
      gen_view_f1_f2_shift_bat (f1, f2, "dcs");
    }
  }
}

void gen_view_f1_f2_xyz_bat (const vcl_string& f1, const vcl_string& f2)
{
  vcl_string runfile = "view_" + f1 + "_" + f2 + "_xyz.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -xyz ";
  run_cmd += f1;
  run_cmd += " -xyz_2 ";
  run_cmd += f2;

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view f1 and f2 with init. alignment: %s.\n", runfile.c_str());
}

void gen_view_f1_f2_gsm_xyz_bat (const vcl_string& f1, const vcl_string& f2)
{
  vcl_string runfile = "view_" + f1 + "_" + f2 + "_gsm_xyz.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -xyz ";
  run_cmd += f1 + "_gsm";
  run_cmd += " -xyz_2 ";
  run_cmd += f2 + "_gsm";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view f1 and f2 after Gaussian smoothing: %s.\n", runfile.c_str());
}

void gen_view_f1_f2_dcs_xyz_bat (const vcl_string& f1, const vcl_string& f2)
{
  vcl_string runfile = "view_" + f1 + "_" + f2 + "_dcs_xyz.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -xyz ";
  run_cmd += f1 + "_dcs";
  run_cmd += " -xyz_2 ";
  run_cmd += f2 + "_dcs";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view f1 and f2 after DCS smoothing: %s.\n", runfile.c_str());
}

void gen_view_f1_f2_gsm_ply_bat (const vcl_string& f1, const vcl_string& f2)
{
  vcl_string runfile = "view_" + f1 + "_" + f2 + "_gsm_ply.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -ply ";
  run_cmd += f1 + "_gsm";
  run_cmd += " -ply_2 ";
  run_cmd += f2 + "_gsm";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view f1 and f2 after Gaussian smoothing: %s.\n", runfile.c_str());
}

void gen_view_f1_f2_dcs_ply_bat (const vcl_string& f1, const vcl_string& f2)
{
  vcl_string runfile = "view_" + f1 + "_" + f2 + "_dcs_ply.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }

  vcl_string run_cmd = "dbsk3dappw -ply ";
  run_cmd += f1 + "_dcs";
  run_cmd += " -ply_2 ";
  run_cmd += f2 + "_dcs";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view f1 and f2 after DCS smoothing: %s.\n", runfile.c_str());
}

void gen_view_f1_f2_shift_bat (const vcl_string& f1, const vcl_string& f2,
                               const vcl_string& key)
{
  vcl_string runfile = "view_" + f1 + "_" + key + "_shift.bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }
  vcl_string run_cmd = "dbsk3dappw -cnpt 1 -f1 ";
  run_cmd += f1;
  run_cmd += " -f2 ";
  run_cmd += f1 + "_" + key;
  run_cmd += " -ply ";
  run_cmd += f1 + "_" + key + ".ply";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view %s %s smoothing shifting: %s.\n", 
              f1.c_str(), key.c_str(), runfile.c_str());

  runfile = "view_" + f2 + "_" + key + "_shift.bat";
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }
  run_cmd = "dbsk3dappw -cnpt 1 -f1 ";
  run_cmd += f2;
  run_cmd += " -f2 ";
  run_cmd += f2 + "_" + key;
  run_cmd += " -ply ";
  run_cmd += f2 + "_" + key + ".ply";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file to view %s %s smoothing shifting: %s.\n", 
              f2.c_str(), key.c_str(), runfile.c_str());
}

//########################################################################

void gen_smreb_bats (const vcl_string& f1, const vcl_string& f2)
{
  //1) Generate smr1_f1_f2.bat : no smoothing
  //   -smreb 2 -smreo 1 -f1 -f2 -af -file 1
  //   
  vcl_string runfile = "smr1_" + f1 + "_" + f2 + ".bat";
  vcl_FILE* fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }
  vcl_string run_cmd = "dbsk3dappw -smreb 2 -smreo 1 -f1 ";
  run_cmd += f1;
  run_cmd += " -f2 ";
  run_cmd += f2;
  run_cmd += " -af 02_init_af.txt -file 1";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file %s.\n", runfile.c_str());

  //2) Generate smr2_f1_f2_gsm.bat: Gaussian smoothing
  //   -smreb 2 -smreo 2 -f1 -f2 -af -file 1
  runfile = "smr2_" + f1 + "_" + f2 + "_gsm.bat";
  fp;
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }
  run_cmd = "dbsk3dappw -smreb 2 -smreo 2 -f1 ";
  run_cmd += f1;
  run_cmd += " -f2 ";
  run_cmd += f2;
  run_cmd += " -af 02_init_af.txt -file 1";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file %s.\n", runfile.c_str());

  //3) Generate smr3_f1_f2_dcs.bat: DCS smoothing
  //   -smreb 2 -smreo 3 -f1 -f2 -af -oaf -file 1
  runfile = "smr3_" + f1 + "_" + f2 + "_dcs.bat";
  if ((fp = vcl_fopen (runfile.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open output bat file %s\n", runfile.c_str());
    return;
  }
  run_cmd = "dbsk3dappw -smreb 2 -smreo 3 -f1 ";
  run_cmd += f1;
  run_cmd += " -f2 ";
  run_cmd += f2;
  run_cmd += " -af 02_init_af.txt -file 1";

  vcl_fprintf (fp, "%s\n", run_cmd.c_str());
  vcl_fclose (fp);
  vul_printf (vcl_cout, "  Generate run file %s.\n", runfile.c_str());
}

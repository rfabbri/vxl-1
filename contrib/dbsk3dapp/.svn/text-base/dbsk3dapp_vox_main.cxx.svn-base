//:
// \file
// \author Ming-Ching Chang
// \date 08/06/2008
//
//        3D shock matching algorithm to test on VOX.
//      
// \verbatim
//   Modifications
//  
// \endverbatim

// To generate the initial XML file:
// dbsk3dappvox -print-def-xml
//
// To run with specified parameters in input_defaults.xml.
// dbsk3dappvox -x input_defaults.xml
//

#include <vcl_iostream.h>
#include <vul/vul_file.h>

#include "dborl_shock3d_match_params.h"
#include "dborl_shock3d_match_params_sptr.h"

#include <dbsk3dr/pro/dbsk3dr_process.h>

int main (int argc, char *argv[]) 
{
  // constructs with the default values
  dborl_shock3d_match_params_sptr params = new dborl_shock3d_match_params ("dborl_shock3d_match");  
  if (!params->parse_command_line_args(argc, argv))
    return 0;

  // always print the params file if an executable to work with ORL web interface
  if (!params->print_params_xml(params->print_params_file()))
    vcl_cout << "problems in writing params file to: " << params->print_params_file() << vcl_endl;

  if (params->exit_with_no_processing() || params->print_params_only())
    return 0;

  // always call this method to actually parse the input parameter file whose name is extracted from the command line
  if (!params->parse_input_xml())
    return 0;

  // 1) load the input shock3d file 1 (a pair of .cms and .fs files)
  vcl_string input_shock3d_1 = params->input_1_dir_() + "/" + params->input_1_name_() + params->input_extension_();
  if (!vul_file::exists(input_shock3d_1)) {
    vcl_cout << "Cannot find shock3d (.cms and .fs) files: " << input_shock3d_1 << "\n";
    return 0;
  }
  
  // 2) load the input shock3d file 2 (a pair of .cms and .fs files)
  vcl_string input_shock3d_2 = params->input_2_dir_() + "/" + params->input_2_name_() + params->input_extension_();
  if (!vul_file::exists(input_shock3d_2)) {
    vcl_cout << "Cannot find shock3d (.cms and .fs) files: " << input_shock3d_2 << "\n";
    return 0;
  }

  dbsk3d_pro* sp0 = new dbsk3d_pro;
  dbsk3d_pro* sp1 = new dbsk3d_pro;
  dbsk3dr_pro* spr = new dbsk3dr_pro (sp0, sp1);
  
  spr->p0()->set_dir_file (input_shock3d_1);
  bool f0_load_success = spr->p0()->load_cms ();  
  if (!f0_load_success) {
    vcl_cout << "Cannot load shock3d (.cms and .fs) files: " << input_shock3d_1 << "\n";
    return 0;
  }
  //Try read the xform file with the same filename and apply it.
  if (spr->p0()->load_hmatrix (spr->p0()->dir_prefix()))
    spr->p0()->apply_xform_hmatrix ();

  spr->p1()->set_dir_file (input_shock3d_2);
  bool f1_load_success = spr->p1()->load_cms ();  
  if (!f1_load_success) {
    vcl_cout << "Cannot load shock3d (.cms and .fs) files: " << input_shock3d_2 << "\n";
    return 0;
  }  
  //Try read the xform file with the same filename and apply it.
  if (spr->p1()->load_hmatrix (spr->p1()->dir_prefix()))
    spr->p1()->apply_xform_hmatrix ();

  //Perform 3D Shock Matching. params->smatch_()
  //-smatch 2: shock hypergraph matching using curve length as compatibility.
  //-smatch 3: shock hypergraph matching using curve sum radius over length as compatibility.
  //-smatch 4: shock hypergraph matching using D.P. curve distance as compatibility.
  //-smatch 5: shock hypergraph matching using D.P. shock curve distance as compatibility.
  //-smatch 6: option 5 plus integration of the global (R, T) in node compatibility.
  //-smatch 7: option 3 plus integration of the global (R, T) in node compatibility.

  //-regrd: register object1 rigidly to object2
  bool match_subset_of_curves = true;
  //-n: max # of ms curves.
  int max_ms_curves = 50;
  if (dbmsh3d_cmd_n() != -1)
    max_ms_curves = dbmsh3d_cmd_n();
  spr->run_shock_match (params->smatch_(), params->regrd_()!=0, 
                        match_subset_of_curves, max_ms_curves);

  vul_printf (vcl_cout, "\nMatching %s (N%d C%d S%d) to ", spr->p0()->dir_file().c_str(), 
              spr->p0()->ms_hypg()->vertexmap().size(), 
              spr->p0()->ms_hypg()->edgemap().size(),
              spr->p0()->ms_hypg()->sheetmap().size());
  vul_printf (vcl_cout, "\n         %s (N%d C%d S%d).\n", spr->p1()->dir_file().c_str(),
              spr->p1()->ms_hypg()->vertexmap().size(), 
              spr->p1()->ms_hypg()->edgemap().size(),
              spr->p1()->ms_hypg()->sheetmap().size());

  //-smicp : ICP (default iteration 100)
  /*if (params->smicp_()) {
    int n_iter = params->smicp_();
    spr->run_shock_match_icp (n_iter, dbmsh3dr_cmd_icpcv());
  }*/

  //output the final matching score into a file.
  vcl_string smatch_match_result_file  = "sk3dr_";
  smatch_match_result_file += params->input_1_name_();
  smatch_match_result_file += "__";
  smatch_match_result_file += params->input_2_name_();
  smatch_match_result_file += ".txt";
  spr->save_shock_match_result_file (params->output_dir_()+smatch_match_result_file);

  vul_printf (vcl_cout, "Matching ");  
  vul_printf (vcl_cout, "%s", params->input_1_name_().c_str());
  vul_printf (vcl_cout, " to ");
  vul_printf (vcl_cout, "%s\n", params->input_2_name_().c_str());
  vul_printf (vcl_cout, "similarity: %f  ", 
              spr->shock_match()->ga_match()->similarity());
  vul_printf (vcl_cout, "normalized similarity: %f\n", 
              spr->shock_match()->ga_match()->norm_similarity ());


  delete sp0, sp1;
  delete spr;

  return 0;
}


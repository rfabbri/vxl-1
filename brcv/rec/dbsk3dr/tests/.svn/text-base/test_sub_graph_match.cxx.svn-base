//This is lemsvxl/brcv/shp/dbskr/tests/test_sub_graph_match.cxx
//  Ming-Ching Chang  Sep 4, 2007
//
//
//  To debug in Visual studio, put argument
//  all D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//    or 
//  test_sub_graph_match D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data input_file n_time.
//
//

#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_ctime.h>
#include <vcl_cstdlib.h>
#include <vnl/vnl_random.h>
#include <vul/vul_printf.h>

#include <dbasn/dbasn_gradasgn.h>
#include <dbsk3d/dbsk3d_ms_node.h>
#include <dbsk3d/algo/dbsk3d_sg_sa_algos.h>
#include <dbsk3d/algo/dbsk3d_ms_fileio.h>
#include <dbsk3dr/dbsk3dr_match.h>

#if 0

//: Repeat the test for N times.
#define REPEAT_N             1     //100

#define SUB_GRAPH_R        0.1     //0: identical graph
#define NOISY_GRAPH_R_V    0.1
#define NOISY_GRAPH_R_C   0.15

dbsk3dr_ms_gradasn_graph* read_sg_file (const vcl_string& input_file);
dbsk3dr_ms_gradasn_graph* make_noisy_sub_graph (dbsk3dr_ms_gradasn_graph* inputSG, 
                                                const float R, const bool keep_isolated_vertex,
                                                const float nrv, const float nrc);
bool test_sub_graph_match (dbsk3dr_match& SM, dbasn_params& GAparams);

MAIN_ARGS (test_sub_graph_match)
{
  testlib_test_start("test_sub_graph_match()");
  //Setup the execution directory from command line parameter.
  vcl_string dir_base;
  if ( argc >= 2 ) {
      dir_base = argv[1];
      #ifdef VCL_WIN32
        dir_base += "\\";
      #else
        dir_base += "/";
      #endif
  }
  //Input file:
  vcl_string input_file = dir_base;
  if (argc >= 3)
    input_file += argv[2];
  else //Use default input_file.
    input_file += "bones1-ht-gt-sm5.sg";
  //n_repeat
  int n_repeat = REPEAT_N;
  if (argc >= 4)
    n_repeat = vcl_atoi (argv[3]);

  dbsk3dr_match SM;  
  dbasn_params GAparams;
  GAparams.wN_ = 0.3f;
  GAparams.wL_ = 1.0f;

  //Repeat the test for N times.
  int i;
  int n_test_success = 0;
  bool r;
  for (i=0; i<n_repeat; i++) {
    dbsk3dr_ms_gradasn_graph* SG0 = read_sg_file (input_file);
    dbsk3dr_ms_gradasn_graph* SG1 = make_noisy_sub_graph (SG0, SUB_GRAPH_R, false, NOISY_GRAPH_R_V, NOISY_GRAPH_R_C);

    //Setup and run the matching.
    ///SM.setup_GA_params (SG0, SG1, GAparams);
    SM.set_G (SG0);
    SM.set_g (SG1);
    SM.set_params (GAparams);
    SM._reset_mem ();

    r = test_sub_graph_match (SM, GAparams);
    if (r)
      n_test_success++;

    delete SG0, SG1;
    SM._free_mem ();
  }
  vul_printf (vcl_cout, "\n===========================================\n");
  vul_printf (vcl_cout, "Final test result: sub-graph matching success: %d (out of total %d).\n", n_test_success, n_repeat);
  TEST("# of incorrect matches (should be 0) ", n_repeat-n_test_success, 0);
  
  return testlib_test_summary();
}

dbsk3dr_ms_gradasn_graph* read_sg_file (const vcl_string& input_file)
{
  dbsk3dr_ms_gradasn_graph* SG = new dbsk3dr_ms_gradasn_graph;
  SG->setup_from_sg (input_file.c_str());
  return SG;
}

//Randomly remove R% of graph edges.
// Parameter R: ratio of edges removed in the sub-graph.
//   - R==0.0: duplicate the inputSG.
//   - R==0.5: roughly half of the edges remain.
//   - R==1.0: empty sub-graph.
// Parameter nrv: ratio of max_noise (w.r.t avg value) added to vertex attribute (radius).
// Parameter nrc: ratio of max_noise (w.r.t avg value) added to curve attribute (length).
//   -
dbsk3dr_ms_gradasn_graph* make_noisy_sub_graph (dbsk3dr_ms_gradasn_graph* inputSG, const float R,
                                                const bool keep_isolated_vertex,
                                                const float nrv, const float nrc)
{
  dbsk3dr_ms_gradasn_graph* SG = new dbsk3dr_ms_gradasn_graph;
  clone_sg_sa (SG, inputSG);

  if (R != 0)
    make_sub_graph (SG, R, keep_isolated_vertex);

  if (nrv != 0)
    perturb_graph_vertex_radius (SG, nrv);

  if (nrc != 0)
    perturb_graph_edge_len (SG, nrc);

  save_to_sg (SG, "d:\\qq_test.sg");

  SG->setup_gradasgn_hypg ();
  return SG;
}

//: Test shock sub-graph matching.
bool test_sub_graph_match (dbsk3dr_match& SM, dbasn_params& GAparams)
{
  //Normalize all nodes, links, corner costs to be within [0 ~ 1].
  SM.normalize_costs (false);
  
  //Run the shock grad assign matching.
  SM.get_assignment();
  SM.print_match_results ();

  //Check result.
  int incorrect = 0;
  for (int a=0; a<SM.M_row()-1; a++) {
    int i = SM.labelGg(a);
    if (i != -1) {
      dbskr_ms_gradasn_vertex* GV = SM.ms_G()->SV_from_ai (a);
      dbskr_ms_gradasn_vertex* gV = SM.ms_g()->SV_from_ai (i);
      if (GV->id() != gV->id()) ///(i != a) //SM.labelGg(labelg[i])) 
        incorrect++;
    }
  }

  vul_printf (vcl_cout, "-------------------------------------\n");
  vul_printf (vcl_cout, "  %d incorrect matches (should be 0) out of %d.\n", incorrect, SM.M_row()-1);
  vul_printf (vcl_cout, "-------------------------------------\n");

  return incorrect == 0;
}

#endif


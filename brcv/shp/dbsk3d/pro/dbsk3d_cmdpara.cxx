//: This is lemsvxlsrc/brcv/shp/dbsk3d/pro/dbsk3d_cmdpara.cxx
//  Creation: Feb 12, 2007   Ming-Ching Chang

#include <dbsk3d/pro/dbsk3d_cmdpara.h>

//#####################################################################
//: dbmsh3d_cmdproc command-line arguments.
//     Type     Variable     Flag      Help text        Default  

//Shock File I/O
vul_arg<char const*>  dbsk3d_cmd_fs_file ("-fs", "Fine-scale medial scaffold file (.fs)\n\
         -v 1,2: shock-gene asso., -v 3: shock links below percentage -pc,\n\
         -n 1,2: visualize shocks/surfaces def -1 for both,\n\
         -file 2, 3: output A12time.txt and A13cost.txt");

vul_arg<char const*>  dbsk3d_cmd_cms_file ("-cms", "Coarse-scale medial scaffold file (.cms)");
vul_arg<char const*>  dbsk3d_cmd_cms_file2 ("-cms_2", "Second coarse-scale medial scaffold file");
vul_arg<char const*>  dbsk3d_cmd_sg_file ("-sg", "Medial scaffold graph file (.sg)");
vul_arg<char const*>  dbsk3d_cmd_sg_file2 ("-sg_2", "Second medial scaffold graph file");

vul_arg<char const*>  dbsk3d_cmd_fs_ofile ("-ofs", "Output fine-scale medial scaffold file (.fs)");
vul_arg<char const*>  dbsk3d_cmd_cms_ofile ("-ocms", "Output coarse-scale medial scaffold file (.cms)");
vul_arg<char const*>  dbsk3d_cmd_sg_ofile ("-osg", "Output medial scaffold graph file (.sg)");

vul_arg<char const*>  dbsk3d_cmd_fs_vtk_ofile ("-ovfs", "Output fine-scale medial scaffold file in VTK format(.vtk)");

//Shock Processing
vul_arg<int>          dbsk3d_cmd_all ("-all", "Process all on -f prefix", 0);
vul_arg<int>          dbsk3d_cmd_allg ("-allg", "Process all (except -vlink) on -f prefix", 0);
vul_arg<int>          dbsk3d_cmd_bks ("-bks", "Bucketing for Leymarie's shock detection", 0);
vul_arg<int>          dbsk3d_cmd_flowsheet ("-flsht", "Leymarie's FlowSheet shock detection", 0);
vul_arg<int>          dbsk3d_cmd_bsphr ("-sphere", "Add bounding sphere, see -sphrr -sphsr", 0);
vul_arg<float>        dbsk3d_cmd_sphrr ("-sphrr", "Bounding sphere radius ratio", 5);
vul_arg<int>          dbsk3d_cmd_sphsr ("-sphsr", "Bounding sphere sample ratio", 1);
vul_arg<int>          dbsk3d_cmd_qhull ("-qhull", "Run QHull on .p3d point cloud file", 0);
vul_arg<int>          dbsk3d_cmd_qhfs ("-qhfs", "Recover full medial scaffold from QHull .vor file", 0);
vul_arg<int>          dbsk3d_cmd_seg ("-seg", "Surface segregation (reconstruction)", 0);
vul_arg<int>          dbsk3d_cmd_segop ("-segop", "Surface segregation option", 0);
vul_arg<int>          dbsk3d_cmd_gsa ("-gsa", "Maintain generator-shock association", 1);
vul_arg<int>          dbsk3d_cmd_reg ("-reg", "Shock regularization", 0);
vul_arg<float>        dbsk3d_cmd_regth ("-regth", "Shock regularization cost threshold", 10.0f);
vul_arg<int>          dbsk3d_cmd_trim_a122 ("-a122", "Trim A12-2 shock sheets", 1);
vul_arg<int>          dbsk3d_cmd_isc ("-isc", "Inner shock component", 0);
vul_arg<float>        dbsk3d_cmd_rmin ("-rmin", "Ratio of min shock pruning radius threshold", 1.0f);
vul_arg<float>        dbsk3d_cmd_rmax ("-rmax", "Ratio of max shock pruning radius threshold", -1.0f);
vul_arg<int>          dbsk3d_cmd_scp ("-scp", "Rib shock sheet compactness pruning", 1);
vul_arg<float>        dbsk3d_cmd_scpth ("-scpth", "Compactness threshold for boundary shock sheet elms", 0.1f);
vul_arg<int>          dbsk3d_cmd_shole ("-shole", "Remove shocks of boundary holes.", 0);
vul_arg<int>          dbsk3d_cmd_comp ("-comp", "The i-th component (0: largest)", 0);
vul_arg<int>          dbsk3d_cmd_ms ("-ms", "Extract coarse-scale medial scaffold", 0);
vul_arg<int>          dbsk3d_cmd_ssopt ("-ssopt", "Shock sheet topo. option in building ms_hypg.", 0);
vul_arg<int>          dbsk3d_cmd_trans ("-trans", "Medial scaffold transition regularization", 0);
vul_arg<int>          dbsk3d_cmd_transo ("-transo", "Medial scaffold transition regularization option", 8); //6
vul_arg<int>          dbsk3d_cmd_transv ("-transv", "Medial scaffold transition vis. (for debug)", 0);
vul_arg<int>          dbsk3d_cmd_tab ("-tab", "A1A3-I shock tab threshold", 50); //25, 10
vul_arg<int>          dbsk3d_cmd_a5 ("-a5", "A5 trans. length threshold", 30); //10.0f, 2.0f
vul_arg<int>          dbsk3d_cmd_cc ("-cc", "Curve contract trans. threshold", 20); //5.0f 1.0f
vul_arg<int>          dbsk3d_cmd_sc ("-sc", "Sheet contract trans. threshold", 30); //5.0f 1.0f
vul_arg<int>          dbsk3d_cmd_nnm ("-nnm", "Node-node merge trans. threshold", 10); //15
vul_arg<int>          dbsk3d_cmd_ncm ("-ncm", "Node-curve merge trans. threshold", 10); //15
vul_arg<int>          dbsk3d_cmd_ccm ("-ccm", "Curve-curve merge trans. threshold", 10); //15
vul_arg<int>          dbsk3d_cmd_ncma1a5 ("-ncma1a5", "Node-curve merge trans. threshold", 10); //5
vul_arg<int>          dbsk3d_cmd_cmxth ("-cmxth", "Curve merge transform validity threshold", 15);
vul_arg<int>          dbsk3d_cmd_smrib ("-smrib", "Smooth medial scaffold rib curves", 0);
vul_arg<int>          dbsk3d_cmd_smribo ("-smribo", "Smooth medial scaffold rib curve option", 5);
vul_arg<int>          dbsk3d_cmd_vlink ("-vlink", "Add virtual link to the MS hypergraph", 0);
///vul_arg<int>          dbsk3d_cmd_vlink ("-vlvo", "Virtual link visualization option", 1);
vul_arg<int>          dbsk3d_cmd_transg ("-transg", "Medial scaffold graph transition regularization", 0);
vul_arg<int>          dbsk3d_cmd_sgsm ("-sgsm", "Medial scaffold graph smoothing", 0);

vul_arg<int>          dbsk3d_cmd_bktseg ("-bktseg", "Surface reconstruction with bucketing", 0);
vul_arg<int>          dbsk3d_cmd_bktpst ("-bktpst", "Pre-stitching the buckets", 0);
vul_arg<int>          dbsk3d_cmd_merge ("-merge", "Merging surface meshes", 0);
vul_arg<int>          dbsk3d_cmd_spd ("-spd", "Compute shock prior distrbution", 0);
vul_arg<int>          dbsk3d_cmd_ifgp ("-ifgp", "In-flow generator propagation.", 0);
vul_arg<int>          dbsk3d_cmd_vsf ("-vsf", "View shock flow type.", 0);
vul_arg<int>          dbsk3d_cmd_vfc ("-vfc", "View flow complex (other subsets: Gaberial Graph, EMST).", 0);
vul_arg<int>          dbsk3d_cmd_vss ("-vss", "View shock sheet and boundary info.", 0);
vul_arg<int>          dbsk3d_cmd_vsel ("-vsel", "Visualize selected curves of larger lengths.", 0);
vul_arg<int>          dbsk3d_cmd_smsh ("-smsh", "Smoothing + Meshing a 3PI scan data.", 0);

//Shape reconstruction, Ridge detection
vul_arg<int>          dbsk3d_cmd_ridge ("-ridge", "Ridge detection", 0);
  /*{"ridge", _INT, 0, &CMD_RIDGE, "1-3: Draw shock to bnd line.\n\
      1: for A13 axials, 2: for A3 ribs, 3: for both A13 and A3 curves\n\
      Draw (4: sectional triangles), (5: ridge_region bnd curves), (6: both of above),\n\
      (7: ridge curves), (8: ridge curves + vectors) or\n\
      (9: ridge_region bnd curves + ridge curves) along A3 ribs.\n\
      (10: A13_bump_region bnd curves), (11: both 5 and 10)\n\
      Use -v 0/1 to hide/show shock sheets, -n for vector len."},*/

//GDT on shock sheets
vul_arg<int>          dbsk3d_cmd_gdts ("-gdts", "Geodesic dist. transform on shock sheets", 0);

//Shock Visualization
vul_arg<int>          dbsk3d_cmd_vs ("-vs", "Shock vis. options.", 1);
vul_arg<int>          dbsk3d_cmd_bnd ("-bnd", "Vis. of boundary (of shocks)\n\
         0:none, 1:points, 2:mesh, 3:pts&mesh, 4:color code.", 1);
vul_arg<float>        dbsk3d_cmd_sheet_transp ("-sts", "Shock sheet transparency", 0.0f);

//Interactivity options
vul_arg<int>          dbsk3d_cmd_dels ("-dels", "Click to delete shock", 0);

//#####################################################################

//: dbsk3dcmd_help.cxx
//  Command line parameter help file.
//  Ming-Ching Chang

#include <vcl_iostream.h>

  //Print out help instructions.
  /*if (dbmsh3d_cmd_help() != -1) {
    switch (dbmsh3d_cmd_help()) {
    case 0: 
      usage_help_info ();
    break;
    case 1: 
      datavis_usage_help ();
    break;
    case 2: 
      dataproc_usage_help ();
    break;
    case 3: 
      shock_usage_help ();
    break;
    case 33: 
      shock_usage_detail ();
    break;
    case 4: 
      shock_match_help ();
    break;
    case 44: 
      shock_match_detail ();
    break;
    case 5: 
      geodesic_usage_help ();
    break;
    case 6: 
      other_usage_help ();
    break;
    default:
    break;
    }
    return RUN_RESULT_SUCCESS;
  }*/

void usage_help_info() 
{
  vcl_cout << "\n  dbsk3d_cmd Usage Help.\n\n";
  vcl_cout << "-hh 0  : this help.\n";
  vcl_cout << "-hh 1  : 3D data visualization.\n";
  vcl_cout << "-hh 2  : 3D data processing.\n";
  vcl_cout << "-hh 3  : Shock computation.\n";
  vcl_cout << "-hh 33 : Shock computation details.\n";
  vcl_cout << "-hh 4  : Shock matching.\n";
  vcl_cout << "-hh 44 : Shock matching details.\n";
  vcl_cout << "-hh 5  : Geodesic computation.\n";
  vcl_cout << "-hh 6  : Other computation.\n";
}

void datavis_usage_help()
{
  vcl_cout << "\n  3D Data Visualization.\n\n";

  vcl_cout << "*.iv   View an OpenInventor file (*.iv)\n";
  vcl_cout << "*.g3d  View a color point cloud file (*.g3d)\n";
  vcl_cout << "*.p3d  View a point cloud file (*.p3d)\n";
  
  vcl_cout << "*.fs   View a fine-scale medial scaffold file (*.fs).\n";
  vcl_cout << "       Specify -v 1 to view both valid and pruned elements.\n";
  vcl_cout << "       Specify -v 2 to view the Gene-to-Shock association.\n";
  vcl_cout << "*.cms  View a coarse-scale shock hypergraph file (*.cms).\n";
  vcl_cout << "*.sg   View a coarse-scale shock graph file (*.sg).\n";

  vcl_cout << "\n  General options:\n\n";
  vcl_cout << "-light 0 off, 1 on.\n";
  vcl_cout << "-bg    0 black, 1 white.\n";
  vcl_cout << "-id    Show object id: 1 vertex, 2 curve, 3 sheet, 4 vertex+curve, 5 all.\n";
  vcl_cout << "-gsize Generator size in pixels, Def=1.\n";
  vcl_cout << "-r     Vertex sphere radius. -r 0 to make them invisible.\n";
  vcl_cout << "-rc    Cube radius. Def=0.01.\n";

  vcl_cout << "\nYou can display multiple files by combining the options.\n";
  vcl_cout << "For example, dbsk3d_cmd -ply2 a-surface.ply2 -p3d b.p3d.\n";
  
  vcl_cout << "\n  Output the result to a IV/VRML file.\n\n";
  vcl_cout << "-oiv   Specify an *.iv file output.\n";
}

void dataproc_usage_help() 
{
  vcl_cout << "\n  3D Data Processing.\n\n";

  vcl_cout << "-pp 1  Process the input file.\n";
  vcl_cout << "    Specify the Translation, Scaling, and Rotation of the input.\n";
  vcl_cout << "      -tx ? -ty ? -tz ? -rx ? -ry ? -rz ? -scale ? \n";
  vcl_cout << "    -1stoct   Shift the input to the first octant.\n";
  vcl_cout << "    -norm ?   Normalize the input to be within the given bounding box size.\n";
  vcl_cout << "    -pert     Perturb the input by adding some noise.\n";
  vcl_cout << "        ex: dbsk3d_cmd -pp para -pert 0.02\n";
  vcl_cout << "    -sub      Subsample the input randomly.\n";

  vcl_cout << "\n-siv 1  Subsample the input IV file to a .p3d file.\n";
  vcl_cout << "    Recursively subsample the IV file\n";
  vcl_cout << "    -rth      Specify the threshold to stop.\n";
}

void shock_usage_help() 
{
  vcl_cout << "\n  Shock Computation.\n\n";

  vcl_cout << "All-in-one Computation:\n";
  vcl_cout << "  -all 2 -f bones1 -th 3 -rmin 1.5 -r 0.02 -dx 14 -dy 14\n";
  vcl_cout << "  -all 1 -f sheep -th 5 -rmin 3 -tab 15 -a5 5 -a15 3 -dx 100 -dy 100\n\n";
  vcl_cout << "  -gui 0: no gui, 1: two views, 2: many views.\n";

  vcl_cout << "Step-by-step Computation:\n";
  vcl_cout << "  -sphere     Add BndSphere for QHull Preprocessing.\n";
  vcl_cout << "  -qhfs       Full shock recovery from QHull.\n";
  vcl_cout << "  -th         Surface meshing threshold.\n";
  vcl_cout << "      -rmin   Radius min for shock pruning.\n";
  vcl_cout << "      ex: -th 1.2 -rmin 0.6 -f bone\n";
  vcl_cout << "  -trans      MS hypergraph transition regularization.\n";
  vcl_cout << "      -tab    Specify the A1A3 tab removal threshold.\n";
  vcl_cout << "  -trasg      MS graph transition regularization.\n";
  vcl_cout << "      -a5     Specify the A5 link elm threshold.\n";
  vcl_cout << "      -a15    Specify the A15 link elm threshold.\n";
  vcl_cout << "      ex: -sgtrans sheep-ht -a5 5 -a15 0.5 -dx 100 -dy 100.\n";
  vcl_cout << "  -smo        MS graph smoothing.\n";
  vcl_cout << "      ex: -smo 3 -f bone -r 0.02 -dx 14 -dy 14.\n";
  vcl_cout << "  -recon      Show shock reconstruction.\n";
  vcl_cout << "      1: A13, 2: A3. 3: both, 4-6 with file outputs.\n";
  vcl_cout << "      ex: -recon 6 -f p2 -bg 1.\n";
  vcl_cout << "          -ply2 p2-surface -g3d p2-tcurve -gsize 3 -bg 1 -light 0.\n";

}

void shock_usage_detail()
{
  vcl_cout << "\n  Shock Computation Details.\n\n";

  vcl_cout << "  -buck     Shock detection via bucketing.\n";
  vcl_cout << "  -shock    Shock detection via flow sheet.\n";
  //        dbsk3d_cmd -shock 2 -p3d aortepts
  
  vcl_cout << "\nMore examples on all-in-one Computation:\n";
  vcl_cout << "  -all 2 -f tetra2 -th 0.1 -rmin 0.01 -boxr 5 -dx 3 -dy 1\n";
  vcl_cout << "  -all 2 -f box -th 3 -rmin 0.6 -mulseed 1\n";
  vcl_cout << "  -all 2 -f bone -th 1.2 -rmin 0.6 -r 0.02 -dx 14 -dy 14\n";
  vcl_cout << "  -all 2 -f bones1 -th 3 -rmin 1.5 -r 0.02 -dx 14 -dy 14\n";
  vcl_cout << "  -all 1 -f sheep -th 5 -rmin 3 -tab 15 -a5 5 -a15 3 -dx 100 -dy 100\n";
  vcl_cout << "  -all 1 -f sheep20k -th 5 -rmin 3 -dx 100 -dy 100\n";
  vcl_cout << "  -all 1 -f pot6 -th 6 -rmin 3 -dx 60 -dy 100\n";
  vcl_cout << "  -all 1 -f aorte -th 5 -rmin 1 -r 0.02 -dx 14 -dy 14\n";
  vcl_cout << "  -all 1 -f pg3 -th 3 -rmin 1 -mulseed 1 -r 0.02 -dx 50 -dy 50\n";
  vcl_cout << "  -all 1 -f k5 -th 3 -rmin 2 -mulseed 1 -r 0.02 -dx 100 -dy 100\n";

  vcl_cout << "\nMore examples on Surface meshing/shock pruning:\n";
  vcl_cout << "  -th 0.001 -rmin -0.001 -boxr 5 -f tetra2 -dx 3 -dy 1\n";
  vcl_cout << "  -th 3 -rmin 0.6 -f box -mulseed 1\n";
  vcl_cout << "  -th 1.2 -rmin 0.6 -f bone -r 0.02 -dx 14 -dy 14\n";
  vcl_cout << "  -th 5 -rmin 3 -f sheep -dx 100 -dx 100 -dy 100\n";
  vcl_cout << "  -th 6 -rmin 3 -f pot6 -dx 60 -dy 100\n";
  vcl_cout << "  -th 5 -rmin 3 -f aorte -r 0.02 -dx 14 -dy 14\n";

  vcl_cout << "\nMore examples on MS graph transition.\n";
  vcl_cout << "       ex:  -sgtrans k5 -a5 9 -dx 100 -dy 100\n";
  vcl_cout << "            -sgtrans bones1-ht -dx 14 -dy 14 -valid 1\n";
  vcl_cout << "            -sgtrans p2 -a15 1 -r 0.1 -dx 11 -dy 11\n";
}

void shock_match_help()
{
  vcl_cout << "\n  Shock Matching Computation.\n\n";
  vcl_cout << "-match 1  See examples:\n";
  vcl_cout << "  -f1 bone\\bone-ht-gt-sm5.sg -f2 bone2\\bone2-ht-gt-sm5.sg -r 0.03\n";
  vcl_cout << "  -f1 bones2\\bones2-ht-gt-sm5 -f2 bones1\\bones1-ht-gt-sm5 -r 0.1\n";
  vcl_cout << "   You can turn -verbose 1 to debug.\n";

//        dbsk3d_cmd -match 1 -f1 bones1-sm5.sg -f2 bones1p-sm5.sg -r 0.03
//        D:\Projects\BrownEyes\lemsvxlsrc\brcv\rec\dbsk3dr\tests\data
//        dbsk3d_cmd -match 1 -f1 bones1.sm5 -f2 bones1p.sm5 -r 0.05 -dx 1
//
//        D:\ProjectData\3DDatabase\DataSheep
//        dbsk3d_cmd -match 1 -f1 sheep2_20kr\sheep2_20kr-ht-gt-sm5 -f2 sheep1_20k\sheep1_20k-ht-gt-sm5 -r 0.03 -dx 100
//
//     Turn the Verbose Mode On to debug the Shock Matching
//      -verbose 1
//
//  B10)Shock MatchFile
//      K:\3DDatabase\DataWrist\E21879
//      -mfile match_pis05Lm_pis05L.txt
//
//  B11)Validate shockmatch against groundtruth
//      J:\VXLMigration\3DDatabase\DataDavid
//      -gtruth Davlid\DavidHead.g3d -g3d DavidHead-30K\DavidHead-30K.reg.g3d
//      Validate shockmatch+icp against groundtruth
//      -gtruth Davlid\DavidHead.g3d -g3d DavidHead-30K\DavidHead-30K.icp.reg.g3d
//
//  B12)Iterative Closest Point (ICP)
//      -icp 1 -g3d1 cap04L.g3d -g3d2 cap05L.g3d
//
//  B13)Registration (Shock Matching + ICP)
//      K:\3DDatabase\DataWrist\E21879
//      -match 1 -sc1 box760.scc1.comp0.TransSC.txt -sc2 box760.scc1.comp0.TransSC.txt -icp 1
//      -match 1 -sc1 S05L\pis05Lm\pis05Lm.scc1.comp0.TransSC2.smooth10.txt -sc2 S05L\pis05L\pis05L.scc1.comp0.TransSC2.smooth10.txt -r 0.01
//
}
  
void shock_match_detail()
{
}

void geodesic_usage_help() 
{
  vcl_cout << "\n  Geodesic Computation.\n\n";
  vcl_cout << "abc\n";
  vcl_cout << "abc\n";
  
//  E) Compute the geodesic distance transform on 2-manifold triangular meshes via FMM
//     D:\ProjectData\3DDatabase\DataTest\geodesic
//     -fmm moai.ply2 -ne 2 -r 0.1
//
}

void other_usage_help() 
{
  vcl_cout << "\n  Other Computation.\n\n";
  vcl_cout << "abc\n";
  vcl_cout << "abc\n";
//  E) Data Generating
//
//  E1)Generate a Parabolic Gutter P3D file
//      -genpg testParaUnif
//  E2)Generate a Cubic Box P3D file
//      -genbox testBox
//  E3)Generate random dbmsh3d_mesh
//      dbsk3d_cmd -grbox rand_1k.p3d -n 1000
//      dbsk3d_cmd -grbox rand_1m/rand_1m.p3d -n 1000000
//
//  F) Closed curve matching for surface meshing
//     dbsk3d_cmd -mesh 1
//
//  G) Convert the *.sli and set of *.con files to the Nuages *.cnt file
//     dbsk3d_cmd -sli xxx.sli -cnt xxx.cnt
//
//  G) Some Important Test Functions
//     In general, use -test 1 to run the test experiments.
//      -test 1
//
//  G1) Test Ozge's New Graduated Assignment
//      D:\Projects\3DShockViewer\Debug
//      -test 1
//  G2) Test artificial dbasn_gradasgn
//      K:\3DDatabase\DataTest
//      -match 1 -sc1 testTri.txt -sc2 testTri2.txt
}





// compare pairs of observations

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include <dbskr/dbskr_tree_edit_pmi.h>
#include <dbskr/dbskr_tree.h>
#include <dbsk2d/algo/dbsk2d_xshock_graph_fileio.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_timer.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/dbru_rcor.h>
#include <dbru/algo/dbru_object_matcher.h>
#include <dbcvr/dbcvr_cv_cor.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>

vsol_polygon_2d_sptr read_con_from_file(vcl_string fname) {
  double x, y;
  char buffer[2000];
  int nPoints;

  vcl_vector<vsol_point_2d_sptr> inp;
  inp.clear();

  vcl_ifstream fp(fname.c_str());
  if (!fp) {
    vcl_cout<<" Unable to Open "<< fname <<vcl_endl;
    return 0;
  }
  //2)Read in file header.
  fp.getline(buffer,2000); //CONTOUR
  fp.getline(buffer,2000); //OPEN/CLOSE flag (not important, we assume close)
  fp >> nPoints;
#if 0
  vcl_cout << "Number of Points from Contour: " << nPoints << vcl_endl;
#endif     
  for (int i=0;i<nPoints;i++) {
    fp >> x >> y;
    vsol_point_2d_sptr vs = new vsol_point_2d(x, y);
    inp.push_back(vs);
  }
  fp.close();
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(inp);
  return poly;
}

dbskr_tree_sptr read_esf_from_file(vcl_string fname, double sampling_ds) {
  dbsk2d_xshock_graph_fileio loader;
  dbskr_tree_sptr tree = new dbskr_tree(sampling_ds);
  dbsk2d_shock_graph_sptr sg = loader.load_xshock_graph(fname);
  tree->acquire(sg);    
  return tree;
}

int main_curve(int argc, char *argv[]) {
  vcl_cout << "Matching observations!\n";

  //: out file contains the wrong mathces if any
  vcl_string database_list, images_dir, cons_dir, out_file, output_dir;

  vcl_cout << "argc: " << argc << vcl_endl;
  if (argc != 13) {
    vcl_cout << "Usage: <program name> <database_list> <images_dir> <cons_dir> <size> <additional_string> <rms> <rest_curve_match_ratio> <ratio (0.01: for dt arclength test)> <0 if line int original implementation, 1 if line int version 4> <increment if line int orig (1), total votes if line int version 4 (15)> <output_dir> <outfile name>\n";
    vcl_cout << "<size> is 032, 064, 128 or 256, it is to be added to create output directory names when necessary\n";
    vcl_cout << "<additional string> is to be added to end of each name in database list, e.g. 055-135-068-030\n";
    vcl_cout << "<output_dir> directory for output images, e.g. shock_matches/ additional string will be added automatically\n";
    return -1;
  }
  database_list = argv[1];
  images_dir = argv[2];
  cons_dir = argv[3];
  //unused int size = atoi(argv[4]);
  vcl_string addition = argv[5];
  vcl_cout << "additional string read: " << addition << "\n";

  double R = 10.0f;
  double rms = atof(argv[6]);
  double rest_ratio = atof(argv[7]);

  double ratio = atof(argv[8]);  
  int dummy_int = atoi(argv[9]);
  bool line_int_original = false;
  if (dummy_int == 0) 
    line_int_original = true;

  double votes = atof(argv[10]);

  output_dir = vcl_string(argv[11])+vcl_string(argv[4])+"-"+addition+"/";
  out_file = vcl_string(argv[12])+"_"+vcl_string(argv[4])+"_";
  
  vcl_vector<vcl_string> database; 
  vcl_ifstream fpd((database_list).c_str());
  if (!fpd.is_open()) {
    vcl_cout << "Unable to open database file!\n";
    return -1;
  }

  char buffer[1000];
  while (!fpd.eof()) {
    vcl_string temp;
    fpd.getline(buffer, 1000);
    temp = buffer;
    if (temp.size() > 1) {
      vcl_cout << "temp: " << temp << " addition: " << addition << vcl_endl;
      database.push_back(temp+"_"+addition);
    }
  }
  fpd.close();
  
  unsigned int D = database.size(); 
  vcl_cout << " D: " << D << "\n";
#if 1
  vcl_cout << "printing database list: \n";
  for (unsigned int i = 0; i<D; i++) {
    vcl_cout << database[i] << "\n";
  }
#endif

  vcl_string dummy = addition;
  char *pch = vcl_strtok ((char *)dummy.c_str(),"-");
  while (pch != NULL)
  {
    printf ("%s\n",pch);
    out_file = out_file + pch + "_";
    pch = strtok (NULL, "-");
  }
  out_file = out_file+"curve_"+argv[6]+"_"+argv[7]+"_"+argv[8]+"_"+argv[9]+".out";
  vcl_cout << "out_file: " << out_file << vcl_endl;
  vcl_string dump_file = "dump_results_" + out_file;
  vcl_cout << "dump_file: " << dump_file << vcl_endl;

  //: load images
  vcl_vector<vil_image_resource_sptr> database_images;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string image_file = images_dir + database[i] + ".png";
    vil_image_resource_sptr image_r = vil_load_image_resource( image_file.c_str() );
    database_images.push_back(image_r);
  }
  vcl_cout << "loaded images\n";

  //: load cons
  vcl_vector<vsol_polygon_2d_sptr> database_polygons;
  for (unsigned int i = 0; i<D; i++) {
    vcl_string con_file = cons_dir + database[i] + ".con";
    vsol_polygon_2d_sptr poly = read_con_from_file(con_file.c_str());
    database_polygons.push_back(poly);
  }

  vcl_cout << "loaded contours, preparing observations..\n";
  vcl_vector<dbinfo_observation_sptr> database_obs;
  for (unsigned int i = 0; i<D; i++) {
    dbinfo_observation_sptr obs = new dbinfo_observation(0, database_images[i], database_polygons[i], true, true, false);
    database_obs.push_back(obs);
  }
  
  vbl_array_2d<double> matching_costs(D, D, 100000);
  vbl_array_2d<float> infos_dt(D, D, 100000);
  vbl_array_2d<float> infos_line(D, D, 100000);

  unsigned int i_start = 0, j_start = 1;
  vcl_ifstream if3;

  if3.open(dump_file.c_str());
  if (if3) {
    int dummy; bool tag = false;
    if3 >> dummy;  // read first i
    for (unsigned int i = 0; i<D; i++) {
      i_start = i;
      for (unsigned int j = i+1; j < D; j++) {
        j_start = j;
        if (if3.eof()) { tag = true; break; }  
        if3 >> dummy;   // read j
        if3 >> matching_costs[i][j];
        matching_costs[j][i] = matching_costs[i][j];

        if3 >> infos_dt[i][j];
        infos_dt[j][i] = infos_dt[i][j];

        if3 >> infos_line[i][j];
        infos_line[j][i] = infos_line[i][j];

        if3 >> dummy;  // try reading i again, if not then it'll break 
      }
      if (tag) break;
    }
    if3.close();
  }
  
  vcl_cout << "i_start: " << i_start << " j_start: " << j_start << vcl_endl;
  vcl_ofstream of3;

  of3.open(dump_file.c_str());
  
  for (unsigned int i = 0; i<=i_start; i++)
    for (unsigned int j = i+1; j < (i == i_start?j_start:D); j++) {
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << infos_dt[i][j] << " " << infos_line[i][j] << "\n";
    }
  of3.close();

  for (unsigned int i = i_start; i<D; i++) {
    unsigned int j = (i == i_start ? j_start : i+1);
    for ( ; j<D; j++) {
      vul_timer t;
      dbcvr_cv_cor_sptr cm_cor1, cm_cor2; double cost1, cost2;
      cm_cor1 = dbru_object_matcher::compute_curve_alignment(database_obs[i],database_obs[j],cost1,R,rms,rest_ratio,false);
      cm_cor2 = dbru_object_matcher::compute_curve_alignment(database_obs[j],database_obs[i],cost2,R,rms,rest_ratio,false);      
      
      if (cost1 < cost2) {
        matching_costs[i][j] = cost1;
        matching_costs[j][i] = cost1;
        vcl_cout << "i: " << i << " j: " << j << " cost " << cost1 << " time: "<< t.real()/1000.0f << " ";
      } else {
        matching_costs[i][j] = cost2;
        matching_costs[j][i] = cost2;
        vcl_cout << "i: " << i << " j: " << j << " cost " << cost2 << " time: "<< t.real()/1000.0f << " "; 
      }

      t.mark();
      dbru_rcor_sptr rcor1, rcor2;
      if (line_int_original) {
        rcor1 = dbru_object_matcher::generate_rcor_curve_matching_line(database_obs[i], database_obs[j], cm_cor1, int(votes), false);
        rcor2 = dbru_object_matcher::generate_rcor_curve_matching_line(database_obs[j], database_obs[i], cm_cor2, int(votes), false);
      } else {
        rcor1 = dbru_object_matcher::generate_rcor_curve_matching_line4(database_obs[i], database_obs[j], cm_cor1, int(votes), false);
        rcor2 = dbru_object_matcher::generate_rcor_curve_matching_line4(database_obs[j], database_obs[i], cm_cor2, int(votes), false);
      }
      float info1 = dbinfo_observation_matcher::minfo(database_obs[i], database_obs[j], rcor1->get_correspondences(), false);
      float info2 = dbinfo_observation_matcher::minfo(database_obs[j], database_obs[i], rcor2->get_correspondences(), false);

      if (info1 > info2) {
        infos_line[i][j] = info1;
        infos_line[j][i] = info1;
        vil_image_resource_sptr out_image = rcor1->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[i]+"_"+database[j]+"-line.png").c_str()); 
        vcl_cout << " info1: " << info1 << " time: "<< t.real()/1000.0f << " ";
      } else {
        infos_line[i][j] = info2;
        infos_line[j][i] = info2;
        vil_image_resource_sptr out_image = rcor2->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[j]+"_"+database[i]+"-line.png").c_str()); 
        vcl_cout << " info2: " << info2 << " time: "<< t.real()/1000.0f << " ";
      }

      t.mark();
      dbru_rcor_sptr rcor3 = dbru_object_matcher::generate_rcor_curve_matching_dt2(database_obs[i], database_obs[j], cm_cor1, float(ratio), false);
      dbru_rcor_sptr rcor4 = dbru_object_matcher::generate_rcor_curve_matching_dt2(database_obs[j], database_obs[i], cm_cor2, float(ratio), false);
      float info3 = dbinfo_observation_matcher::minfo(database_obs[i], database_obs[j], rcor3->get_correspondences(), false);
      float info4 = dbinfo_observation_matcher::minfo(database_obs[j], database_obs[i], rcor4->get_correspondences(), false);

      if (info3 > info4) {
        infos_dt[i][j] = info3;
        infos_dt[j][i] = info3;
        vil_image_resource_sptr out_image = rcor3->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[i]+"_"+database[j]+"-dt.png").c_str()); 
        vcl_cout << " info3: " << info3 << " time: "<< t.real()/1000.0f << " secs\n";
      } else {
        infos_dt[i][j] = info4;
        infos_dt[j][i] = info4;
        vil_image_resource_sptr out_image = rcor4->get_appearance2_on_pixels1();
        vil_save_image_resource(out_image, (output_dir+database[j]+"_"+database[i]+"-dt.png").c_str()); 
        vcl_cout << " info4: " << info4 << " time: "<< t.real()/1000.0f << " secs\n";
      }


      of3.open(dump_file.c_str(), vcl_ofstream::app);
      of3 << i << " " << j << " " << matching_costs[i][j] << " " << infos_dt[i][j] << " " << infos_line[i][j] << "\n";
      of3.close();
   }
  }

  vcl_ofstream of((out_file).c_str());
  of << "\n curve costs: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << matching_costs[i][j] << " ";
    of << "\n";
  }

  of << "\n info distance transform costs: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << infos_dt[i][j] << " ";
    of << "\n";
  }

  of << "\n info line costs: \n" << D << " " << D << "\n";
  for (unsigned i = 0; i<D; i++) {
    for (unsigned j = 0; j<D; j++)
      of << infos_line[i][j] << " ";
    of << "\n";
  }

  of.close();

  return 0;
}

int main(int argc, char *argv[]) {
  main_curve(argc, argv);
}

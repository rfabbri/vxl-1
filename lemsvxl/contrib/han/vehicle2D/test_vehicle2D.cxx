#include "vehicle2D.h"

#include "ps_xz.h"







int main(int argc, char* argv[])
{
  vcl_vector<vgl_point_3d<double> >pts;
  vcl_vector<vgl_point_3d<double> >pts_f;
  vcl_vector<vgl_point_3d<double> >pts_r;
  //    vcl_string wc_filename = wc_path.path;
  // test if fname is a directory
  //vul_file::exists
  vgl_point_3d<double>(1,0,0);

  vcl_vector <vnl_double_2> p0,p1;
  p0.push_back(vnl_double_2 (628.987, 303.468));
  p0.push_back(vnl_double_2 (638.962, 308.468));
  p0.push_back(vnl_double_2 (  682.082, 313.468));
  p0.push_back(vnl_double_2 (  695.967, 318.468));
  p0.push_back(vnl_double_2 (  706.547, 323.468));
  p0.push_back(vnl_double_2 (  709.972, 328.468));
  p0.push_back(vnl_double_2 (  708.142, 333.468));
  p0.push_back(vnl_double_2 (   711.837, 338.468));
  p0.push_back(vnl_double_2 (   709.232, 343.468));
  p0.push_back(vnl_double_2 (  656.862, 348.468));
  p0.push_back(vnl_double_2 (   673.372, 353.468));
  p0.push_back(vnl_double_2 ( 691.277, 358.468));
  p0.push_back(vnl_double_2 (674.447, 363.468));
  p0.push_back(vnl_double_2 (686.022, 368.468));
  p0.push_back(vnl_double_2 (697.657, 373.468));
  p0.push_back(vnl_double_2 (700.322, 378.468));
  p0.push_back(vnl_double_2 (693.162, 383.468));
  p0.push_back(vnl_double_2 (686.972, 388.468));
  p0.push_back(vnl_double_2 (679.542, 393.468));
  p0.push_back(vnl_double_2 (689.977, 398.468));
  p0.push_back(vnl_double_2 (696.342, 403.468));
  p0.push_back(vnl_double_2 (663.142, 408.468));
  p0.push_back(vnl_double_2 (650.157, 413.468));
  p0.push_back(vnl_double_2 (679.157, 418.468));
  p0.push_back(vnl_double_2 (672.532, 423.468));
  p0.push_back(vnl_double_2 (672.532, 428.468));
  p0.push_back(vnl_double_2 (648.782, 433.468));
  p0.push_back(vnl_double_2 (648.782, 438.468));
  p0.push_back(vnl_double_2 (648.782, 443.468));
  p0.push_back(vnl_double_2 (646.032, 448.468));
  p0.push_back(vnl_double_2 (630.957, 453.468));
  p0.push_back(vnl_double_2 (637.052, 458.468));
  p0.push_back(vnl_double_2 (604.702, 463.468));



  p1.push_back(vnl_double_2 (250.942, 303.468));
  p1.push_back(vnl_double_2 ( 248.507, 308.468));
  p1.push_back(vnl_double_2 ( 247.997, 313.468));
  p1.push_back(vnl_double_2 ( 247.217, 318.468));
  p1.push_back(vnl_double_2 ( 248.832, 323.468));
  p1.push_back(vnl_double_2 ( 187.552, 328.468));
  p1.push_back(vnl_double_2 ( 212.412, 333.468));
  p1.push_back(vnl_double_2 ( 214.347, 338.468));
  p1.push_back(vnl_double_2 ( 207.452, 343.468));
  p1.push_back(vnl_double_2 ( 192.777, 348.468));
  p1.push_back(vnl_double_2 ( 233.377, 353.468));
  p1.push_back(vnl_double_2 ( 225.432, 358.468));
  p1.push_back(vnl_double_2 ( 257.467, 363.468));
  p1.push_back(vnl_double_2 ( 268.912, 368.468));
  p1.push_back(vnl_double_2 ( 263.407, 373.468));
  p1.push_back(vnl_double_2 ( 285.352, 378.468));
  p1.push_back(vnl_double_2 ( 312.812, 383.468));
  p1.push_back(vnl_double_2 ( 296.092, 388.468));
  p1.push_back(vnl_double_2 ( 323.182, 393.468));
  p1.push_back(vnl_double_2 ( 365.592, 398.468));
  p1.push_back(vnl_double_2 ( 377.382, 403.468));
  p1.push_back(vnl_double_2 ( 393.582, 408.468));
  p1.push_back(vnl_double_2 (431.592, 413.468));
  p1.push_back(vnl_double_2 ( 474.802, 418.468));
  p1.push_back(vnl_double_2 ( 556.407, 423.468));
  p1.push_back(vnl_double_2 ( 418.952, 428.468));
  p1.push_back(vnl_double_2 ( 428.712, 433.468));
  p1.push_back(vnl_double_2 ( 432.752, 438.468));
  p1.push_back(vnl_double_2 ( 448.382, 443.468));
  p1.push_back(vnl_double_2 ( 462.437, 448.468));
  p1.push_back(vnl_double_2 ( 493.272, 453.468));
  p1.push_back(vnl_double_2 ( 495.607, 458.468));
  p1.push_back(vnl_double_2 ( 520.147, 463.468));




  if (argc != 2 &&argc != 3) {
    vcl_cout << "USAGE: ps_xz.exe <res*.txt> \n";
    return 1;
  }

  vcl_string image_filename(argv[1]);

  //bottom_=vul_string_atof(argv[3]);
 bottom_=0.5;
  vcl_vector<vnl_double_2> p0a,p1a;
  read_result_txt_file_vgl_point_3d(argv[1],vul_string_to_bool(argv[2]),p0a,p1a,bottom_);

  vcl_cout<<"aaa"<<vcl_endl;

  vehicle2D carA(p0a,p1a);
  carA.print();
  carA.com();

  carA.sum(10.0,20.0);
  carA.sum(10.0,30.0);
  carA.sum(10.0,40.0);
  carA.sum(60.0,70.0);
  carA.sum(60.0,80.0);
  carA.sum(60.0,90.0);

   carA.sum(70.0,80.0);
    carA.sum(70.0,90.0);

    carA.find_probe_8();
  //vehicle2D modelB(500,   170, 80, //double cabin_l,
   //                  150,  80,  90, 628.987)  ;
  //modelB.print();
  //modelB.com();
  //vcl_cout<<modelB.sim_check(carA, modelB);



  exit(1);
  if (argc!=3)
  {
    vcl_cout<<"Usage: <vrml_file_name> <scale>"<<vcl_endl;
    exit(-1);
  }


  bool rotation;




  vehicle2D c;
  //int m=c.read_wrl_file(pts, argv[1]);


  vcl_string where=".";
  vcl_string File =where+"/del-AEC-1.wrl";
  vcl_string Filef=where+"/delf-AEC-1.wrl";
  vcl_string Filer=where+"/delr-AEC-1.wrl"; 
  int m=c.read_bb_box_file("bbox_cam_my.txt");
  m=c.read_wrl_file(pts, File, true);
  m=c.read_wrl_file(pts_f, Filef, true);
  m=c.read_wrl_file(pts_r, Filer, true);

  //which_to_read.push_back(File);
  //which_to_read.push_back(Filef);
  //which_to_read.push_back(Filer);
  //
  //which_to_read.push_back(where+"/bbox_cam_my.txt");



  vehicle2D a(pts);
  a.set(pts_f,1);
  a.set(pts_r,2);
  vgl_polygon<double> pgs=a.get_XY_con(1);
  // pgs.print(vcl_cout);

  for (unsigned int s = 0; s < pgs.num_sheets(); ++s)
    for (unsigned int p = 0; p < pgs[s].size(); ++p)
      vcl_cout<<pgs[s][p].x()<<", "<< pgs[s][p].y()<<vcl_endl;

  vcl_cout<< "\n\n\n"<<vcl_endl;
  pgs=a.get_XY_con(2);
  // pgs.print(vcl_cout);

  for (unsigned int s = 0; s < pgs.num_sheets(); ++s)
    for (unsigned int p = 0; p < pgs[s].size(); ++p)
      vcl_cout<<pgs[s][p].x()<<", "<< pgs[s][p].y()<<vcl_endl;

  exit(1);
  //a.curvature(1.2,0);
  a.curvature(atof(argv[2]),0);
  vcl_vector <double> dl=a.distance();

  mbl_stats_1d stats0(dl);
  vcl_cerr<<stats0<<vcl_endl;

}

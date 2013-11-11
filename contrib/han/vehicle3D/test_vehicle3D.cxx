#include "vehicle3D.h"









int main(int argc, char* argv[])
{
  vcl_vector<vgl_point_3d<double> >pts;
  vcl_vector<vgl_point_3d<double> >pts_f;
  vcl_vector<vgl_point_3d<double> >pts_r;
  //    vcl_string wc_filename = wc_path.path;
  // test if fname is a directory
  //vul_file::exists
  vgl_point_3d<double>(1,0,0);

  /*pts.push_back(vgl_point_3d<double>(0,0,0));
  pts.push_back(vgl_point_3d<double>(1,0,0));
  pts.push_back(vgl_point_3d<double>(2,0,0));
  pts.push_back(vgl_point_3d<double>(2,1,0));
  pts.push_back(vgl_point_3d<double>(2,2,0));
  pts.push_back(vgl_point_3d<double>(2,3,0));
  pts.push_back(vgl_point_3d<double>(3,3,0));
  pts.push_back(vgl_point_3d<double>(3,4,0));
  pts.push_back(vgl_point_3d<double>(3,5,0));
  pts.push_back(vgl_point_3d<double>(4,5,0));
  pts.push_back(vgl_point_3d<double>(5,5,0));
  pts.push_back(vgl_point_3d<double>(6,5,0));
  pts.push_back(vgl_point_3d<double>(7,6,0));
  pts.push_back(vgl_point_3d<double>(8,10,0));
  pts.push_back(vgl_point_3d<double>(9,12,0));
  pts.push_back(vgl_point_3d<double>(10,13,0));
  pts.push_back(vgl_point_3d<double>(11,15,0));
  */  

  /*
  pts.push_back(vgl_point_3d<double>( -7.62034, 5.35773,4.87148));
  pts.push_back(vgl_point_3d<double>( -7.61878, 5.33982,4.83886));
  pts.push_back(vgl_point_3d<double>(-8.78265, 5.57182,4.54215));
  pts.push_back(vgl_point_3d<double>(-8.08844, 5.27432,4.33316));
  pts.push_back(vgl_point_3d<double>(-8.11899, 5.26825,4.29968));
  pts.push_back(vgl_point_3d<double>(-8.14901, 5.26558,4.26584));
  pts.push_back(vgl_point_3d<double>(-8.17713,5.26346,4.23194));
  pts.push_back(vgl_point_3d<double>(-8.26092,5.2794,4.19599));
  pts.push_back(vgl_point_3d<double>(-8.50601,5.38112,4.1503));
  pts.push_back(vgl_point_3d<double>(-8.75264,5.4524, 4.10784));
  pts.push_back(vgl_point_3d<double>(-8.79475,5.45653,4.07303));
  pts.push_back(vgl_point_3d<double>(-8.88661,5.4643,4.03777));
  pts.push_back(vgl_point_3d<double>(-8.9788,5.48678,4.00075));
  pts.push_back(vgl_point_3d<double>(-9.07517,5.5102,3.96357));
  pts.push_back(vgl_point_3d<double>(-9.20177,5.54635,3.92477));
  pts.push_back(vgl_point_3d<double>(-9.4002,5.60262,3.88661));
  pts.push_back(vgl_point_3d<double>(-9.65223,5.69378,3.84492));
  pts.push_back(vgl_point_3d<double>(-7.93992,5.04888,3.88346));
  pts.push_back(vgl_point_3d<double>(-8.26511,5.14199,3.80384));
  pts.push_back(vgl_point_3d<double>(-8.17788,5.10413,3.77471));
  pts.push_back(vgl_point_3d<double>(-7.85387,4.99415,3.75501));
  pts.push_back(vgl_point_3d<double>(-7.52968,4.85893,3.73891));
  pts.push_back(vgl_point_3d<double>(-5.63283,4.39547,3.16718));
  pts.push_back(vgl_point_3d<double>(-5.61943,4.37078,3.13775));
  pts.push_back(vgl_point_3d<double>(-5.84861,4.42483,3.09612));
  pts.push_back(vgl_point_3d<double>(-6.11907,4.49008,3.05261));
  pts.push_back(vgl_point_3d<double>(-7.70447,4.94361,2.60677));
  pts.push_back(vgl_point_3d<double>(-8.65035, 5.30383, 2.49797));
  pts.push_back(vgl_point_3d<double>(-8.56297, 5.27991, 2.47933));
  pts.push_back(vgl_point_3d<double>(-7.34644, 4.80564, 2.52841));
  pts.push_back(vgl_point_3d<double>(-7.52818, 4.8501, 2.48675));
  pts.push_back(vgl_point_3d<double>(-7.62605, 4.85318, 2.45242));
  pts.push_back(vgl_point_3d<double>(-8.44965, 5.24031, 2.33098));
  pts.push_back(vgl_point_3d<double>(-8.6866 ,5.33036, 2.29738));
  pts.push_back(vgl_point_3d<double>(-8.1779, 4.98103, 2.22471));
  pts.push_back(vgl_point_3d<double>(-8.0918, 4.94544, 2.19703));
  pts.push_back(vgl_point_3d<double>(-8.8277, 5.33178, 2.08005));
  pts.push_back(vgl_point_3d<double>(-8.96507, 5.37481, 2.04611));
  pts.push_back(vgl_point_3d<double>(-8.956 ,5.37406, 2.01132));
  pts.push_back(vgl_point_3d<double>(-8.9632, 5.38038, 1.97515));
  pts.push_back(vgl_point_3d<double>(-8.92187, 5.36738, 1.94269));
  pts.push_back(vgl_point_3d<double>(-8.87931, 5.35459, 1.91019));
  pts.push_back(vgl_point_3d<double>(-8.85265, 5.35039, 1.87599));
  pts.push_back(vgl_point_3d<double>(-8.87996, 5.36595, 1.83792));
  pts.push_back(vgl_point_3d<double>(-8.92276, 5.38539, 1.79904));
  pts.push_back(vgl_point_3d<double>(-8.93676, 5.39362, 1.76234));
  pts.push_back(vgl_point_3d<double>(-8.93105, 5.39485, 1.72699));
  pts.push_back(vgl_point_3d<double>(-8.92392, 5.39832, 1.69113));
  pts.push_back(vgl_point_3d<double>(-8.91812, 5.40579, 1.65439));
  pts.push_back(vgl_point_3d<double>(-8.86598, 5.39461, 1.62142));
  pts.push_back(vgl_point_3d<double>(-8.80297, 5.37474, 1.59031));
  pts.push_back(vgl_point_3d<double>(-8.73469, 5.35315, 1.55447));
  pts.push_back(vgl_point_3d<double>(-8.70112, 5.3473 ,1.52558));
  pts.push_back(vgl_point_3d<double>(-8.59958, 5.32081, 1.49581));
  pts.push_back(vgl_point_3d<double>(-8.56272, 5.31597, 1.46158));
  pts.push_back(vgl_point_3d<double>(-8.52925, 5.31211, 1.42713));
  pts.push_back(vgl_point_3d<double>(-8.52483, 5.31875, 1.39045));
  */

  if (argc!=3)
  {
    vcl_cout<<"Usage: <vrml_file_name> <scale>"<<vcl_endl;
    exit(-1);
  }

  
  bool rotation;
  



  vehicle3D c;
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


  
  vehicle3D a(pts);
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

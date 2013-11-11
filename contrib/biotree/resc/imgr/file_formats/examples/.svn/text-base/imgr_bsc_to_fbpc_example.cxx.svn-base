//: 
// \file  imgr_bsc_to_fbpc_example.cxx
// \brief  an example of converting bin file from coordinate system to reconstructed system
// \author    Kongbin Kang
// \date        2006-04-03
// 
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vgl/vgl_vector_3d.h>
#include <vsol/vsol_cylinder.h>
#include <xscan/xscan_scan.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>

struct CYLINDER
{
  vsol_cylinder cyl_;
  double intensity_;

  CYLINDER(vsol_cylinder const & cyl, double strength) : cyl_(cyl), intensity_(strength) {}
};

typedef vcl_vector<CYLINDER > cylinder_vec;

cylinder_vec  read_cylinders(char* file_name, int& num, double& min_strength, double& max_strength)
{
  int ver;
  double strength;
  cylinder_vec cylinders;

  vsl_b_ifstream istream(file_name);
  vsl_b_read(istream, ver);
  vsl_b_read(istream, num);
  
  for (int i=0; i < num; i++) {
    vsl_b_read(istream, strength);
    if (min_strength > strength)
      min_strength = strength;
    if (max_strength < strength)
      max_strength = strength;
  
    vcl_cout << strength << " min=" << min_strength << " max=" << max_strength << vcl_endl;

    vsol_cylinder cylinder;
    cylinder.b_read(istream);
    
    cylinders.push_back(CYLINDER(cylinder, strength));

    vcl_cout << cylinder << vcl_endl;

  }

  return cylinders;
}

int main(int argc, char* argv[])
{
  if(argc < 12)
  {
    vcl_cout << "Usage: " << argv[0] <<" cylinder_file reconlog_file scan_file output_file minx(mm) miny(mm) minz(mm) maxx(mm) maxy(mm) maxz(mm) spacing(mm)\n ";
    exit(1);
  }

  int num_cylinders;
  double min_strength;
  double max_strength;
  
  double minx, miny, minz, maxx, maxy, maxz, spacing;

  vcl_stringstream ss;
  ss << argv[5];
  ss >> minx;
  ss.clear();
  
  ss << argv[6];
  ss >> miny;
  ss.clear();

  ss << argv[7];
  ss >> minz;
  ss.clear();

  ss << argv[8];
  ss >> maxx;
  ss.clear();

  ss << argv[9];
  ss >> maxy;
  ss.clear();
   
  ss << argv[10];
  ss >> maxz;
  ss.clear();
 
  ss << argv[11];
  ss >> spacing;
  ss.clear();
  
  cylinder_vec cylinders = read_cylinders(argv[1], 
      num_cylinders, min_strength, max_strength);

  vsl_b_ofstream ostream(argv[4]);

  vcl_ifstream scanfile(argv[3]);
  if(!scanfile)
  {
    vcl_cout <<"Open scan file failed\n";
    exit(2);
  }

  xscan_scan scan;
  scanfile >> scan;

  imgr_skyscan_reconlog skyscan(argv[2], scan);

  cylinder_vec selected_cyls;
  
  for(int i=0; i < num_cylinders; i++)
  {
    vsol_cylinder& cyl = cylinders[i].cyl_;
    vgl_point_3d<double> center_bsc(cyl.center().x()*spacing + minx,
        cyl.center().y()*spacing+ miny,
        cyl.center().z()*spacing + minz );
    
    vgl_point_3d<double> c = skyscan.bsc_to_fbpc(center_bsc);
 
    cyl.set_center(c);
   
    cyl.set_length(1.);
  
    // select cylinder inside the ROI
#if 0
    if( c.x() > 396 && c.y() > 376 && c.x() < 410 && c.y() < 384)
#endif
    if(1)
    {
       vcl_cout <<"cylinder segment is centered on" << c <<"\n";
       selected_cyls.push_back(cylinders[i]);
    }
     
  }

  int ver = 1;
  vsl_b_write(ostream, ver);

  num_cylinders = selected_cyls.size();
  
  vsl_b_write(ostream, num_cylinders);
  
  for(int i=0; i < num_cylinders; i++)
  {
    vsl_b_write(ostream, selected_cyls[i].intensity_);
    selected_cyls[i].cyl_.b_write(ostream);
  }

  vcl_cout <<"center of the box is on " << 
    skyscan.bsc_to_fbpc(vgl_point_3d<double>((minx+maxx)/2, (miny+maxy)/2, (minz+maxz)/2) )<<"\n";
  return 0;
}

#include "vol3d_reader.h"

#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_load.h>
#include <vcl_fstream.h>


vol3d_reader::vol3d_reader(vcl_vector<vil_image_resource_sptr> &img_vec,
                           vgl_box_2d<int> const & roi)
:res_(0)
{
  // to include both the extreme points,adding 1 to roi.width() & roi.height()

  res_ =  (vil3d_new_image_resource(roi.width()+1, roi.height()+1, img_vec.size(), 
    1, VIL_PIXEL_FORMAT_UINT_16));
  int x_min = roi.min_x();
  int x_max = roi.max_x();
  int y_min = roi.min_y();
  int y_max = roi.max_y();

  // to include both the extreme points,adding 1 to roi.width() & roi.height()

  vil3d_image_view<vxl_uint_16> img_3d_view(roi.width()+1,roi.height()+1,img_vec.size(),1);
  img_3d_view.fill(0);

  unsigned int k = 0;
  for (vcl_vector<vil_image_resource_sptr>::iterator iter = img_vec.begin();iter != img_vec.end();iter++) {
    vil_image_view<vxl_uint_16> img_view = *((*iter)->get_copy_view(0,(*iter)->ni(),0,(*iter)->nj()));
    for (unsigned int i = x_min;i<=x_max;i++) {
      for (unsigned int j = y_min;j<=y_max;j++) {
        img_3d_view(i - x_min,j - y_min,k,0) = img_view(i,j);
      }
    }
    k++;
  }
  res_->put_view(img_3d_view);
}
  
vol3d_reader::vol3d_reader(vcl_string input_txt_file)
:res_(0)
{
   
  vcl_ifstream ifstr(input_txt_file.c_str());
  
  unsigned int dimx,dimy,dimz;

  ifstr >> dimx ;
  ifstr >> dimy ;
  ifstr >> dimz ;

  res_ =  (vil3d_new_image_resource(dimx,dimy,dimz,1,VIL_PIXEL_FORMAT_UINT_16));

  vil3d_image_view<vxl_uint_16> img_3d_view(dimx,dimy,dimz,1);
  img_3d_view.fill(0);

  //vcl_string txt_file = "C:\\scale_selection\\testing_vol3d_reader.txt";
  //
  //vcl_ofstream ofstr (txt_file.c_str());

  double value;
  vcl_vector<double> val_vec;
  for (unsigned int count = 0;count < dimx*dimy*dimz;count++) {
    ifstr >> value;
    val_vec.push_back(value);
  }
  ifstr.close();

  int count = 0;
  for (unsigned int k = 0;k<dimz;k++) {
    for (unsigned int j = 0;j<dimy;j++) {
        for (unsigned int i = 0;i<dimx;i++) {
          vxl_uint_16 &ref = img_3d_view(i,j,k,0);
          ref = val_vec[count];
          count++;
          //   ofstr << img_3d_view(i,j,k,0) <<" ";
         }
          //  ofstr << vcl_endl;
        }
      // ofstr << "end of slice " << k << vcl_endl;
      }
// ofstr.close();
  res_->put_view(img_3d_view);
}
  
vol3d_reader::vol3d_reader(vcl_string input_txt_file, vgl_box_3d<int> const &roi)
{
  
  // read the whole 3D data, if the box is empty
  if (roi.is_empty()) {
    res_ = vil3d_load_image_resource(input_txt_file.data()); 
    vcl_cout << "ni=" << res_->ni() << " nj=" << res_->nj() << 
      " nk=" << res_->nk() << vcl_endl;

  } else {
    int start = roi.min_z(); 
    int end = roi.max_z();

    // find the last ., right before the extension
    int cut_point = input_txt_file.rfind(".");

    // the images in the dataset are named in the format ...####.tif
    input_txt_file.erase(cut_point-4,8);

    vcl_vector<vil_image_resource_sptr> img_res_vec;
    vcl_vector<vcl_string> file_names;
    for (unsigned i = start;i<=end;i++)
      {
      vcl_string push_in_str = input_txt_file;
      vcl_string numstring;
      vcl_stringstream sstream;
      sstream << i; 
      sstream >> numstring;
      /*
      char buffer[30];
      itoa (i,buffer,10);
      */
      if(i < 10)
        push_in_str.append("000");
      else if(i < 100)
        push_in_str.append("00");
      else
        push_in_str.append("0");

//      push_in_str.append(buffer);
      push_in_str.append(numstring);
      push_in_str.append(".tif");
      file_names.push_back(push_in_str);
      }

  for (unsigned int i = 0; i < file_names.size();i++) {
    vil_image_resource_sptr res_sptr = vil_load_image_resource(file_names[i].c_str());
    img_res_vec.push_back(res_sptr);
  }

  vgl_box_2d<int> box_2d(roi.min_x(), roi.max_x(), roi.min_y(), roi.max_y());
  vol3d_reader vil_3d_data(img_res_vec, box_2d);
  vil3d_image_resource_sptr res_ = vil_3d_data.vil_3d_image_resource();
  }
}

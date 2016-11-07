//: 
// \file    vol3d_real_data_radius_detection_example.cxx
// \brief   a example of testing radius detection on real data
// \author  pradeep
// \date    Aug 2006


#include <vol3d/algo/io/vol3d_reader.h>
#include <vol3d/algo/vol3d_radius_detection.h>
#include <vil/vil_load.h>

#include <vcl_cstddef.h>
#include <vcl_cstring.h>
#include <vcl_limits.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <biov/biov_examiner_tableau.h>
#include <vcl_limits.h>
#include <vil3d/algo/vil3d_histogram.h>
#include <vil3d/vil3d_save.h>
#include <bsta/bsta_histogram.h>
#include <bsta/bsta_otsu_threshold.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>

#include <bgui3d/bgui3d.h>

biov_examiner_tableau_new setup_biov_examiner_tableau(SbVec3s dim,uint8_t * voxels)
    {


    SoGroup *root = new SoGroup;
  
root->ref();
SoVolumeRendering::init();

  // Add SoVolumeData to scene graph
  SoVolumeData * volumedata = new SoVolumeData();
  volumedata->setVolumeData(dim, voxels, SoVolumeData::UNSIGNED_BYTE);
  root->addChild(volumedata);
  
  // Add TransferFunction (color map) to scene graph

 SoTransferFunction * transfunc = new SoTransferFunction();
  transfunc->predefColorMap = SoTransferFunction::PHYSICS;
  root->addChild(transfunc);
  
  // Add VolumeRender to scene graph
  SoVolumeRender * volrend = new SoVolumeRender();
  root->addChild(volrend);

  biov_examiner_tableau_new tab3d(root, transfunc);
  // root->unref();
  return tab3d;
    }



int main(int argc, char * argv[])
    {
vcl_string input = argv[1];
unsigned int start = atoi(argv[2]);
unsigned int end = atoi(argv[3]);

int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);
    delete []my_argv;

vcl_vector<vcl_string>file_names;
int cut_point = input.find(".tif");

// the images in the dataset are named in the format ...####.tif
  input.erase(cut_point-4,8);

vcl_vector<vil_image_resource_sptr>img_res_vec;

  for (unsigned i = start;i<=end;i++)
      {
     vcl_string push_in_str = input;
      char buffer[30];

      itoa (i,buffer,10);

       if(i < 10)
           push_in_str.append("000");
        else if(i < 100)
            push_in_str.append("00");
        else
          push_in_str.append("0");

        push_in_str.append(buffer);
        push_in_str.append(".tif");

      file_names.push_back(push_in_str);
      }

for (unsigned int i = 0;i<=end - start;i++)
    {
    vil_image_resource_sptr res_sptr = vil_load_image_resource(file_names[i].c_str());
    img_res_vec.push_back(res_sptr);
   
    }


//  vgl_box_2d<int> box(510,529,309,348);
  vgl_box_2d<int> box (617,722,612,707);
//   vgl_box_2d<int> box(470,527,68,109);

vol3d_reader vil_3d_data(img_res_vec,box);
vil3d_image_resource_sptr img_sptr = vil_3d_data.vil_3d_image_resource();


vil3d_image_view<vxl_uint_16> img_3d = img_sptr->get_view();

 bgui3d_init();

  vcl_vector<double>volume_dat;

  double min_val = vcl_numeric_limits<float >::max();
    double max_val = vcl_numeric_limits<float >::min();
    double avg;


 

    for (unsigned int k = 0;k<img_sptr->nk();k++)
      {
        for (unsigned int j = 0;j<img_sptr->nj();j++)
          {
          for (unsigned int i = 0;i<img_sptr->ni();i++)
              {
              volume_dat.push_back(img_3d(i,j,k,0));
           
               if (min_val>img_3d(i,j,k,0))
    min_val = img_3d(i,j,k,0);
    if (max_val<img_3d(i,j,k,0))
max_val = img_3d(i,j,k,0);
              }
          }
      }




  vbl_array_3d<double>vol_array(img_sptr->ni(),img_sptr->nj(),img_sptr->nk());

  for (unsigned int i = 0;i < vol_array.get_row1_count();i++)
      for (unsigned int j = 0;j < vol_array.get_row2_count();j++)
          for (unsigned int k = 0;k < vol_array.get_row3_count();k++)
              {
              vol_array[i][j][k] = img_3d(i,j,k,0);
              }



  avg = (min_val + max_val)/2;

  vcl_vector<double> count;
 vil3d_histogram(img_3d,count,min_val,max_val,max_val - min_val + 1);

 bsta_histogram<double> b_hist(min_val,max_val,count);
 bsta_otsu_threshold<double> b_thresh(b_hist);      
unsigned int marcate = b_thresh.bin_threshold();
double std = vcl_sqrt(b_hist.variance(0,marcate));



  vol3d_radius_detection rd(2,8);
// vbl_array_3d<double> rads = rd.radius(vol_array,3000);
  vbl_array_3d<double> rads = rd.radius(vol_array,2000/3,float(0.3));
 // vbl_array_3d<double> rads = rd.radius(img_sptr, 17300, double(2));
 
  //vol3d_radius_detection rd(2, 16);

  //vbl_array_3d<double> rads = rd.radius(img_sptr, 1, double(0));

  vbl_array_3d<unsigned char>result_vol(img_sptr->ni(),img_sptr->nj(),img_sptr->nk()); 

  for(int i = 0; i < img_sptr->ni(); i ++)
    for(int j = 0; j < img_sptr->nj(); j++)
      for(int k = 0; k < img_sptr->nk(); k ++)
        result_vol[i][j][k] = (unsigned char)rads[i][j][k]*20;

 vbl_array_3d<unsigned char> result_vol_shuffled(img_sptr->nk(),img_sptr->nj(),img_sptr->ni());
  for (int k = 0;k < img_sptr->nk();k++)
 for(int j = 0; j < img_sptr->nj(); j++)
  for(int i = 0; i < img_sptr->ni(); i ++)           
      result_vol_shuffled[k][j][i] = result_vol[i][j][k];

 
 vil3d_image_view<unsigned char> output_view(img_sptr->ni(),img_sptr->nj(),img_sptr->nk(),1);

for(int k = 0; k < img_sptr->nk(); k ++)
    for(int j = 0; j < img_sptr->nj(); j++)
      for(int i = 0; i < img_sptr->ni(); i ++)
          {
          unsigned char &ref = output_view(i,j,k,0);
        ref = rads[i][j][k];
          }

        vcl_string output = "C:\\scale_selection\\gipl\\mercox_dataset.gipl";

vil3d_save(output_view,output.c_str());

vcl_string txt_file_2 = "C:\\scale_selection\\radius_labeling_for_mercox_data.txt"; 
          vcl_ofstream stream(txt_file_2.c_str());

  
      for(int k = 0; k<rads.get_row3_count(); k++)
      {
      for(int i = 0; i <rads.get_row1_count(); i++)  
        {
       for(int j = 0; j < rads.get_row2_count(); j++)
          {
          
stream << rads[i][j][k] << " " ;
          }
      stream << vcl_endl;
        }
stream << "end of row " << k << vcl_endl;
    }
stream.close();

                    
 const size_t blocksize = volume_dat.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

   for(unsigned long i = 0; i < blocksize; i++)
 voxels[i] =  result_vol_shuffled.data_block()[i];
  

 

SbVec3s dim = SbVec3s(img_sptr->ni(),img_sptr->nj(),img_sptr->nk());

  biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;

    }

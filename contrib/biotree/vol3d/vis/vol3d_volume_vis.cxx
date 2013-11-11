//: 
// \file   vol3d_volume_vis.cxx
// \brief  visualizer for the 3D volume data 
// \author Gamze Tunali
// \date   May 2006
// 

#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif

#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_load.h>
#include <vil3d/vil3d_save.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vgl/vgl_box_3d.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui3d/bgui3d.h>
#include <biov/biov_examiner_tableau.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>
#include <vol3d/algo/io/vol3d_reader.h>

#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoCylinder.h>

/*template <class T>
vbl_array_3d<double> 
convert_to_vbl_array(vil3d_image_view_base_sptr view_sptr,
                     int min_x, int min_y, int min_z,
                     int max_x, int max_y, int max_z)
{
  int x = max_x - min_x;
  int y = max_y - min_y;
  int z = max_z - min_z;
  vbl_array_3d<double> volume(x, y, z);
  


  for (int k=0; k<=z; k++) {
    for (int j=0; j<=y; j++) {
      for (int i=0; i<=x; i++) {  
        vil3d_image_view<T> view = *view_sptr;
        if ((i > 0) && (j>0) && (k>0) && 
            (i < static_cast<int> (view.ni())) && 
            (j < static_cast<int> (view.nj())) && 
            (k < static_cast<int> (view.nk())))  {
          T voxel_val = 0;
          voxel_val = view(i+min_x, j+min_y, k+min_z);
          double val = voxel_val;
          volume[i][j][k] = val;
        } else {
          volume[i][j][k] = 0.0;
        }
        
      }
    }
  }
  return volume;
}*/

int main(int argc, char** argv)
{
  // initialize vgui
  // Ming: force option "--mfc-use-gl" to use gl
  //       so that it is MUCH faster if running on a
  //       computer with nice graphic card.
  //vgui::init(argc, argv);
  int my_argc = argc+1;
  char** my_argv = new char*[argc+1];
  for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
  my_argv[argc] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // initialize bgui_3d
  bgui3d_init();

  vcl_string fname="", box_fname="", scan_fname="", log_fname="";
  double max_intensity = -1e23;
  double min_intensity = 1e23;
  double intensity=0;

  // Parse arguments
  for (int i = 1; i < argc; i++) {
    vcl_string arg (argv[i]);

    // this argument can be a volume data file like *.gipl or a path with wild characters to the 
    // reconstructed images like: C:\\test_images\\filters\\newcast35um2_orig\\scan35um_rec####.bmp
    if (arg == vcl_string ("-x")) { fname = vcl_string(argv[++i]);}

    else if (arg == vcl_string ("-b")) {box_fname = vcl_string(argv[++i]);}

    else if (arg == vcl_string ("-s")) {scan_fname = vcl_string(argv[++i]);}

    else if (arg == vcl_string ("-l")) {log_fname = vcl_string(argv[++i]);}

    else
    {
      vcl_cout << "Usage: " << argv[0] << "[-x 3D data file(s)][-b box][-s scan][-l log] " << vcl_endl;
      throw -1;
    }
  }
  
  if (  fname == ""){
    vcl_cout << "Data File not specified" << vcl_endl; 
    return(1);
  }

  vgl_box_3d<double> box;
  if (box_fname == "") {
    // visualize the whole area
    vcl_cout << "BOX is empty, will show the whole 3D data" << vcl_endl;
    box.empty();

  } else {
    vcl_ifstream box_file(box_fname.c_str());
    box.read(box_file);
    box_file.close();
    vcl_cout << "BOX\n" << box << vcl_endl;
    xscan_scan scan;

    if ((scan_fname != "") && (log_fname != "")){
      vcl_ifstream scan_file(scan_fname.data());
      scan_file >> scan;
      scan_file.close();
      imgr_skyscan_reconlog header(log_fname.data(), scan);

      vgl_point_3d<double> min = header.bsc_to_fbpc(box.min_point());
      vgl_point_3d<double> max = header.bsc_to_fbpc(box.max_point());
   
      box.set_min_point(min);
      box.set_max_point(max);
    }
  } 

  vgl_box_3d<int> recon_box(box.min_x(), box.min_y(), box.min_z(), box.max_x(), 
    box.max_y(), box.max_z());

  vcl_vector<double> field;
  //vcl_ofstream fs("C:\\test_images\\filters\\mercox\\recon_experiments\\vol.txt");
  vcl_ofstream fs("C:\\test_images\\filters\\newcast35um_reconstructed\\vol.txt");

  vol3d_reader reader(fname, recon_box);
  vil3d_image_resource_sptr img_res_sptr = reader.vil_3d_image_resource();
  vil3d_image_view_base_sptr view_3d = img_res_sptr->get_view();

  for (unsigned k = 0; k < img_res_sptr->nk(); k++) {
    fs << "z=" << img_res_sptr->nk() << vcl_endl;
    for (unsigned j = 0; j < img_res_sptr->nj(); j++) {
      for (unsigned i = 0; i < img_res_sptr->ni(); i++) {  
        if (view_3d->pixel_format() == VIL_PIXEL_FORMAT_BYTE) {
          vil3d_image_view<unsigned char> view = *view_3d;
          unsigned char voxel_val;
          if ((i > 0) && (j>0) && (k>0) && 
              (i<view.ni()) && (j<view.nj()) && (k<view.nk())) 
              voxel_val = view(i,j,k);
          else
              voxel_val = 0;
          intensity = (int) voxel_val;
          fs << intensity << " ";
        } else if (view_3d->pixel_format() == VIL_PIXEL_FORMAT_UINT_16) {
            vil3d_image_view<unsigned short> view = *view_3d;
            unsigned short voxel_val;
            if ((i > 0) && (j>0) && (k>0) && 
              (i<view.ni()) && (j<view.nj()) && (k<view.nk())) 
              voxel_val = view(i,j,k);
            else
              voxel_val = 0;
            intensity = voxel_val;
            fs << intensity << " ";
        } else if (view_3d->pixel_format() == VIL_PIXEL_FORMAT_FLOAT) {
            vil3d_image_view<float> view = *view_3d;
            float voxel_val;
            if ((i > 0) && (j>0) && (k>0) && 
              (i<view.ni()) && (j<view.nj()) && (k<view.nk())) 
              voxel_val = view(i,j,k);
            else
              voxel_val = 0;
            intensity = voxel_val;
            fs << intensity << " ";
        } else if (view_3d->pixel_format() == VIL_PIXEL_FORMAT_DOUBLE) {
            vil3d_image_view<double> view = *view_3d;
            double voxel_val;
            if ((i > 0) && (j>0) && (k>0) && 
              (i<view.ni()) && (j<view.nj()) && (k<view.nk())) 
              voxel_val = view(i,j,k);
            else
              voxel_val = 0;
            intensity = voxel_val;
            fs << intensity << " ";
        } else {
          vcl_cerr << "Pixel Format " << view_3d->pixel_format() << " is not supported" ;
        }

        if(max_intensity < intensity)
            max_intensity = intensity;
          if(min_intensity > intensity)
            min_intensity = intensity;
          field.push_back(intensity);
        }
        fs << vcl_endl;
      }
      fs << vcl_endl;
    }

  const size_t blocksize = field.size(); 
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

  vcl_cout << field.size() << vcl_endl;
  for(unsigned long i = 0; i < blocksize; i++) {
    voxels[i] = static_cast<uint8_t>((field[i] - min_intensity)*255/(max_intensity - min_intensity));
  }
  
  SbVec3s dim = SbVec3s(img_res_sptr->ni()+1, img_res_sptr->nj()+1, img_res_sptr->nk()+1);

  // visualization
  bgui3d_init();
  // make scene contain camera and light
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
  root->unref();
  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  return return_value;
}

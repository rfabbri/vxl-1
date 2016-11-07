//:
// \file  biov_main.cxx
// \brief  Example volume rendering of a directory of 16-bit tif images.
// \author    Jason Mallios
// \date        2005-07-05
//
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vil/vil_image_view.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_convert.h>
#include <vil3d/vil3d_new.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/file_formats/vil3d_slice_list.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/vil3d_math.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_pixel_format.h>

#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_menu.h>
#include <biov/biov_examiner_tableau.h>
#include <bgui3d/bgui3d.h>
#include <slice/sliceFileManager.h>

#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoClipPlane.h>
#include <Inventor/manips/SoClipPlaneManip.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoObliqueSlice.h>
#include <VolumeViz/nodes/SoOrthoSlice.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/SbImage.h>

// If there is a performance lag and you set
// My Computer>Properties>Advanced>Environment Variables>User variables>CVR_DEBUG to 1
// and receive the following output, make sure your OpenGL driver is at least 1.2
// I don't know how to do that, so good luck.
//Coin info in SoVolumeRender::GLRender(): The OpenGL driver does not support 3D texturing, will fall back on 2D textures.


template <class T>
inline void my_convert_given_range(const vil3d_image_view<T>& src,
                vil3d_image_view<vxl_byte>& dest,
                const T& min_b,
                const T& max_b)
{
        /*
        T min_b;
        T max_b;
  vil3d_math_value_range(src,min_b,max_b);
  */
  double a = -1.0*double(min_b);
  double b = 0.0;
  if (max_b-min_b >0) b = 255.0/(max_b-min_b);
  dest.set_size(src.ni(), src.nj(), src.nk(), src.nplanes());
  for (unsigned p = 0; p < src.nplanes(); ++p)
    for (unsigned k = 0; k < src.nk(); ++k)
      for (unsigned j = 0; j < src.nj(); ++j)
        for (unsigned i = 0; i < src.ni(); ++i){
                dest(i,j,k,p) = static_cast<vxl_byte>( b*( src(i,j,k,p)+ a ) );
                if(src(i,j,k,p) < min_b) dest(i,j,k,p) = 0;
                if(src(i,j,k,p) > max_b) dest(i,j,k,p) = 255;
                //vcl_cerr << src(i,j,k,p) << " --> " << (int)dest(i,j,k,p) << "\n" ;
        }
}


int main(int argc, char** argv)
{

  // initialize vgui
#if _WIN32
    // NECESSARY TO TAKE ADVANTAGE OF 3D TEXTURES USING MFC
    int my_argc = argc+1;
    char** my_argv = new char*[argc+1];
    for (int i=0; i<argc; i++)
    my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc, my_argv);
    delete []my_argv;
#else
    vgui::init(argc, argv);
#endif

    if(argc < 2){
                vcl_cerr << "usage : " << argv[0] << " <file>" << "\n" ;
                return 1;
    }
    vcl_string file = argv[1];
    vil3d_image_view<vxl_byte> volume_byte;
    if (file.find_first_of("#",0) == file.npos)
    {
    //reading from a binary file 
            vil3d_image_view<float> volume;
            vsl_b_ifstream bfs_in(file.c_str());
            vsl_b_read(bfs_in, volume);
            bfs_in.close();

            if(volume.ni() == 0 || volume.nj() == 0 || volume.nk() == 0){
                    vcl_cerr << "unable to load straight vil3d; trying sliceFile\n";
                    volume = sliceFileManager<float>::read(file);
            }
            if(volume.ni() == 0 || volume.nj() == 0 || volume.nk() == 0){
                    vcl_cerr << "unable to load image, exiting\n";
                    return 1;
            }

            /*
            vcl_cerr << "taking log ... ";
            for(int k = 0; k < volume.nk(); k++){
            for(int j = 0; j < volume.nj(); j++){
            for(int i = 0; i < volume.ni(); i++){
                    if(volume(i,j,k) != 0)
                    volume(i,j,k) = vcl_log(volume(i,j,k));
                    
            }}}
            vcl_cerr << " done\n";

            */


            float min,max;
            vil3d_math_value_range(volume,min,max);
            vcl_cerr << "stretching to range " << min << " " << max << "\n";
            vil3d_convert_stretch_range(volume,volume_byte);

            if(argc > 2 
                            && (vcl_strncmp(argv[argc-1],"mip",3) != 0)
                            && (vcl_strncmp(argv[argc-1],"sum",3) != 0) 
                            ){
                    vcl_cerr << "reading in " << argv[2] << "\n";
                    vil3d_image_view<float> volume2;
                    vsl_b_ifstream bfs_in(argv[2]);
                    vsl_b_read(bfs_in, volume2);
                    bfs_in.close();

                    vcl_cerr << "creating new image resource\n";
                    vil3d_image_resource_sptr res = vil3d_new_image_resource(volume.ni(),
                                                                 volume.nj(),
                                                                 volume.nk() + volume2.nk(),
                                                                 1,
                                                                 volume.pixel_format());
                    res->put_view(volume,0,0,0);
                    res->put_view(volume2,0,0,volume.nk());
                    vcl_cerr << "getting view of image resource\n";
                    vil3d_image_view<float> combined = res->get_view();
                    vcl_cerr << "combined size is " 
                            << combined.ni() << " " << combined.nj() << " " << combined.nk() << "\n";
                    vcl_cerr << "converting to volume_byte\n";


                    vsl_b_ofstream bfs_out("combined.float");
                    vsl_b_write(bfs_out, combined);
                    bfs_out.close();

                    vil3d_convert_stretch_range(combined,volume_byte);

            } 
    }
    //reading from a slice list, i.e.  image###.tif
    else{
            vil3d_slice_list_format format;
            vil3d_image_resource_sptr res = format.make_input_image(file.c_str());
            vcl_cerr << "loading from " << file 
                    << " res size is " 
                    << res->ni () << " " << res->nj() << " " << res->nk() << "\n" ;
        int minx = 0;
        int miny = 0;
        int minz = 0;
        int dimx = res->ni();
        int dimy = res->nj();
        int dimz = res->nk();


        if(argc > 7){
                minx = vcl_atoi(argv[2]);
                miny = vcl_atoi(argv[3]);
                minz = vcl_atoi(argv[4]);
                dimx = vcl_atoi(argv[5]);
                dimy = vcl_atoi(argv[6]);
                dimz = vcl_atoi(argv[7]);
        }

        vcl_cerr << "getting from " << minx << " " << miny << " " << minz << " dimension " << dimx << " " << dimy << " " << dimz << "\n";
        if(res->pixel_format() == VIL_PIXEL_FORMAT_UINT_16){
                vcl_cerr << "VIL_PIXEL_FORMAT_UINT_16\n";
                vil3d_image_view<vxl_uint_16> view = res->get_view(minx,dimx,miny,dimy,minz,dimz);
                unsigned short min,max;
                vil3d_math_value_range(view,min,max);
                vcl_cerr << "image range " << min << " " << max << "\n";

                vcl_cerr << "view size = " << view.ni() << " "    << view.nj() << " "     << view.nk() << vcl_endl; 
                //vil3d_convert_stretch_range(view,volume_byte);
                my_convert_given_range(view,volume_byte,(vxl_uint_16)0000,(vxl_uint_16)65535);
        }
        else if(res->pixel_format() == VIL_PIXEL_FORMAT_FLOAT){
                vcl_cerr << "VIL_PIXEL_FORMAT_FLOAT\n";
            vil3d_image_view<float> view = res->get_view(minx,dimx,miny,dimy,minz,dimz);
                float min,max;
                vil3d_math_value_range(view,min,max);
                vcl_cerr << "image range " << min << " " << max << "\n";

                /*
            vcl_cerr << "taking log ... ";
            for(int k = 0; k < view.nk(); k++){
            for(int j = 0; j < view.nj(); j++){
            for(int i = 0; i < view.ni(); i++){
                    if(view(i,j,k) != 0)
                    view(i,j,k) = vcl_log(view(i,j,k));
                    
            }}}
            vcl_cerr << " done\n";
            */

                vcl_cerr << "view size = " << view.ni() << " "    << view.nj() << " "     << view.nk() << vcl_endl; 
                vil3d_convert_stretch_range(view,volume_byte);

        }
        else{
                vcl_cerr << "assuming byte ...\n";
                vil3d_image_view<vxl_byte> view = res->get_view(minx,dimx,miny,dimy,minz,dimz);
                volume_byte = view;
        }
    }

    //-----------------------------------------------------------------



    vcl_cerr << "volume size = " << volume_byte.ni() << " "    
            << volume_byte.nj() << " "     
            << volume_byte.nk() << vcl_endl; 

    vxl_byte min,max;
    vil3d_math_value_range(volume_byte,min,max);
    vcl_cerr << " Min = " << (int)min << " Max = " << (int)max << "\n" ;
    bgui3d_init();
  // make scene containing camera and light
  SoGroup *root = new SoGroup;

  root->ref();

  SoVolumeRendering::init();
  

  // Add SoVolumeData to scene graph
  SoVolumeData * volumedata = new SoVolumeData();
  volumedata->storageHint.setValue(SoVolumeData::TEX3D);
//  volumedata->setPageSize(SbVec3s(512,512,512));
  //volumedata->useCompressedTexture.setValue(0);
  //volumedata->usePalettedTexture.setValue(1);
  volumedata->setVolumeData(SbVec3s(volume_byte.ni(), volume_byte.nj(),volume_byte.nk()), 
                  volume_byte.origin_ptr(), 
                  SoVolumeData::UNSIGNED_BYTE);
  root->addChild(volumedata);



  // Add TransferFunction (color map) to scene graph
  SoTransferFunction * transfunc = new SoTransferFunction();
  transfunc->predefColorMap = SoTransferFunction::PHYSICS;
  transfunc->reMap(1,255);
  root->addChild(transfunc);



  /*
  SoObliqueSlice* slice = new SoObliqueSlice();
  slice->alphaUse = SoObliqueSlice::ALPHA_OPAQUE;
  slice->plane.setValue(SbPlane(SbVec3f(1,1,0), 0)); 
  root->addChild(slice);
  */



  /*
  SoGetBoundingBoxAction ba(ex->getViewportRegion());
  ba.apply(root);
  SbBox3f box = ba.getBoundingBox();
  */
  SbBox3f box(0,0,0,1,1,1);
  SoClipPlaneManip * manip = new SoClipPlaneManip;
  manip->setValue(box, SbVec3f(0.0f, 0.0f, 1.0f), -1.02f);
  root->insertChild(manip, 0);





  // Add VolumeRender to scene graph
  SoVolumeRender * volrend = new SoVolumeRender();
  if(argc > 2 && !vcl_strncmp(argv[argc-1],"mip",3)){
          volrend->composition = SoVolumeRender::MAX_INTENSITY;
  }
  else if(argc > 2 && !vcl_strncmp(argv[argc-1],"sum",3)){
          volrend->composition = SoVolumeRender::SUM_INTENSITY;
  }
  else{
          volrend->interpolation.setValue(SoVolumeRender::NEAREST);
  }
  volrend->viewAlignedSlices.setValue(1);
  root->addChild(volrend);
  SoSeparator *sep = new SoSeparator();
  root->addChild(sep);

  biov_examiner_tableau_new tab3d(root, transfunc);

  root->unref();

  vgui_shell_tableau_new shell(tab3d);

  int return_value = vgui::run(shell, 400, 400, tab3d->create_menus());

  return 0;
}

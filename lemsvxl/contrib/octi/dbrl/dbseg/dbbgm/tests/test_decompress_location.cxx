#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <dbbgm/bbgm_image_of.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>
#include <dbbgm/bbgm_wavelet_compressor.h>
#include <vcl_string.h>
#include <dbbgm/bbgm_wavelet.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/vis/dbbgm_image_tableau.h>
#include <vul/vul_arg.h>
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_image_tableau.h>
#include <vgui/vgui_composite_tableau.h>
#include <vgui/vgui_grid_tableau.h>
#include <vgui/vgui_range_map_params.h>

static void test_decompress_location(int argc, char* argv[])

{
	if (argc<2){
		vcl_cerr<<"no path provided !";
		return;
	}
	vul_arg<char*> initPath("-b","Base Path","C:\\");
	vul_arg_parse(argc,argv);
	
	vcl_string basePath=vul_file::dirname(initPath.value_);
	vcl_string wv_path=basePath+"\\out_wv";
	vcl_string bg_path=basePath+"\\out_bg";
	vul_file::make_directory_path(wv_path.c_str());
	vul_file::make_directory_path(bg_path.c_str());
	bbgm_wavelet_compressor* BWC = new bbgm_wavelet_compressor(argc,argv);
	
	
	//BWC->wavelet_decomposition_full(2,2);
	//BWC->saveWaveletVector(wv_path,vcl_string("wv_"),vcl_string("tiff"));
	
	//vil_save(im2->spatialDomain(),(wv_path+"\\LL_TRUE.tiff").c_str());
	//BWC->saveWaveletVector(wv_path,vcl_string("img_"),vcl_string("tiff")); */
	int level=3;
	bbgm_wavelet<vil_image_view <float> > im=BWC->wavelet_decompositionAt(level,10,0);
	//vil_image_view <float> imLL1=im.getSubband(LL,3);
	//vil_save(imLL1,(wv_path+"\\LL3.tiff").c_str());
	//vil_save(BWC->testInterpImage(),(wv_path+"\\interp_image.tiff").c_str());
	//vil_image_view<float> viewSpatial=BWC->waveletAt(3)->spatialDomain();
	//BWC->trainModel(0,-1);
	//BWC->saveModel(false,(bg_path+"\\bgmodel.mdl"));
	
	int fact=int(pow(2.0,level));
	int ni=BWC->spatialAt(0).ni();
	int nj=BWC->spatialAt(0).nj();
	
	vil_image_view <float> output(ni,nj);
	vil_image_view<float> subband=im.getSubband(LL,0);
	
	float tmp;
	for (unsigned int i=180,ii=0;i<260;i++,ii++)
		for(unsigned int j=180,jj=0;j<260;j++,jj++)
		{
		  im.decompressLocation(i,j,tmp);
		  output(ii,jj)=tmp;
		  vcl_cout<<tmp<<" and the actual location "<<(BWC->spatialAt(0))(i,j)<<vcl_endl; 
		}
			
	//BWC->loadModel(bg_path+"\\bgmodel.mdl",true);
	//dbbgm_image_tableau_new bg_tab(BWC->model()); 
	bgui_image_tableau_new wv_tab(*im.waveletDomain());
	bgui_image_tableau_new output_tab(output);
	bgui_image_tableau_new subband_tab(subband);
	char** my_argv = new char*[1];
	my_argv[0] = new char[13];
	char* temp = my_argv[0];
	vcl_strcpy(my_argv[0], "--mfc-use-gl");
	 
	int argcc=1;
   // Initialize the toolkit.
    vgui::init(argcc, my_argv);

	
	 vgui_grid_tableau_new grid_tab(2,2);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(wv_tab), 0,0);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(subband_tab), 1,0);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(output_tab), 0,1);
	 wv_tab->set_mapping(new vgui_range_map_params(0,1,3.0f));
	 output_tab->set_mapping(new vgui_range_map_params(0,1,3.0f));
	 subband_tab->set_mapping(new vgui_range_map_params(0,1,3.0f));
	 vgui_shell_tableau_new shell(grid_tab);
	 vgui::run(shell, 1280, 720);
	 

    // Put a shell tableau at the top of our tableau tree.
   
  	
}



TESTMAIN_ARGS(test_decompress_location);
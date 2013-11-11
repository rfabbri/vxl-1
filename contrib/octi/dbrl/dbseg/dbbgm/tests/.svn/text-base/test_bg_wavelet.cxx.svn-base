#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vul/vul_file.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_mixture_fixed.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/algo/bsta_adaptive_updater.h>
#include <dbbgm/bbgm_update.h>
#include <dbsta/bsta_gaussian_indep.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_random.h>
#include <dbbgm/bbgm_wavelet_compressor.h>
#include <vcl_string.h>
#include <vcl_sstream.h>
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
#include <dbbgm/vis/dbbgm_inspector_tool.h>

static void test_bg_wavelet(int argc, char* argv[])

{
	if (argc<2){
		vcl_cerr<<"no path provided !";
		return;
	}
	vul_arg<char*> initPath("-b","Base Path","C:\\");
	vul_arg<int> level_arg("-l","Level",0);
	vul_arg_parse(argc,argv);
	
	vcl_string basePath=vul_file::dirname(initPath.value_);
	vcl_string wv_path=basePath+"\\out_wv";
	vcl_string bg_path=basePath+"\\out_bg";
	vul_file::make_directory_path(wv_path.c_str());
	vul_file::make_directory_path(bg_path.c_str());
	bbgm_wavelet_compressor* BWC = new bbgm_wavelet_compressor(argc,argv);
	
	/*
	BWC->wavelet_decomposition_full(2,2);
	BWC->saveWaveletVector(wv_path,vcl_string("wv_"),vcl_string("tiff"));
	
	vil_save(im2->spatialDomain(),(wv_path+"\\LL_TRUE.tiff").c_str());
	BWC->saveWaveletVector(wv_path,vcl_string("img_"),vcl_string("tiff")); */
	
	//bbgm_wavelet<vil_image_view <float> > im=BWC->wavelet_decompositionAt(3,2,0);
	//vil_image_view <float> imLL1=im.getSubband(LL,3);
	//vil_save(imLL1,(wv_path+"\\LL3.tiff").c_str());
	//vil_save(BWC->testInterpImage(),(wv_path+"\\interp_image.tiff").c_str());
	//vil_image_view<float> viewSpatial=BWC->waveletAt(3)->spatialDomain();
	//BWC->trainModel(0,-1);
	BWC->saveModel(true,(bg_path+"\\bgmodel.mdl"));
	BWC->loadModel(bg_path+"\\bgmodel.mdl",true);
	int level=level_arg.value_;
	int wv=2;
	BWC->wavelet_decomposition_model(level,wv);
	BWC->saveWaveletModel(bg_path+"\\wvModel.mdl");
	
	//BWC->loadWaveletModel(bg_path+"\\wvModel.mdl");
	
	
	
	vcl_stringstream mystream;
	mystream<<"mmerge and "<<level<<" level decomposition "<<" and wv no "<<wv<<"_";
	BWC->saveBackgroundVideoSequence(0,1,0.2f,false,bg_path,mystream.str(),vcl_string("bmp"),level);
    //BWC->testInterp();
	//BWC->displayParam(true,"variance",0,true,(wv_path+"\\mean_c0.tiff"));
	BWC->displayParam(true,"mean",0,true,(wv_path+"\\wv_mean_c0.tiff"));
	BWC->displayParam(false,"mean",0,true,(wv_path+"\\mean_c0.tiff"));
	BWC->displayParam(true,"mean",0,false,(wv_path+"\\wv_mean_c0_nsc.tiff"));
	BWC->displayParam(false,"mean",0,false,(wv_path+"\\mean_c0_nsc.tiff"));

	BWC->displayParam(true,"variance",0,false,(wv_path+"\\wv_var_c0_nsc.tiff"));
	BWC->displayParam(false,"variance",0,false,(wv_path+"\\var_c0_nsc.tiff"));
	BWC->displayParam(true,"variance",0,true,(wv_path+"\\wv_var_c0.tiff"));
	BWC->displayParam(false,"variance",0,true,(wv_path+"\\var_c0.tiff"));

	BWC->displayParam(true,"weight",0,false,(wv_path+"\\wv_wt_c0_nsc.tiff"));
	BWC->displayParam(false,"weight",0,false,(wv_path+"\\wt_c0_nsc.tiff"));
	BWC->displayParam(true,"weight",0,true,(wv_path+"\\wv_wt_c0.tiff"));
	BWC->displayParam(false,"weight",0,true,(wv_path+"\\wt_c0.tiff"));
	BWC->displayParam(false,"mean",0,true,(wv_path+"\\mean_c0.tiff"));
		
	dbbgm_image_tableau_new bg_tab(BWC->model()); 
	dbbgm_image_tableau_new wv_tab(BWC->wvModel());

	bbgm_image_sptr wvSubband=BWC->modelWavelet()->getSubband(LL,0);
	dbbgm_image_tableau_new subband_tab(wvSubband);
	bgui_image_tableau_new prob_map_tab(BWC->pMap());
	
	char** my_argv = new char*[1];
	my_argv[0] = new char[13];
	char* temp = my_argv[0];
	vcl_strcpy(my_argv[0], "--mfc-use-gl");
	 
	int argcc=1;
   // Initialize the toolkit.
    vgui::init(argcc, my_argv);

	
	 vgui_grid_tableau_new grid_tab(2,2);
	 wv_tab->set_mapping(new vgui_range_map_params(0,1,0,1,0,1,3.0f,3.0f,3.0f,false,false,false));
	 prob_map_tab->set_mapping(new vgui_range_map_params(0,1,1.0f));
	 grid_tab->add_at(vgui_viewer2D_tableau_new(bg_tab), 0,0);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(subband_tab), 1,0);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(wv_tab), 0,1);
	 grid_tab->add_at(vgui_viewer2D_tableau_new(prob_map_tab), 1,1);
	 vgui_shell_tableau_new shell(grid_tab);
	 vgui::run(shell, 1280, 720);
	 

    // Put a shell tableau at the top of our tableau tree.
   
  
	
	
	//vil_save(viewSpatial,(wv_path+"\\Spatial.tiff").c_str());
	
}



TESTMAIN_ARGS(test_bg_wavelet);
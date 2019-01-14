// This is breye/vorl/exec/vorl_fgdetect_con_finder_exec.cxx
//:
// \file
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <fstream>
#include <cstdio>

#include <vidpro/process/vidpro_save_con_process.h>

#include <vorl/vorl.h>
#include <dbbgm/pro/dbbgm_aerial_bg_model_process.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process1.h>
//#include <dbvrl/pro/dbvrl_register_images_process.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <bpro/bpro_parameters_sptr.h>
#include <bpro/bpro_process_sptr.h>

#include <dbbgm/pro/dbbgm_load_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <vidpro/vidpro_repository.h>
#include <vnl/vnl_matrix.h>
#include <vimt/vimt_transform_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <fstream>
#include <vbl/vbl_bounding_box.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>

void 
write_polygons(std::string filenaming,std::vector<std::vector<vsol_polygon_2d_sptr> > polygons,std::ofstream &ofile,
               int ni,int nj,int s0, int se)
{



  std::string con_file_rootname="aa";


  std::cout<< polygons.size()<<std::endl;

  //unused bool first = true;

  // 4-23-2007 reversed again..
  s0=polygons.size()-1;
  
  for (unsigned int i = 0 ; i < polygons.size() ; i++ )
  {
    std::ofstream outfp;



    //create a unique name for this contour based on the root filename
    char filename[200];
    //std::sprintf(filename, "%s_%2d.con", con_file_rootname.c_str(), i);
    
    // 4-23-2007 reversed again..
   /*
    if (i<10) std::sprintf(filename, "0000%1d.con", (int)s0);
    else if (i>99) std::sprintf(filename, "00%3d.con", (int)s0);
    else  std::sprintf(filename, "000%2d.con", (int)s0);*/


    if (s0<10) std::sprintf(filename, "0000%1d.con", (int)s0);
    else if (s0>99) std::sprintf(filename, "00%3d.con", (int)s0);
    else  std::sprintf(filename, "000%2d.con", (int)s0);

    // 4-23-2007 reversed again..
    //s0++;
    s0--;



    std::string fn=filenaming+"//"+filename;
    //create the file
    //outfp.open(filename, std::ios::out);
    outfp.open(fn.c_str(), std::ios::out);

    if (!outfp){
      std::cout << " Error writing file  " << filename << std::endl;
      return;// false;
    }

    //2) start writing out the contour to the file
    outfp << "CONTOUR" << std::endl;

    outfp << "CLOSE" << std::endl;


    //ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
    if (polygons[i].size()==0) continue;

    //for (unsigned ll=0;ll<polygons[i].size();ll++)
    //    if (
    outfp<<polygons[i][0]->size()<<"\n";
    //ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
    for(unsigned j=0;j<polygons[i].size();j++)
    {
      //ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
      //ofile<<"X: "<<"\n";
      for(unsigned k=0;k<polygons[i][j]->size();k++)
      {outfp<<polygons[i][j]->vertex(k)->x()<<" ";
      outfp<<polygons[i][j]->vertex(k)->y()<<" ";}
      outfp<<"\n";


    }
    outfp.close();
  }





  return;// true;






//    std::ofstream ofile(filename.c_str());
    ofile<<"NFRAMES: "<<polygons.size()<<"\n";
    for(unsigned i=0;i<polygons.size();i++)
    {
        ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
        ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
        for(unsigned j=0;j<polygons[i].size();j++)
        {
            ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
            ofile<<"X: "<<"\n";
            for(unsigned k=0;k<polygons[i][j]->size();k++)
                ofile<<polygons[i][j]->vertex(k)->x()<<" ";
            ofile<<"\n";
            ofile<<"Y: "<<"\n";
            for(unsigned k=0;k<polygons[i][j]->size();k++)
                ofile<<polygons[i][j]->vertex(k)->y()<<" ";
            ofile<<"\n";

 
        }
    }

    ofile.close();
}

int main(int argc, char** argv)
{
  REG_STORAGE(vidpro_image_storage);
  //REG_STORAGE(dbbgm_distribution_image_storage);
  REG_STORAGE(dbbgm_image_storage);
  REG_STORAGE(vidpro_vsol2D_storage);

  //REG_STORAGE(dbbgm_storage);

 //REG_PROCESS( dbbgm_load_bg_model_process );

   //****//std::vector<std::string> input_names_bg;
   //****//input_names_bg.push_back("video");
   //****//input_names_bg.push_back("dbbgm");
  
   //****//bpro_process_sptr bgbuilder(new dbbgm_aerial_bg_model_process()); //dbbgm_distribution_image
   //****//bgbuilder->set_input_names(input_names_bg);

  //****//vorl_manager::instance()->add_process_to_args(bgbuilder);

  //****//
  bpro_process_sptr dbbgm_loader(new dbbgm_load_bg_model_process1());
  //dbbgm_loader->set_input_names(std::vector<std::string>(1,"dbbgm0"));
  dbbgm_loader->set_output_names(std::vector<std::string>(1,"dbbgm_image"));
  vorl_manager::instance()->add_process_to_args(dbbgm_loader);
  //****//
  bpro_process_sptr fgdetector(new dbbgm_aerial_fg_uncertainity_detect_process1());

  std::vector<std::string> inputnames;
  inputnames.push_back("video");

  //inputnames.push_back("as012");//this is the key 12/25/2006 DOngjin
  inputnames.push_back("a");//4-23-07 Make the name simpler

  fgdetector->set_input_names(inputnames);
  fgdetector->set_output_names(std::vector<std::string>(1,"ForegroundDetected"));
  vorl_manager::instance()->add_process_to_args(fgdetector);

  bpro_process_sptr  blobfinder(new dbdet_blob_finder_process());
  blobfinder->set_input_names(std::vector<std::string>(1,"ForegroundDetected"));
  blobfinder->set_output_names(std::vector<std::string>(1,"blobs"));
  vorl_manager::instance()->add_process_to_args(blobfinder);

  
   vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  vorl_manager::instance()->parse_params(argc, argv); 
  vorl_manager::instance()->load_video();
  
  //****//
   //****////vorl_manager::instance()->add_process_to_queue(dbbgm_loader);
   //****////vorl_manager::instance()->run_process_queue_on_current_frame();
   //****//// vorl_manager::instance()->clear_process_queue();
   
  //****//vorl_manager::instance()->finish_process_queue();
  //****//

  //****//vorl_manager::instance()->add_process_to_queue(bgbuilder);

  //****//std::cout<<"\n building background model";
  //****//std::cout.flush();
 

  ///
///std::set<bpro_storage_sptr> modified; 
  ///


  //****//bgbuilder->set_input_names(input_names_bg);
 
  //****//bgbuilder->set_output_names(std::vector<std::string>(1,"bgmodel"));
  //****//vorl_manager::instance()->add_process_to_args(bgbuilder);



  // vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  int s0=0,se=0;
  bool s0_flag=true;
  vorl_manager::instance()->rewind(); 
  

  do {

    if (s0_flag) {
      s0=rep->current_frame();
      s0_flag=false;
    }
    se++;
    std::cout<<rep->current_frame()<<std::endl;
    
  } while(vorl_manager::instance()->next_frame());

  se+=s0-1;
  std::cout<<se<<std::endl;



std::set<bpro_storage_sptr> modified; 
  rep->initialize_global();
    //****//
  vorl_manager::instance()->first_frame();
  vorl_manager::instance()->add_process_to_queue(dbbgm_loader);
  
  vorl_manager::instance()->run_process_queue_on_current_frame();
  //vorl_manager::instance()->run_process_queue_on_current_frame(&modified);
   vorl_manager::instance()->finish_process_queue();
   vorl_manager::instance()->clear_process_queue();
     rep->print_summary();
   std::cout<<  rep->get_data_by_name("as012")<<std::endl;
  //****//vorl_manager::instance()->finish_process_queue();
  //****//


/*
do {
//   std::cout<<"\n 1";
    float status = vorl_manager::instance()->status()/2.0;
    vorl_manager::instance()->write_status(status);
 //    std::cout<<"\n 2";
    vorl_manager::instance()->run_process_queue_on_current_frame();

 //    std::cout<<"\n 3";
    }  while(
      vorl_manager::instance()->next_frame()&&
     // vorl_manager::instance()->next_frame()&&
      vorl_manager::instance()->next_frame()&&
      vorl_manager::instance()->next_frame()&&
      vorl_manager::instance()->next_frame()&&
      vorl_manager::instance()->next_frame()
      );


 vorl_manager::instance()->finish_process_queue();
 */
  std::cout<<"\n detecting polygons";
  std::cout.flush();
  // detecting foreground and finding polygons
  vorl_manager::instance()->rewind();
  vorl_manager::instance()->clear_process_queue();
  //vorl_manager::instance()->add_process_to_queue(regimg);
  vorl_manager::instance()->add_process_to_queue(fgdetector);
  vorl_manager::instance()->add_process_to_queue(blobfinder);
  ////vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  rep->print_summary();

  do {
    float status = vorl_manager::instance()->status()/2.0 + 0.5;
    vorl_manager::instance()->write_status(status);

    std::cout<<"\n fg 4";
    vorl_manager::instance()->run_process_queue_on_current_frame();
    std::cout<<"\n fg 5";
  }  while(
   // vorl_manager::instance()->next_frame()&&
    vorl_manager::instance()->next_frame());

//std::cout<<"\n fg 6";
  //vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  //: getting ni , nj  of the images
  vidpro_image_storage_sptr img_storage;
  bpro_storage_sptr inputimg= rep->get_data_by_name_at("video",0);

  img_storage.vertical_cast(inputimg);
  int ni=img_storage->get_image()->ni();
  int nj=img_storage->get_image()->nj();
std::cout<< img_storage->frame();
  vorl_manager::instance()->rewind();
  vorl_manager::instance()->clear_process_queue();
  //:retreiveing the polygons
  std::vector<std::vector<vsol_polygon_2d_sptr> > all_polygons;
  do {
      
      vidpro_vsol2D_storage_sptr polygons_storage;
      bpro_storage_sptr result= rep->get_data_by_name("blobs");
      polygons_storage.vertical_cast(result);
      std::vector<vsol_spatial_object_2d_sptr> temp=polygons_storage->all_data();
      std::vector<vsol_polygon_2d_sptr>  temp_polygons;

      int max_size=-1; int max_index=-1;
      for (unsigned int j=0;j<temp.size();j++) {

          std::cout<<j<<" "<<temp[j]->cast_to_region()->cast_to_polygon()->size()<<std::endl;
          if ((int)temp[j]->cast_to_region()->cast_to_polygon()->size()>max_size) {
              max_size=temp[j]->cast_to_region()->cast_to_polygon()->size();
              max_index=j;
              std::cout<<j<<" "<<temp[j]->cast_to_region()->cast_to_polygon()->size()<<std::endl;
          }
      }


       if (max_index>-1)
       temp_polygons.push_back(temp[max_index]->cast_to_region()->cast_to_polygon());

       else
      for(unsigned int i=0;i<temp.size();i++)
        temp_polygons.push_back(temp[i]->cast_to_region()->cast_to_polygon());
      
      
      std::cout<<temp_polygons.size()<<std::endl;

      all_polygons.push_back(temp_polygons);
  } while(
   // vorl_manager::instance()->next_frame()&&
    vorl_manager::instance()->next_frame());

  std::string fname;
  bpro_filepath filepath;
  //blobfinder->parameters()->get_value("-polyfile",filepath);
  std::string dirname=vorl_manager::instance()->get_output_dir();
  std::string inputfile=dirname+"//poly.txt";

   
  ///regimg->parameters()->get_value("-fhmg",filepath);
  
  std::ofstream ofile(inputfile.c_str());
  //ofile<<"FILEID: "<<vorl_manager::instance()->get_video_fileid()<<"\n";

 // write_transformed_polygons(filepath.path,all_polygons,ofile,ni,nj);
  // write_polygons(filepath.path,all_polygons,ofile,ni,nj);






 /* int s0=0,se=0;
  bool s0_flag=true;
  vorl_manager::instance()->rewind();
  do {

    if (s0_flag) {
      s0=rep->current_frame();
      s0_flag=false;
    }
    se++;
    std::cout<<rep->current_frame()<<std::endl;
    
  } while(vorl_manager::instance()->next_frame());

  se+=s0-1;
  std::cout<<se<<std::endl;*/
   write_polygons(dirname,all_polygons,ofile,ni,nj,s0,se);
  
  ofile.close();

  // Save part of the repository
  std::set<std::string> repos_names;
  repos_names.insert("dbbgm");
  repos_names.insert("ForegroundDetected");
  repos_names.insert("blobs");
  //repos_names.insert("blobs");
  std::string output_dir = vorl_manager::instance()->get_output_dir();
  std::string binfile=output_dir+"/bin.rep";
  //vorl_manager::instance()->save_repository(binfile, repos_names);

return 0; 
}


// This is breye1/vorl1/exec/vorl1_fg_detect_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <dbbgm/pro/dbbgm_load_bg_model_process1.h>
#include <dbbgm/pro/dbbgm_aerial_fg_uncertainity_detect_process1.h>
#include <dbdet/pro/dbdet_blob_finder_process.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_process_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vnl/vnl_matrix.h>
#include <vimt/vimt_transform_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_fstream.h>
#include <vbl/vbl_bounding_box.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
void write_polygons( vcl_vector<vcl_vector<vsol_polygon_2d_sptr> > polygons,vcl_ofstream &ofile, int ni,int nj )
{
    //vcl_ofstream ofile(filename.c_str());
    ofile<<"NFRAMES: "<<polygons.size()<<"\n";



    // iterating over frames
    for(unsigned int i=0;i<polygons.size();i++)
    {   
        //: iterating over polygons in one frame
        ofile<<"NOBJECTS: "<<polygons[i].size()<<"\n";
        ofile<<"NPOLYS: "<<polygons[i].size()<<"\n";
        for(unsigned int j=0;j<polygons[i].size();j++)
        {
            ofile<<"NVERTS: "<<polygons[i][j]->size()<<"\n";
            ofile<<"X: "<<"\n";
            for(unsigned int k=0;k<polygons[i][j]->size();k++)
            {
                ofile<<polygons[i][j]->vertex(k)->x()<<" ";
            }
            ofile<<"\n";

            ofile<<"Y: "<<"\n";
            for(unsigned int k=0;k<polygons[i][j]->size();k++)
            {
               ofile<<polygons[i][j]->vertex(k)->y()<<" ";
            }
            ofile<<"\n";
        }   
    }


}

int main(int argc, char** argv)
{
     
    
    REG_STORAGE(dbbgm_image_storage);
    REG_STORAGE(vidpro1_image_storage);
    REG_STORAGE(vidpro1_vsol2D_storage);
  
 
  bpro1_process_sptr loadbg(new dbbgm_load_bg_model_process1()); 
  loadbg->set_output_names(vcl_vector<vcl_string>(1,"bgmodel"));
  vorl1_manager::instance()->add_process_to_args(loadbg);

  bpro1_process_sptr fgdetector(new dbbgm_aerial_fg_uncertainity_detect_process1());

  vcl_vector<vcl_string> inputnames;
  inputnames.push_back("video");
  inputnames.push_back("bgmodel");

  fgdetector->set_input_names(inputnames);
  fgdetector->set_output_names(vcl_vector<vcl_string>(1,"ForegroundDetected"));
  vorl1_manager::instance()->add_process_to_args(fgdetector);

  bpro1_process_sptr  blobfinder(new dbdet_blob_finder_process());
  blobfinder->set_input_names(vcl_vector<vcl_string>(1,"ForegroundDetected"));
  blobfinder->set_output_names(vcl_vector<vcl_string>(1,"blobs"));
  vorl1_manager::instance()->add_process_to_args(blobfinder);

  vorl1_manager::instance()->parse_params(argc, argv); 
  vorl1_manager::instance()->load_video();

  vorl1_manager::instance()->add_process_to_queue(loadbg);
  vorl1_manager::instance()->run_process_queue_on_current_frame();
  vorl1_manager::instance()->clear_process_queue();
 
  vcl_cout<<"\n detecting polygons";
  vcl_cout.flush();
  // detecting foreground and finding polygons
  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
  //vorl1_manager::instance()->add_process_to_queue(regimg);
  vorl1_manager::instance()->add_process_to_queue(fgdetector);
  vorl1_manager::instance()->add_process_to_queue(blobfinder);
  vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  rep->print_summary();

  do {
    float status = vorl1_manager::instance()->status()/2.0 + 0.5;
    vorl1_manager::instance()->write_status(status);
    vorl1_manager::instance()->run_process_queue_on_current_frame();

  }  while(vorl1_manager::instance()->next_frame());


  //vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  //: getting ni , nj  of the images
  vidpro1_image_storage_sptr img_storage;
  bpro1_storage_sptr inputimg= rep->get_data_by_name_at("video",0);

  img_storage.vertical_cast(inputimg);
  int ni=img_storage->get_image()->ni();
  int nj=img_storage->get_image()->nj();

  vorl1_manager::instance()->rewind();
  vorl1_manager::instance()->clear_process_queue();
 

return 0; 
}



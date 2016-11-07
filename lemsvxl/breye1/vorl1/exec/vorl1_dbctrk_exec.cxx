// This is breye1/vorl1/exec/vorl1_ctrk_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <vidpro1/vidpro1_repository.h>
#include <dbctrk/pro/dbctrk_process.h>
#include <dbctrk/pro/dbctrk_storage.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbctrk/dbctrk_benchmarking.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vgl/vgl_point_2d.h>


int main(int argc, char** argv)
{
  // Register storage types and processes
  REG_STORAGE(vidpro1_image_storage);
  REG_STORAGE(dbctrk_storage);

  //==========================================================

  // Make each process and add it to the list of program args
  bpro1_process_sptr my_process(new dbctrk_process()); 
  my_process->set_input_names(vcl_vector<vcl_string>(1,"video"));
  my_process->set_output_names(vcl_vector<vcl_string>(1,"curves"));
  vorl1_manager::instance()->add_process_to_args(my_process); 

  vorl1_manager::instance()->parse_params(argc, argv); 
  vcl_string output_dir = vorl1_manager::instance()->get_output_dir(); 
  vcl_string output_ps = vorl1_manager::instance()->get_output_ps_dir(); 
//  vcl_string output_video_dir = vorl1_manager::instance()->get_output_video_dir(); 
  vcl_string video_assoc_dir = vorl1_manager::instance()->get_video_assoc_dir()+"//contour"; 
  vorl1_manager::instance()->load_video();

  //: loading the ground truth file
  vcl_vector<vcl_vector<vgl_point_2d<double> > > contours2d;
  if(vul_file::is_directory(video_assoc_dir.c_str()))
  for (vul_file_iterator fn=video_assoc_dir+"//*.con"; fn; ++fn) {
    if(vul_file::exists(fn()))
    {
      vcl_vector<vgl_point_2d<double> > points;
      points.clear(); 
    
      dbctrk_benchmarking::read_outlines(fn(),points);
      contours2d.push_back(points);
    vcl_cout<<"\n the number of points are";
    }
  }

  unsigned int index=0;
  vidpro1_repository_sptr rep = vorl1_manager::instance()->repository();
  do{
       vorl1_manager::instance()->write_status();
       vorl1_manager::instance()->run_process_on_current_frame(my_process);
       double tp,tn;
       bpro1_storage_sptr result = rep->get_data_by_name("curves");
       vcl_vector< dbctrk_tracker_curve_sptr > tracked_curves;
       dbctrk_storage_sptr dbctrk_storage_obj;
       dbctrk_storage_obj.vertical_cast(result);
       dbctrk_storage_obj->get_tracked_curves(tracked_curves);
       

       int frame = dbctrk_storage_obj->frame();
       if(frame>1)
       {
          if(contours2d.size()>index)
           {  
             dbctrk_benchmarking::in_or_out(contours2d[index],tracked_curves,tp,tn);  
             vorl1_manager::instance()->set_performance(tp,tn);
            }
          else
           {  tp=-1;tn=-1;     }

       }
       dbctrk_benchmarking::write_moving_curves_to_ps(tracked_curves,output_ps,frame);
       index++;
  }while(vorl1_manager::instance()->next_frame());
  // Write final status notice (100%)
  vorl1_manager::instance()->write_status(1.0);

  vcl_string binfile=output_dir+"/bin.dat";
  vorl1_manager::instance()->save_repository(binfile);
  vorl1_manager::instance()->write_performance("");

  return 0; 
}





// This is breye/vorl/exec/vorl_compute_homography_exec.cxx
//:
// \file

#include <vorl/vorl.h>
#include <dbvrl/pro/dbvrl_compute_homography_process.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <bpro/bpro_parameters_sptr.h>
#include <bpro/bpro_process_sptr.h>

int main(int argc, char** argv)
{
  REG_STORAGE(vidpro_image_storage);
    
  // Make each process and add it to the list of program args
  bpro_process_sptr process(new dbvrl_compute_homography_process()); 

  process->set_input_names(vcl_vector<vcl_string>(1,"video"));
  vorl_manager::instance()->add_process_to_args(process); 
  vorl_manager::instance()->parse_params(argc, argv); 
  vcl_string fname;
  process->parameters()->get_value("-fout",fname);
  vcl_string dirname=vorl_manager::instance()->get_output_dir();


  vcl_string inputfile=dirname+"//"+fname;
    

  process->parameters()->set_value("-fout",inputfile);

  vorl_manager::instance()->load_video();

  do{
    vorl_manager::instance()->run_process_on_current_frame(process);
}while(vorl_manager::instance()->next_frame());
  vorl_manager::instance()->finish_process_queue();


  
  

  return 0; 
}





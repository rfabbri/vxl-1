// This is breye1/vorl1/exec/vorl1_compute_homography_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <dbcvr/pro/dbcvr_curvematch_process.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>
#include <bpro1/bpro1_parameters_sptr.h>
#include <bpro1/bpro1_process_sptr.h>

int main(int argc, char** argv)
{
  //REG_STORAGE(vidpro1_image_storage);

  // Make each process and add it to the list of program args
  bpro1_process_sptr process(new dbcvr_curvematch_process()); 

  //process->set_input_names(vcl_vector<vcl_string>(1,"video"));
  vorl1_manager::instance()->add_process_to_args(process); 
  vorl1_manager::instance()->parse_params(argc, argv); 
  //vcl_string fname;
  //process->parameters()->get_value("-fout",fname);
  //vcl_string dirname=vorl1_manager::instance()->get_output_dir();


  //vcl_string inputfile=dirname+"//"+fname;


  // process->parameters()->set_value("-fout",inputfile);

  //vorl1_manager::instance()->load_video();

  //do{
  vorl1_manager::instance()->run_process_on_current_frame(process);
  //}while(vorl1_manager::instance()->next_frame());
  vorl1_manager::instance()->finish_process_queue();

  return 0; 
}


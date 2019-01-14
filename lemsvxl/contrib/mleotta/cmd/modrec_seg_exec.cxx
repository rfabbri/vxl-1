// This is contrib/mleotta/cmd/modrec_seg_exec.cxx
//:
// \file

#include <vorl1/vorl1.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_repository.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage.h>

#include <dbbgm/pro/dbbgm_model_process.h>
#include <dbbgm/pro/dbbgm_equalize_process.h>
#include <modrec/pro/modrec_classify_process.h>

#include <vul/vul_arg.h>
#include <fstream>

#include <vil/vil_convert.h>
#include <vil/vil_new.h>



bool read_frame_bounds(const std::string& file_name,
                       std::vector<std::pair<unsigned,unsigned> >& bounds)
{

  std::ifstream ifs(file_name.c_str());
  if(!ifs.is_open()){
    std::cerr << "could not load vehicle list file: "<<file_name<< std::endl;
    return false;
  }

  unsigned int f1, f2, last=0;
  std::string car_type;
  char dir;
  while(!ifs.eof()){
    ifs >> f1 >> f2 >> car_type >> dir;
    if(f1 > last){
      if(dir == 'r')
        bounds.push_back(std::pair<unsigned, unsigned>(f1,f2));
    }
    else{
      while(!bounds.empty() && f1 <= bounds.back().second)
        bounds.pop_back();
    }
    if(f2 > last)
      last = f2;
  }

  ifs.close();
  return true;
}



// The Main Function
int main(int argc, char** argv)
{
  // Register storage types and processes
  REG_STORAGE( vidpro1_image_storage );
  REG_STORAGE( dbbgm_image_storage );

  //==========================================================

  // Make each process and add them to the list of program args
  bpro1_process_sptr bg_equalizer(new dbbgm_equalize_process());
  std::vector<std::string> bg_equalizer_input_names;
  bg_equalizer_input_names.push_back("video");
  bg_equalizer_input_names.push_back("bg_model");
  bg_equalizer_input_names.push_back("video_eq");
  bg_equalizer->set_input_names(bg_equalizer_input_names);
  vorl1_manager::instance()->add_process_to_args(bg_equalizer);

  bpro1_process_sptr bg_modeller(new dbbgm_model_process());
  std::vector<std::string> bg_modeller_input_names;
  bg_modeller_input_names.push_back("video_eq");
  bg_modeller_input_names.push_back("bg_model");
  bg_modeller->set_input_names(bg_modeller_input_names);
  vorl1_manager::instance()->add_process_to_args(bg_modeller);

  bpro1_process_sptr classifier(new modrec_classify_process());
  std::vector<std::string> classifier_input_names;
  classifier_input_names.push_back("video_eq");
  classifier_input_names.push_back("bg_model");
  classifier->set_input_names(classifier_input_names);
  std::vector<std::string> classifier_output_names;
  classifier_output_names.push_back("labels");
  classifier->set_output_names(classifier_output_names);
  vorl1_manager::instance()->add_process_to_args(classifier);

  //===========================================================

  vul_arg<std::string> vehicle_list("--vehicle_list","list of vehicle frames","");

  // Parse the arguments and load the video
  vorl1_manager::instance()->parse_params(argc, argv);
  vorl1_manager::instance()->load_video();

  //===========================================================
  std::vector<std::pair<unsigned, unsigned> > frame_bounds;
  if(!read_frame_bounds(vehicle_list(),frame_bounds))
    return -1;


  //===========================================================

  // Set up the process queue to be run on all frames
  vorl1_manager::instance()->add_process_to_queue(bg_equalizer);
  vorl1_manager::instance()->add_process_to_queue(bg_modeller);

  bool valid = true;
  for(unsigned m=0; m<frame_bounds.size(); ++m){
    std::cout << "Modelling background" << std::endl;
    // run the process queue on a segment between cars
    while(vorl1_manager::instance()->current_frame() < frame_bounds[m].first && valid){
      std::cout << '.' << std::flush;
      //float status = vorl1_manager::instance()->status()/2.0;
      //vorl1_manager::instance()->write_status(status);
      vorl1_manager::instance()->run_process_queue_on_current_frame();
      valid = vorl1_manager::instance()->next_frame();
    }

    vorl1_manager::instance()->add_process_to_queue(classifier);
    while(vorl1_manager::instance()->current_frame() <= frame_bounds[m].second && valid){
      std::cout << '+' << std::flush;
      //float status = vorl1_manager::instance()->status()/2.0;
      //vorl1_manager::instance()->write_status(status);
      vorl1_manager::instance()->run_process_queue_on_current_frame();
      valid = vorl1_manager::instance()->next_frame();
    }
    std::cout << "\nAligning Models" << std::endl;
    vorl1_manager::instance()->finish_process_queue(frame_bounds[m].first,
                                                   frame_bounds[m].second);
    vorl1_manager::instance()->delete_last_process();
  }

  std::set<std::string> names;
  names.insert("labels");
  std::string output_dir = vorl1_manager::instance()->get_output_dir();
  std::string binfile=output_dir+"/labels.rep";
  vorl1_manager::instance()->save_repository(binfile, names);

  return 0;
}





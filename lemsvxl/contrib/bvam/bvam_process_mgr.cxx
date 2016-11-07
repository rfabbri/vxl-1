#include "bvam_process_mgr.h"
#include "bvam_world_params.h"

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>

bvam_process_mgr* bvam_process_mgr::instance_ = 0;

bvam_process_mgr* bvam_process_mgr::instance()
{
  if (!instance_) {
    instance_ = new bvam_process_mgr();
  }
  return bvam_process_mgr::instance_;
}

bvam_process_mgr::~bvam_process_mgr()
{
  delete parser_;
}

bool bvam_process_mgr:: init(vcl_string config_XML)
{
  bvam_io_config_parser* parser = 0;
  parser = parse_config(config_XML);
  if (parser == 0) {
    vcl_cerr << "Configuration File is not a valid XML!\n";
    return false;
  }

  // set the world parameters
  bvam_world_params* world_params = bvam_world_params::instance();
  world_params->set_params(parser->model_dir(), parser->corner(), 
    parser->voxel_dim(), parser->voxel_length(), bvam_world_params::apm_unknown);
 
  return true;
}

bool bvam_process_mgr:: run()
{
  // get the process list from the parser and run them
  vcl_vector<bvam_io_process*> processes = parser_->processes();
  bvam_processor* processor_ = new bvam_processor();
  for (unsigned i=0; i<processes.size(); i++) {
    bvam_io_process* p = processes[i];

    if (p->process_type.compare(BVAM_PROCESS_TRAIN_TAG) == 0) {
      bvam_io_process_train* p_info = static_cast<bvam_io_process_train*> (p);
      bvam_image_schedule s;
      s.load(p_info->image_path, p_info->camera_path, 
        p_info->light_path, p_info->schedule.order_by_date);
      schedule(p_info->schedule, s);
      processor_->train(s, p_info->output_path);
    } 
    
    else if (p->process_type.compare(BVAM_PROCESS_DETECT_CHANGE_TAG) == 0) {
      bvam_io_process_detect* p_info = static_cast<bvam_io_process_detect*> (p);
      bvam_image_schedule s;
      s.load(p_info->image_path, p_info->camera_path, 
        p_info->light_path, p_info->schedule.order_by_date);
      schedule(p_info->schedule, s);
      processor_->detect_changes(s, p_info->output_path);
    }

    else if (p->process_type.compare(BVAM_PROCESS_RENDER_EXP_TAG) == 0) {
      bvam_io_process_render_expected* p_info = static_cast<bvam_io_process_render_expected*> (p);
      bvam_image_schedule s;
      s.load("NONE", p_info->camera_path, p_info->light_path, p_info->schedule.order_by_date);
      schedule(p_info->schedule, s);
      processor_->render_expected(p_info->x, p_info->y, s, p_info->output_path);
    }

    else if (p->process_type.compare(BVAM_PROCESS_RENDER_VIEW_TAG) == 0) {
      bvam_io_process_render_from_view* p_info = static_cast<bvam_io_process_render_from_view*> (p);
      bvam_image_schedule view_s;
      view_s.load(p_info->image_path, p_info->camera_path, "NONE", p_info->view_schedule.order_by_date);
      schedule(p_info->view_schedule, view_s);
      bvam_image_schedule s;
      s.load(p_info->image_path, p_info->camera_path, "NONE", p_info->schedule.order_by_date);
      schedule(p_info->schedule, s);
      processor_->render_virtual_view(p_info->x, p_info->y, view_s, p_info->view_camera_index, s, p_info->output_path);
    }
  }

  delete processor_;
  return true;
}

bvam_io_config_parser* bvam_process_mgr::parse_config(vcl_string fname)
{
 
  if (fname.size() == 0)
    return 0;

  bvam_io_config_parser* parser = new bvam_io_config_parser();
  vcl_FILE* xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile){
    vcl_cerr << fname.c_str() << " error on opening\n";
    delete parser;
    return 0;
  }
  if (!parser->parseFile(xmlFile)) {
    vcl_cerr << XML_ErrorString(parser->XML_GetErrorCode()) << " at line "
             << parser->XML_GetCurrentLineNumber() << vcl_endl;

    delete parser;
    return 0;
  }
  vcl_cout << "finished parsing sucessfuly!" << vcl_endl;
  return parser;
}

bool bvam_process_mgr::schedule(bvam_io_schedule schedule, 
                                bvam_image_schedule& s)
{
  //s.schedule_subset(schedule.start, schedule.end, schedule.inc);
  if (schedule.order_by_date) {
    s.order_by_date();
  } else if (schedule.shuffle) {
    s.shuffle();
  }

  if (schedule.subset)
    s.schedule_subset(schedule.start, schedule.end, schedule.inc);
  else 
    s.schedule_all();

  return true;
}

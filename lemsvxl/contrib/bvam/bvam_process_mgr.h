#ifndef bvam_process_mgr_h_
#define bvam_process_mgr_h_

#include "bvam_processor.h"
#include "io/bvam_io_config_parser.h"

class bvam_process_mgr
{
public:
  static bvam_process_mgr* instance();
  virtual ~bvam_process_mgr();

  //void register_process(vcl_string appear_model, bvam_processor_interf* processor);
  bool init(vcl_string config_XML);
  bool run();
protected:
  bvam_process_mgr();
  static bvam_process_mgr* instance_;

  bvam_io_config_parser* parser_;

private:
  bvam_io_config_parser* parse_config(vcl_string fname);
  bool schedule(bvam_io_schedule schedule, bvam_image_schedule& s);
};

#endif

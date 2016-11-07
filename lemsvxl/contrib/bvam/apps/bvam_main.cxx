#include "bvam_process_mgr.h"
#include "bvam_macros.h"

int main(int argc, char** argv)
{
  vcl_string XML_path = argv[1];
  bvam_process_mgr::instance()->init(XML_path);
  bvam_process_mgr::instance()->run();
}

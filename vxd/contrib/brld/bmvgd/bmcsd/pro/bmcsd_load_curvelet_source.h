// This is bmcsd_load_curvelet_source.h
#ifndef bmcsd_load_curvelet_source_h
#define bmcsd_load_curvelet_source_h
//:
//\file
//\brief 
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 07/24/10 15:53:48 EDT
//

#include <bprod/bprod_process.h>
#include <sdet/sdet_edgemap.h>
#include <sdet/sdet_edgemap_sptr.h>
#include <sdet/sdet_curvelet_map.h>
#include <sdetd/io/sdetd_cvlet_map_io.h>
#include <sdetd/pro/sdetd_sel_storage.h>
#include <sdetd/pro/sdetd_sel_storage_sptr.h>


//: Reads .cvlet files into curvelet maps
class bmcsd_load_curvelet_source : public bprod_source {
public:
  bmcsd_load_curvelet_source(std::string fname)
    : fname_(fname)
      {}

  bprod_signal execute() {
#ifndef NDEBUG
    std::cout << "Started curvelet loading.\n";
#endif
    sdet_edgemap_sptr edge_map;

    sdetd_sel_storage_sptr output_sel = sdetd_sel_storage_new();

    bool retval = sdetd_load_cvlet_map(fname_, edge_map, output_sel->CM());
    if (!retval)
      return BPROD_INVALID;

    output_sel->set_EM(edge_map);

    output(0, output_sel);
#ifndef NDEBUG
    std::cout << "Finished curvelet loading.\n";
#endif
    return BPROD_VALID;
  }

private:
  std::string fname_;
};

#endif // bmcsd_load_curvelet_source_h

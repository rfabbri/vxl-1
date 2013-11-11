#include <bprb/bprb_func_process.h>
//:
// \file

#include <vcl_iostream.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <brdb/brdb_value.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/bsta_basic_functors.h>
#include <dbsta/algo/bsta_adaptive_updater.h>
#include <dbsta/bsta_histogram.h>

typedef bsta_gauss_if3 bsta_gauss3_t;
typedef bsta_gauss3_t::vector_type vector3_;
typedef bsta_num_obs<bsta_gauss3_t> gauss_type3;
typedef bsta_mixture<gauss_type3> mix_gauss_type3;
typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
typedef bbgm_image_of<obs_mix_gauss_type3> imageofgaussmix3;

//: Process construct function
bool bbgm_select_mixture_averaging_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> in_types(2);
  in_types[0]="int";//path for saved distribution image
  in_types[1]="bbgm_image_sptr";//pointer to distribution image
  pro.set_input_types(in_types);
  return true;
}

//: Process exectute function
bool bbgm_select_mixture_averaging_process(bprb_func_process& pro)
{
  // Sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << "In bbgm_select_mixture_averaging::execute - invalid inputs\n";
    return false;
  }

  compact_type type =static_cast<compact_type>(pro.get_input<int>(0));
  bbgm_image_sptr bgm = pro.get_input<bbgm_image_sptr>(1);
   if (!bgm) {
    vcl_cerr << "Null background image\n";
    return false;
  }
  bbgm_image_of<obs_mix_gauss_type3> *input_model =
					static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
  for (unsigned i=0;i<input_model->ni();i++)
	  for (unsigned j=0;j<input_model->nj();j++)
		  (*input_model)(i,j).compactify=type;

  return true;
}
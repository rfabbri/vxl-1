
//:
// \file

#include "mw_extend_corresp_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <mw/algo/mw_discrete_corresp_algo.h>
#include <mw/pro/mw_discrete_corresp_storage.h>





//: Constructor
mw_extend_corresp_process::mw_extend_corresp_process()
{
  if( 
      !parameters()->add( "extend to subcurves" ,             "-bsubcurves" ,    false )
    ) {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
mw_extend_corresp_process::~mw_extend_corresp_process()
{
}


//: Clone the process
bpro1_process*
mw_extend_corresp_process::clone() const
{
  return new mw_extend_corresp_process(*this);
}


//: Return the name of this process
vcl_string
mw_extend_corresp_process::name()
{
  return "Extend MW Corresp";
}


//: Return the number of input frame for this process
int
mw_extend_corresp_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
mw_extend_corresp_process::output_frames()
{
  return 2;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > mw_extend_corresp_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "mw pt corresp" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > mw_extend_corresp_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "mw pt corresp" );
  return to_return;
}


//: Execute the process
bool
mw_extend_corresp_process::execute()
{
  // read data ------------
  bool retval;
  retval = get_vsols(0, 0, &a1_);
  if (!retval) return false;
  retval = get_vsols(0, 1, &b1_);
  if (!retval) return false;
  retval = get_vsols(1, 0, &a0_);
  if (!retval) return false;
  retval = get_vsols(1, 1, &b0_);
  if (!retval) return false;

  mw_discrete_corresp_storage_sptr c_sto;
  c_sto.vertical_cast(input_data_[1][2]);  
  if (!c_sto) {
    vcl_cout << "Error: corresp storage null\n";
    return false;
  }
  acorr_ = c_sto->corresp();
  vcl_cout << "Corresp NAME: " << c_sto->name() << vcl_endl;
  vcl_cout << "Corresp: " << " : \n" << *acorr_ << vcl_endl;

  if (acorr_->checksum() != mw_discrete_corresp_algo::compute_checksum(a0_, a1_)) {
    vcl_cerr << "ERROR: input correspondence incompatible with input vsols\n";
    return false;
  }

  // extend corresp ------------
  mw_discrete_corresp *bcorr_ptr = new mw_discrete_corresp;
  mw_discrete_corresp &bcorr =*bcorr_ptr;

  bool do_subcurves=false;
  parameters()->get_value( "-bsubcurves" , do_subcurves);
  if (do_subcurves) {
    mw_discrete_corresp_algo::extend_to_subcurves(a0_, a1_, b0_, b1_, *acorr_, &bcorr);
  } else {
    mw_discrete_corresp_algo::extend(a0_, a1_, b0_, b1_, *acorr_, &bcorr);
  }

  // now output to frame -1 if possible
  mw_discrete_corresp_storage_sptr 
     b_storage = mw_discrete_corresp_storage_new();
  
  b_storage->set_corresp(bcorr_ptr);

  output_data_[1].push_back(b_storage);
  return true;
}

bool mw_extend_corresp_process::
get_vsols(unsigned v, unsigned input_id, vcl_vector< vsol_polyline_2d_sptr > *pcurves)
{
  vcl_vector< vsol_polyline_2d_sptr > &curves = *pcurves;
  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[v][input_id]);

  vcl_vector< vsol_spatial_object_2d_sptr > base = input_vsol->all_data();

  curves.resize(base.size(),0);
  for (unsigned i=0; i<curves.size(); ++i) {
    curves[i] = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

    if (!curves[i]) {
      vcl_cout << "Non-polyline found in frame; but only POLYLINES supported!" << vcl_endl;
      return false;
    }
  }

  vcl_cout << "Num curves: " << curves.size() << vcl_endl;

  return true;
}


bool
mw_extend_corresp_process::finish()
{
  return true;
}

// This is brl/bpro/bpro_mview_process_manager.h
#ifndef bpro_mview_process_manager_h_
#define bpro_mview_process_manager_h_

//:
// \file
// \brief This file defines a class base process manager
// It is templated to allow its childs to be singleton managers 
// \author Isabel Restrepo
// \date 3/27/07
//
// \verbatim
//  Modifications
//           
// \endverbatim


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_string.h>

#include <vbl/vbl_ref_count.h>
#include <bpro/bpro_process_sptr.h>
#include <bpro/bpro_storage_sptr.h>
#include <bpro/bpro_process_manager.h>

class bpro_mview_process_manager : public bpro_process_manager<bpro_mview_process_manager>
{
public:

   //: Destructor
  virtual ~bpro_mview_process_manager();

  
  bool run_process( const bpro_process_sptr& process,
                    vcl_set<bpro_storage_sptr>* modified = NULL );

  //: FIXME - this is not implemented
  bool run_process_queue(){ return false; }


  //set by bvis or vorl according to desired setup
  void set_input_view_ids(vcl_vector<unsigned int>const& vids)
    {input_view_ids_=vids;}
  void set_output_view_ids(vcl_vector<unsigned int>const& vids)
  {output_view_ids_=vids;}

  // FIXME: this should do something -- MJL
  bool run_process_queue_on_enabled_views(){ return false;}

  friend class bpro_process_manager<bpro_mview_process_manager>;
protected:
  
  //: Constructor
  bpro_mview_process_manager();

  //: Members
  vcl_vector<unsigned int> input_view_ids_;
  vcl_vector<unsigned int> output_view_ids_;
};

#endif // bpro_mview_process_manager_h_

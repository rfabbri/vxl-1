// This is breye/bvis/bvis_manager_traits.h
#ifndef vil_pixel_traits_h_
#define vil_pixel_traits_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
#include <vcl_string.h>
#include <bpro/bpro_mview_process_manager.h>
#include <vidpro/vidpro_process_manager.h>
//:
// \file
// \brief Templated traits for different bvis managers
// Allows templated functions to take different actions depending on the
// type of manager e.g. video or multi-view.
template <class T>
class bvis_manager_traits;

class bvis_video_manager;
VCL_DEFINE_SPECIALIZATION
class bvis_manager_traits<bvis_video_manager>
{
 public:

  //: String indicating the name of the manager
  static vcl_string name() {return "bvis_video_manager";}
  static vidpro_process_manager* process_manager()
    {return vidpro_process_manager::instance();}
};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class bvis_manager_traits<bvis_video_manager const> : 
public bvis_manager_traits<bvis_video_manager> {};
#endif

class bvis_mview_manager;
VCL_DEFINE_SPECIALIZATION
class bvis_manager_traits<bvis_mview_manager>
{
 public:

  //: String indicating the name of the manager
  static vcl_string name() {return "bvis_mview_manager";}
  static bpro_mview_process_manager* process_manager()
    {return bpro_mview_process_manager::instance();}

};

#if !VCL_CANNOT_SPECIALIZE_CV
VCL_DEFINE_SPECIALIZATION
class bvis_manager_traits<bvis_mview_manager const> : 
public bvis_manager_traits<bvis_mview_manager> {};
#endif

#endif // bvis_manager_traits_h_

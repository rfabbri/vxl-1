// This is contrib/bvam/pro/bvam_render_virtual_view_process.h
#ifndef bvam_render_virtual_view_process_h_
#define bvam_render_virtual_view_process_h_

//:
// \file
// \brief // A process that renders a video frame from a new viewpoint (used for 3-D registration)
//           
// \author Daniel Crispell
// \date 02/10/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_render_virtual_view_process : public bprb_process
{
 public:
  
   bvam_render_virtual_view_process();

  //: Copy Constructor (no local data)
  bvam_render_virtual_view_process(const bvam_render_virtual_view_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_render_virtual_view_process(){};

  //: Clone the process
  virtual bvam_render_virtual_view_process* clone() const {return new bvam_render_virtual_view_process(*this);}

  vcl_string name(){return "BvamRenderVirtualView";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_


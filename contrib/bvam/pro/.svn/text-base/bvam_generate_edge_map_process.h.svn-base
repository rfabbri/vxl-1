// This is contrib/bvam/pro/bvam_generate_edge_map_process.h
#ifndef bvam_generate_edge_map_process_h_
#define bvam_generate_edge_map_process_h_

//:
// \file
// \brief // A process that takes a grayscale image and returns the corresponding edge map
//           
// \author Ibrahim Eden
// \date 03/05/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvam_generate_edge_map_process : public bprb_process
{
 public:
   bvam_generate_edge_map_process();

  //: Copy Constructor (no local data)
  bvam_generate_edge_map_process(const bvam_generate_edge_map_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvam_generate_edge_map_process(){};

  //: Clone the process
  virtual bvam_generate_edge_map_process* clone() const 
  { return new bvam_generate_edge_map_process(*this); }

  vcl_string name(){return "BvamGenerateEdgeMapProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif // bvam_generate_edge_map_process_h_

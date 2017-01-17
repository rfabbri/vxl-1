#ifndef dbxml_track_h_
#define dbxml_track_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author T Orechia
// \brief The class for parameter conversion.
//
// 
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <dbxml/dbxml_tracks.h>


//#include <vcl_map.h>

class dbxml_track : public vbl_ref_count
{
 public:
         dbxml_track(int startframe, int numframes, int width, int height,vcl_vector<dbxml_generic_ptr> tracks){
   startframe_=startframe;
   numframes_ = numframes;
   width_ = width;
   height_=height;
   tracks_ = tracks;
   };
         ~dbxml_track(){};

int startframe_;
int numframes_;
int width_;
int height_;
vcl_vector<dbxml_generic_ptr> tracks_;

        
  //:specific sub_class string names
 

 
#endif

};
#ifndef dbxml_tracks_h_
#define dbxml_tracks_h_
//-----------------------------------------------------------------------------
//:
// \file
// \author T Orechia
// \brief The class for parameter conversion.
//
// 
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#include <vbl/vbl_ref_count.h>
#include <vcl_string.h>
#include <vcl_map.h>

//#include <vcl_map.h>

class dbxml_tracks : public vbl_ref_count
{
 public:
         dbxml_tracks(float x, float y){
     y_=y;
     x_=x;
   };
         ~dbxml_tracks(){};

float x_;
float y_;
 
 
};
#endif

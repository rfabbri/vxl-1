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
#include <string>
#include <map>

//#include <map>

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


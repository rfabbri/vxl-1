#include <expatpp/expatpplib.h>
#include <stdio.h>
#include <string.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
//#include <vcl_fstream.h>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>


class track_coord_info  {
  public:
  vcl_string x;
  vcl_string y;
  vcl_string x_margin;
  vcl_string y_margin;
};
class track_info {
public://functions
  track_info(track_info* info);
  track_info();
  int get_count();
  void setTrackCoordindateInfo ( track_coord_info* info);
  void addTrackCoordinateInfo();
  vcl_string getWidth();
  vcl_string getHeight();
  void setX(vcl_string *s);
  void setY(vcl_string *s);
  void setYmargin(vcl_string *s);
  void setXmargin(vcl_string *s);
  void setWidth(vcl_string *s);
  void setHeight(vcl_string *s);

   //Data
   vcl_vector<track_coord_info> coordinates; 

private:
    void increment_count();

    //temporary Data
    track_coord_info *pTrackCoordindateInfo;  // pointer to current coorindates in parser

    //Data
    int count;
    vcl_string width;
    vcl_string height;


};

class track_parser : public expatpp {
public:
  track_parser(track_info *info);
  virtual void startElement(const XML_Char* name, const XML_Char** atts);
  virtual void endElement(const XML_Char* name);
private:
  void handleWindowAtts(const XML_Char** atts);
  void handleTrackAtts(const XML_Char** atts);
  void WriteIndent();
  
  //Data
  int mDepth;
  track_info *pInfo;   //pointer to user data area to return results of parser.

};

#include <expatpp/expatpplib.h>
#include <stdio.h>
#include <string.h>
#include <cstdio>
#include <cassert>
//#include <fstream>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>

class track_coord_info  {
  public:
  std::string x;
  std::string y;
  std::string x_margin;
  std::string y_margin;
};
class track_info {
public://functions
  track_info(track_info* info);
  track_info();
  int get_count();
  void setTrackCoordindateInfo ( track_coord_info* info);
  void addTrackCoordinateInfo();
  std::string getWidth();
  std::string getHeight();
  void setX(std::string *s);
  void setY(std::string *s);
  void setYmargin(std::string *s);
  void setXmargin(std::string *s);
  void setWidth(std::string *s);
  void setHeight(std::string *s);

   //Data
   std::vector<track_coord_info> coordinates; 

private:
    void increment_count();

    //temporary Data
    track_coord_info *pTrackCoordindateInfo;  // pointer to current coorindates in parser

    //Data
    int count;
    std::string width;
    std::string height;


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

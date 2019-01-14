/* \file track.cpp
  Parses the track.xml file and produces a strack_info structure with the its attributes.
*/
#include "track.h"

#include <stdio.h>
#include <string.h>

#include <cstdio>
#include <cassert>


//#include <fstream>
#include <vul/vul_arg.h>
#include <vil/vil_save.h>

/*
  ** vidl1 is DEPRECATED - if you need it, a copy is in breye1/vidl1
  ** but rewriting to use the more efficient vidl is recommended

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_clip.h>
#include <vidl1/vidl1_io.h>
*/

track_info::track_info()
{
  count=0;
  // pInfo = 0;
}
track_info::track_info(track_info* pinfo)
{
  count=0;
}

void track_info::increment_count()
{
  count = count +1;
}


int track_info::get_count()
{
  return(count);
}

void track_info::setHeight(std::string *s)
{
  height = *s;
}
void track_info::setWidth(std::string *s)
{
  width = *s;
}
std::string track_info::getWidth()
{
 return( width);
}
std::string track_info::getHeight()
{
 return( height);
}


void track_info::setX(std::string *s)
{
 pTrackCoordindateInfo->x = *s;
}
void track_info::setY(std::string *s)
{
 pTrackCoordindateInfo->y = *s;
} 
void track_info::setYmargin(std::string *s)
{
 pTrackCoordindateInfo->y_margin = *s;  

}
void track_info::setXmargin(std::string *s)
{
 pTrackCoordindateInfo->x_margin = *s;  

}

void track_info::setTrackCoordindateInfo ( track_coord_info* info){
  pTrackCoordindateInfo = info;
}
void track_info::addTrackCoordinateInfo()
{
    coordinates.push_back(*pTrackCoordindateInfo);
    increment_count();

}
/*std::vector<std::string> track_info::get_vector()
{
  return(count);
}
*/

//-----
//--- PARSER --
//--

//  track():mDepth(0){};



track_parser::track_parser (track_info* info) 
  {
    pInfo = info;
  }


void 
track_parser ::WriteIndent()
{
//for (int i = 0; i < mDepth; i++)
//  putchar('\t');
}


void 
track_parser::handleWindowAtts(const XML_Char** atts)
{
   for (int i=0; atts[i]; i++) {
      if (strcmp(atts[i],"width") ==0)
        pInfo->setWidth(new std::string(atts[i+1]));
      else if (strcmp(atts[i],"height") ==0)
        pInfo->setHeight(new std::string(atts[i+1]));
    }

}

void 
track_parser::handleTrackAtts(const XML_Char** atts)
{
     for (int i=0; atts[i]; i++) {
      if (strcmp(atts[i],"x") ==0)
        pInfo->setX(new std::string(atts[i+1]));
      else if (strcmp(atts[i],"y") ==0)
        pInfo->setY(new std::string(atts[i+1]));
      else if (strcmp(atts[i],"y_margin") ==0)
        pInfo->setYmargin(new std::string(atts[i+1]));
      else if (strcmp(atts[i],"x_margin") ==0)
        pInfo->setXmargin(new std::string(atts[i+1]));
    }

} 
void 
track_parser ::startElement(const char* name, const char** atts)
{
  if(strcmp(name,"window")==0){
     handleWindowAtts(atts);
  }
  else if(strcmp(name,"track")==0){
     pInfo->setTrackCoordindateInfo ( new track_coord_info());
     handleTrackAtts(atts);

  }
//WriteIndent();
//puts(name);
//if (atts) { /* write list of attributes indented below element */
//int i;
//  for (i=0; atts[i]; i++) {
//       WriteIndent();
//       putchar('-'); putchar(' ');
//       puts(atts[i]);
//  }
//}
  mDepth++;
}


void 
track_parser ::endElement(const char* name)
{
   if(strcmp(name,"track")==0){
     pInfo->addTrackCoordinateInfo();
   }

  mDepth--;
}






/*

int main(int argc, char** argv)
{
  std::FILE *xmlFile;
  track_info info;
  track_parser parser(&info);

//int depth = 0;



  // --- Program Arguments ---
  vul_arg<std::string> filename("-filename", "input xml file");
  vul_arg<std::string> video("-video", "video file");
  vul_arg_parse(argc, argv);
  vidl1_movie_sptr my_movie ;

  if (  filename()== ""){
    fprintf(stderr,
 File not specified\n");
 
    return(1);
  }
    xmlFile = std::fopen(filename().c_str(), "r");
    if (!xmlFile){
      fprintf(stderr, " %s error on opening", filename );
      return(1);
    }
   if (!parser.parseFile(xmlFile)) {
     fprintf(stderr,
      "%s at line %d\n",
      XML_ErrorString(parser.XML_GetErrorCode()),
      parser.XML_GetCurrentLineNumber()
      );
     return 1;
   }
   puts("\nfinished!");

  int i=info.get_count();

  for (std::vector<track_coord_info>::iterator arg_it = info.coordinates.begin();
 arg_it != info.coordinates.end(); ++arg_it){
    std::string sx = arg_it->x;
    std::string sy = arg_it->y;
    puts("\nOuptupt x y");
    puts(sx.c_str());
    puts(sy.c_str());
    i--;
  }
  info.coordinates.clear();
  return(0);
}
*/




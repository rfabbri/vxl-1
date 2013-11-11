/* \file track.cpp
  Parses the track.xml file and produces a strack_info structure with the its attributes.
*/
#include "proc_io_run_xml_parser.h"

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_sstream.h>
#include <vcl_iostream.h>

//#include <vcl_fstream.h>

//-----
//--- PARSER --
//--
template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

proc_io_run_xml_parser::proc_io_run_xml_parser() 
  {
    
  }


void 
proc_io_run_xml_parser ::WriteIndent()
{
//for (int i = 0; i < mDepth; i++)
//  putchar('\t');
}
void 
proc_io_run_xml_parser ::cdataHandler(vcl_string name, vcl_string data)
{
  if (name.compare("log") == 0) {
    log_file_.assign(data);
  } else if (name.compare("scan") == 0) {
    scan_file_.assign(data);
  } else if (name.compare("box") == 0) {
    box_file_.assign(data);
  } else if (name.compare("output") == 0) {
    output_file_.assign(data);
  } else if (name.compare("filter_type") == 0) {
    if (data.compare("gaussian") == 0)
      filter_type_ = GAUSSIAN;
    else if (data.compare("no_noise") == 0)
      filter_type_ = NO_NOISE;
    else if(data.compare("edge_detector_x") == 0)
      filter_type_ = EDGE_DETECTOR_X;
    else if(data.compare("edge_detector_y") == 0)
      filter_type_ = EDGE_DETECTOR_Y;
    else if(data.compare("edge_detector_z") == 0)
      filter_type_ = EDGE_DETECTOR_Z;
    else
      filter_type_ = FILTER_UNDEF;
  } else if (name.compare("splatting_type") == 0) {
     if (data.compare("parallel") == 0)
       splatting_type_  = PARALLEL;
     else if (data.compare("conebeam") == 0)
       splatting_type_  = CONE_BEAM;
     else
       splatting_type_  = SPLAT_UNDEF;
  } else if (name.compare("algo_type") == 0) {
     if (data.compare("proc") == 0)
       algo_type_  = PROC;
     else if (data.compare("proc_splr") == 0)
       algo_type_  = PROC_SPLR;
     else
       algo_type_  = ALGO_UNDEF;
  }
   cdata = ""; 
}

void 
proc_io_run_xml_parser::handleAtts(const XML_Char** atts)
{
//   for (int i=0; atts[i]; i++) {
//     vcl_cout << "Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      /*if (strcmp(atts[i],"width") ==0)
        pInfo->setWidth(new vcl_string(atts[i+1]));
      else if (strcmp(atts[i],"height") ==0)
        pInfo->setHeight(new vcl_string(atts[i+1]));*/
//    }

}

void 
proc_io_run_xml_parser::startElement(const char* name, const char** atts)
{
 
//  vcl_cout<< "element=" << name << vcl_endl; 

  /*if (strcmp(name,"log") == 0){
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "fname") == 0)
        convert(atts[i+1], log_file_);
    }
  }
  else if (strcmp(name,"scan") == 0){
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "fname") == 0)
        convert(atts[i+1], scan_file_);
    }
  }
  else if (strcmp(name,"box") == 0){
    for (int i=0; atts[i]; i+=2) {
      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "fname") == 0)
        convert(atts[i+1], box_file_);
    }
  }
  else */if (strcmp(name, "scale") == 0){
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      //scale_elm.add_attribute(atts[i], atts[i+1]);
      if (strcmp(atts[i], "x") == 0)
        convert(atts[i+1], scale_x_);
      else if (strcmp(atts[i], "y") == 0)
        convert(atts[i+1], scale_y_);
      else if (strcmp(atts[i], "z") == 0)
        convert(atts[i+1], scale_z_);
    }
  }
  else if (strcmp(name,"filter_spec") == 0){
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "rad") == 0)
        convert(atts[i+1], filter_radius_);
      else if (strcmp(atts[i], "len") == 0)
        convert(atts[i+1], filter_length_);
      else if (strcmp(atts[i], "res") == 0)
        convert(atts[i+1], resolution_);
    }
  }
  else if (strcmp(name,"orientation") == 0){
    double x, y, z;
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "x") == 0) 
        convert(atts[i+1], x);
      else if (strcmp(atts[i], "y") == 0)
        convert(atts[i+1], y);
      else if (strcmp(atts[i], "z") == 0)
        convert(atts[i+1], z);
    }
    vgl_vector_3d<double> f_orient(x, y, z);
    orientations_.push_back(f_orient);
    }
}


void 
proc_io_run_xml_parser::endElement(const char* name)
{
   // first check if the last element has some cdata
   if (cdata.size() > 0) {
     cdataHandler(name, cdata);
     cdata= "";
   }
}

void proc_io_run_xml_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements

  putchar('(');
  fwrite(s, len, 1, stdout);
  puts(")");
  cdata.append(s, len);
}





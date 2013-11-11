/* \file proc_io_filter_xml_parser
  Parses the filter response xml files and forms objects out of the parameters
*/
#include "proc_io_filter_xml_parser.h"

#include <stdio.h>
#include <string.h>

#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>

#include <xmvg/xmvg_gaussian_filter_descriptor.h>
#include <xmvg/xmvg_no_noise_filter_descriptor.h>
#include <splr/splr_pizza_slice_symmetry.h>
//-----
//--- PARSER --
//--

template <typename T>
void convert(const char* t, T& d)
{
  vcl_stringstream strm(t);

  strm >> d;

}

void 
proc_io_filter_xml_parser ::WriteIndent()
{
//for (int i = 0; i < mDepth; i++)
//  putchar('\t');
}
xmvg_composite_filter_descriptor
proc_io_filter_xml_parser::composite_filter_descr()
{
  return xmvg_composite_filter_descriptor(descriptor_list);
}

/* commented out -- will be fixed soon

biob_worldpt_field<xmvg_filter_response<double> > proc_io_filter_xml_parser::worldpt_field() 
{
  //if (explicit_worldpt_roster.num_points() > 0) {
    biob_worldpt_roster* roster = new biob_explicit_worldpt_roster(explicit_worldpt_roster);
    //    splr_pizza_slice_symmetry* s = new splr_pizza_slice_symmetry(
    //    splr_symmetry_camera0_to_camera1, roster);
    biob_worldpt_roster_sptr sym_roster = new splr_symmetry_worldpt_roster(s);
    biob_worldpt_field<xmvg_filter_response<double> > field(sym_roster);
    field.set_values(responses_);
    return field;
  //} 
  //return field;
}
*/

void 
proc_io_filter_xml_parser::handleAtts(const XML_Char** atts)
{
//   for (int i=0; atts[i]; i++) {
//     vcl_cout << "Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
//    }

}

void 
proc_io_filter_xml_parser ::cdataHandler(vcl_string name, vcl_string data)
{
  // create a vector of tokens out of cdata and convert them later
  vcl_vector<vcl_string> tokens;
  int length = data.size();
  const char * str = data.c_str();
  vcl_string token = "";
  for (int i=0; i<length; i++) {
    if ((str[i] == ' ') || (str[i] == '\n')) {
      if (token.size() > 0) {
        tokens.push_back(token);
        token = "";
      }
    } else {
      token += str[i];
    }
  }
  // check for the last token
  if (token.size() > 0) 
    tokens.push_back(token);
      
  // read the filter responses
  if (name.compare("response") == 0) {
    // first create some element in the response vector
    if (responses_.size() == 0)
    { 
      for (int i=0; i < (response_dimx_*response_dimy_*response_dimz_); i++) {
        xmvg_filter_response<double> r(filter_num_);
        responses_.push_back(r);
      }
    }

    double resp;
    for (unsigned int i=0; i<tokens.size(); i++) {
      convert(tokens[i].c_str(), resp);
      responses_[i][filter_id_-1] = resp;
    }
  } else if (name.compare("rotation_axis") == 0) {
      double val;
      for (unsigned int i=0; i<tokens.size(); i++) {
        convert(tokens[i].c_str(), val);
        filter_rot_axis[i] = val;
      }

  } else if (name.compare("xmvg_filter_response") == 0) {
    vnl_vector<double> resp_arr(filter_num_);
    double val;
    for (unsigned int i=0; i<tokens.size(); i++) {
      convert(tokens[i].c_str(), val);
      resp_arr[i] = val;
    }
    xmvg_filter_response<double> resp(resp_arr);
    responses_.push_back(resp);
  } 
}

void 
proc_io_filter_xml_parser::startElement(const char* name, const char** atts)
{
//  vcl_cout<< "element=" << name << vcl_endl; 

  //: will deprecate, proc_version 1 element
  if(strcmp(name,"responses")== 0){
     //handleAtts(atts);
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "filter_num") == 0)
        convert(atts[i+1], filter_num_);
      else if (strcmp(atts[i], "dimx") == 0)
        convert(atts[i+1], response_dimx_);
      else if (strcmp(atts[i], "dimy") == 0)
        convert(atts[i+1], response_dimy_);
      else if (strcmp(atts[i], "dimz") == 0)
        convert(atts[i+1], response_dimz_);
    }
  //: will deprecate, proc_version 1 element
  } else if (strcmp(name,"response") == 0 ) {
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "ID") == 0)
          convert(atts[i+1], filter_id_);
    }
    // clean up the char data, in case there are other unused ones in there
    cdata_ = "";
  } else if (strcmp(name,"xmvg_filter_response") == 0 ) {
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "size") == 0)
          convert(atts[i+1], filter_num_);
      biob_field_type = "response";
    }
  } else if (strcmp(name, "active_box") == 0) {
    double min_x, min_y, min_z, max_x, max_y, max_z;
    for (int i=0; atts[i]; i+=2) {
      //vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "min_x") == 0)
          convert(atts[i+1], min_x);
      else if (strcmp(atts[i], "min_y") == 0)
          convert(atts[i+1], min_y);
      else if (strcmp(atts[i], "min_z") == 0)
          convert(atts[i+1], min_z);
      else if (strcmp(atts[i], "max_x") == 0)
          convert(atts[i+1], max_x);
      else if (strcmp(atts[i], "max_y") == 0)
          convert(atts[i+1], max_y);
      else if (strcmp(atts[i], "max_z") == 0)
          convert(atts[i+1], max_z);
    }
    active_box_ = vgl_box_3d<double> (min_x, min_y, min_z, max_x, max_y, max_z);
  } else if (strcmp(name,"biob_grid_worldpt_roster") == 0 ) {
    for (int i=0; atts[i]; i+=2) {
//      vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
      if (strcmp(atts[i], "num_points_x") == 0)
          convert(atts[i+1], response_dimx_);
      else if (strcmp(atts[i], "num_points_y") == 0)
          convert(atts[i+1], response_dimy_);
      else if (strcmp(atts[i], "num_points_z") == 0)
          convert(atts[i+1], response_dimz_);
    }
  } else if (strcmp(name,"orientation") == 0 ) {
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
    vgl_vector_3d<double> v(x, y, z);
    filter_orient = vgl_vector_3d<double> (x, y, z);
    orientations_.push_back(v);
 
    // clean up the char data
    cdata_ = "";
  } else if (strcmp(name, "box") == 0) {
      double min_x, min_y, min_z, max_x, max_y, max_z;
      for (int i=0; atts[i]; i+=2) {
//        vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
        if (strcmp(atts[i], "min_x") == 0)
          convert(atts[i+1], min_x);
        else if (strcmp(atts[i], "min_y") == 0)
          convert(atts[i+1], min_y);
        else if (strcmp(atts[i], "min_z") == 0)
          convert(atts[i+1], min_z);
        else if (strcmp(atts[i], "max_x") == 0)
          convert(atts[i+1], max_x);
        else if (strcmp(atts[i], "max_y") == 0)
          convert(atts[i+1], max_y);
        else if (strcmp(atts[i], "max_z") == 0)
          convert(atts[i+1], max_z);
      }
      filter_box = vgl_box_3d<double> (min_x, min_y, min_z, max_x, max_y, max_z);
  } else if (strcmp(name, "centre") == 0 ) { 
      double x, y, z;
      for (int i=0; atts[i]; i+=2) {
//        vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
        if (strcmp(atts[i], "x") == 0)
          convert(atts[i+1], x);
        else if (strcmp(atts[i], "y") == 0)
          convert(atts[i+1], y);
        else if (strcmp(atts[i], "z") == 0)
          convert(atts[i+1], z);
      }
      filter_center = vgl_point_3d<double> (x, y, z);
  }  else if (strcmp(name, "biob_worldpt_roster_point") == 0) {
      double x, y, z;
      for (int i=0; atts[i]; i+=2) {
 //       vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
        if (strcmp(atts[i], "x") == 0)
          convert(atts[i+1], x);
        else if (strcmp(atts[i], "y") == 0)
          convert(atts[i+1], y);
        else if (strcmp(atts[i], "z") == 0)
          convert(atts[i+1], z);
      }
      explicit_worldpt_roster.add_point(vgl_point_3d<double> (x, y, z));
  } else if (strcmp(name, "camera0_to_camera1") == 0) {
      double x, y, z, r;
      for (int i=0; atts[i]; i+=2) {
//        vcl_cout << "  Attr=" << atts[i] << "->" << atts[i+1] << vcl_endl;
        if (strcmp(atts[i], "x") == 0)
          convert(atts[i+1], x);
        else if (strcmp(atts[i], "y") == 0)
          convert(atts[i+1], y);
        else if (strcmp(atts[i], "z") == 0)
          convert(atts[i+1], z);
        else if (strcmp(atts[i], "r") == 0)
          convert(atts[i+1], r);
      }
      splr_symmetry_camera0_to_camera1 = vnl_quaternion<double> (x, y, z, r);
  }else if (strcmp(name, "bioproc_filtering_proc")==0) {
    current_elm = "bioproc_filtering_proc";
  }else if (strcmp(name, "bioproc_splr_filtering_proc")==0) {
    current_elm = "bioproc_splr_filtering_proc";
  }else if (strcmp(name, "vnl_quaternion") == 0) {
      biob_field_type = "rotation";
      double x, y, z, r;
      for (int i=0; atts[i]; i+=2) {
        if (strcmp(atts[i], "x") == 0)
          convert(atts[i+1], x);
        else if (strcmp(atts[i], "y") == 0)
          convert(atts[i+1], y);
        else if (strcmp(atts[i], "z") == 0)
          convert(atts[i+1], z);
        else if (strcmp(atts[i], "r") == 0)
          convert(atts[i+1], r);
      }
      if (strcmp(current_elm.data(), "bioproc_splr_filtering_proc") == 0) 
        splr_rotations.push_back(vnl_quaternion<double> (x, y, z, r));
  }
}


void 
proc_io_filter_xml_parser::endElement(const char* name)
{
   if(strcmp(name,"response")==0){
     if (cdata_.size() > 0) {
        cdataHandler(name, cdata_);
      }
   } else if (strcmp(name,"xmvg_filter_response")==0){
     if (cdata_.size() > 0) {
        cdataHandler(name, cdata_);
      }
   } else if (strcmp(name,"name")==0) {
     filter_name = cdata_;
   } else if (strcmp(name,"rotation_angle")==0) {
     convert(cdata_.data(), filter_rot_angle);
   } else if (strcmp(name,"rotation_axis")==0) {
     cdataHandler(name, cdata_);
   } else if (strcmp(name,"sigma_r")==0) {
     convert(cdata_.data(), filter_sigma_r);
   } else if (strcmp(name,"sigma_z")==0) {
     convert(cdata_.data(), filter_sigma_z);
   } else if (strcmp(name,"inner_radius")==0) {
     convert(cdata_.data(), filter_inner_rad);
   } else if (strcmp(name,"outer_radius")==0) {
     convert(cdata_.data(), filter_outer_rad);
   } else if (strcmp(name,"resolution") == 0) {
      convert(cdata_.data(), resolution_);
   } else if ((strcmp(name,"xmvg_gaussian_filter_descriptor")==0) ||
     (strcmp(name, "xmvg_no_noise_filter_descriptor") == 0) ||
     (strcmp(name,"composite_filter")==0)){
     if (strcmp(filter_name.data(), "gaussian_filter") == 0) {
        xmvg_gaussian_filter_descriptor d(filter_sigma_r, filter_sigma_z,
                                       filter_center, filter_orient);
        descriptor_list.push_back(d);
     } else if (strcmp(filter_name.data(), "no_noise_filter") == 0) {
        xmvg_no_noise_filter_descriptor d(filter_inner_rad, filter_outer_rad,
                                       filter_center, filter_orient);
        descriptor_list.push_back(d);
     }
     }  else if (strcmp(name, "biob_worldpt_field") == 0) {
       if (strcmp(biob_field_type.data(), "response") == 0) {
          biob_worldpt_roster_sptr roster = new biob_explicit_worldpt_roster(explicit_worldpt_roster);
          splr_response_field_ = biob_worldpt_field<xmvg_filter_response<double> > (roster);
          splr_response_field_.set_values(responses_);
       } else if (strcmp(biob_field_type.data(), "rotation") == 0) {
          biob_worldpt_roster_sptr roster = new biob_explicit_worldpt_roster(explicit_worldpt_roster);
          splr_rotation_field_ = biob_worldpt_field<vnl_quaternion <double> > (roster);
          splr_rotation_field_.set_values(splr_rotations);
       }
     }
   cdata_ = "";
}

void proc_io_filter_xml_parser::charData(const XML_Char* s, int len)
{
  const int leadingSpace = skipWhiteSpace(s);
  if (len==0 || len<=leadingSpace)
     return;  // called with whitespace between elements
  cdata_.append(s, len);
}

bool parse(vcl_string fname, proc_io_filter_xml_parser& parser) 
{
  vcl_FILE *xmlFile;

  if (fname.size() == 0){
    vcl_cout << "File not specified" << vcl_endl;
    return false;
  }

  xmlFile = vcl_fopen(fname.c_str(), "r");
  if (!xmlFile){
    vcl_cout << fname << "-- error on opening" << vcl_endl;
    return false;
  }
  if (!parser.parseFile(xmlFile)) {
    /*fprintf(stderr,
      "%s at line %d\n",
      XML_ErrorString(parser.XML_GetErrorCode()),
      parser.XML_GetCurrentLineNumber()
      );*/
     vcl_cout << XML_ErrorString(parser.XML_GetErrorCode()) << " at line " <<
        parser.XML_GetCurrentLineNumber() << vcl_endl;
     return false;
   }
   vcl_cout << "finished parsing!" << vcl_endl;

  return true;
}




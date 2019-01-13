#include "sdetd_cvlet_map_io.h"
#include <cstring>
#include <vul/vul_file.h>

#ifdef HAS_BOOST
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#endif


#ifdef HAS_BOOST
static bool sdetd_load_cvlet_map_gzip(std::string filename, sdet_edgemap_sptr &edge_map, sdet_curvelet_map &cvlet_map);
static bool sdetd_save_cvlet_map_gzip(std::string filename, sdet_curvelet_map &cvlet_map);
#endif

static bool sdetd_load_cvlet_map_ascii(std::string filename, sdet_edgemap_sptr &edge_map, sdet_curvelet_map &cvlet_map);

static bool sdetd_save_cvlet_map_ascii(std::string filename, sdet_curvelet_map &cvlet_map);

bool sdetd_load_cvlet_map(std::string filename, sdet_edgemap_sptr &edge_map, sdet_curvelet_map &cvlet_map)
{
  std::string ext = vul_file::extension(filename);

  if (ext == ".gz") {
#ifdef HAS_BOOST
    return sdetd_load_cvlet_map_gzip(filename, edge_map, cvlet_map);
#else
    std::cerr << "Error: .gz compressed cvlet file was provided, but boost wasn't found\n";
    return false;
#endif
  } else
    return sdetd_load_cvlet_map_ascii(filename, edge_map, cvlet_map);
}

bool sdetd_save_cvlet_map(std::string filename, sdet_curvelet_map &cvlet_map)
{
  std::string ext = vul_file::extension(filename);

  if (ext == ".gz") {
#ifdef HAS_BOOST
    return sdetd_save_cvlet_map_gzip(filename, cvlet_map);
#else
    std::cerr << "Error: .gz compressed output cvlet file requested, but boost wasn't found\n";
    return false;
#endif
  } else
    return sdetd_save_cvlet_map_ascii(filename, cvlet_map);
}

//: Save a curvelet map as a .cvlet file
bool sdetd_save_cvlet_map_ascii(std::string filename, sdet_curvelet_map &cvlet_map)
{
  //1) If file open fails, return.
  std::ofstream outfp(filename.c_str(), std::ios::out);

  if (!outfp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }
  outfp.precision(5);

  //2) write out the header block
  outfp << "CVLET_MAP v2.0" << std::endl << std::endl;

  outfp << "[BEGIN HEADER]" << std::endl;
  outfp << "WIDTH=" << cvlet_map.EM_->width() << std::endl;
  outfp << "HEIGHT=" << cvlet_map.EM_->height() << std::endl;
  outfp << "EDGE_COUNT=" << cvlet_map.EM_->num_edgels()  << std::endl;

  outfp << "CM TYPE=CC2" << std::endl;
  outfp << "N_RADIUS=" << cvlet_map.params_.rad_ << std::endl;
  outfp << "DX=" << cvlet_map.params_.dpos_ << std::endl;
  outfp << "DT=" << cvlet_map.params_.dtheta_ << std::endl;
  outfp << "TOKEN_LENGTH=" << cvlet_map.params_.token_len_ << std::endl;

  outfp << "[END HEADER]" << std::endl;
  outfp << std::endl << std::endl;

  //1) next save the edgemap
  outfp << "[BEGIN EDGEMAP]" << std::endl;
  outfp << "# Format :  [EID] [Sub_Pixel_Pos] Sub_Pixel_Dir Strength" << std::endl;
  
  for (unsigned i=0; i<cvlet_map.EM_->edgels.size(); i++){
    sdet_edgel* e = cvlet_map.EM_->edgels[i];
    outfp << "[" << e->id << "] " << "[" << e->pt.x() << ", " << e->pt.y() << "]   " << e->tangent << " " << e->strength << " " << std::endl;
  }
  outfp << "[END EDGEMAP]" << std::endl;
  outfp << std::endl << std::endl;

  //2) Then save the cvlet map
  outfp << "[BEGIN CVLETMAP]" << std::endl;
  outfp << "# Format :  [EID] (number of curvelets)" << std::endl;
  outfp << "            [edgels id] (forward) [ref_pt.x()] [ref_pt.y()] [ref_theta] [pt.x()] [pt.y()] [theta k] length property" << std::endl;
  for (unsigned i=0; i<cvlet_map.EM_->edgels.size(); i++){
    outfp << "<" << std::endl;
    outfp << "[" << i << "] ";

    //get the list curvelets anchored at this edge
    sdet_curvelet_list& cvlets = cvlet_map.curvelets(i);
    outfp << "(" << cvlets.size() << ")" << std::endl;

    sdet_curvelet_list_iter cvit = cvlets.begin();
    for (; cvit != cvlets.end(); cvit++){
      (*cvit)->print(outfp); 
    }
    outfp << ">" << std::endl;
  }
  outfp << "[END CVLETMAP]" << std::endl;

  return true;
}

#ifdef HAS_BOOST
//: Save a curvelet map as a .cvlet file
bool sdetd_save_cvlet_map_gzip(std::string filename, sdet_curvelet_map &cvlet_map)
{
  //1) If file open fails, return.
  std::ofstream outfp_orig(filename.c_str(), std::ios::out | std::ios::binary);

  if (!outfp_orig){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }
  outfp_orig.precision(5);

  boost::iostreams::filtering_ostream outfp;
  outfp.push(boost::iostreams::gzip_compressor());
  outfp.push(outfp_orig);

  //2) write out the header block
  outfp << "CVLET_MAP v1.0" << std::endl << std::endl;

  outfp << "[BEGIN HEADER]" << std::endl;
  outfp << "WIDTH=" << cvlet_map.EM_->width() << std::endl;
  outfp << "HEIGHT=" << cvlet_map.EM_->height() << std::endl;
  outfp << "EDGE_COUNT=" << cvlet_map.EM_->num_edgels()  << std::endl;

  outfp << "CM TYPE=CC2" << std::endl;
  outfp << "N_RADIUS=" << cvlet_map.params_.rad_ << std::endl;
  outfp << "DX=" << cvlet_map.params_.dpos_ << std::endl;
  outfp << "DT=" << cvlet_map.params_.dtheta_ << std::endl;
  outfp << "TOKEN_LENGTH=" << cvlet_map.params_.token_len_ << std::endl;

  outfp << "[END HEADER]" << std::endl;
  outfp << std::endl << std::endl;

  //1) next save the edgemap
  outfp << "[BEGIN EDGEMAP]" << std::endl;
  outfp << "# Format :  [EID] [Sub_Pixel_Pos] Sub_Pixel_Dir Strength" << std::endl;
  
  for (unsigned i=0; i<cvlet_map.EM_->edgels.size(); i++){
    sdet_edgel* e = cvlet_map.EM_->edgels[i];
    outfp << "[" << e->id << "] " << "[" << e->pt.x() << ", " << e->pt.y() << "]   " << e->tangent << " " << e->strength << " " << std::endl;
  }
  outfp << "[END EDGEMAP]" << std::endl;
  outfp << std::endl << std::endl;

  //2) Then save the cvlet map
  outfp << "[BEGIN CVLETMAP]" << std::endl;
  outfp << "# Format :  [EID] (number of curvelets)" << std::endl;
  outfp << "            [edgels id] (forward) [ref_pt.x() ref_pt.y() ref_theta pt.x() pt.y() theta k] length property" << std::endl;
  for (unsigned i=0; i<cvlet_map.EM_->edgels.size(); i++){
    outfp << "<" << std::endl;
    outfp << "[" << i << "] ";

    //get the list curvelets anchored at this edge
    sdet_curvelet_list& cvlets = cvlet_map.curvelets(i);
    outfp << "(" << cvlets.size() << ")" << std::endl;

    sdet_curvelet_list_iter cvit = cvlets.begin();
    for (; cvit != cvlets.end(); cvit++){
      (*cvit)->print(outfp); 
    }
    outfp << ">" << std::endl;
  }
  outfp << "[END CVLETMAP]" << std::endl;

  return true;
}
#endif

//: Loads an ascii file containing a curvelet map as well as the edgemap on which it is defined.
bool sdetd_load_cvlet_map_ascii(std::string filename, sdet_edgemap_sptr &edge_map, sdet_curvelet_map &cvlet_map)
{
  //Note: The cvlet map is defined on an existing edgemap so the cvlet map also contains the edgemap
  char buffer[2048];

  //1)If file open fails, return.
  std::ifstream infp(filename.c_str(), std::ios::in);
  if (!infp){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  //check the first line for file type
  infp.getline(buffer,2000);
  if (std::strncmp(buffer, "CVLET_MAP v2.0", sizeof("CVLET_MAP v2.0")-1))
    return false; 

  //2) read the header block
  int num_edges, w=0, h=0;
  double rad, dtheta, dpos, token_len;
  bool adap_uncer = false;
  //unsigned CM_type=1;
  double max_k = 0.3, max_gamma = 0.02;

  bool end_of_header = false;
  while (!end_of_header)
  {
    //read next line
    infp.getline(buffer,2000);

    //if (!std::strncmp(buffer, "[BEGIN HEADER]", sizeof("[BEGIN HEADER]")-1))

    if (!std::strncmp(buffer, "WIDTH", sizeof("WIDTH")-1))
      sscanf(buffer,"WIDTH=%d", &w);

    else if (!std::strncmp(buffer, "HEIGHT", sizeof("HEIGHT")-1))
      sscanf(buffer,"HEIGHT=%d", &h);

    else if (!std::strncmp(buffer, "EDGE_COUNT", sizeof("EDGE_COUNT")-1))
      sscanf(buffer,"EDGE_COUNT=%d", &num_edges);

    //else if (!std::strncmp(buffer, "CM TYPE", sizeof("CM TYPE")-1))
    //  sscanf(buffer,"CM TYPE=%d", &CM_type);

    else if (!std::strncmp(buffer, "N_RADIUS", sizeof("N_RADIUS")-1))
      sscanf(buffer,"N_RADIUS=%lf", &(rad));

    else if (!std::strncmp(buffer, "DX", sizeof("DX")-1))
      sscanf(buffer,"DX=%lf", &(dpos));

    else if (!std::strncmp(buffer, "DT", sizeof("DT")-1))
      sscanf(buffer,"DT=%lf", &(dtheta));

    else if (!std::strncmp(buffer, "TOKEN_LENGTH", sizeof("TOKEN_LENGTH")-1))
      sscanf(buffer,"TOKEN_LENGTH=%lf", &(token_len));

    else if (!std::strncmp(buffer, "[END HEADER]", sizeof("[END HEADER]")-1))
      end_of_header = true;
  }

  //update the edgemap and the curvelet map with the header info
  if (w==0 && h==0) { w=1000; h=1000; } // make sure w and h are reasonable

  // instantiate a new edge_map 
  edge_map = new sdet_edgemap(w, h);

  //add it to the curvelet map
  cvlet_map.set_edgemap(edge_map);

  //also set the right paramters to the curvelet map
  cvlet_map.set_parameters(sdet_curvelet_params(sdet_curve_model::CC2, rad, dtheta, dpos, adap_uncer, token_len, max_k, max_gamma));

  //3) read the edge map
  int id;
  double x, y, dir, conf;

  bool end_of_edge_block = false;
  while (!end_of_edge_block)
  {
    //read next line
    infp.getline(buffer,2000);

    if (std::strlen(buffer)<2 || buffer[0]=='#')
      continue;

    else if (!std::strncmp(buffer, "[BEGIN EDGEMAP]", sizeof("[BEGIN EDGEMAP]")-1))
      continue;
    
    else if (!std::strncmp(buffer, "[END EDGEMAP]", sizeof("[END EDGEMAP]")-1)){
      end_of_edge_block = true;
      continue;
    }

    //scan the line for edge info
    sscanf(buffer,"[%d] [%lf, %lf]  %lf %lf", &(id), &(x), &(y), &(dir), &(conf));

    //create an edgel token and insert it into the edgemap (the order of insertion should take care of the ids)
    edge_map->insert(new sdet_edgel(vgl_point_2d<double>(x, y), dir, conf));
  }

  //make sure we have all the edges
  assert(edge_map->edgels.size()==(unsigned)num_edges);

  //also resize the curvelet map
  cvlet_map.resize(num_edges);

  //4) read the curvelet map
  unsigned num_cvlets;
  int eid;
  unsigned char dummy, tag;
  double length, quality;

  bool end_of_cvlet_block = false;
  while (!end_of_cvlet_block)
  {
    //read next line
    infp.getline(buffer,2000);

    if (std::strlen(buffer)==0 || buffer[0]=='#')
      continue;

    //if (!std::strncmp(buffer, "[BEGIN CVLETMAP]", sizeof("[BEGIN CVLETMAP]")-1))

    //cvlet block
    if (buffer[0]=='<')
    {
      //read edgel id
      infp.getline(buffer,2000);
      sscanf(buffer,"[%d] (%d)", &(id), &(num_cvlets));

      //read all the curvelets anchored at this edgel
      for (unsigned i=0; i<num_cvlets; i++){
        sdet_edgel* e = edge_map->edgels[id];
        sdet_curvelet* cvlet = new sdet_curvelet(e);

        //read the edgel chain
        infp >> dummy;  // '['
        infp >> eid;
        while (!infp.fail()) //read in all the edgel ids until the "]" is reached
        { 
          cvlet->edgel_chain.push_back(edge_map->edgels[eid]);
          infp >> eid;
        }
        infp.clear(); //clear the fail bit
        infp >> dummy;  // ']'

        //read the forward/backward tag
        infp >> dummy; // '('
        infp >> tag;  // 'F' or 'B'
        if (tag == 'F')      cvlet->forward = true;
        else                 cvlet->forward = false;
        infp >> dummy; // ')'

        //now read the curve model
        //temp: assume CC3 curve model
        sdet_CC_curve_model_3d* cm = new sdet_CC_curve_model_3d();
        cm->read(infp);
        cm->compute_best_fit();//cvlet->edgel_chain
        cvlet->curve_model = cm;

        //some more curvelet parameters
        infp >> length >> quality;
        cvlet->length = length;
        cvlet->quality = quality;

        //read to the end of the line
        infp.getline(buffer,2000);

        //add it into the curvelet map
        cvlet_map.add_curvelet(cvlet);
      }

      //make sure that we are at the end of the cvlet block
      infp.getline(buffer,2000);
      if (buffer[0]!='>')
        return false;

    }

    else if (!std::strncmp(buffer, "[END CVLETMAP]", sizeof("[END CVLETMAP]")-1))
      end_of_cvlet_block = true;
  }

  return true;
}

#ifdef HAS_BOOST
//: Loads an ascii file containing a curvelet map as well as the edgemap on which it is defined.
bool sdetd_load_cvlet_map_gzip(std::string filename, sdet_edgemap_sptr &edge_map, sdet_curvelet_map &cvlet_map)
{
  //Note: The cvlet map is defined on an existing edgemap so the cvlet map also contains the edgemap
  char buffer[2048];

  //1)If file open fails, return.
  std::ifstream infp_orig(filename.c_str(), std::ios::in | std::ios::binary);
  if (!infp_orig){
    std::cout << " Error opening file  " << filename.c_str() << std::endl;
    return false;
  }

  boost::iostreams::filtering_istream infp;
  infp.push(boost::iostreams::gzip_decompressor());
  infp.push(infp_orig);

  //check the first line for file type
  infp.getline(buffer,2000);
  if (std::strncmp(buffer, "CVLET_MAP v1.0", sizeof("CVLET_MAP v1.0")-1))
    return false; 

  //2) read the header block
  int num_edges, w=0, h=0;
  double rad, dtheta, dpos, token_len;
  bool adap_uncer = false;
  //unsigned CM_type=1;
  double max_k = 0.3, max_gamma = 0.02;

  bool end_of_header = false;
  while (!end_of_header)
  {
    //read next line
    infp.getline(buffer,2000);

    //if (!std::strncmp(buffer, "[BEGIN HEADER]", sizeof("[BEGIN HEADER]")-1))

    if (!std::strncmp(buffer, "WIDTH", sizeof("WIDTH")-1))
      sscanf(buffer,"WIDTH=%d", &w);

    else if (!std::strncmp(buffer, "HEIGHT", sizeof("HEIGHT")-1))
      sscanf(buffer,"HEIGHT=%d", &h);

    else if (!std::strncmp(buffer, "EDGE_COUNT", sizeof("EDGE_COUNT")-1))
      sscanf(buffer,"EDGE_COUNT=%d", &num_edges);

    //else if (!std::strncmp(buffer, "CM TYPE", sizeof("CM TYPE")-1))
    //  sscanf(buffer,"CM TYPE=%d", &CM_type);

    else if (!std::strncmp(buffer, "N_RADIUS", sizeof("N_RADIUS")-1))
      sscanf(buffer,"N_RADIUS=%lf", &(rad));

    else if (!std::strncmp(buffer, "DX", sizeof("DX")-1))
      sscanf(buffer,"DX=%lf", &(dpos));

    else if (!std::strncmp(buffer, "DT", sizeof("DT")-1))
      sscanf(buffer,"DT=%lf", &(dtheta));

    else if (!std::strncmp(buffer, "TOKEN_LENGTH", sizeof("TOKEN_LENGTH")-1))
      sscanf(buffer,"TOKEN_LENGTH=%lf", &(token_len));

    else if (!std::strncmp(buffer, "[END HEADER]", sizeof("[END HEADER]")-1))
      end_of_header = true;
  }

  //update the edgemap and the curvelet map with the header info
  if (w==0 && h==0) { w=1000; h=1000; } // make sure w and h are reasonable

  // instantiate a new edge_map 
  edge_map = new sdet_edgemap(w, h);

  //add it to the curvelet map
  cvlet_map.set_edgemap(edge_map);

  //also set the right paramters to the curvelet map
  cvlet_map.set_parameters(sdet_curvelet_params(sdet_curve_model::CC2, rad, dtheta, dpos, adap_uncer, token_len, max_k, max_gamma));

  //3) read the edge map
  int id;
  double x, y, dir, conf;

  bool end_of_edge_block = false;
  while (!end_of_edge_block)
  {
    //read next line
    infp.getline(buffer,2000);

    if (std::strlen(buffer)<2 || buffer[0]=='#')
      continue;

    else if (!std::strncmp(buffer, "[BEGIN EDGEMAP]", sizeof("[BEGIN EDGEMAP]")-1))
      continue;
    
    else if (!std::strncmp(buffer, "[END EDGEMAP]", sizeof("[END EDGEMAP]")-1)){
      end_of_edge_block = true;
      continue;
    }

    //scan the line for edge info
    sscanf(buffer,"[%d] [%lf, %lf]  %lf %lf", &(id), &(x), &(y), &(dir), &(conf));

    //create an edgel token and insert it into the edgemap (the order of insertion should take care of the ids)
    edge_map->insert(new sdet_edgel(vgl_point_2d<double>(x, y), dir, conf));
  }

  //make sure we have all the edges
  assert(edge_map->edgels.size()==(unsigned)num_edges);

  //also resize the curvelet map
  cvlet_map.resize(num_edges);

  //4) read the curvelet map
  unsigned num_cvlets;
  int eid;
  unsigned char dummy, tag;
  double length, quality;

  bool end_of_cvlet_block = false;
  while (!end_of_cvlet_block)
  {
    //read next line
    infp.getline(buffer,2000);

    if (std::strlen(buffer)==0 || buffer[0]=='#')
      continue;

    //if (!std::strncmp(buffer, "[BEGIN CVLETMAP]", sizeof("[BEGIN CVLETMAP]")-1))

    //cvlet block
    if (buffer[0]=='<')
    {
      //read edgel id
      infp.getline(buffer,2000);
      sscanf(buffer,"[%d] (%d)", &(id), &(num_cvlets));

      //read all the curvelets anchored at this edgel
      for (unsigned i=0; i<num_cvlets; i++){
        sdet_edgel* e = edge_map->edgels[id];
        sdet_curvelet* cvlet = new sdet_curvelet(e);

        //read the edgel chain
        infp >> dummy;  // '['
        infp >> eid;
        while (!infp.fail()) //read in all the edgel ids until the "]" is reached
        { 
          cvlet->edgel_chain.push_back(edge_map->edgels[eid]);
          infp >> eid;
        }
        infp.clear(); //clear the fail bit
        infp >> dummy;  // ']'

        //read the forward/backward tag
        infp >> dummy; // '('
        infp >> tag;  // 'F' or 'B'
        if (tag == 'F')      cvlet->forward = true;
        else                 cvlet->forward = false;
        infp >> dummy; // ')'

        //now read the curve model
        //temp: assume CC3 curve model
        sdet_CC_curve_model_3d* cm = new sdet_CC_curve_model_3d();
        cm->read(infp);
        cm->compute_best_fit();//cvlet->edgel_chain
        cvlet->curve_model = cm;

        //some more curvelet parameters
        infp >> length >> quality;
        cvlet->length = length;
        cvlet->quality = quality;

        //read to the end of the line
        infp.getline(buffer,2000);

        //add it into the curvelet map
        cvlet_map.add_curvelet(cvlet);
      }

      //make sure that we are at the end of the cvlet block
      infp.getline(buffer,2000);
      if (buffer[0]!='>')
        return false;

    }

    else if (!std::strncmp(buffer, "[END CVLETMAP]", sizeof("[END CVLETMAP]")-1))
      end_of_cvlet_block = true;
  }

  return true;
}
#endif

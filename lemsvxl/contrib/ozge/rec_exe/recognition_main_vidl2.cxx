
// aerial_vehicle_segmentation.cpp : Defines the entry point for the DLL application.
#include<cstdio>
//#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>

//#include <vil/vil_image_resource.h>
//#include <bpro1/bpro1_parameters_sptr.h>
//#include <bpro1/bpro1_parameters.h>
//#include <bpro1/bpro1_process_sptr.h>
//#include <vidpro1/vidpro1_repository.h>
//#include <vidpro1/process/vidpro1_load_video_process.h>
//#include <vidpro1/vidpro1_process_manager.h>
//#include <brip/brip_vil_float_ops.h>
//#include <vsol/vsol_point_2d.h>
//#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsl/vsl_binary_io.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_label.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation_matcher.h>
#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>

#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
//#include <brip/brip_vil_float_ops.h>
#include <vil/vil_convert.h>
//std::vector<vgl_point_2d<double> > meanpts;

#if HAS_DSHOW
#include <cstdio>
#include <vil/vil_flip.h>
#include <vil/vil_decimate.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vidl/vidl_dshow_file_istream.h>
#endif

bool check_labels(dbru_label_sptr input_label, dbru_label_sptr object_label) 
{
  if (input_label->view_angle_bin_         != object_label->view_angle_bin_ ||
      input_label->shadow_angle_bin_       != object_label->shadow_angle_bin_ ||
      input_label->shadow_length_          != object_label->shadow_length_)
      return false;

  if (input_label->motion_orientation_bin_ == object_label->motion_orientation_bin_ )
       //||
      //input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_+1)%8 ||
      //input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_-1+8)%8 )
      return true;
  return false;
}

bool combine_osls(unsigned int out_handle, unsigned int handle1, unsigned int handle2) 
{
  vbl_array_1d<dbru_object_sptr> *out_osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (out_handle);  
  vbl_array_1d<dbru_object_sptr> *osl1 = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle1);  
  vbl_array_1d<dbru_object_sptr> *osl2 = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle2);  

  std::cout << "out osl initial size: " << out_osl->size() << " (should be zero)\n";
  for (unsigned int i = 0; i<osl1->size(); i++) {
    out_osl->push_back((*osl1)[i]);
  }
  std::cout << "added: " << osl1->size() << " obs from osl1\n";
  
  for (unsigned int i = 0; i<osl2->size(); i++) {
    out_osl->push_back((*osl2)[i]);
  }
  std::cout << "added: " << osl2->size() << " obs from osl2\n";

  return true;
}

unsigned int load_osl(const char *osl_file_name) 
{
  std::cout << "loading osl: " << osl_file_name << " ...";
  vbl_array_1d<dbru_object_sptr> *osl = new vbl_array_1d<dbru_object_sptr>();
  vsl_b_ifstream obfile(osl_file_name);
  unsigned int size;
  vsl_b_read(obfile, size);
  for (unsigned int i = 0; i<size; i++) {
    dbru_object_sptr obj = new dbru_object();
    obj->b_read(obfile);
    osl->push_back(obj);
  }
  std::cout << " Done!\n";

  obfile.close();
  std::cout << osl->size() << " objects and observations are read from binary input file\n";
  return reinterpret_cast<unsigned int>(osl);
}

bool write_osl(unsigned int handle, const char *osl_file_name) {
  
  std::cout << "writing osl ";
  vbl_array_1d<dbru_object_sptr> *osl_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  std::cout << "size: " << osl_objects->size();
  
  if (osl_objects->size() > 0) {
    //vsl_add_to_binary_loader(dbru_object);
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, osl_objects->size());
    for (unsigned i = 0; i<osl_objects->size(); i++) {
      (*osl_objects)[i]->b_write(obfile);
    }
    obfile.close();
    std::cout << " objects and observations are written to binary output file\n";
    return true;
  } else {
    std::cout << "No objects to write\n";
    return false;
  }

}


#if HAS_DSHOW


bool extract_observations(unsigned int handle, const char *video_file_name, const char *output_dir, int factor, float sigma) {

  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  std::cout << "osl size: " << osl->size() << std::endl;

  std::cout << "CAUTION: Assuming all the objects in the osl file are coming from the same video stream!\n";
  std::cout << "Make sure that the frame numbers are original frame numbers of the stream!\n";

  vidl_dshow_file_istream vs(video_file_name);
  if (vs.is_open() && vs.is_seekable()) {
    std::cout << "Stream is opened ok!\n";
  } else {
    std::cout << "Problems in opening video stream or it is not seekable! Quitting!\n";
    return false;
  }

  // correlate frame numbers with obj number and polygon number
  
  // assume max frame number is 100000
  std::vector<std::pair<int, int> > tmp;
  vbl_array_1d< std::vector<std::pair<int, int> > > frame_to_ids(100000, tmp);

  int max_frame = 0;
  for (unsigned i = 0; i<osl->size(); i++) {
    dbru_object_sptr obj = (*osl)[i];

    for (unsigned int j = 0; j<obj->polygon_cnt_; j++) {
      vsol_polygon_2d_sptr poly = obj->get_polygon(j);
      unsigned int s = poly->size();
      if (s == 0) 
        continue;
      
      //: real original frame number is one minus stored in the object file
      int real_j = obj->start_frame_+j;
      real_j--;
      
      //: add this frame number into the map for this object
      if (real_j < 0 || real_j >= 100000) {
        std::cout << "frame number is not withing our 100,000 limit or negative, quitting!\n";
        return false;
      }

      if (max_frame < real_j) 
        max_frame = real_j;

      (frame_to_ids[real_j]).push_back(std::pair<int, int> (i, j));    
      std::cout << "adding frame: " << real_j << " obj id: " << i << " poly id: " << j << std::endl;
    }
  }

  std::cout << "MAX FRAME is: " << max_frame << std::endl;

  // now load the video from min_frame to max_frame
  
  //std::map<int, std::pair<unsigned int, unsigned int> >::iterator iter = frame_to_ids.begin();
  int current_frame_no = 0;
  while (frame_to_ids[current_frame_no].size() == 0) 
    current_frame_no++;

  std::cout << "seeking to frame number: " << current_frame_no << "... ";
  vs.seek_frame(current_frame_no);  std::cout << "Done!\n";

  int height, width;
  vidl_frame_sptr frame = vs.current_frame();
  vil_image_resource_sptr imgr;
  if (frame) {
   height = frame->nj();
   width = frame->ni();
   std::cout << "orig frame number: " << vs.frame_number() << " width (ni): " << width << " height (nj): " << height << " ... ";
 
   std::cout << " processing frame... ";
   vil_image_view<vxl_byte> img, img_ud;
   vil_image_view<float> dummy, dummy2, dummy3(height/factor, width/factor), dummy4;
   vidl_convert_to_view_rgb(frame,img);
   img_ud = vil_flip_ud(img);
   vil_convert_planes_to_grey(img_ud, dummy4);  //brip_vil_float_ops::convert_to_float(img_ud);
   vil_gauss_filter_2d(dummy4, dummy, sigma, unsigned(std::floor(3*sigma+0.5)));
   dummy2 = vil_decimate(dummy, factor, factor);
   dummy3.deep_copy(dummy2);
   imgr = vil_new_image_resource_of_view(dummy3);
   std::cout << " Done!\n";


   } else {
    std::cout << "could not read frame!\nQuitting\n";
    return false;
  }

  for(int kk = current_frame_no; kk <= max_frame; )
  {
    for (unsigned jj = 0; jj < frame_to_ids[kk].size(); jj++) {
      std::pair<int, int> id_pair = frame_to_ids[kk][jj];
      std::cout << "extracting obj id: " << id_pair.first << " poly id: " << id_pair.second << std::endl;
      dbru_object_sptr obj = (*osl)[id_pair.first];
      vsol_polygon_2d_sptr poly = obj->get_polygon(id_pair.second);
      unsigned int s = poly->size();
      if (s == 0) {
        std::cout << "CAUTION: 0 sized polygon, this should not happen!!!!\n";
        return false;
      }

      dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly, true, true, false);
      if (!obs->scan(0, imgr)) {
        std::cout << "Could not scan the object!\n";
        return false;
      }
      obj->set_observation(id_pair.second, obs);

#if 1
      // output the images
      char buffer[1000];
      
      std::sprintf(buffer, "./%s/image_obj%d-poly%d.png", output_dir, id_pair.first, id_pair.second);
      std::string filename = buffer;
      poly->compute_bounding_box();

      int w = (int)std::floor(poly->get_max_x()-poly->get_min_x()+10+0.5);
      int h = (int)std::floor(poly->get_max_y()-poly->get_min_y()+10+0.5);

      int mx = (int)std::floor(poly->get_min_x());
      int my = (int)std::floor(poly->get_min_y());
      int maxx = (int)std::ceil(poly->get_max_x());
      int maxy = (int)std::ceil(poly->get_max_y());

      dbinfo_region_geometry_sptr geo = obs->geometry();
      vil1_memory_image_of<float> image_out(w,h);

      for (int y = 0; y<h; y++ ) 
        for (int x = 0; x<w; x++)
          image_out[y][x] = 255;

      dbinfo_feature_data_base_sptr d0 = (obs->features())[0]->data();
      
      assert(d0->format() == DBINFO_INTENSITY_FEATURE);
      dbinfo_feature_data<vbl_array_1d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(d0);
      vbl_array_1d<float>& v0 = cd0->single_data();       
      unsigned npts0 = v0.size();

      for(unsigned k = 0; k<npts0; ++k)
      {
        float vv0 = v0[k];
        vgl_point_2d<float> coord = geo->point(k);
        int yy = (int)(coord.y()-my+5);
        int xx = (int)(coord.x()-mx+5);
        image_out[yy][xx] = vv0;
      }

      vil1_memory_image_of<unsigned char> output_img = brip_vil1_float_ops::convert_to_byte(image_out, 0, 255.0f);
      vil_image_resource_sptr output_sptr = vil1_to_vil_image_resource(output_img);
      vil_save_image_resource(output_sptr, filename.c_str());
      
#endif
    }

    kk++;
    if (kk > max_frame) break;
    vs.advance();

    bool tag = false;
    while (frame_to_ids[kk].size() == 0) {
      vs.advance();
      kk++;
      if (kk > max_frame) {
        tag = true;
        break;
      }
    } 
    if (tag)
      break;

    frame = vs.current_frame();
    if (frame) {
      height = frame->nj();
      width = frame->ni();
      std::cout << "orig frame number: " << vs.frame_number() << " width (ni): " << width << " height (nj): " << height << " ... ";

      std::cout << " processing frame... ";
      vil_image_view<vxl_byte> img, img_ud;
      vil_image_view<float> dummy, dummy2, dummy3(height/factor, width/factor), dummy4;
      vidl_convert_to_view_rgb(frame,img);
      img_ud = vil_flip_ud(img);
      vil_convert_planes_to_grey(img_ud, dummy4);  //brip_vil_float_ops::convert_to_float(img_ud);
      vil_gauss_filter_2d(dummy4, dummy, sigma, unsigned(std::floor(3*sigma+0.5)));
      dummy2 = vil_decimate(dummy, factor, factor);
      dummy3.deep_copy(dummy2);
      imgr = vil_new_image_resource_of_view(dummy3);
      std::cout << " Done!\n";
    } else {
      std::cout << "could not read frame!\nQuitting\n";
      return false;
    }
  }

  return true;
}


//: db_list_filename is the xml file that stores each object, and its polygons
//  video_list_file_name is <id, directory> pair for each video which have objects in the database
int createosl(unsigned int handle, const char *object_list_filename, const char *video_file_name, const char *osl_file_name, int factor, float sigma) 
{ 
  //std::ofstream ofile("temp.txt");
  //std::string home("/projects/vorl1/");
  //std::string home("d:/lockheed_videos/");
  std::cout << "CAUTION: Assuming all the objects in the input file are coming from the same video stream!\n";
  std::cout << "Make sure that the frame numbers are original frame numbers of the stream!\n";

  vidl_dshow_file_istream vs(video_file_name);
  if (vs.is_open() && vs.is_seekable()) {
    std::cout << "Stream is opened ok!\n";
  } else {
    std::cout << "Problems in opening video stream or it is not seekable! Quitting!\n";
    return 0;
  }
  
  vbl_array_1d<dbru_object_sptr> *osl_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  osl_objects->clear();
  std::cout << "initial database size should be zero: " << osl_objects->size() << std::endl;
  
  std::string output_dir = osl_file_name;
  output_dir = output_dir.substr(0, output_dir.length()-4);
  std::cout << "will output images into the directory: " << output_dir << " , please create this directory under the current dir\n";

  std::cout << "reading objects...\n";
  std::ifstream dbfp(object_list_filename);
  if (!dbfp) {
    std::cout << "Problems in opening object list file!\n";
    return -1;
  }

  // first determine the number of objects in the file\n";
  char buffer[1000]; 
  std::string line;

  int size = 0;
  // found the first object
  std::string object_string = "<object";
  while (!dbfp.eof()) {
    std::string dummy;
    //dbfp.getline(buffer, 1000);
    dbfp >> dummy;
    //line = buffer;
    //std::cout << "line: " << line << std::endl;
    //std::cout << "dummy: " <<dummy << std::endl;
    //pos1 = line.find("<object", 0);
    //if (pos1 != std::string::npos)
    if (dummy == object_string) 
      size++;
  }
  dbfp.close();
  std::cout << "Found " << size << " objects in the xml file\n";
  if (!size) {
    std::cout << "objects file do not contain any objects! Quitting!\n";
    return -1;
  }

  std::ifstream dbfp2(object_list_filename);
  if (!dbfp2) {
    std::cout << "Problems in opening object list file!\n";
    return -1;
  }

  dbfp2.getline(buffer, 1000);  // comment 
  
  for (int i = 0; i<size; i++) {
    std::cout << "reading object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp2)) { 
      std::cout << "problems in reading object number: " << i << std::endl;
      return 0;
    }
    
    std::cout << "read: " << (*obj) << "extracting observations assuming 1 polygon per frame" << std::endl;
    
    std::cout << "seeking to frame number: " << obj->start_frame_-1 << "... ";
    vs.seek_frame(obj->start_frame_-1);
    std::cout << "Done!\n";
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      vsol_polygon_2d_sptr poly = obj->get_polygon(j-obj->start_frame_);
      int s = poly->size();
      if (s == 0) {
        obj->add_observation(0);  // add null pointer as observation
        vs.advance();
        continue;
      }

      vidl_frame_sptr frame = vs.current_frame();
      int height, width;
      if (frame) {
        height = frame->nj();
        width = frame->ni();
        std::cout << "orig frame number: " << vs.frame_number() << " width (ni): " << width << " height (nj): " << height << " ... ";
      } else {
        std::cout << "could not read frame!\nQuitting\n";
        return 0;
      }
      
      vs.advance();
      
      
      //vil_image_view<vil_rgb<vxl_byte> > img, img_ud, dummy, dummy2, dummy3(height/factor, width/factor);
      //vil_image_view<vil_rgb<vxl_byte> > img, img_ud;
      vil_image_view<vxl_byte> img, img_ud;
      vil_image_view<float> dummy, dummy2, dummy3(height/factor, width/factor);
      vidl_convert_to_view_rgb(frame,img);
      img_ud = vil_flip_ud(img);
      
      vil_image_view<float> dummy4;
      vil_convert_planes_to_grey(img_ud, dummy4);  //brip_vil_float_ops::convert_to_float(img_ud);

      //vil_gauss_filter_2d(img_ud, dummy, sigma, unsigned(std::floor(3*sigma+0.5)));
      vil_gauss_filter_2d(dummy4, dummy, sigma, unsigned(std::floor(3*sigma+0.5)));

      dummy2 = vil_decimate(dummy, factor, factor);
      dummy3.deep_copy(dummy2);

      std::cout << "dummy3 ni: " << dummy3.ni() << " nj: " << dummy3.nj() << std::endl;
      vil_image_resource_sptr imgr = vil_new_image_resource_of_view(dummy3);
      std::cout << "imgr ni: " << imgr->ni() << " nj: " << imgr->nj() << std::endl;
      
      std::cout << " IP Done! getting polygon: " << j-obj->start_frame_ << " from obj\n";
      
      dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly, true, true, false);
      if (!obs->scan(0, imgr)) {
        std::cout << "Could not scan the object!\n";
        return -1;
      }
      obj->add_observation(obs);
#if 1
      // output the images
      char buffer[1000];
      
      std::sprintf(buffer, "./%s/image_obj%d-poly%d.png",output_dir.c_str(), i, j-obj->start_frame_);
      std::string filename = buffer;
      poly->compute_bounding_box();

      int w = (int)std::floor(poly->get_max_x()-poly->get_min_x()+10+0.5);
      int h = (int)std::floor(poly->get_max_y()-poly->get_min_y()+10+0.5);

      int mx = (int)std::floor(poly->get_min_x());
      int my = (int)std::floor(poly->get_min_y());
      int maxx = (int)std::ceil(poly->get_max_x());
      int maxy = (int)std::ceil(poly->get_max_y());

      dbinfo_region_geometry_sptr geo = obs->geometry();
      vil1_memory_image_of<float> image_out(w,h);

      for (int y = 0; y<h; y++ ) 
        for (int x = 0; x<w; x++)
          image_out[y][x] = 255;

      dbinfo_feature_data_base_sptr d0 = (obs->features())[0]->data();
      
      assert(d0->format() == DBINFO_INTENSITY_FEATURE);
      dbinfo_feature_data<vbl_array_1d<float> >* cd0 = 
          dbinfo_feature_data<vbl_array_1d<float> >::ptr(d0);
      vbl_array_1d<float>& v0 = cd0->single_data();       
      unsigned npts0 = v0.size();

      for(unsigned k = 0; k<npts0; ++k)
      {
        float vv0 = v0[k];
        vgl_point_2d<float> coord = geo->point(k);
        int yy = (int)(coord.y()-my+5);
        int xx = (int)(coord.x()-mx+5);
        image_out[yy][xx] = vv0;
      }

      vil1_memory_image_of<unsigned char> output_img = brip_vil1_float_ops::convert_to_byte(image_out, 0, 255.0f);
      vil_image_resource_sptr output_sptr = vil1_to_vil_image_resource(output_img);
      vil_save_image_resource(output_sptr, filename.c_str());
      
#endif
    }

    if (obj->get_observations_size() == obj->polygon_cnt_)
      //database_objects.push_back(obj);
      osl_objects->push_back(obj);
    else {
      std::cout << "problems in object " << *(obj) << " skipping!\n";
      continue;
    }
  }

  if (osl_objects->size() > 0) {
    //vsl_add_to_binary_loader(dbru_object);
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, osl_objects->size());
    for (unsigned i = 0; i<osl_objects->size(); i++) {
      (*osl_objects)[i]->b_write(obfile);
    }
    obfile.close();
    std::cout << "Objects and observations are written to binary output file\n";
  }

  return 0;
}
#endif //#if HAS_DSHOW

//: db_list_filename is the xml file that stores each object, and its polygons
//  video_list_file_name is <id, directory> pair for each video which have objects in the database
bool create_empty_osl(unsigned int handle, const char *object_list_filename, const char *osl_file_name) 
{ 
  //std::ofstream ofile("temp.txt");
  //std::string home("/projects/vorl1/");
  //std::string home("d:/lockheed_videos/");
  std::cout << "CAUTION: Assuming all the objects in the input file are coming from the same video stream!\n";
  std::cout << "Make sure that the frame numbers are original frame numbers of the stream!\n";

  vbl_array_1d<dbru_object_sptr> *osl_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  osl_objects->clear();
  std::cout << "initial database size should be zero: " << osl_objects->size() << std::endl;
  
  std::cout << "reading objects...\n";
  std::ifstream dbfp(object_list_filename);
  if (!dbfp) {
    std::cout << "Problems in opening object list file!\n";
    return false;
  }

  // first determine the number of objects in the file\n";
  char buffer[1000]; 
  std::string line;

  int size = 0;
  // found the first object
  std::string object_string = "<object";
  while (!dbfp.eof()) {
    std::string dummy;
    dbfp >> dummy;
    if (dummy == object_string) 
      size++;
  }
  dbfp.close();
  std::cout << "Found " << size << " objects in the xml file\n";
  
  if (!size) {
    std::cout << "objects file do not contain any objects! Quitting!\n";
    return false;
  }

  std::ifstream dbfp2(object_list_filename);
  if (!dbfp2) {
    std::cout << "Problems in opening object list file!\n";
    return false;
  }

  dbfp2.getline(buffer, 1000);  // comment 
  
  for (int i = 0; i<size; i++) {
    std::cout << "reading object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp2)) { 
      std::cout << "problems in reading object number: " << i << std::endl;
      return false;
    }
    
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      // just add null for all observations they will be scaned later
      obj->add_observation(0);  // add null pointer as observation
    }

    if (obj->get_observations_size() == obj->polygon_cnt_)
      osl_objects->push_back(obj);
    else {
      std::cout << "problems in object " << *(obj) << " skipping!\n";
      return false;
    }
  }

  if (osl_objects->size() > 0) {
    //vsl_add_to_binary_loader(dbru_object);
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, osl_objects->size());
    for (unsigned i = 0; i<osl_objects->size(); i++) {
      (*osl_objects)[i]->b_write(obfile);
    }
    obfile.close();
    std::cout << "Objects and observations are written to binary output file\n";
  }

  return true;
}



//: db_list_filename is the xml file that stores each object, and its polygons
//  add the objects in file to the binary osl, no observations are created!!!
bool add_to_osl_only_objects(unsigned int handle, const char *object_list_filename) 
{ 

  //std::ofstream ofile("temp.txt");
  //std::string home("/projects/vorl1/");
  //std::string home("d:/lockheed_videos/");
  std::cout << "CAUTION: Assuming all the objects in the input file are coming from the same video stream!\n";
  std::cout << "Make sure that the frame numbers are original frame numbers of the stream!\n";

  vbl_array_1d<dbru_object_sptr> *osl_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  std::cout << "initial database size: " << osl_objects->size() << std::endl;
  
  std::cout << "reading objects...\n";
  std::ifstream dbfp(object_list_filename);
  if (!dbfp) {
    std::cout << "Problems in opening object list file!\n";
    return false;
  }

  // first determine the number of objects in the file\n";
  char buffer[1000]; 
  std::string line;

  int size = 0;
  // found the first object
  std::string object_string = "<object";
  while (!dbfp.eof()) {
    std::string dummy;
    //dbfp.getline(buffer, 1000);
    dbfp >> dummy;
    //line = buffer;
    //std::cout << "line: " << line << std::endl;
    //std::cout << "dummy: " <<dummy << std::endl;
    //pos1 = line.find("<object", 0);
    //if (pos1 != std::string::npos)
    if (dummy == object_string) 
      size++;
  }
  dbfp.close();
  std::cout << "Found " << size << " objects in the xml file\n";
  
  if (!size) {
    std::cout << "objects file do not contain any objects! Quitting!\n";
    return false;
  }

  std::ifstream dbfp2(object_list_filename);
  if (!dbfp2) {
    std::cout << "Problems in opening object list file!\n";
    return false;
  }

  dbfp2.getline(buffer, 1000);  // comment 
  
  for (int i = 0; i<size; i++) {
    std::cout << "reading object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp2)) { 
      std::cout << "problems in reading object number: " << i << std::endl;
      return false;
    }
    
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      // just add null for all observations they will be scaned later
      obj->add_observation(0);  // add null pointer as observation
    }

    if (obj->get_observations_size() == obj->polygon_cnt_)
      osl_objects->push_back(obj);
    else {
      std::cout << "problems in object " << *(obj) << " skipping!\n";
      return false;
    }
  }

  return true;
}

bool write_osl_to_database(unsigned int handle, int mot, int view, int shadow, int length, const char *database_file)
{
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  std::cout << "osl size: " << osl->size() << std::endl;
  std::map<std::string, int> category_id_map;
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;

  std::ofstream dbfp(database_file);
  if (!dbfp) {
    std::cout << "Problems in opening database file!\n";
    return false;
  }

  std::vector<std::pair<unsigned int, unsigned int> > ids;

  dbru_label_sptr target_label = new dbru_label("null", mot, view, shadow, length);
  for (unsigned i = 0; i<osl->size(); i++) {
    dbru_object_sptr obj = (*osl)[i];
    
    for (unsigned j = 0; j<obj->polygon_cnt_; j++) {
      if (obj->get_polygon(j)->size() == 0) continue;
      
      dbru_label_sptr input_label = obj->labels_[j];
      if (check_labels(input_label, target_label)) {
        ids.push_back(std::pair<unsigned int, unsigned int> (i, j));
      }

    }
  }

  dbfp << ids.size() << std::endl;
  for (unsigned i = 0; i<ids.size(); i++) {
    dbfp << ids[i].first << " " << ids[i].second << "\n";
  }

  dbfp.close();
  return true;

}

int display_osl(unsigned int handle, int mot, int view, int shadow, int length, bool print_labels) {
  vbl_array_1d<dbru_object_sptr> *osl = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  std::cout << "initial database size: " << osl->size() << std::endl;
  std::map<std::string, int> category_id_map;
  category_id_map["minivan"] = 0;
  category_id_map["suv"] = 0;
  category_id_map["utility"] = 0;
  category_id_map["car"] = 1;
  category_id_map["pick-up truck"] = 2;
  category_id_map["van"] = 3;

  std::vector<int> category_cnts(4, 0);
  std::vector<int> category_frame_cnts(4, 0);
  
  dbru_label_sptr target_label = new dbru_label("null", mot, view, shadow, length);
  for (unsigned i = 0; i<osl->size(); i++) {
    dbru_object_sptr obj = (*osl)[i];
    dbru_label_sptr input_label = obj->labels_[0];
    if (print_labels)
      std::cout << (*input_label) << std::endl;

    if (check_labels(input_label, target_label)) 
      category_cnts[category_id_map[obj->category_]]++;
    for (unsigned j = 0; j<obj->polygon_cnt_; j++) {
      if (obj->get_polygon(j)->size() == 0) continue;
      dbru_label_sptr input_label = obj->labels_[j];
      if (check_labels(input_label, target_label)) {
        std::cout << "obj: " << i << " poly: " << j << " label: " << (*input_label) << std::endl;
        category_frame_cnts[category_id_map[obj->category_]]++;
      }
    }
  }
  std::cout << "car cnt: " << category_cnts[1] << " frame cnt: " << category_frame_cnts[1] << std::endl;
  std::cout << "pickup cnt: " << category_cnts[2] << " frame cnt: " << category_frame_cnts[2] << std::endl;
  std::cout << "utility cnt: " << category_cnts[0] << " frame cnt: " << category_frame_cnts[0] << std::endl;

  return 0;
}

unsigned int find_first_occurance(std::string name, std::vector<std::string>& names) {
  
  for (unsigned i = 0; i<names.size(); i++) {
    std::string name_i = names[i];
    if (name == names[i]) 
      return i;
  }
  std::cout << "IF HERE NOT RETURNED!!!\n";
  return 10000000;
}

bool analyse_out_file(std::string database_file, bool curve) {
  std::cout << "analysing the output file: " << database_file << std::endl;
  std::ifstream is(database_file.c_str());
  if (!is) {
    std::cout << "Problems in opening database file!\n";
    return false;
  }
  
  std::string p = "pick-up";
  std::string u = "utility";
  std::string c = "car";
  std::string null = "null";

  std::string dummy;
  is >> dummy; is >> dummy; // db size:
  std::cout << "read: " << dummy << std::endl;
  int size;
  is >> size;
  std::cout << " size: " << size << std::endl;
  std::vector<std::string> names, shape_matches, info_matches, info_dt_matches;
  for (int i = 0; i<size; i++) {
    std::string name;
    is >> dummy;
    if (dummy == "total")
      break;

    if (dummy == p) {
      is >> dummy;
      name = "pick-up_"+dummy;
      //names.push_back("pick-up_"+dummy);
    } else {
      //names.push_back(dummy);
      name = dummy;
    }

    is >> dummy;  // polyid
    //name = name + "_" + dummy;
    names.push_back(name);
    
    is >> dummy; is >> dummy; is >> dummy; // sm best match
    is >> dummy; 
    if (dummy == p) {
      is >> dummy;
      shape_matches.push_back("pick-up");
    } else {
      char *category = std::strtok ((char *)(dummy.c_str()),"_");
      std::string d2 = category;
      shape_matches.push_back(d2);
    }
    
    is >> dummy; is >> dummy; is >> dummy; // info best match
    is >> dummy; 
    if (dummy == p) {
      is >> dummy;
      info_matches.push_back("pick-up");
    } else {
      char *category = std::strtok ((char *)(dummy.c_str()),"_");
      std::string d2 = category;
      info_matches.push_back(d2);
    }
    
    if (curve) {
      is >> dummy; is >> dummy; is >> dummy; // info_dt best match
      is >> dummy; 
      if (dummy == p) {
        is >> dummy;
        info_dt_matches.push_back("pick-up");
      } else {
        char *category = std::strtok ((char *)(dummy.c_str()),"_");
        std::string d2 = category;
        info_dt_matches.push_back(d2);
      }
    }
  }
  is.close();

  /*if (!(names.size() == size)) {
    std::cout << "problems in the input file\n";
    return false;
  }*/

  std::map<std::pair<std::string, std::string>, int> instance_shape_cnt;
  std::map<std::pair<std::string, std::string>, int> instance_info_cnt;
  std::map<std::pair<std::string, std::string>, int> instance_info_dt_cnt;
  
  // initialize these as zero for all categories and each name
  for (unsigned i = 0; i<names.size(); i++) {
    std::cout << names[i] << std::endl;
    std::pair<std::string, std::string> key(names[i], c);
    instance_shape_cnt[key] = 0;
    instance_info_cnt[key] = 0;
    instance_info_dt_cnt[key] = 0;
    key = std::pair<std::string, std::string> (names[i], p);
    instance_shape_cnt[key] = 0;
    instance_info_cnt[key] = 0;
    instance_info_dt_cnt[key] = 0;
    key = std::pair<std::string, std::string> (names[i], u);
    instance_shape_cnt[key] = 0;
    instance_info_cnt[key] = 0;
    instance_info_dt_cnt[key] = 0;
  }

  std::map<std::pair<std::string, std::string>, int>::iterator iter;
  for (unsigned i = 0; i<names.size(); i++) {
    std::string name = names[i];
    /*
    std::cout << "i: " << i << " name: " << names[i] << " ";
    std::cout << " shape match: " << shape_matches[i] << " info match: " << info_matches[i] << std::endl;
   */
    std::pair<std::string, std::string> key(name, shape_matches[i]);
    instance_shape_cnt[key]++;
    
    key = std::pair<std::string, std::string> (name, info_matches[i]);
    instance_info_cnt[key]++;

    if (curve) {
      key = std::pair<std::string, std::string> (name, info_dt_matches[i]);
      instance_info_dt_cnt[key]++;
    }

  }

  for (iter = instance_shape_cnt.begin(); iter != instance_shape_cnt.end(); iter++) {
    std::cout << "for " << (iter->first).first << " " << (iter->first).second << " shape corrects: " << iter->second << std::endl;
  }

  int total_cnt = 0;
  int correct_cnt = 0;

  for (iter = instance_shape_cnt.begin(); iter != instance_shape_cnt.end(); iter++) {
    std::string name = (iter->first).first;
    if ((iter->first).second != c) continue;
    total_cnt++;
    
    //unsigned int id = find_first_occurance(name, names);
    //if (instance_shape_classification[id] == null) {
      std::pair<std::string, std::string> c_key(name, c);
      std::pair<std::string, std::string> p_key(name, p);
      std::pair<std::string, std::string> u_key(name, u);
      int c_cnt = instance_shape_cnt[c_key];
      int p_cnt = instance_shape_cnt[p_key];
      int u_cnt = instance_shape_cnt[u_key];

      std::string::size_type pos;
      if (c_cnt >= p_cnt && c_cnt >= u_cnt) {
        if (name.find(c) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << c << "\n";
      }

      if (p_cnt >= c_cnt && p_cnt >= u_cnt) {
        if (name.find(p) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << p << "\n";
      }

      if (u_cnt >= p_cnt && u_cnt >= c_cnt) {
        if (name.find(u) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << u << "\n";
      }

        
    //}

  }
  std::cout << "shape correct: " << correct_cnt << " out of " << total_cnt << " perc: " << ((double)correct_cnt/total_cnt)*100.0f << std::endl;


  total_cnt = 0;
  correct_cnt = 0;

  for (iter = instance_info_cnt.begin(); iter != instance_info_cnt.end(); iter++) {
    std::string name = (iter->first).first;
    if ((iter->first).second != c) continue;
    total_cnt++;
    
    //unsigned int id = find_first_occurance(name, names);
    //if (instance_shape_classification[id] == null) {
      std::pair<std::string, std::string> c_key(name, c);
      std::pair<std::string, std::string> p_key(name, p);
      std::pair<std::string, std::string> u_key(name, u);
      int c_cnt = instance_info_cnt[c_key];
      int p_cnt = instance_info_cnt[p_key];
      int u_cnt = instance_info_cnt[u_key];

      std::string::size_type pos;
      if (c_cnt >= p_cnt && c_cnt >= u_cnt) {
        if (name.find(c) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << c << "\n";
      }

      if (p_cnt >= c_cnt && p_cnt >= u_cnt) {
        if (name.find(p) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << p << "\n";
      }

      if (u_cnt >= p_cnt && u_cnt >= c_cnt) {
        if (name.find(u) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << u << "\n";
      }

        
    //}

  }
  std::cout << "info correct: " << correct_cnt << " out of " << total_cnt << " perc: " << ((double)correct_cnt/total_cnt)*100.0f << std::endl;

  if (curve) {
  int total_cnt = 0;
  int correct_cnt = 0;

  for (iter = instance_info_dt_cnt.begin(); iter != instance_info_dt_cnt.end(); iter++) {
    std::string name = (iter->first).first;
    if ((iter->first).second != c) continue;
    total_cnt++;
    
    //unsigned int id = find_first_occurance(name, names);
    //if (instance_shape_classification[id] == null) {
      std::pair<std::string, std::string> c_key(name, c);
      std::pair<std::string, std::string> p_key(name, p);
      std::pair<std::string, std::string> u_key(name, u);
      int c_cnt = instance_info_dt_cnt[c_key];
      int p_cnt = instance_info_dt_cnt[p_key];
      int u_cnt = instance_info_dt_cnt[u_key];

      std::string::size_type pos;
      if (c_cnt >= p_cnt && c_cnt >= u_cnt) {
        if (name.find(c) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << c << "\n";
      }

      if (p_cnt >= c_cnt && p_cnt >= u_cnt) {
        if (name.find(p) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << p << "\n";
      }

      if (u_cnt >= p_cnt && u_cnt >= c_cnt) {
        if (name.find(u) != std::string::npos)
          correct_cnt++;

        std::cout << name << " " << u << "\n";
      }

        
    //}

  }
  std::cout << "info dt correct: " << correct_cnt << " out of " << total_cnt << " perc: " << ((double)correct_cnt/total_cnt)*100.0f << std::endl;
  }


/*
  for (iter = instance_info_cnt.begin(); iter != instance_info_cnt.end(); iter++) {
    std::cout << "for " << (iter->first).first << " " << (iter->first).second << " info corrects: " << iter->second << std::endl;
    std::string name = (iter->first).first;
    unsigned int id = find_first_occurance(name, names);
    if (instance_info_classification[id] == null) {
      std::pair<std::string, std::string> c_key(name, c);
      std::pair<std::string, std::string> p_key(name, p);
      std::pair<std::string, std::string> u_key(name, u);
      int c_cnt = instance_info_cnt[c_key];
      int p_cnt = instance_info_cnt[p_key];
      int u_cnt = instance_info_cnt[u_key];

      if (c_cnt > p_cnt && c_cnt > u_cnt)
        instance_info_classification[id] = c;
      if (p_cnt > c_cnt && p_cnt > u_cnt)
        instance_info_classification[id] = p;
      if (u_cnt > p_cnt && u_cnt > c_cnt)
        instance_info_classification[id] = u;
    }

  }

  if (curve) 
    for (iter = instance_info_dt_cnt.begin(); iter != instance_info_dt_cnt.end(); iter++) {
      std::cout << "for " << (iter->first).first << " " << (iter->first).second << " info dt corrects: " << iter->second << std::endl;
      std::string name = (iter->first).first;
      unsigned int id = find_first_occurance(name, names);
      if (instance_info_dt_classification[id] == null) {     
        std::pair<std::string, std::string> c_key(name, c);
        std::pair<std::string, std::string> p_key(name, p);
        std::pair<std::string, std::string> u_key(name, u);
        int c_cnt = instance_info_dt_cnt[c_key];
        int p_cnt = instance_info_dt_cnt[p_key];
        int u_cnt = instance_info_dt_cnt[u_key];
    
        if (c_cnt > p_cnt && c_cnt > u_cnt)
          instance_info_dt_classification[id] = c;
        if (p_cnt > c_cnt && p_cnt > u_cnt)
          instance_info_dt_classification[id] = p;
        if (u_cnt > p_cnt && u_cnt > c_cnt)
          instance_info_dt_classification[id] = u;
      }
    }
   
  
/*
  for (unsigned i = 0; i<names.size(); i++) {
    std::string name = names[i];
    std::cout << "instance:\t" << name << " shape class: " << instance_shape_classification[i] << std::endl;
    std::cout << "instance:\t" << name << " info class: " << instance_info_classification[i] << std::endl;
    if (curve)
      std::cout << "instance:\t" << name << " info dt class: " << instance_info_dt_classification[i] << std::endl;
  }
  */ 
  return true;
}

void print_usage() {
  std::cout << "USAGE:\n 0: create empty osl and write out: <object file> <out_osl>\n";
  std::cout << "1: add to osl (no observations extracted) and write out: <object file> <in_osl> <out_osl>\n";
  std::cout << "2: extract observations and write out: <video file> <in_osl> <out_osl> <output_dir> <decimate factor 2> <sigma for image smoothing before decimation, 1.0>\n";
  std::cout << "3: display osl content <in_osl> <motion> <view> <shadow> <shadow length> <1 if print labels, otherwise 0>\n";
  std::cout << "4: create database file <in_osl> <motion> <view> <shadow> <shadow length> <db_file>\n";
  std::cout << "5: analyse out file of mathcing processes: <out_file_name> <0 if shock, 1 if curve> \n";
  std::cout << "6: combine two osl files: <in_osl_1> <in_osl_2> <out_osl>\n";
}

#if HAS_DSHOW
int main(int argc, char *argv[]) {

  if (argc < 2) {
    print_usage();
    return 0;
  }

  std::string obj_file, out_osl, in_osl, in_osl2, video_file, database_file, output_dir;
  int option = atoi(argv[1]);
  switch (option) {
    case 0: { 
      if (argc != 4) {print_usage(); return 0;}
      obj_file = argv[2];
      out_osl = argv[3];
      vbl_array_1d<dbru_object_sptr> *osl = new vbl_array_1d<dbru_object_sptr>();
      create_empty_osl(reinterpret_cast<unsigned int>(osl), obj_file.c_str(), out_osl.c_str());
      break;
            }
    case 1: {
      if (argc != 5) {print_usage(); return 0;}
      obj_file = argv[2];
      in_osl = argv[3];
      out_osl = argv[4];
      
      //LOAD OSL PREVIOUSLY CREATED
      vbl_array_1d<dbru_object_sptr> *osl = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));
      
      if (!add_to_osl_only_objects(reinterpret_cast<unsigned int>(osl), obj_file.c_str())) {
        std::cout << "objects not added!\nQuitting!\n";
        return 0;
      }

      write_osl(reinterpret_cast<unsigned int>(osl), out_osl.c_str());
      break;
            }
    case 2: {
      if (argc != 8) {print_usage(); return 0;}
      video_file = argv[2];
      in_osl = argv[3];
      out_osl = argv[4];
      output_dir = argv[5];
      int factor = atoi(argv[6]);
      float sigma = float(atof(argv[7]));
      
      //LOAD OSL PREVIOUSLY CREATED
      vbl_array_1d<dbru_object_sptr> *osl = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));
      extract_observations(reinterpret_cast<unsigned int>(osl), video_file.c_str(), output_dir.c_str(), factor, sigma);
      write_osl(reinterpret_cast<unsigned int>(osl), out_osl.c_str());
      break;
            }
    case 3: {
      if (argc != 8) {print_usage(); return 0;}
      in_osl = argv[2];
      int mot = atoi(argv[3]);
      int view = atoi(argv[4]);
      int sh = atoi(argv[5]);
      int sh_l = atoi(argv[6]);
      int dummy = atoi(argv[7]);
      bool print_label = true;
      if (dummy == 0)
        print_label = false;

      //LOAD OSL PREVIOUSLY CREATED
      vbl_array_1d<dbru_object_sptr> *osl = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));
      display_osl(reinterpret_cast<unsigned int>(osl),mot, view, sh, sh_l, print_label);
      break;
            }
    case 4: {
      if (argc != 8) {print_usage(); return 0;}
      in_osl = argv[2];
      int mot = atoi(argv[3]);
      int view = atoi(argv[4]);
      int sh = atoi(argv[5]);
      int sh_l = atoi(argv[6]);
      database_file = argv[7];

      //LOAD OSL PREVIOUSLY CREATED
      vbl_array_1d<dbru_object_sptr> *osl = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));
      display_osl(reinterpret_cast<unsigned int>(osl),mot, view, sh, sh_l, false);
      write_osl_to_database(reinterpret_cast<unsigned int>(osl),mot, view, sh, sh_l, database_file.c_str());
      break;
            }
    case 5: {
      std::cout << "argc: " << argc << std::endl;
      if (argc != 4) {print_usage(); return 0;}
      database_file = argv[2];
      int dummy = atoi(argv[3]);
      std::cout << "read option: " << dummy << std::endl;
      bool curve = true;
      if (dummy == 0)
        curve = false;
      analyse_out_file(database_file, curve);
      break;
            }
     case 6: {
      if (argc != 5) {print_usage(); return 0;}
      in_osl = argv[2];
      in_osl2 = argv[3];
      out_osl = argv[4];
      
      //LOAD OSL PREVIOUSLY CREATED
      vbl_array_1d<dbru_object_sptr> *osl1 = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl.c_str()));

      vbl_array_1d<dbru_object_sptr> *osl2 = 
        reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (load_osl(in_osl2.c_str()));

      vbl_array_1d<dbru_object_sptr> *outosl = new vbl_array_1d<dbru_object_sptr>();
      combine_osls(reinterpret_cast<unsigned int>(outosl), 
                   reinterpret_cast<unsigned int>(osl1), 
                   reinterpret_cast<unsigned int>(osl2));
      
      write_osl(reinterpret_cast<unsigned int>(outosl), out_osl.c_str());
      break;
            }
    default: {print_usage(); return 0;}
  };
  return 0;
}
#endif //#if HAS_DSHOW

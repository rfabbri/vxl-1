//---------------------------------------------------------------------
// This is breye/vorl/exec/vorl_mirec_exec.cxx
//:
// \file
// \brief process to find recognition category 
//        of a given "unknown" object by comparing it one by one
//        by each object in the given "database" list
//        The recognition method used is mutual information between regions under
//        known region pixel correspondences
//
// \author
//  O.C. Ozcanli - July 06, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vorl/vorl.h>
#include <vidpro/vidpro_repository.h>

#include <vsol/vsol_polygon_2d.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>

#include <dbru/dbru_object_sptr.h>
#include <dbru/dbru_object.h>
#include <dbru/dbru_label.h>
#include <dbru/pro/dbru_mutual_info_process.h>

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_io.h>

#include <vil/vil_save.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_crop.h>
#include <brip/brip_vil_float_ops.h>

//#include <vul/vul_file_iterator.h>
//#include <vul/vul_file.h>
//#include <vgl/vgl_point_2d.h>

bool parse_database_file(vcl_istream &dataf, 
                         vcl_vector<vcl_string> &filenames, 
                         vcl_vector<vcl_string> &paths, 
                         vcl_vector<vcl_string> &filedirs,
                         vcl_vector<int> &object_ids) 
{
  char buffer[1000];
  dataf.getline(buffer, 1000);
  vcl_string line(buffer);
  //: scan all lines before the table
  while(line.find("<table", 0) == vcl_string::npos)  { // not found, continue
    dataf.getline(buffer, 1000);
    line = vcl_string(buffer);
  }

  vcl_string::size_type pos1, pos2;
  while(line.find("</table", 0) == vcl_string::npos)  { // not found, continue
    if (line.find("<filename", 0) != vcl_string::npos) { // found
      pos1 = line.find("![CDATA[",0);
      pos2 = line.find("]", pos1+1);
      vcl_string dummy = line.substr(pos1+8, pos2-pos1-8);
      //vcl_cout << "filename: " << dummy<< vcl_endl;
      filenames.push_back(dummy);
    } else if (line.find("<path", 0) != vcl_string::npos) { // found
      pos1 = line.find("![CDATA[",0);
      pos2 = line.find("]", pos1+1);
      vcl_string dummy = line.substr(pos1+8, pos2-pos1-8);
      //vcl_cout << "path: " << dummy<< vcl_endl;
      paths.push_back(dummy);
    } else if (line.find("<filedir", 0) != vcl_string::npos) { // found
      pos1 = line.find("![CDATA[",0);
      pos2 = line.find("]", pos1+1);
      vcl_string dummy = line.substr(pos1+8, pos2-pos1-8);
      //vcl_cout << "filedir: " << dummy << vcl_endl;
      filedirs.push_back(dummy);
    } else if (line.find("<objectid", 0) != vcl_string::npos) { // found
      pos1 = line.find(">",0);
      pos2 = line.find("<", pos1+1);
      vcl_string dummy = line.substr(pos1+1, pos2-pos1-1);
      int id;
      sscanf(dummy.c_str(), "%d", &id);
      //vcl_cout << "id: " << id;
      object_ids.push_back(id);
    } 

    dataf.getline(buffer, 1000);
    line = vcl_string(buffer);
    //vcl_cout << line << vcl_endl;
  }

  if (filenames.size() != paths.size() || 
      filenames.size() != filedirs.size()) {
    return false;
  }
  
  return true;
  
}

bool check_labels(dbru_label_sptr input_label, dbru_label_sptr object_label) 
{
  if (input_label->view_angle_bin_         != object_label->view_angle_bin_ ||
      input_label->shadow_angle_bin_       != object_label->shadow_angle_bin_ ||
      input_label->shadow_length_          != object_label->shadow_length_)
      return false;

  if (input_label->motion_orientation_bin_ == object_label->motion_orientation_bin_ ||
      input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_+1)%8 ||
      input_label->motion_orientation_bin_ == (object_label->motion_orientation_bin_-1+8)%8 )
      return true;
  return false;
}

int main(int argc, char** argv)
{
  // Register storage types and processes
  REG_STORAGE(vidpro_image_storage);
  REG_STORAGE(vidpro_vsol2D_storage);

  //==========================================================

  // Make each process and add it to the list of program args
  bpro_process_sptr my_process(new dbru_mutual_info_process()); 
  
  vcl_vector<vcl_string> input_names;
  input_names.push_back("vsol2D0");
  input_names.push_back("video");
  input_names.push_back("vsol2D1");
  input_names.push_back("video1");

  my_process->set_input_names(input_names);
  my_process->set_output_names(vcl_vector<vcl_string>(1,"image"));
  my_process->parameters()->add( "Associated segmented contour file:" , "-segcorfile" , bpro_filepath("","*") ); 
  my_process->parameters()->add( "Associated database file:" , "-databasefile" , bpro_filepath("","*") ); 
  
  vorl_manager::instance()->add_process_to_args(my_process); 
  vorl_manager::instance()->parse_params(argc, argv); 
  vcl_string output_dir = vorl_manager::instance()->get_output_dir(); 
  //vcl_cout << output_dir << vcl_endl;
  
  vorl_manager::instance()->load_video_clip();
  vidpro_repository_sptr rep = vorl_manager::instance()->repository();
  
  bpro_filepath segcorfile, databasefile;
  my_process->parameters()->get_value("-databasefile", databasefile);
  my_process->parameters()->get_value("-segcorfile", segcorfile);

  //vcl_cout << "database file: " << databasefile << vcl_endl;
  //vcl_cout << "object segmentation file: " << segcorfile << vcl_endl;

  vcl_ifstream dataf(databasefile.path.c_str());
  if (!dataf) {
    vcl_cout << "Problems in opening associated database file of the algorithm!\n";
    return 0;
  }

  vcl_ifstream fp(segcorfile.path.c_str());
  if (!fp) {
    vcl_cout << "Problems in opening associated file of input object!\n";
    return 0;
  }

  //-----------------------------------
  //: database file is opened, parse it
  vcl_vector<vcl_string> filenames;
  vcl_vector<vcl_string> paths;
  vcl_vector<vcl_string> filedirs;
  vcl_vector<int> object_ids;

  if (!parse_database_file(dataf, filenames, paths, filedirs, object_ids)) {
    vcl_cout << "Problems in parsing database file!\n";
    return 0;
  }
  dataf.close();
  
  dbru_object_sptr input_object = new dbru_object();
  if (!input_object->read_xml(fp)) {
    vcl_cout << "Problems in parsing input object!!\n";
    return 0;
  }
  fp.close();

  int start_frame = vorl_manager::instance()->start_frame();
  int end_frame = vorl_manager::instance()->end_frame();
  if (input_object->polygon_cnt_ == 0 || input_object->start_frame_ > start_frame ||
                                         input_object->end_frame_ < end_frame) 
  {
    vcl_cout << "No polygons or Input object start and end frames do not match with loaded frames\n";
    return 0;
  }

  //vcl_cout << "input object: \n" << *input_object << vcl_endl;
 
  int input_object_id = vorl_manager::instance()->get_object_id();
  
  //vcl_string home("/projects/vorl/");
  vcl_string home("d:/lockheed_videos/");
  vcl_vector<dbru_object_sptr> database_objects;  
  //vcl_vector<vidl1_movie_sptr> database_movies;  
  
  vcl_cout << "reading database objects...\n";
  //: read the objects from their associated files, eliminate the ones with wrong labels, also eliminate 
  //  input object from the database if its also there
  
  vcl_map<int, vidl1_movie_sptr> video_map;
  vcl_vector<int> database_ids;
  for (unsigned int i = 0; i<filenames.size(); i++) {
    //vcl_cout << "reading database object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    vcl_string dummy = home+filedirs[i];
    //vcl_cout << "database object id: " << object_ids[i] << " assocfile: " << dummy << vcl_endl;
    vcl_ifstream ifp(dummy.c_str());
    
    if (!ifp) {
      vcl_cout << "Could not open file with path: " << dummy << vcl_endl;
      return 0;
    }

    if (!obj->read_xml(ifp)) {
      vcl_cout << "problems in parsing segmented contours of database object number: " << i;
      vcl_cout << " with path: " << dummy << vcl_endl;
      return 0;
    }

    ifp.close();

    //: do not add itself into the database
    if (input_object_id == object_ids[i]) continue;

    //: load video file for this database object if necessary
    vcl_map<int, vidl1_movie_sptr>::iterator it = video_map.find(obj->video_id_);
    if (it == video_map.end()) {  // not loaded yet, load it

      vcl_string video_filename;
      if (paths[i].size() == 0 || paths[i] == "null")
        video_filename = home + filenames[i];
      else
        video_filename = home + filenames[i] + "/" + paths[i];

      //vcl_cout << "video filename: " << video_filename << vcl_endl;
      vidl1_movie_sptr my_movie = vidl1_io::load_movie(video_filename.c_str());
      if (!my_movie) {
        vcl_cout << "problems in loading video of database object number: " << i;
        vcl_cout << " with video file name: " << video_filename << vcl_endl;
        return 0;
      }
      video_map[obj->video_id_] = my_movie;
    } 
    
    
    //vcl_cout << "read: " << *obj << vcl_endl;
    database_objects.push_back(obj);
    //database_movies.push_back(my_movie);
    database_ids.push_back(object_ids[i]);
  }
  unsigned int database_size = database_objects.size();
  vcl_cout << "loaded all objects successfully! database size: " << database_size << " checking input comparability!" << vcl_endl;
  bool comparable = false;
  
  //: for now assuming all object frames have the same label
  //for (unsigned int k = 0; k<input_object->polygon_cnt_; k++) {
  
    dbru_label_sptr input_label = input_object->labels_[0];
  
    for (unsigned int i = 0; i<database_size; i++) {
      dbru_object_sptr obj = database_objects[i];
      if (obj->category_ != input_object->category_) 
        continue;

      for (unsigned int j = 0; j<obj->polygon_cnt_; j++) {
        vsol_polygon_2d_sptr poly2 = obj->get_polygon(j);
        if (poly2->size() <= 0) continue;
        dbru_label_sptr object_label = obj->labels_[j];
        if (!check_labels(input_label, object_label)) continue;
        
        comparable = true;
        break;
      }
        
      if (comparable) break;
    }

  //}

  if (!comparable) {
    vcl_cout << "There exists no database object with the same category and label, exiting!\n";
    return 0;
  }
  vcl_cout << "There exists obejcts comparable to input, start testing...!\n";
    
  vorl_manager::instance()->add_process_to_queue(my_process);
  end_frame = end_frame - start_frame + 1;
  for (int j = start_frame-input_object->start_frame_; j<end_frame; j++) {
    
    vorl_manager::instance()->write_status();
    vsol_polygon_2d_sptr poly = input_object->get_polygon(j);
    if (poly->size() <= 0) {
      //: report as wrongly classified
      vorl_manager::instance()->set_performance(0, 1);
      continue; 
    }
    
    dbru_label_sptr input_label = input_object->labels_[j];
    
    vcl_vector< vsol_spatial_object_2d_sptr > contour1;
    dbru_label_sptr label = input_object->get_label(j);
      
    contour1.push_back(poly->cast_to_spatial_object());
    vidpro_vsol2D_storage_sptr vsol1 = vidpro_vsol2D_storage_new();
    vsol1->add_objects(contour1, "poly1");
    vsol1->set_name("vsol2D0");
    if (!rep->store_data(vsol1)) {
      vcl_cout << "Problems in adding input object polygon into repository as vsol2D\n";
      return 0;
    }
    
    float best_total_info = 0;
    int best_id = 0;
    int best_frame = 0;
    bpro_storage_sptr best_str; 

    int cnt = 0;
    for (unsigned int i = 0; i<database_size; i++) {

      dbru_object_sptr obj = database_objects[i];
      
      for (unsigned int k = 0; k < obj->polygon_cnt_; k++) {
        vidl1_frame_sptr pframe = video_map[obj->video_id_]->get_frame(obj->start_frame_+k); //database_movies[i]->first();
        vsol_polygon_2d_sptr poly2 = obj->get_polygon(k);
        if (poly2->size() <= 0) continue;
        
        //: add image
        vidpro_image_storage_sptr image_storage = vidpro_image_storage_new();
        vil_image_resource_sptr img=pframe->get_resource();
        image_storage->set_image( img );
        image_storage->set_name("video1");
        if (!rep->store_data(image_storage)) {
          vcl_cout << "Problems in adding database object: " << i << " polygon no: " << k << " into repository as image\n";
          return 0;
        }
          
        //: add contour
        vcl_vector< vsol_spatial_object_2d_sptr > contour2;
        contour2.push_back(poly2->cast_to_spatial_object());
        vidpro_vsol2D_storage_sptr vsol2 = vidpro_vsol2D_storage_new();
        vsol2->add_objects(contour2, "poly2");
        vsol2->set_name("vsol2D1");
        if (!rep->store_data(vsol2)) {
          vcl_cout << "Problems in adding database object: " << i << " polygon no: " << k << " into repository as vsol2D\n";
          return 0;
        }

        dbru_label_sptr object_label = obj->labels_[k];
        if (!check_labels(input_label, object_label)) continue;

        vcl_cout << "input object (" << input_object_id << ")";
        vcl_cout << " polygon (" << j << ")";
        vcl_cout << " vs database object (" << database_ids[i] << ")";
        vcl_cout << " polygon (" << k << ") ";

        vorl_manager::instance()->run_process_queue_on_current_frame();
        dbru_mutual_info_process* my_process_ptr = dynamic_cast<dbru_mutual_info_process*> (my_process.ptr());
        float total_info = my_process_ptr->get_total_info();
        //vcl_cout << total_info << vcl_endl;
        if (total_info > best_total_info) {
          best_total_info = total_info;
          best_id = i;
          best_frame = k;
          best_str = rep->get_data(image_storage->type(), 0, 2);
        }

        //: clear repository from images and vsol of this database object
        if (!rep->pop_data(image_storage->type()) ||  // output image
            !rep->pop_data(image_storage->type()) ||  // img
            !rep->pop_data(vsol2->type())) {          // vsol
          vcl_cout << "Problems in removing database object: " << i << " polygon no: " << k << " from repository\n";
          return 0;
        }
        cnt++;
      }
    }
  
    if (input_label->category_name_ != database_objects[best_id]->get_label(best_frame)->category_name_) {
      vcl_cout << "classified WRONGly as: " << database_objects[best_id]->get_label(best_frame)->category_name_;
                                           //(true positive, true negative)  (no negatives in this case)
      vcl_cout << " compared with: " << cnt << " frames ";
      vcl_cout << "best total info: " << best_total_info << vcl_endl;
      //: set false positive and negative the same
      vorl_manager::instance()->set_performance(0, 0);
    } else {
      vcl_cout << "classified CORRECTly as: " << database_objects[best_id]->get_label(best_frame)->category_name_;
      vcl_cout << " compared with: " << cnt << " frames ";
      vcl_cout << "best total info: " << best_total_info << vcl_endl;
      vorl_manager::instance()->set_performance(1, 1);
    }

    vidpro_image_storage_sptr output_image;
    output_image.vertical_cast(best_str);
    vil_image_resource_sptr output_img = output_image->get_image();
    char output_image_name[1000];
    sprintf(output_image_name, "%s//out_image_%d_%d_%d_%d.png", output_dir.c_str(), input_object_id, j, database_ids[best_id], best_frame);
    //: assume that output is saved as unsigned char
    vil_image_view<unsigned char> oi = output_img->get_view();
    vil_save(oi, output_image_name);
    
    rep->go_to_next_frame();
  }
  
  // Write final status notice (100%)
  vorl_manager::instance()->write_status(1.0);
  vorl_manager::instance()->write_performance("");
  
  return 0; 
}





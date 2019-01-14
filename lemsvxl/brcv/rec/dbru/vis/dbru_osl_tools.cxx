// This is brcv/rec/dbru/vis/dbru_osl_tools.cxx
//:
// \file

#include "dbru_osl_tools.h"
#include <sstream>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_new.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui_command.h> 
#include <vgui/vgui_viewer2D_tableau.h>
#include <vgui/vgui_shell_tableau.h>
#include <bgui/bgui_image_tableau.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <bvis1/bvis1_manager.h>
#include <bvis1/bvis1_mapper.h>
#include <bpro1/bpro1_storage.h>
#include <bpro1/bpro1_storage_sptr.h>
#include <dbru/vis/dbru_osl_displayer.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <dbinfo/dbinfo_observation.h>

#include <dbru/pro/dbru_osl_storage.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_label.h>
#include <dbru/dbru_object.h>
#include <dbskr/algo/dbskr_object_matcher.h>
#include <dbinfo/dbinfo_observation.h>

#include <vidl1/vidl1_movie.h>
#include <vidl1/vidl1_io.h>
#include <vidl1/vidl1_frame_sptr.h>

#include <dbinfo/dbinfo_object_matcher.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <vgl/vgl_distance.h>


//----------------------------------------------------------------------------
//: A vgui command to edit an osl
class dbru_add_objects_command : public vgui_command
{
public:
  dbru_add_objects_command(dbru_osl_add_objects_tool* tool) : tool_(tool) {}
  void execute()
  {
    static std::string objects_file ="/home/dec/objects/*";
    static std::string video_file ="/home/dec/images/";
    static int start_frame = 0;
    static std::string ext = "*.xml";
    static std::string ext2 = "*.*";
    //static std::string cls = "";
    //static std::string doc = "";
    static int videoid = 0;
    vgui_dialog param_dlg("Add Objects");
    param_dlg.file("Objects File", ext, objects_file);
    param_dlg.field("Start frame of polygons in the original video stream: ", start_frame);
    param_dlg.field("Video Id", videoid);
    param_dlg.file("Video File", ext2, video_file);
    if(!param_dlg.ask())
      return;
    if(!tool_->add_objects(objects_file, videoid, video_file, videoid, start_frame))
      std::cout << "objects not successfully added\n";
  }
  dbru_osl_add_objects_tool* tool_;
};

//----------------------------------------------------------------------------
//: A vgui command to edit an osl
class dbru_add_object_ins_command : public vgui_command
{
public:
  dbru_add_object_ins_command(dbru_osl_add_objects_tool* tool) : tool_(tool) {}
  void execute()
  {
    static std::string objects_file ="/home/dec/objects/*";
    static std::string video_file ="/home/dec/images/";
    static std::string poly_file ="/home/dec/";
    static std::string ins_file ="/home/dec/";
    static std::string ext = "*.xml";
    static std::string ext2 = "*.*";
    static std::string ext3 = "*.txt";
    static std::string ext4 = "*.bin";
    //static std::string cls = "";
    //static std::string doc = "";
    static int videoid = 0;
    static int offset = 0;
    static int start_frame = 0;
    vgui_dialog param_dlg("Add Objects with Instances");

    param_dlg.file("Polygon File", ext3, poly_file);
    //: In this GUI frames are loaded only if they are in the form of image files in a directory
    //  For example if original video used to create polygon file was a .m2t file
    //  then images need to be extracted and saved in a directory to be used by this GUI
    //  And in that case that directory not necessarily contains images that start from frame 0 in the original video
    //  The starting frame of the polygons in the polygon file should be supplied via this dialog
    //  If polygons had been created and labeled from an image sequence which is identically being used via this GUI
    //  then start_frame number is simply 0
    param_dlg.field("Start frame of polygons in the original video stream: ", start_frame);
    param_dlg.field("Offset to start in Polygon file", offset);
    param_dlg.file("Objects File", ext, objects_file);
    param_dlg.field("Video Id", videoid);
    param_dlg.file("Video File", ext2, video_file);
    param_dlg.file("Multiple Instance File", ext4, ins_file);
    static double ratio = 0.5f;
    param_dlg.field("Ratio of width to use as threshold for overlap: ", ratio); 
    static bool add_refined_polys = false;
    param_dlg.checkbox("Want to use multiple instance polygons as outlines in the OSL?", add_refined_polys);
    
    if(!param_dlg.ask())
      return;
    if(!tool_->add_objects_with_ins(objects_file, videoid, video_file, poly_file, ins_file, offset, start_frame, ratio, add_refined_polys))
      std::cout << "objects not successfully added\n";
  }
  dbru_osl_add_objects_tool* tool_;
};


//get track storage from the storage pointer
static dbru_osl_storage_sptr get_osl_storage(bpro1_storage_sptr const& sto)
{
  dbru_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(sto);
  return osl_storage;
}

static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dbru_osl_storage", -1);
  return data;
}

//get osl from the storage pointer
static dbru_osl_sptr get_osl()
{
  dbru_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(storage());
  if (!osl_storage)
    return 0;
  else
    return osl_storage->osl();
}

static vil_image_resource_sptr get_image()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  if(!res->go_to_frame(bvis1_manager::instance()->current_frame()))
    return (vil_image_resource*)0;
  bpro1_storage_sptr sto = res->get_data("image");
  if(!sto)
    return (vil_image_resource*)0;
  std::cout << "Image Storage Name " << sto->name() << '\n';
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(sto);
  if(!image_storage)
    return (vil_image_resource*)0;
  return image_storage->get_image();
}

static bool get_current_database(dbru_osl_sptr osl, std::vector<std::pair<unsigned, unsigned> >* db, vgui_grid_tableau_sptr tableau)
{
  if (!tableau) {
    std::cout << " get_current_database() - tableau_ is not set in dbru_osl_add_objects_tool\n";
    return false;
  }

  std::vector<int> *col_pos = new std::vector<int>();
  std::vector<int> *row_pos = new std::vector<int>();
  std::vector<int> *times = new std::vector<int>();
  tableau->get_selected_positions(col_pos, row_pos, times);

  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*col_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
    
    std::pair<unsigned, unsigned> p;
    p.first = unsigned((*row_pos)[i]/2);
    dbru_object_sptr obj = osl->get_object(p.first);
    unsigned k;
    obj->get_non_null_polygon((*col_pos)[i], k);
    p.second = k;
    bool exists = false;
    for (std::vector<std::pair<unsigned, unsigned> >::iterator iter = db->begin(); iter != db->end(); iter++)
      if (p.first == iter->first && p.second == iter->second)
        exists = true;
    if (!exists)
      db->push_back(p);
  }

  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  
  return true;
}

//============================== OSL Edit Tool ============================

//Constructor
dbru_osl_add_objects_tool::dbru_osl_add_objects_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), osl_storage_(NULL), osl_(NULL), active_(false), tableau_(NULL), movie_id_(-1), my_movie_(0)
{
}

//: Destructor
dbru_osl_add_objects_tool::~dbru_osl_add_objects_tool()
{
}


//: Return the name of this tool
std::string dbru_osl_add_objects_tool::name() const
{
  return "Add objects to dbru_OSL";
}


//: Set the tableau to work with
bool
dbru_osl_add_objects_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_osl_add_objects_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Add Objects", new dbru_add_objects_command(this));
  pop_menu.add("Add Objects with Multiple Instances", new dbru_add_object_ins_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbru_osl_add_objects_tool::handle( const vgui_event & e,
                              const bvis1_view_tableau_sptr& view )
{
  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos/2 < osl_->n_objects()) {
        dbru_object_sptr obj = osl_->get_object(row_pos/2);
        unsigned i;
        obj->get_non_null_polygon(col_pos, i);
        if (i < obj->n_polygons()) {
          std::cout << "----------------------------------------------------------\n";
          std::cout << "OSL object id: " << row_pos/2 << " prototype id: " << i << "\n";
          dbru_label_sptr lbl = osl_->get_label(row_pos/2, i);
          std::cout << *lbl;
          if (!obj->get_instance(i))
            std::cout << "\t multiple instance:\t none\n";
          else
            std::cout << "\t multiple instance:\t exists\n";
          if (!obj->get_observation(i))
            std::cout << "\t observation:\t none\n";
          else
            std::cout << "\t observation:\t exists\n";
          std::cout << "----------------------------------------------------------\n";
        }
      }
      
    }
  return false;
}

void dbru_osl_add_objects_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  osl_ = osl_storage_->osl();
}
//------------------  OSL edit tool methods ---------------------------

bool dbru_osl_add_objects_tool::add_objects(std::string const& objects_file, int const videoid, std::string const& image_file, int movie_id, int start_frame)
{
  //need an osl to add the prototype
  if(!osl_) {
    std::cout << "dbru_osl_add_objects_tool::add_objects() - OSL problems, load an OSL or create a new OSL, objects are not added!\n";
    return false;
  }

  if (movie_id_ < 0 || !my_movie_) 
    my_movie_ = vidl1_io::load_movie(image_file.c_str());
  else if (movie_id != movie_id_) {
    delete my_movie_.ptr();
    my_movie_ = vidl1_io::load_movie(image_file.c_str());
  }

  movie_id_ = movie_id;
  if (!my_movie_) {
    std::cout << "problems in loading video with video file name: " << image_file << std::endl;
    return false;
  }
  
  std::ifstream dbfp(objects_file.c_str());
  if (!dbfp) {
    std::cout << "Problems in opening db object list file!\n";
    return false;
  }

  std::cout << "reading database objects...\n";
  std::vector<dbru_object_sptr> objects;
  read_objects_from_file(objects_file.c_str(), objects);
  std::cout << objects.size() << " objects in the file\n";
 
  unsigned size = objects.size();
  // first read and extract all objects in this file, if there is no problem add to OSL
  for (unsigned i = 0; i<size; i++) {
    dbru_object_sptr obj = objects[i];
    
    std::cout << "read obj: " << i << " extracting observations assuming 1 polygon per frame" << std::endl;
    
    //for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
    for (unsigned int j = obj->start_frame_, k = 0; k<obj->n_polygons(); j++, k++) {
      if (int(j) > obj->end_frame_)
        std::cout << "WARNING: object end frame is less than number of labeled polygons, will try to access the next frame anyways!!\n";
      vidl1_frame_sptr frame = my_movie_->get_frame(j-start_frame);
      if (!frame) {
        std::cout << "Frame is not accesible!! None of the objects are added, exiting!\n";
        return false;
      }
      vil_image_resource_sptr imgr = frame->get_resource();
      if (!imgr) {
        std::cout << "Frame is not accesible!! None of the objects are added, exiting!\n";
        return false;
      }
      vsol_polygon_2d_sptr poly = obj->get_polygon(k);
      int s = poly->size();
      if (s > 0) {
        dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly, true, true, false);
        if (obs->scan(0, imgr))
          obj->add_observation(obs);
        else {
          std::cout << "problems in scaning object " << *(obj) << " with id: " << i << " in the file. None of the objects are added to the OSL!\n";
          return false;
        }
      } else {
        obj->add_observation(0);  // add null pointer as observation
      }
    }

    if (obj->n_observations() == obj->n_polygons())
      objects.push_back(obj);
    else {
      std::cout << "Polygon number is different than frame numberproblems in object with id: " << i << " in the file. None of the objects are added to the OSL!\n";
      return false;
    }
  }

  osl_->add_objects(objects);
  
  //display the frame
  bvis1_manager::instance()->display_current_frame(true);
  //bvis1_manager::instance()->regenerate_all_tableaux();

  return true;
}

//: To generate an OSL where edgel based recognition methods can be run,
//  Vishal's processes to find foreground polygons, track them and create multiple instances should be run initially,
//  then labeled objects of those polygons can be used
//  to add them into the OSL with additional info in the form of multiple instances over many frames.
//  Initial foreground polygons are usually noisy, however vishal's multiple instances contain a refined
//  version of them via combining info over many frames after tracking,
//  add_refined_polys: when this option is true, 
//                     the refined polygons of multiple instances are added as object polygons into the OSL, regardless of the original
//                     labeled polygons in the xml file.
//                     we're changing object polygons optionally in case the user wants to create an OSL to run outline based recognition 
//                     algorithms with the same framework 
//  offset: the number of frames that is skipped in multiple_ins_file with respect to polygon file
//          i.e. all the polygons in "offset" many frames in the polygon file are ignored while multiple instances are
//               created, so begin process after skipping them here as well 
bool dbru_osl_add_objects_tool::add_objects_with_ins(std::string const& objects_file, 
                                                      int const movie_id, 
                                                      std::string const& video_file, 
                                                      std::string const& poly_file, 
                                                      std::string const& multiple_ins_file,
                                                      int offset,
                                                      int start_frame,
                                                      double ratio,
                                                      bool add_refined_polys)
{
  //need an osl to add the prototype
  if(!osl_) {
    std::cout << "dbru_osl_add_objects_tool::add_objects_with_ins() - OSL problems, load an OSL or create a new OSL, objects are not added!\n";
    return false;
  }

  if (movie_id_ < 0 || !my_movie_) 
    my_movie_ = vidl1_io::load_movie(video_file.c_str());
  else if (movie_id != movie_id_) {
    if (!my_movie_)
      delete my_movie_.ptr();
    my_movie_ = vidl1_io::load_movie(video_file.c_str());
  }

  movie_id_ = movie_id;
  if (!my_movie_) {
    std::cout << "problems in loading video with video file name: " << video_file << std::endl;
    return false;
  }

  std::vector<std::vector< vsol_polygon_2d_sptr > > frame_polys;
  int videoid = read_poly_file(poly_file.c_str(), frame_polys);
  if (videoid != movie_id) {
    std::cout << "Polygon file's video id does not match the id of the given video sequence\n";
    return false;
  }

  std::vector<std::vector<dbru_multiple_instance_object_sptr> > frame_ins;
  read_ins_file(multiple_ins_file.c_str(), frame_ins);

  std::cout << "number of frames in poly file: " << frame_polys.size() << "\n";
  std::cout << "number of frames in instance file: " << frame_ins.size() << "\n";
  std::cout << "will start to use from the frame " << offset << " in poly file to associate multiple instances to polygons\n";
  
  std::ifstream dbfp(objects_file.c_str());
  if (!dbfp) {
    std::cout << "Problems in opening db object list file!\n";
    return false;
  }

  std::cout << "reading database objects... ";
  std::vector<dbru_object_sptr> objects;
  read_objects_from_file(objects_file.c_str(), objects);
  std::cout << objects.size() << " objects in the file\n";
  
  // extract objects from images, if they can be located in poly file and a multiple instance exists then add to OSL
  for (unsigned i = 0; i<objects.size(); i++) {
    dbru_object_sptr obj = objects[i];

    //: get video file for this database object
    if (obj->video_id_ != movie_id)
      continue;
    
    std::cout << "extracting obj: " << i << " assuming 1 polygon per frame" << std::endl;
    
    int last_ins_in_poly = offset + frame_ins.size();

    for (unsigned int j = obj->start_frame_, k = 0; k<obj->n_polygons(); j++, k++) {
      // first find this polygon in polygon file to locate its multiple instance
      vsol_polygon_2d_sptr poly = obj->get_polygon(k);
      int s = poly->size();

      if (s > 0) {
        std::cout << "checking all polygons in all frames in the polygon file... ";
        bool same = false;
        int kk; unsigned kkk;
        for (kk = offset; kk < last_ins_in_poly; kk++) {
          for (kkk = 0; kkk < frame_polys[kk].size(); kkk++) {
            vsol_polygon_2d_sptr poly2 = frame_polys[kk][kkk];
            //std::cout << "to: \n\t\t" << *(poly2->vertex(0)) << "\n"; 
            if (*poly2 == *poly) {
              same = true;
              break;
            }
          }
          if (same)
            break;
        }
        if (same) {  // we located objects polygon in poly file, this means we can find its multiple instance
        
          //std::cout << " FOUND!\n";
          poly->compute_bounding_box();
          vsol_box_2d_sptr poly_box = poly->get_bounding_box();
          vgl_point_2d<double> cent_poly(poly->get_min_x()+(poly->get_max_x()-poly->get_min_x())/2.0f, 
                                         poly->get_min_y()+(poly->get_max_y()-poly->get_min_y())/2.0f);
          double threshold = (poly_box->height() > poly_box->width() ? poly_box->height() : poly_box->width())*ratio;
          
          dbru_multiple_instance_object_sptr ins = 0;
          // for each multiple instance in frame kk-offset, find the one with sufficient overlap to poly
          if (kk-offset < (int)frame_ins.size()) {
            for (unsigned m = 0; m <frame_ins[kk-offset].size(); m++) {
              vsol_polygon_2d_sptr pi = frame_ins[kk-offset][m]->get_poly();
              pi->compute_bounding_box();
              // if the centers of the bounding boxes are sufficiently close then declare overlap
              vsol_box_2d_sptr pi_box = pi->get_bounding_box();
              vgl_point_2d<double> cent_pi(pi->get_min_x()+(pi->get_max_x()-pi->get_min_x())/2.0f, 
                                           pi->get_min_y()+(pi->get_max_y()-pi->get_min_y())/2.0f);
              if (vgl_distance(cent_poly, cent_pi) < threshold) {
                if (!ins)
                  ins = frame_ins[kk-offset][m];
                else {
                  std::cout << "Second attempt to write an instance.. OSL WAS NOT SAVED, LOWER THE THRESHOLD ratio\n";
                  return false;
                }
              }
            }
          }
          if (!ins) 
            std::cout << "instance was not located properly, or it was empty in the instance file\n";
          else {
            //: we're changing object polygons optionally if the user wants to run outline based recognition 
            //  algorithms on this OSL
            if (add_refined_polys)
              obj->set_polygon(k, ins->get_poly());

          }

          obj->add_instance(ins);
        
        } else {  // we could not locate the object!!
          std::cout << "WARNING: Object polygon could not be located in the foreground polygon file!!!\n Check object and polygon files!\n";
          obj->add_instance(0);  // add null pointer as instance
        }

        if (int(j) > obj->end_frame_)
          std::cout << "WARNING: object end frame is less than number of labeled polygons, will try to access the next frame anyways!!\n";
        vidl1_frame_sptr frame = my_movie_->get_frame(j-start_frame);
        if (!frame) {
          std::cout << "Frame is not accesible!! None of the objects are added, exiting!\n";
          return false;
        }
        vil_image_resource_sptr imgr = frame->get_resource();
        if (!imgr) {
          std::cout << "Frame is not accesible!! None of the objects are added, exiting!\n";
          return false;
        }

        //: the polygon might have been changed
        dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, obj->get_polygon(k), true, true, false);
        if (obs->scan(0, imgr))
          obj->add_observation(obs);
        else {
          std::cout << "problems in scaning object " << *(obj) << " with id: " << i << " in the file. None of the objects are added to the OSL!\n";
          return false;
        }

      } else {
        obj->add_instance(0);
        obj->add_observation(0);  // add null pointer as observation
      }
      
    }

    if (obj->n_instances() != obj->n_polygons()) {
      std::cout << "Polygon number is different than instance number, problems in object with id: " << i << " in the file. None of the objects are added to the OSL!\n";
      return false;
    }

    if (obj->n_observations() != obj->n_polygons()) {
      std::cout << "Polygon number is different than observation number, problems in object with id: " << i << " in the file. None of the objects are added to the OSL!\n";
      return false;
    }

  }

  osl_->add_objects(objects);
  std::cout << "WARNING: A multiple instance may not exist for each prototype\n";
  std::cout << "         Such prototypes should not be included in databases to compare edgel based recognition algorithms to outline based algorithms\n";
  
  //display the frame
  bvis1_manager::instance()->display_current_frame(true);
  //bvis1_manager::instance()->regenerate_all_tableaux();

  return true;

}


//============================== Match Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui command to match a query
class dbru_match_query_command : public vgui_command
{
public:
  dbru_match_query_command(dbru_osl_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    //static std::string image_file ="c:/images/*";
    static std::string ext = "*.*";
    static float dx = 1.0f;
    static float dr = 0.2f;
    static float ds = 0.1f;
    static float da = 0.1f;
    static float ratio = 0.1f;
    static float thresh = 0.9f;
    static unsigned Nobs = 300;
    static float coef = 0.6f;
    static bool expand = false;
    static bool use_int = true;
    static bool use_grad = true;
    static bool forward_and_reverse = true;
    vgui_dialog param_dlg("Match Query");
    param_dlg.field("dx", dx);
    param_dlg.field("dr", dr);
    param_dlg.field("ds", ds);
    param_dlg.field("da", da);
    param_dlg.field("ratio", ratio);
    param_dlg.field("valid_thresh", thresh);
    param_dlg.field("Nobs", Nobs);
    //param_dlg.field("Expansion Coef", coef);
    //param_dlg.file("Image File", ext, image_file);
    //param_dlg.checkbox("Expand", expand);
    param_dlg.checkbox("Use Intensity", use_int);
    param_dlg.checkbox("Use Gradient", use_grad);
    param_dlg.checkbox("Forward And Reverse", forward_and_reverse);
    if(!param_dlg.ask())
      return;
    if(!use_int&&!use_grad)
      {
        std::cout << "must use at least one information channel\n";
        return;
      }
    //std::vector<std::string> classes;
    std::vector<float> match_scores;
    std::vector<vil_image_resource_sptr> match_images;
    //if(tool_->load_query(image_file, expand, coef))
    if(tool_->query_set())  
      if(!tool_->match_query(dx, dr, ds, da, ratio, thresh, Nobs,
                             use_int, use_grad, forward_and_reverse,
                             match_scores, match_images))
        {
          std::cout << "query not successfully matched\n";
          return;
        }
    unsigned i = 0;
    std::cout << "--- Scores ---\n";
    //for(std::vector<std::string>::iterator cit = classes.begin();
    //    cit != classes.end(); ++cit, ++i)
    for ( i = 0; i<match_scores.size(); i++)
      std::cout << "(" << i << " = " << match_scores[i] << ") " << std::flush;
    std::cout << "\n";
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    for(i=0; i<n_match; ++i) {
      char buffer[1000];
      sprintf(buffer, "output_%d.png", i);
      vil_save_image_resource(match_images[i], buffer);
    }
    /*
    vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(n_match, 1);    
    for(i=0; i<n_match; ++i)
      {
        bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_images[i]);
        vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
        vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);
        grid->add_at(stab, i, 0);
      }
    //popup adjuster
      
    vgui_dialog inline_tab("Matched Query");
    inline_tab.inline_tableau(grid, 500, 100);
    if (!inline_tab.ask())
      return;
      */
  }
  dbru_osl_match_tool* tool_;
};

//: A vgui command to match a query
class dbru_match_query_opt_command : public vgui_command
{
public:
  dbru_match_query_opt_command(dbru_osl_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    //static std::string image_file ="c:/images/*";
    static std::string ext = "*.*";
    static float dx = 1.0f;
    static float dr = 0.2f;
    static float ds = 0.1f;
    static float ratio = 0.1f;
    vgui_dialog param_dlg("Match Query OPT");
    param_dlg.field("dx", dx);
    param_dlg.field("dr", dr);
    param_dlg.field("ds", ds);
    param_dlg.field("ratio", ratio);
    if(!param_dlg.ask())
      return;
   
    std::vector<float> match_scores;
    std::vector<vil_image_resource_sptr> match_images;
    if(tool_->query_set())  
      if(!tool_->match_query_opt(dx, dr, ds, ratio, match_scores, match_images))
        {
          std::cout << "query not successfully matched\n";
          return;
        }
    unsigned i = 0;
    std::cout << "--- Scores ---\n";
    for ( i = 0; i<match_scores.size(); i++)
      std::cout << "(" << i << " = " << match_scores[i] << ") " << std::flush;
    std::cout << "\n";
    /*unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    for(i=0; i<n_match; ++i) {
      char buffer[1000];
      sprintf(buffer, "output_%d.png", i);
      vil_save_image_resource(match_images[i], buffer);
    }*/
    /*
    vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(n_match, 1);    
    for(i=0; i<n_match; ++i)
      {
        bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_images[i]);
        vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
        vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);
        grid->add_at(stab, i, 0);
      }
    //popup adjuster
      
    vgui_dialog inline_tab("Matched Query");
    inline_tab.inline_tableau(grid, 500, 100);
    if (!inline_tab.ask())
      return;
      */
  }
  dbru_osl_match_tool* tool_;
};



class dbru_match_query_interval_command : public vgui_command
{
public:
  dbru_match_query_interval_command(dbru_osl_match_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    //static std::string image_file ="c:/images/*";
    //static std::string ext = "*.*";
    static float xmin = -3.0f;
    static float xmax = 3.0f;
    static float ymin = -3.0f;
    static float ymax = 3.0f;
    static float theta_min = -0.2f;
    static float theta_max = 0.2f;
    static float scale_min = 0.9f;
    static float scale_max = 1.1f;
    static float aspect_min = 0.9f;
    static float aspect_max = 1.1f;
    static float valid_thresh = 0.9f;
    static unsigned n_intervals = 500;
    //static float coef = 0.6f;
    //static bool expand = false;
    static bool forward_and_reverse = false;
    vgui_dialog param_dlg("Transform Query");
    //param_dlg.file("Image File", ext, image_file);
    //param_dlg.checkbox("Expand ", expand);
    //param_dlg.field("Coef ", coef);
    param_dlg.field("xmin", xmin);
    param_dlg.field("xmax", xmax);
    param_dlg.field("ymin", ymin);
    param_dlg.field("ymax", ymax);
    param_dlg.field("theta_min", theta_min);
    param_dlg.field("theta_max", theta_max);
    param_dlg.field("scale_min", scale_min);
    param_dlg.field("scale_max", scale_max);
    param_dlg.field("aspect_min", aspect_min);
    param_dlg.field("aspect_max", aspect_max);
    param_dlg.field("valid_thresh", valid_thresh);
    param_dlg.field("Nintervals", n_intervals);
    param_dlg.checkbox("Forward and Reverse ", forward_and_reverse);
    if(!param_dlg.ask())
      return;
    //std::vector<std::string> classes;
    std::vector<float> match_scores;
    std::vector<vil_image_resource_sptr> match_images;
    //if(tool_->load_query(image_file, expand, coef))
    if(tool_->query_set()) {
      if(!tool_->match_query_interval(xmin, xmax, ymin, ymax,
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      n_intervals, valid_thresh, 
                                      forward_and_reverse,
                                      match_scores, match_images))
        {
          std::cout << "query not successfully matched\n";
          return;
        }
    } else {
      std::cout << "query observation is not picked, press p on the query prototype to select it.\n";
    }
    unsigned i = 0;
    //std::cout << "---Class Scores ---\n";
    //for(std::vector<std::string>::iterator cit = classes.begin();
    //    cit != classes.end(); ++cit, ++i)
    //  std::cout << *cit << " = " << match_scores[i] << '\n'<< std::flush;
    for ( ; i < match_scores.size(); i++)
      std::cout << "(" << i << " = " << match_scores[i] << ") " << std::flush;
    std::cout << "\n";
    unsigned n_match = match_images.size();
    if(n_match==0)
      return;
    for(i=0; i<n_match; ++i) {
      char buffer[1000];
      sprintf(buffer, "output_%d.png", i);
      vil_save_image_resource(match_images[i], buffer);
    }
    /*
    vgui_grid_tableau_sptr grid = vgui_grid_tableau_new(n_match, 1);    
    for(i=0; i<n_match; ++i)
      {
        bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_images[i]);
        vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
        vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);
        grid->add_at(stab, i, 0);
      }
    //popup adjuster
      
    vgui_dialog inline_tab("Matched Query");
    inline_tab.inline_tableau(grid, 500, 100);
    if (!inline_tab.ask())
      return;
      */
  }
  dbru_osl_match_tool* tool_;
};

//Constructor
dbru_osl_match_tool::dbru_osl_match_tool( const vgui_event_condition& disp,
                                          const vgui_event_condition& drop,
                                          const vgui_event_condition& set) :
  gesture_disp_(disp), gesture_drop_(drop), gesture_set_query_(set), osl_storage_(NULL), osl_(NULL),
  active_(false), tableau_(NULL), query_set_(false) 
{
}


//: Destructor
dbru_osl_match_tool::~dbru_osl_match_tool()
{
}


//: Return the name of this tool
std::string dbru_osl_match_tool::name() const
{
  return "Match dbru_OSL";
}


//: Set the tableau to work with
bool
dbru_osl_match_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_osl_match_tool::get_popup( const vgui_popup_params& params, 
                                  vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Info Match Query", new dbru_match_query_command(this));
  pop_menu.add("Info Match Query Interval", new dbru_match_query_interval_command(this));
  pop_menu.add("Match Query OPT (DLL Method)", new dbru_match_query_opt_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbru_osl_match_tool::handle( const vgui_event & e,
                               const bvis1_view_tableau_sptr& view )
{
  if (gesture_set_query_(e)) {
    if (!tableau_) {
      std::cout << "tableau_ is not set in dbru_osl_match_tool\n";
      return false;
    }
    std::pair<unsigned, unsigned> query_obs_ids_;
    unsigned int col_pos, row_pos;
    tableau_->get_active_position(&col_pos, &row_pos);
    query_obs_ids_.first = row_pos/2;
    query_obs_ids_.second = col_pos;
    std::cout << "query is set with col_pos: " << col_pos << " row_pos: " << row_pos << std::endl;
    query_set_ = true;
    return false;
  }

  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos/2 < osl_->n_objects()) {
        dbru_object_sptr obj = osl_->get_object(row_pos/2);
        unsigned i;
        obj->get_non_null_polygon(col_pos, i);
        if (i < obj->n_polygons()) {
          std::cout << "----------------------------------------------------------\n";
          std::cout << "OSL object id: " << row_pos/2 << " prototype id: " << i << "\n";
          dbru_label_sptr lbl = osl_->get_label(row_pos/2, i);
          std::cout << *lbl;
          if (!obj->get_instance(i))
            std::cout << "\t multiple instance:\t none\n";
          else
            std::cout << "\t multiple instance:\t exists\n";
          if (!obj->get_observation(i))
            std::cout << "\t observation:\t none\n";
          else
            std::cout << "\t observation:\t exists\n";
          std::cout << "----------------------------------------------------------\n";
        }
      }
      
    }

  return false;
}

void dbru_osl_match_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  if(osl_storage_)
    osl_ = osl_storage_->osl();
  else
    std::cout << "Failed to activate osl_tools\n";
}
//------------------  OSL match tool methods ---------------------------
/*bool dbru_osl_match_tool::load_query(std::string const& path,
                                       const bool expand,
                                       const float coef)
{
  image_ = vil_load_image_resource(path.c_str());
  if(!image_)
    return false;
  if(expand)
    {
      vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(image_);
      image_ = 
        vil_new_image_resource_of_view(brip_vil_float_ops::double_resolution(fview, coef));
    }
  return true;
}*/

bool dbru_osl_match_tool::match_query(const float dx, const float dr,
                                        const float ds, const float da, 
                                        const float ratio, 
                                        const float valid_thresh,
                                        const unsigned Nob, 
                                        bool use_int, bool use_grad, 
                                        bool forward_and_reverse,
                                        //std::vector<std::string>& classes,
                                        std::vector<float>& match_scores,
                                        std::vector<vil_image_resource_sptr>& match_images){
  //need a osl to add the prototype
  if(!osl_)
    return false;

  // get the query observation from the image
  dbinfo_observation_sptr obsq = osl_->get_prototype(query_obs_ids_.first, query_obs_ids_.second);

  if (!obsq) {
    std::cout << "Query observation is not appropriately set!\n";
    return false;
  }

  //for now use only the first prototype in each class (FIXME)
  //classes = osl_->classes();
  //for(std::vector<std::string>::iterator cit = classes.begin();
  //    cit != classes.end(); ++cit)
  //  {
  //    dbinfo_observation_sptr obsdb = osl_->prototype(*cit, 0);
  std::vector<std::pair<unsigned, unsigned> > *db = new std::vector<std::pair<unsigned, unsigned> >();
  if (!get_current_database(osl_, db, tableau_)) {
    std::cout << "Problems in getting db!\n";
    return false;
  }

  std::pair<unsigned, unsigned> p;
  for (unsigned i = 0; i<db->size(); i++) 
  {
    p = (*db)[i];
    dbinfo_observation_sptr obsdb = osl_->get_prototype(p.first, p.second);
    if(!obsdb)
      continue;
    vil_image_resource_sptr match_imagef;
    vil_image_resource_sptr match_imager;
    vil_image_resource_sptr dummy;
    float minfor = 0.0;
    float minfof = 
      dbinfo_object_matcher::minfo_rigid_alignment_rand(obsq, obsdb,
                                                        dx, dr, ds,
                                                        ratio,
                                                        Nob, match_imagef,dummy, dummy,
                                                        false,
                                                        1.0f,
                                                        use_int,
                                                        use_grad);
    if(forward_and_reverse)
      minfor = 
        dbinfo_object_matcher::minfo_rigid_alignment_rand(obsdb, obsq,
                                                          dx, dr, ds,
                                                          ratio,
                                                          Nob, match_imager,dummy, dummy,
                                                          false,
                                                          1.0f,
                                                          use_int,
                                                          use_grad);
    if(minfof>minfor)
      {
        //          match_scores.push_back(minfof);
        if(match_imagef)
          match_images.push_back(match_imagef);
      }
    else
      {
        //          match_scores.push_back(minfor);
        if(match_imager)
          match_images.push_back(match_imager);
      }
    match_scores.push_back(minfof+minfor);
  }
  return true;
}
bool dbru_osl_match_tool::match_query_interval(const float xmin, const float xmax,
                                                 const float ymin, const float ymax, 
                                                 const float theta_min, const float theta_max,
                                                 const float scale_min, const float scale_max,
                                                 const float aspect_min, const float aspect_max,
                                                 const unsigned n_intervals,
                                                 const float valid_thresh,
                                                 bool forward_and_reverse,
                                                 //std::vector<std::string>& classes,
                                                 std::vector<float>& match_scores,
                                                 std::vector<vil_image_resource_sptr>& match_images)
{
  //need a osl to add the prototype
  if(!osl_)
    return false;

  // get the query observation from the image
  dbinfo_observation_sptr obsq = osl_->get_prototype(query_obs_ids_.first, query_obs_ids_.second);

  if (!obsq) {
    std::cout << "Query observation is not appropriately set!\n";
    return false;
  }

  //for now use only the first prototype in each class (FIXME)
  //classes = osl_->classes();
  //for(std::vector<std::string>::iterator cit = classes.begin();
   //   cit != classes.end(); ++cit)
   // {
  std::vector<std::pair<unsigned, unsigned> > *db = new std::vector<std::pair<unsigned, unsigned> >();
  if (!get_current_database(osl_, db, tableau_)) {
    std::cout << "Problems in getting db!\n";
    return false;
  }

  std::pair<unsigned, unsigned> p;
  for (unsigned i = 0; i<db->size(); i++) 
  {
      p = (*db)[i];
      dbinfo_observation_sptr obsdb = osl_->get_prototype(p.first, p.second);
      if(!obsdb)
        continue;
      vil_image_resource_sptr match_imagef;
      vil_image_resource_sptr match_imager;
      float minfor = 0.0;
      float minfof = dbinfo_object_matcher::
        minfo_alignment_in_interval(obsq, obsdb, 
                                    xmin, xmax,
                                    ymin, ymax, 
                                    theta_min, theta_max,
                                    scale_min, scale_max,
                                    aspect_min, aspect_max,
                                    n_intervals,
                                    valid_thresh,
                                    match_imagef);
      if(forward_and_reverse)
        minfor = dbinfo_object_matcher::
          minfo_alignment_in_interval(obsdb, obsq, 
                                      xmin, xmax,
                                      ymin, ymax, 
                                      theta_min, theta_max,
                                      scale_min, scale_max,
                                      aspect_min, aspect_max,
                                      n_intervals,
                                      valid_thresh,
                                      match_imager);
          
      if(minfof>minfor)
        {
          if(match_imagef)
            match_images.push_back(match_imagef);
        }
      else
        {
          if(match_imager)
            match_images.push_back(match_imager);
        }
      match_scores.push_back(minfof+minfor);
    }
  return true;
}

bool dbru_osl_match_tool::match_query_opt(const float dx, const float dr,
                                          const float ds,
                                          const float ratio, 
                                          std::vector<float>& match_scores,
                                          std::vector<vil_image_resource_sptr>& match_images) {
  //need a osl to add the prototype
  if(!osl_)
    return false;

  // get the query observation from the image
  dbinfo_observation_sptr obsq = osl_->get_prototype(query_obs_ids_.first, query_obs_ids_.second);

  if (!obsq) {
    std::cout << "Query observation is not appropriately set!\n";
    return false;
  }

  std::vector<std::pair<unsigned, unsigned> > *db = new std::vector<std::pair<unsigned, unsigned> >();
  if (!get_current_database(osl_, db, tableau_)) {
    std::cout << "Problems in getting db!\n";
    return false;
  }

  std::pair<unsigned, unsigned> p;
  for (unsigned i = 0; i<db->size(); i++) 
  {
    p = (*db)[i];
    dbinfo_observation_sptr obsdb = osl_->get_prototype(p.first, p.second);
    if(!obsdb)
      continue;
    vil_image_resource_sptr match_imagef;
    vil_image_resource_sptr match_imager;
    vil_image_resource_sptr dummy;
    float minfo = dbskr_object_matcher::minfo_rigid_alignment_search(obsq, obsdb, dx, dr, ds, ratio);
    match_scores.push_back(minfo);
  }
  return true;
}

//============================== Delete Observations Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------

//Constructor
dbru_osl_delete_observations_tool::dbru_osl_delete_observations_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), active_(false), tableau_(NULL)
{
}


//: Destructor
dbru_osl_delete_observations_tool::~dbru_osl_delete_observations_tool()
{
}


//: Return the name of this tool
std::string dbru_osl_delete_observations_tool::name() const
{
  return "Delete Selected Observations from OSL";
}


//: Set the tableau to work with
bool
dbru_osl_delete_observations_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}

//: Handle events
bool
dbru_osl_delete_observations_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos/2 < osl_->n_objects()) {
        dbru_object_sptr obj = osl_->get_object(row_pos/2);
        unsigned i;
        obj->get_non_null_polygon(col_pos, i);
        if (i < obj->n_polygons()) {
          std::cout << "----------------------------------------------------------\n";
          std::cout << "OSL object id: " << row_pos/2 << " prototype id: " << i << "\n";
          dbru_label_sptr lbl = osl_->get_label(row_pos/2, i);
          std::cout << *lbl;
          if (!obj->get_instance(i))
            std::cout << "\t multiple instance:\t none\n";
          else
            std::cout << "\t multiple instance:\t exists\n";
          if (!obj->get_observation(i))
            std::cout << "\t observation:\t none\n";
          else
            std::cout << "\t observation:\t exists\n";
          std::cout << "----------------------------------------------------------\n";
        }
      }
      
    }

  return false;
}

void dbru_osl_delete_observations_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  if(osl_storage_)
    osl_ = osl_storage_->osl();
  else {
    std::cout << "Failed to activate osl_tools\n";
    return;
  }

  std::cout << "delete_observations tool active\n";
  std::cout << "USAGE: Selected observations will be deleted, if all observations of an object are deleted then its removed completely\n";

  if (!tableau_) {
    std::cout << " dbru_osl_delete_observations_tool::activate() - tableau_ is not set in dbru_osl_add_objects_tool\n";
    return;
  }

  std::vector<int> *col_pos = new std::vector<int>();
  std::vector<int> *row_pos = new std::vector<int>();
  std::vector<int> *times = new std::vector<int>();
  tableau_->get_selected_positions(col_pos, row_pos, times);

  //remove the row 2n+1 if row 2n is already in row_pos
  for (unsigned i = 0; i<row_pos->size(); i++) {
    if ((*row_pos)[i] < 0)
      continue;
    int n = (*row_pos)[i]/2;
    for (unsigned j = 0; j < row_pos->size(); j++) {
      if (i == j) continue;
      int nn = (*row_pos)[j]/2;
      if (nn == n && (*col_pos)[i] == (*col_pos)[j]) {
        (*row_pos)[j] = -1;
        break;
      }
    }
      
  }
  for (unsigned i = 0; i<col_pos->size(); i++) {
    if ((*col_pos)[i] < 0 || (*row_pos)[i] < 0)
      continue;
    
    if (unsigned((*row_pos)[i]/2) < osl_->n_objects()) {
      dbru_object_sptr obj = osl_->get_object((*row_pos)[i]/2);

      unsigned k;
      obj->get_non_null_polygon((*col_pos)[i], k);
      osl_->null_prototype( (*row_pos)[i]/2, k );
    }
    
  }

  for (unsigned i = 0; i < osl_->n_objects(); i++) {
    dbru_object_sptr obj = osl_->get_object(i);
    bool remove = true;
    for (unsigned j = 0; j < obj->n_observations(); j++) 
      if (obj->get_observation(j)) {
        remove = false;
        break;
      }
    if (remove)
      if (!osl_->remove_object(i))
        std::cout << "problems in removing object in row: " << i << std::endl;
  }

  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
  bvis1_manager::instance()->display_current_frame(true);
  return;
}


//============================== Save Current Database into Txt File Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------

//: A vgui command 
class dbru_save_selected_prototypes_command : public vgui_command
{
public:
  dbru_save_selected_prototypes_command(dbru_osl_save_db_file_tool* tool) : tool_(tool) {}
  void execute()
  {
    tool_->save_selected_prototypes();
  }
  dbru_osl_save_db_file_tool* tool_;
};

//: A vgui command 
class dbru_save_based_on_bins_command : public vgui_command
{
public:
  dbru_save_based_on_bins_command(dbru_osl_save_db_file_tool* tool) : tool_(tool) {}
  void execute()
  {
    tool_->save_based_on_bins();
  }
  dbru_osl_save_db_file_tool* tool_;
};


//Constructor
dbru_osl_save_db_file_tool::dbru_osl_save_db_file_tool(const vgui_event_condition& disp) :
  gesture_disp_(disp), active_(false), tableau_(NULL)
{
}


//: Destructor
dbru_osl_save_db_file_tool::~dbru_osl_save_db_file_tool()
{
}


//: Return the name of this tool
std::string dbru_osl_save_db_file_tool::name() const
{
  return "Save current selected database into a TXT file";
}


//: Set the tableau to work with
bool
dbru_osl_save_db_file_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}

//: Add popup menu items
void 
dbru_osl_save_db_file_tool::get_popup( const vgui_popup_params& params, 
                                 vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Save selected instances as database", new dbru_save_selected_prototypes_command(this));
  pop_menu.add("Save based on label", new dbru_save_based_on_bins_command(this));
  menu.add("OSL", pop_menu); 
}

//: Handle events
bool
dbru_osl_save_db_file_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  if (gesture_disp_(e))
    if (tableau_) {
      unsigned col_pos, row_pos;
      tableau_->get_active_position(&col_pos, &row_pos);
      if (row_pos/2 < osl_->n_objects()) {
        dbru_object_sptr obj = osl_->get_object(row_pos/2);
        unsigned i;
        obj->get_non_null_polygon(col_pos, i);
        if (i < obj->n_polygons()) {
          std::cout << "----------------------------------------------------------\n";
          std::cout << "OSL object id: " << row_pos/2 << " prototype id: " << i << "\n";
          dbru_label_sptr lbl = osl_->get_label(row_pos/2, i);
          std::cout << *lbl;
          if (!obj->get_instance(i))
            std::cout << "\t multiple instance:\t none\n";
          else
            std::cout << "\t multiple instance:\t exists\n";
          if (!obj->get_observation(i))
            std::cout << "\t observation:\t none\n";
          else
            std::cout << "\t observation:\t exists\n";
          std::cout << "----------------------------------------------------------\n";
        }
      }
      
    }

  return false;
}
void dbru_osl_save_db_file_tool::save_selected_prototypes()
{
  static std::string db_file ="/home/dec/";
  static std::string ext = "*.txt";
  static bool check_obs = true;
  vgui_dialog param_dlg("Save Database File");
  param_dlg.file("Database File", ext, db_file);
  param_dlg.checkbox("Test existance of observations? (Otherwise multiple instances)", check_obs);
  if(!param_dlg.ask())
    return;

  std::vector<int> *col_pos = new std::vector<int>();
  std::vector<int> *row_pos = new std::vector<int>();
  std::vector<int> *times = new std::vector<int>();
  tableau_->get_selected_positions(col_pos, row_pos, times);

  std::ofstream of(db_file.c_str());
  if (!of) {
    std::cout << "dbru_osl_save_db_file_tool::save_selected_prototypes() - file not opened\n";
    return;
  }

  //remove the row 2n+1 if row 2n is already in row_pos
  for (unsigned i = 0; i<row_pos->size(); i++) {
    if ((*row_pos)[i] < 0)
      continue;
    int n = (*row_pos)[i]/2;
    for (unsigned j = 0; j < row_pos->size(); j++) {
      if (i == j) continue;
      int nn = (*row_pos)[j]/2;
      if (nn == n && (*col_pos)[i] == (*col_pos)[j]) {
        (*row_pos)[j] = -1;
        break;
      }
    }
  }

  std::vector<unsigned> temp1, temp2;
  
  for (unsigned i = 0; i<col_pos->size(); i++) {
    unsigned osi = (*col_pos)[i];
    unsigned oi = (*row_pos)[i];
    if (oi < 0 || osi < 0)
      continue;
    oi = oi/2;
    
    if (oi < osl_->n_objects()) {
      dbru_object_sptr obj = (*osl_)[oi];
     
      unsigned k;
      obj->get_non_null_polygon(osi, k);
      
      if (check_obs) {
        if (obj->get_observation(k)) { // if not null
          temp1.push_back(oi);
          temp2.push_back(k);
        }
      } else {
        if (obj->get_instance(k)) { // if not null
          temp1.push_back(oi);
          temp2.push_back(k);
        }
      }

    }
  }

  of << temp1.size() << std::endl;
  for (unsigned i = 0; i < temp1.size(); i++) {
    of << temp1[i] << " " << temp2[i] << "\n";
  }
  of.close();
  
  std::cout << "selected observations are saved to: " << db_file << "\n";
  col_pos->clear();
  row_pos->clear();
  times->clear();
  delete col_pos;
  delete row_pos;
  delete times;
}

void dbru_osl_save_db_file_tool::activate()
{
  bpro1_storage_sptr junk = storage();
  osl_storage_ = get_osl_storage(junk);
  if(osl_storage_)
    osl_ = osl_storage_->osl();
  else {
    std::cout << "Failed to activate osl_tools\n";
    return;
  }

  std::cout << "save_db_file tool active\n";

  if (!tableau_) {
    std::cout << " dbru_osl_delete_observations_tool::activate() - tableau_ is not set in dbru_osl_add_objects_tool\n";
    return;
  }

  return;
}

void dbru_osl_save_db_file_tool::save_based_on_bins()
{
  static std::string db_file ="/home/dec/";
  static std::string ext = "*.txt";
  vgui_dialog param_dlg("Save Database File");
  static int motion_bin = -1;
  static int view_bin = -1;
  static int shadow_bin = -1;
  static int shadow_length = -1;
  static bool check_obs = true;
  param_dlg.file("Database File", ext, db_file);
  param_dlg.checkbox("Check observations? (Otherwise multiple instances)", check_obs);
  param_dlg.field("Motion bin (leave -1 if non-relevant): ", motion_bin);
  param_dlg.field("View Angle bin (leave -1 if non-relevant): ", view_bin);
  param_dlg.field("Shadow Angle bin (leave -1 if non-relevant): ", shadow_bin);
  param_dlg.field("Shadow length (leave -1 if non-relevant): ", shadow_length);
  if(!param_dlg.ask())
    return;

  std::ofstream of(db_file.c_str());
  if (!of) {
    std::cout << "dbru_osl_save_db_file_tool::save_based_on_bins() - file not opened\n";
    return;
  }

  std::vector<unsigned> temp1, temp2;

  for (unsigned i = 0; i < osl_->size(); i++) {
    dbru_object_sptr obj = (*osl_)[i];
    unsigned lbl_cnt = obj->n_labels();

    if (check_obs) {
      if (lbl_cnt != obj->n_observations())
        std::cout << "CAUTION: number of observations does not equal to number of labels in this object!!\n";
      else
        std::cout << "Checking non-null observations and collecting ones with the right label...\n";
      for (unsigned j = 0; j < obj->n_observations(); j++) {
        if (obj->get_observation(j) && j < lbl_cnt && obj->get_label(j)) {
          dbru_label_sptr lbl = obj->get_label(j);
          if (motion_bin >= 0 && lbl->motion_orientation_bin_ != motion_bin)
            continue;
          if (view_bin >= 0 && lbl->view_angle_bin_ != view_bin)
            continue;
          if (shadow_bin >= 0 && lbl->shadow_angle_bin_ != shadow_bin)
            continue;
          if (shadow_length >= 0 && lbl->shadow_length_ != shadow_length)
            continue;            
          temp1.push_back(i);
          temp2.push_back(j);
        }
      }
    } else {
      if (lbl_cnt != obj->n_instances())
        std::cout << "CAUTION: number of instances does not equal to number of labels in this object!!\n";
      else
        std::cout << "Checking non-null instances and collecting ones with the right label...\n";
      for (unsigned j = 0; j < obj->n_instances(); j++) {
        if (obj->get_instance(j) && j < lbl_cnt && obj->get_label(j)) {
          dbru_label_sptr lbl = obj->get_label(j);
          if (motion_bin >= 0 && lbl->motion_orientation_bin_ != motion_bin)
            continue;
          if (view_bin >= 0 && lbl->view_angle_bin_ != view_bin)
            continue;
          if (shadow_bin >= 0 && lbl->shadow_angle_bin_ != shadow_bin)
            continue;
          if (shadow_length >= 0 && lbl->shadow_length_ != shadow_length)
            continue;            
          temp1.push_back(i);
          temp2.push_back(j);
        }
      }
    }
  }

  of << temp1.size() << std::endl;
  for (unsigned i = 0; i < temp1.size(); i++) {
    of << temp1[i] << " " << temp2[i] << "\n";
  }
  of.close();
  
  std::cout << "Selections are saved to: " << db_file << "\n";
}

/*
//============================== Transform Query Tool ============================
// ==========COMMANDS=========
//
//----------------------------------------------------------------------------
//: A vgui_command to load a query
class dbru_load_command : public vgui_command
{
public:
  dbru_load_command(dbru_osl_transform_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static std::string image_file ="c:/images/*";
    static std::string ext = "*.*";
    static bool query = true;
    static bool expand = false;
    static float coef=0.6f;
    vgui_dialog param_dlg("Load Prototype/Query");
    param_dlg.checkbox("Query(check)/Prototype(no check)", query);
    param_dlg.file("Image File", ext, image_file);
    param_dlg.checkbox("Expand", expand);
    param_dlg.field("Expansion Coef", coef);
    if (!param_dlg.ask())
      return;
    if(!tool_->load(image_file, query, expand, coef))
      std::cout << "Image not sucessfully loaded\n";
  }
  dbru_osl_transform_tool* tool_;
};
//: A vgui command to transform a query
class dbru_transform_query_command : public vgui_command
{
public:
  dbru_transform_query_command(dbru_osl_transform_tool* tool) 
    : tool_(tool) {}
  void execute()
  {
    static std::string image_file ="c:/images/*";
    static std::string ext = "*.*";
    static float xmin = -3.0f;
    static float xmax = 3.0f;
    static float ymin = -3.0f;
    static float ymax = 3.0f;
    static float theta_min = -0.2f;
    static float theta_max = 0.2f;
    static float scale_min = 0.9f;
    static float scale_max = 1.1f;
    static float aspect_min = 0.9f;
    static float aspect_max = 1.1f;
    static float valid_thresh = 0.9f;
    static unsigned n_intervals = 500;
    vgui_dialog param_dlg("Transform Query");
    param_dlg.field("xmin", xmin);
    param_dlg.field("xmax", xmax);
    param_dlg.field("ymin", ymin);
    param_dlg.field("ymax", ymax);
    param_dlg.field("theta_min", theta_min);
    param_dlg.field("theta_max", theta_max);
    param_dlg.field("scale_min", scale_min);
    param_dlg.field("scale_max", scale_max);
    param_dlg.field("aspect_min", aspect_min);
    param_dlg.field("aspect_min", aspect_max);
    param_dlg.field("valid_thresh", valid_thresh);
    param_dlg.field("Nintervals", n_intervals);
    if(!param_dlg.ask())
      return;
    vil_image_resource_sptr match_image;
    float max_info =0;
    if(!tool_->transform_query(xmin, xmax, ymin, ymax,
                               theta_min, theta_max,
                               scale_min, scale_max,
                               aspect_min, aspect_max,
                               n_intervals, valid_thresh,
                               max_info,
                               match_image))
      {
        std::cout << "query not successfully transformed\n";
        return;
      }

    
    bgui_image_tableau_sptr itab = bgui_image_tableau_new(match_image);
    vgui_viewer2D_tableau_sptr vtab = vgui_viewer2D_tableau_new(itab);
    vgui_shell_tableau_sptr stab = vgui_shell_tableau_new(vtab);

    //popup the match image
    vgui_dialog inline_tab("Transformed Query");
    inline_tab.inline_tableau(stab, 500, 100);
    if (!inline_tab.ask())
      return;
  }
  dbru_osl_transform_tool* tool_;
};


//Constructor
dbru_osl_transform_tool::dbru_osl_transform_tool( const vgui_event_condition& lift,
                                                      const vgui_event_condition& drop) :
  gesture_lift_(lift), gesture_drop_(drop), 
  active_(false), tableau_(NULL), query_(NULL), proto_(NULL) 
{
}


//: Destructor
dbru_osl_transform_tool::~dbru_osl_transform_tool()
{
}


//: Return the name of this tool
std::string dbru_osl_transform_tool::name() const
{
  return "Transform OSL";
}


//: Set the tableau to work with
bool
dbru_osl_transform_tool::set_tableau( const vgui_tableau_sptr& tableau )
{
  tableau_ = vgui_grid_tableau_sptr(dynamic_cast<vgui_grid_tableau*>(tableau.ptr()));
  if( tableau.ptr() == NULL )
    return false;
  return true;
}
//: Add popup menu items
void 
dbru_osl_transform_tool::get_popup( const vgui_popup_params& params, 
                                      vgui_menu &menu )
{
  vgui_menu pop_menu;
  pop_menu.add("Load", new dbru_load_command(this));
  pop_menu.add("Transform Query", new dbru_transform_query_command(this));
  menu.add("OSL", pop_menu); 
}


//: Handle events
bool
dbru_osl_transform_tool::handle( const vgui_event & e,
                                   const bvis1_view_tableau_sptr& view )
{
  return false;
}

void dbru_osl_transform_tool::activate()
{
  std::cout << "Transform tool active\n";
}
//------------------  OSL transform tool methods ---------------------------
bool dbru_osl_transform_tool::load(std::string const& path,
                                     const bool query,
                                     const bool expand,
                                     const float coef)
{
  vil_image_resource_sptr image = vil_load_image_resource(path.c_str());
  if(!image)
    return false;
  if(expand)
    {
      vil_image_view<float> fview = brip_vil_float_ops::convert_to_float(image);
      image = vil_new_image_resource_of_view(brip_vil_float_ops::
                                             double_resolution(fview, coef));
    }
  if(query)
    query_ = image;
  else proto_ = image;
  return true;
}

bool dbru_osl_transform_tool::
transform_query(const float xmin, const float xmax,
                const float ymin, const float ymax, 
                const float theta_min, const float theta_max,
                const float scale_min, const float scale_max,
                const float aspect_min, const float aspect_max,
                const unsigned n_intervals,
                const float valid_thresh,
                float& max_info,
                vil_image_resource_sptr& match_image
                )
{
  if(!query_||!proto_)
    {
      std::cout << "Missing image data \n";
      return false;
    }
  // create a query observation from the image
  dbinfo_observation_sptr obsq = new dbinfo_observation(0, query_, 0,
                                                        true, true, false);
  // create a prototype observation from the image
  dbinfo_observation_sptr obsp = new dbinfo_observation(0, proto_, 0,
                                                        true, true, false);
  
  float minfo = dbinfo_object_matcher::
    minfo_alignment_in_interval(obsq, obsp, xmin, xmax,
                                ymin, ymax, 
                                theta_min, theta_max,
                                scale_min, scale_max,
                                aspect_min, aspect_max,
                                n_intervals,
                                valid_thresh,
                                match_image);
  return true;
}

*/


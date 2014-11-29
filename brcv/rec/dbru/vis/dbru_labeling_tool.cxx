// This is brcv/rec/dbru/vis/dbru_labeling_tool.cxx
//:
// \file

#include <dbru/vis/dbru_labeling_tool.h>
#include <dbru/dbru_label.h>
#include <dbru/dbru_object.h>
#include <bvis1/bvis1_manager.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vgui/vgui_projection_inspector.h>
#include <vcl_iostream.h>
#include <vcl_ctime.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h> // for rand()
#include <vgui/vgui.h> 
#include <vgui/vgui_style.h>
#include <vgui/vgui_dialog.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vbl/vbl_array_1d.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vil1/vil1_vil.h>
#include <vil/vil_save.h>
//#include <dbru/dbru_osl_sptr.h>
#include <dbru/dbru_osl.h>
//#include <dbru/dbru_osl_storage_sptr.h>
#include <dbru/pro/dbru_osl_storage.h>

//#include <dbdet/lvwr/dbdet_lvwr_fit.h>

#define MIN_PERCENTAGE  (70.0f)



static bpro1_storage_sptr storage()
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  bpro1_storage_sptr data = res->get_data_at("dbru_osl_storage", -1);
  return data;
}

//get track storage from the storage pointer
static dbru_osl_sptr get_osl()
{
  dbru_osl_storage_sptr osl_storage;
  osl_storage.vertical_cast(storage());
  if (!osl_storage)
    return 0;
  else
    return osl_storage->osl();
}


static vil_image_resource_sptr get_image(int frame_no)
{
  vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
  
  if(!res->go_to_frame(frame_no))
    return (vil_image_resource*)0;
  bpro1_storage_sptr sto = res->get_data("image");
  if(!sto)
    return (vil_image_resource*)0;
  //vcl_cout << "Image Storage Name " << sto->name() << '\n';
  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(sto);
  if(!image_storage)
    return (vil_image_resource*)0;
  return image_storage->get_image();
}


//: Constructor - protected
dbru_labeling_tool::dbru_labeling_tool()
{
  activation_ok_ = true;
  set_objects_ = false;

  gesture_remove_latest = vgui_event_condition(vgui_key('u'), vgui_MODIFIER_NULL, true);       
  gesture_tab = vgui_event_condition(vgui_key('j'), vgui_MODIFIER_NULL, true);
  gesture_label = vgui_event_condition(vgui_key('l'), vgui_MODIFIER_NULL, true);
  //gesture_set = vgui_event_condition(vgui_key('s'), vgui_MODIFIER_NULL, true);
  gesture_set = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);
  //gesture_set_empty = vgui_event_condition(vgui_LEFT, vgui_SHIFT, true);
  gesture_create = vgui_event_condition(vgui_MIDDLE, vgui_MODIFIER_NULL, true);;       
  gesture_create2 = vgui_event_condition(vgui_key('k'), vgui_MODIFIER_NULL, true);;       
  gesture_print = vgui_event_condition(vgui_key('p'), vgui_MODIFIER_NULL, true);
  gesture_output = vgui_event_condition(vgui_key('o'), vgui_MODIFIER_NULL, true);
  gesture_output_bin = vgui_event_condition(vgui_key('O'), vgui_MODIFIER_NULL, true);
  gesture_right_arrow = vgui_event_condition(vgui_CURSOR_RIGHT, vgui_MODIFIER_NULL, true);
  gesture_left_arrow = vgui_event_condition(vgui_CURSOR_LEFT, vgui_MODIFIER_NULL, true);
  
  gesture_push_to_osl = vgui_event_condition(vgui_key('P'), vgui_MODIFIER_NULL, true);
  //gesture_delete_object = vgui_event_condition(vgui_DELETE, vgui_SHIFT, true);
  gesture_delete_object = vgui_event_condition(vgui_MIDDLE, vgui_SHIFT, true);
  gesture_delete_object2 = vgui_event_condition(vgui_key('K'), vgui_MODIFIER_NULL, true);
}

vcl_string
dbru_labeling_tool::name() const
{
  return "Vehicle Labeling Tool";
}

//: return true if two input polygons overlap sufficiently 
bool dbru_labeling_tool::overlap(vsol_polygon_2d_sptr poly, vgui_soview2D_polygon* poly2) {
  
  vsol_point_2d_sptr p = poly->centroid();
  float x = 0, y = 0;
  poly2->get_centroid(&x, &y);
  if (vcl_sqrt(vcl_pow(x-p->x(), 2)+vcl_pow(y-p->y(), 2)) < 2)  // almost identical
    return true;
  else 
    return false;
  /*
  vcl_vector<vgl_point_2d<float> > points;
  for (unsigned int i = 0; i<poly->size(); i++) {
    vgl_point_2d<float> f( (float)poly->vertex(i)->x(),
                           (float)poly->vertex(i)->y());

    points.push_back(f);
  }

  vgl_polygon<float> p1(points);

  vgl_polygon<float> p(poly2->x, poly2->y, poly2->n);
  vgl_polygon_scan_iterator<float> psi(p, true);

  //go through the pixels once to check each pixel if its inside poly or not
  int cnt = 0;
  int cnt_overlap = 0;
  for (psi.reset(); psi.next();)
    for (int x = psi.startx(); x<=psi.endx(); x++)
    {
      int y = psi.scany();
      cnt++;
      if (p1.contains(float(x), float(y)))
        cnt_overlap++;
    }

  if ( (float(cnt_overlap)/float(cnt))*100 > MIN_PERCENTAGE)
    return true;
  else
    return false;
  */
}

//: This is called when the tool is activated
void dbru_labeling_tool::activate() {
  
  choices_.push_back("car");
  choices_.push_back("pick-up truck");
  choices_.push_back("suv");
  choices_.push_back("minivan");
  choices_.push_back("utility");
  choices_.push_back("van");
  choices_.push_back("truck");
  choices_.push_back("long truck");
  choices_.push_back("hummer");
  choices_.push_back("bus");
  choices_.push_back("null");
  choice_ = 0;

  latest_label_ = new dbru_label();

  vgui_dialog open_dl("Open file");
  vcl_string poly_filename = "";
  vcl_string object_filename = ".xml";  // may not be supplied if it does not exist
  poly_filename.append(".txt");
  static vcl_string regexp = "*.*";
  open_dl.file("Video Object Polynoms Filename: ", regexp, poly_filename);
  //open_dl.file("Objects Filename (if exists for this video): ", regexp, object_filename);
  //open_dl.checkbox("set objects from this file", set_objects_);
  start_frame_ = 1;
  end_frame_ = 1;
  open_dl.field("Start frame number in original video stream", start_frame_);
  open_dl.field("End frame number in original video stream", end_frame_);
  offset_ = 0;
  open_dl.field("Offset to start using polygons loaded from poly.txt file", offset_);
  vcl_string category_names_file = "*.txt";
  open_dl.file("Use categories from this file (if needed): ", regexp, category_names_file);
 
  open_dl.ask();
  vcl_ifstream fs(poly_filename.c_str());
  
  if (!fs) {
    vcl_cout << "Problems in opening file: " << poly_filename << "\n";
    activation_ok_ = false;
  } else {
    vcl_string dummy;

    fs >> dummy; // VIDEOID:
    if (dummy != "VIDEOID:" && dummy != "FILEID:" && dummy != "VIDEOFILEID:") {
      vcl_cout << "No video id specified in input file!\n";
      video_id_ = 0;
      return;
    } else {
      fs >> video_id_;
      fs >> dummy;   // NFRAMES:
    }
    
    int frame_cnt; 
    fs >> frame_cnt;
    if (frame_cnt != static_cast<int>(end_frame_) - static_cast<int>(start_frame_) + 1) {
      vcl_cout << "Caution: Inconsistency with the number of frames in the polygon file and start and end frames of the video!!!\n";
    }

    //: initialize polygon label vector
    for (int i = 0; i<frame_cnt-offset_; i++) {
      vcl_vector<dbru_label_sptr> tmp;
      polygon_labels_.push_back(tmp);
    }

    //: initialize polygon vector
    for (int i = 0; i<frame_cnt-offset_; i++) {
      vcl_vector<vgui_soview2D_polygon*> tmp;
      polygons_.push_back(tmp);
      //vcl_vector<vgui_soview2D_polygon*> tmp2;
      //lvwr_polygons_.push_back(tmp2);
    }

    active_style_ = vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 1.0f);
    normal_style_ = vgui_style::new_style(0.0f, 0.0f, 1.0f, 3.0f, 1.0f);
    //lvwr_style_ = vgui_style::new_style(0.0f, 1.0f, 0.0f, 3.0f, 3.0f);

    //: initialize object style vector
    for (int i = 0; i<frame_cnt-offset_; i++) {
      vcl_vector<vgui_style_sptr> tmp;
      object_polygon_styles_.push_back(tmp);
    }

    int offset = offset_;
    for (int i = 0; i<frame_cnt; i++)  {  // read each frame
      //: currently assuming that for each frame I have one polygon for each object,
      //  but if we decide to use shadow separately, there might be
      //  more than one in the future.
      fs >> dummy;   // NOBJECTS:   
      fs >> dummy;   // 

      int polygon_cnt;
      fs >> dummy;   // NPOLYS:
      fs >> polygon_cnt;
      for (int j = 0; j<polygon_cnt; j++) {
        fs >> dummy; // NVERTS: 
        int vertex_cnt;
        fs >> vertex_cnt;
        vcl_vector<float> x_corners(vertex_cnt), y_corners(vertex_cnt);
        
        fs >> dummy; // X: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> x_corners[k];

        fs >> dummy; // Y: 
        for (int k = 0; k<vertex_cnt; k++) 
          fs >> y_corners[k];

        vgui_soview2D_polygon *poly = new vgui_soview2D_polygon(vertex_cnt, &x_corners[0], &y_corners[0],true);
#if 0
        vcl_cout << "frame: " << i << " polygon:\nX:";
        for (int k = 0; k<vertex_cnt; k++) 
          vcl_cout << x_corners[k] << " ";
        vcl_cout << "\nY: ";
        for (int k = 0; k<vertex_cnt; k++) 
          vcl_cout << y_corners[k] << " ";
        vcl_cout << "\n";
#endif
        if (offset == 0) {
          polygons_[i-offset_].push_back(poly);
          dbru_label_sptr empty_label = new dbru_label;
          polygon_labels_[i-offset_].push_back(empty_label);
          //: no object assignment yet so just normal style
          object_polygon_styles_[i-offset_].push_back(normal_style_);
        }

      }
      if (offset > 0) 
        offset--;
    }
  }
  
  fs.close();
#if 0
  vcl_ifstream os(object_filename.c_str());
  
  if (os) {
    vcl_cout << "Objects file opened, labels will be loaded if polygons overlap sufficiently\n";

    char buffer[1000];
    os.getline(buffer, 1000); //<? xml version = "1.0" encoding = "UTF-8" ?>
    os.getline(buffer, 1000); //<contour_segmentation>
    
    bool flag = true;
    do {
      dbru_object_sptr temp = new dbru_object();
      flag = temp->read_xml(os);
      if (!flag) break;

      vgui_style_sptr style;
      if (set_objects_) {
        objects_.push_back(temp);
        //: create a random style for this object
        float r = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
        float g = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
        float b = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
        style = vgui_style::new_style(r, g, b, 3.0f, 3.0f);
        object_styles_.push_back(style);   
      }

      for (unsigned int i = 0; i<temp->polygon_cnt_; i++) {
        int frame = temp->start_frame_+i;
        vsol_polygon_2d_sptr poly = temp->polygons_[i];
        dbru_label_sptr label = temp->labels_[i];

        for (unsigned int j = 0; j<polygons_[frame].size(); j++) {
          if (overlap(poly, polygons_[frame][j])) {
            *(polygon_labels_[frame][j]) = *label;
            if (set_objects_) {
              object_polygon_styles_[frame][j] = style;
            }
            break;
          }
        }
        
      }
       
    } while (flag);

  } else {
    vcl_cout << "No objects file, please create labels as well\n";
  }

  print_object_status();
  os.close();
#endif

  vcl_ifstream fss(category_names_file.c_str());
  
  if (!fss) {
    vcl_cout << "No category name file is specified, existing categories are:\n" ;
    for (unsigned kk = 0; kk < choices_.size(); kk++)
      vcl_cout << "\t" << choices_[kk] << "\n";
  } else {
    vcl_cout << "reading category names from the file...\n";
    vcl_string dummy;
    fss >> dummy; 
    do {  
      bool exists = false;
      for (unsigned kk = 0 ; kk < choices_.size(); kk++)
        if (dummy == choices_[kk]) {
          exists = true;
          break;
        }
      if (!exists)
        choices_.insert(choices_.begin(),dummy);
      fss >> dummy; 
    } while (!fss.eof() && dummy.size() > 0);
    fss.close();
    vcl_cout << "Category names are: \n";
    for (unsigned kk = 0; kk < choices_.size(); kk++)
      vcl_cout << "\t" << choices_[kk] << "\n";
  }

  vcl_cout << "The tool is activated!!!\n";
  frame_no_ = bvis1_manager::instance()->current_frame()-start_frame_;//JLM
  active_polygon_no_ = 0;
  active_object_no_ = -1;
  
  vcl_srand(time(NULL));

  if (!get_osl())
    vcl_cout << "WARNING: No osl_storage, load an OSL or create an empty OSL\n";

  bvis1_manager::instance()->post_overlay_redraw();
  return;
}

//: Set the tableau to work with, just checking if video is loaded in this tool
bool
dbru_labeling_tool::set_tableau ( const vgui_tableau_sptr& tableau )
{
  if( tableau.ptr() != NULL && tableau->type_name() == "vgui_image_tableau" ){
    return true;
  }

  vcl_cout << "NON vgui_image_tableau is set!! name is : " << tableau->type_name() << " \n";
  return false;
}

bool
dbru_labeling_tool::handle( const vgui_event & e, 
                             const bvis1_view_tableau_sptr& view )
{
  float ix, iy;
  vgui_projection_inspector().window_to_image_coordinates(e.wx, e.wy, ix, iy);
  frame_no_ = bvis1_manager::instance()->current_frame()-start_frame_;//JLM

  //: as mouse moves update active polygon 
  if ((e.modifier == vgui_MODIFIER_NULL) && (e.type == vgui_MOTION)) {
    if (frame_no_ < 0) return false;
    int temp = active_polygon_no_;
    //: find min distance polygon
    if (polygons_[frame_no_].size() == 0) return false;
    float min_dist = polygons_[frame_no_][0]->distance_squared(ix, iy);
    active_polygon_no_ = 0;
    for (unsigned int i = 1; i<polygons_[frame_no_].size(); i++) {
      float temp = polygons_[frame_no_][i]->distance_squared(ix, iy);
      if (temp < min_dist) {
        min_dist = temp;
        active_polygon_no_ = i;
      }
    }

    if (temp != active_polygon_no_) {
      vcl_cout << "\n------- frame: " << frame_no_ << " ---- poly: " << active_polygon_no_ << "----\n";
      dbru_label_sptr lbl = polygon_labels_[frame_no_][active_polygon_no_];
      vcl_cout << *lbl;
      vcl_cout << "-----------------------------------------------\n";
    }

    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_print(e)) {
    print_object_status();
    return false;
  }

  //: make the current active polygons object active if there is an object containing it
  if (gesture_tab(e)) {   
    int no = -1;
    for (vcl_map<unsigned, vcl_map<unsigned, unsigned>* >::iterator iterpm = object_polygon_map_.begin();
      iterpm != object_polygon_map_.end(); iterpm++) {
        vcl_map<unsigned, unsigned> *map = iterpm->second;
        bool done = false;
        for (vcl_map<unsigned, unsigned>::iterator iter = map->begin(); iter != map->end(); iter++) {
          if (frame_no_ == iter->first && active_polygon_no_ == iter->second) { // found the object
            no = iterpm->first;
            done = true;
            break;
          }
        }
        if (done)
          break;
    }
    
    if (no < 0 || no >= int(objects_.size()))
      vcl_cout << "Inconsistencies!!! Active object could not be changed!\n";
    else {
      active_object_no_ = no;
      vcl_cout << "changed active object to: " << no << vcl_endl;
    }
    print_object_status();
    return false;
  }

  
  if (gesture_right_arrow(e) || gesture_left_arrow(e)) {
    
    
    if (frame_no_ < 0 || frame_no_ >= int(polygons_.size())) return false;
    if (polygons_[frame_no_].size() == 0) return false;

    //: find min distance polygon
    float min_dist = polygons_[frame_no_][0]->distance_squared(ix, iy);
    active_polygon_no_ = 0;
    for (unsigned int i = 1; i<polygons_[frame_no_].size(); i++) {
      float temp = polygons_[frame_no_][i]->distance_squared(ix, iy);
      if (temp < min_dist) {
        min_dist = temp;
        active_polygon_no_ = i;
      }
    }
    
    bvis1_manager::instance()->post_overlay_redraw();
    return false;
  }

  if (gesture_label(e)) {
    get_labeling_info(latest_label_);
    //*(polygon_labels_[frame_no_][active_polygon_no_]) = *(latest_label_);

    vcl_cout << "current label: \n";
    vcl_cout << *latest_label_;
    //vcl_cout << "assigned polygon label: ";
    //vcl_cout << *(polygon_labels_[frame_no_][active_polygon_no_]);

    return false;
  }

  // Undo the latest add
  if (gesture_remove_latest(e)) {
    dbru_object_sptr obj = objects_[active_object_no_];
    int n_removed = obj->remove_last_polygon();
    if (n_removed <= 0)
      vcl_cout << "Active object had no polygons so none removed!!\n";
    else {
      vcl_cout << "Removed last element from active object\n";
      //objects_[active_object_no_]->set_end_frame(objects_[active_object_no_]->end_frame_-1);
      objects_[active_object_no_]->set_end_frame(objects_[active_object_no_]->end_frame_-n_removed);
      
      vcl_map<unsigned, unsigned> *map = object_polygon_map_[active_object_no_];
      vcl_map<unsigned, unsigned>::iterator iter = map->find(frame_no_);
      if (iter != map->end()) {
        unsigned poly_no = (*map)[frame_no_];
        if (object_polygon_styles_[frame_no_][poly_no] == object_styles_[active_object_no_]) {
          object_polygon_styles_[frame_no_][poly_no] = normal_style_;
          polygon_labels_[frame_no_][poly_no] = new dbru_label();
          vcl_cout << "changed the style to normal!\n";
        }
        map->erase(iter);
      } 

      if (objects_[active_object_no_]->end_frame_ < objects_[active_object_no_]->start_frame_)
        objects_[active_object_no_]->start_frame_ = objects_[active_object_no_]->end_frame_ = -1;
    }
    return false;
  }

  // Undo the latest add
  if (gesture_delete_object(e) || gesture_delete_object2(e)) {
    
    if (active_object_no_ < 0 || active_object_no_ >= int(objects_.size()))
      return false;
    dbru_object_sptr obj = objects_[active_object_no_];
    
    vcl_map<unsigned, vcl_map<unsigned, unsigned>* >::iterator iterpm = object_polygon_map_.find(active_object_no_);
    
    if (iterpm != object_polygon_map_.end()) {
      vcl_map<unsigned, unsigned> *map = iterpm->second;
      for (vcl_map<unsigned, unsigned>::iterator iter = map->begin(); iter != map->end(); iter++) { 
        unsigned frame_no = iter->first;
        unsigned poly_no = iter->second;
        if (object_polygon_styles_[frame_no][poly_no] == object_styles_[active_object_no_]) {
            object_polygon_styles_[frame_no][poly_no] = normal_style_;
            polygon_labels_[frame_no][poly_no] = new dbru_label();
            vcl_cout << "changed the style to normal!\n";
        }
      }
      map->clear();
      // decrease the object no in the rest of the map
      vcl_vector<vcl_map<unsigned, unsigned> > temp;
      vcl_map<unsigned, vcl_map<unsigned, unsigned>* >::iterator iterpm_saved = iterpm;
      for (iterpm++ ; iterpm  != object_polygon_map_.end(); iterpm++) { 
        vcl_map<unsigned, unsigned> tmpmap = *(iterpm->second);
        //vcl_cout << "saving: " << tmpmap << vcl_endl;
        temp.push_back(tmpmap);
      }

      object_polygon_map_.erase(iterpm_saved, object_polygon_map_.end());
      for (unsigned i = 0, j = active_object_no_; i<temp.size() && j < objects_.size()-1; j++, i++) {
        //vcl_cout << "readding: " << temp[i] << vcl_endl;
        object_polygon_map_[j] = new vcl_map<unsigned, unsigned>(temp[i]);
      }

    } else {
      vcl_cout << "should not happen\n";
    }
    
    objects_.erase(objects_.begin()+active_object_no_);
    object_styles_.erase(object_styles_.begin()+active_object_no_);
    active_object_no_ = objects_.size()-1;
    print_object_status();
    return false;
  }

  //: create a new object instance
  if (gesture_create(e) || gesture_create2(e)) {
    // create object with video id with -1 start and end frames so set them explicitly when first adding 
    // a polygon
    dbru_object_sptr obj = new dbru_object(video_id_);

    objects_.push_back(obj);
    active_object_no_ = objects_.size()-1;

    //: create a random style for this object
    float r = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
    float g = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
    float b = (   (float)vcl_rand() / ((float)(RAND_MAX)+1.0f) );
    vgui_style_sptr style = vgui_style::new_style(r, g, b, 3.0f, 3.0f);
    object_styles_.push_back(style);   

    print_object_status();
    return false;
  }

  //: add this polygon to the active object
  if (gesture_set(e)) {

    if (frame_no_ < 0 || active_polygon_no_ < 0) return false;

    if (objects_.size() == 0 || active_object_no_ < 0) {
      vcl_cout << "First create an object pressing middle button or k key\n";
      return false;
    }

    if (polygons_[frame_no_].size() == 0) {
      vcl_cout << "No polygons in this frame to set!!\n";
      return false;
    }

    int orig_frame_no = frame_no_+start_frame_;
 
    // check if this is the first time we're adding a polygon to this object
    if (objects_[active_object_no_]->start_frame_ < 0) {   // if first time
      objects_[active_object_no_]->set_start_frame(orig_frame_no);
      objects_[active_object_no_]->set_end_frame(orig_frame_no);
      vcl_map<unsigned, unsigned>* map = new vcl_map<unsigned, unsigned>();
      (*map)[frame_no_] = active_polygon_no_;
      object_polygon_map_[active_object_no_] = map;
    } else {   // if not first time, check if any other polygons have been set for this object, in that case overwrite it
      vcl_map<unsigned, unsigned>* map = object_polygon_map_[active_object_no_];
      vcl_map<unsigned, unsigned>::iterator iter = map->find(frame_no_);
      if (iter != map->end()) {
        unsigned poly_no = iter->second;
        object_polygon_styles_[frame_no_][poly_no] = normal_style_;
        polygon_labels_[frame_no_][poly_no] = new dbru_label();
      }
      (*map)[frame_no_] = active_polygon_no_;

      if (objects_[active_object_no_]->end_frame_ > orig_frame_no) {
        vcl_cout << "!!!CAUTION: You cannot add a polygon previous than the last added polygon to this object!!\nCANNOT GO BACK!\n";
        print_object_status();
        return false;
      }
    }

    object_polygon_styles_[frame_no_][active_polygon_no_] = object_styles_[active_object_no_];
    vgui_soview2D_polygon *poly_view = polygons_[frame_no_][active_polygon_no_];

    if ((polygon_labels_[frame_no_][active_polygon_no_])->category_name_ == "null")
      vcl_cout << "assigned polygon label: " << *(polygon_labels_[frame_no_][active_polygon_no_]);
    else {
      vcl_cout << "This polygon have been labeled before, change the active object to this one and remove this polygon, then add again with the correct label\n";
      return false;
    }

    *(polygon_labels_[frame_no_][active_polygon_no_]) = *(latest_label_);
    
    float *x = (*poly_view).x;
    float *y = (*poly_view).y;
    int n = (*poly_view).n;

    vcl_vector<vsol_point_2d_sptr> vertices;
    for (int i = 0; i<n; i++) 
      vertices.push_back(new vsol_point_2d(x[i], y[i]));

    vsol_polygon_2d_sptr poly = new vsol_polygon_2d(vertices);
 
    while (orig_frame_no - objects_[active_object_no_]->end_frame_ > 1) {
      // add an empty polygon
      vsol_polygon_2d_sptr empty_poly = new vsol_polygon_2d();
      objects_[active_object_no_]->add_polygon(empty_poly, latest_label_);
      objects_[active_object_no_]->set_end_frame(objects_[active_object_no_]->end_frame_ + 1);
    }

    objects_[active_object_no_]->add_polygon(poly, polygon_labels_[frame_no_][active_polygon_no_]);
    objects_[active_object_no_]->set_end_frame(orig_frame_no);

    return false;
  }
#if 0
  //: set a null polygon (with 0 vertices) for the current frame to the active object
  //  (no polygon is created for that object in this frame)
  if (gesture_set_empty(e)) {

    if (objects_.size() == 0) {
      vcl_cout << "First create an object pressing middle button or c key\n";
      return false;
    }

    int orig_frame_no = frame_no_+start_frame_;
    
    // if this is the first time a polygon is added   
    if (objects_[active_object_no_]->start_frame_ < 0) {
      vsol_polygon_2d_sptr empty_poly = new vsol_polygon_2d();
      objects_[active_object_no_]->add_polygon(empty_poly, latest_label_);
   
      objects_[active_object_no_]->set_start_frame(orig_frame_no);
      objects_[active_object_no_]->set_end_frame(orig_frame_no);
    }

    if (objects_[active_object_no_]->end_frame_ > orig_frame_no) {
      vcl_cout << "You cannot add a polygon previous than the last added polygon to this object!!\nCANNOT GO BACK!\n";
      print_object_status();
      return false;
    }

    while (orig_frame_no - objects_[active_object_no_]->end_frame_ > 0) {
      // add an empty polygon
      vsol_polygon_2d_sptr empty_poly = new vsol_polygon_2d();
      objects_[active_object_no_]->add_polygon(empty_poly, latest_label_);
      objects_[active_object_no_]->set_end_frame(objects_[active_object_no_]->end_frame_ + 1);
    }

    return false;
  }
#endif

  // save the output
  if (gesture_output(e)) {

    vcl_string filename = ".xml";

    static vcl_string regexp = "*.*";

    vgui_dialog save_dl("Save file");
    save_dl.inline_file("Filename: ", regexp, filename);
    
    if (save_dl.ask()) {

      vcl_ofstream df(filename.c_str());
      
      if (!df)
        vcl_cout << "Problems in opening files: " << filename << "\n";
      else {

        df << "<?xml version = \"1.0\" encoding = \"UTF-8\" ?>\n";
        df << "\t<contour_segmentation";
                df << "\t object_cnt=\""<<objects_.size()<<"\""<<">\n";
        for (unsigned int i = 0; i<objects_.size(); i++)
          objects_[i]->write_xml(df);
        df << "\t</contour_segmentation>\n";
        df.close();
        vcl_cout << "Label info written to the specified files!\n";
      }
      
    }

    
    return false;
  }

  if (gesture_output_bin(e)) {

    vcl_string binext = ".bin";
        vcl_string xmlext = ".xml";
        vcl_string txtext = ".txt";

    static vcl_string regexp = "*.*";
    vgui_dialog save_dl("Save Binary file");
    save_dl.inline_file("Input Filename (XML): ", regexp, xmlext);
        save_dl.inline_file("Video List filename: ", regexp, txtext);
        save_dl.inline_file("OutFilename: ", regexp, binext);
    
    if (save_dl.ask()) {
                createosl((char *)xmlext.c_str(),(char *)txtext.c_str(),(char *)binext.c_str());
    }

    
    return false;
  }

    if (gesture_push_to_osl(e)) {
      vcl_cout << "Pushing active obj into OSL, extracting observations: ";
      dbru_object_sptr active_obj = objects_[active_object_no_];
      if (active_obj->n_observations() != active_obj->n_polygons()) {
        // clear all the observations and reextract
        active_obj->clear_only_observations();
        
        for (unsigned j = 0; j < active_obj->n_polygons(); j++) 
        {
          unsigned i = active_obj->start_frame()+j;
          vil_image_resource_sptr img = get_image(i);
          if (!img || int(i) > active_obj->end_frame())
            vcl_cout << j << " Frame image problems!! Observation cannot be extracted!\n";
          else {
            vcl_cout << j << " ";
            vsol_polygon_2d_sptr poly = active_obj->get_polygon(j);
            dbinfo_observation_sptr obs = new dbinfo_observation(0, img, poly, true, true, false);
            obs->scan(0, img);
            active_obj->add_observation(obs);
          }
        }
      }
      dbru_osl_sptr osl = get_osl();
      if (!osl)
        vcl_cout << "WARNING: No osl_storage, load an OSL or create an empty OSL, Active Object is not pushed!\n";
      else {
        osl->add_object(active_obj);
        vcl_cout << " done!\n";
      } 
    }
  
  if (e.type == vgui_DRAW_OVERLAY) {
    
    if (frame_no_ >= 0 && unsigned(frame_no_) < polygons_.size()) {
            
      //: draw each polygon in this frame
      for (unsigned int i = 0; i<polygons_[frame_no_].size(); i++) {
        object_polygon_styles_[frame_no_][i]->apply_all();
        polygons_[frame_no_][i]->draw();
      }

      if (active_polygon_no_ >= 0 && unsigned(active_polygon_no_) < polygons_[frame_no_].size()) {
        active_style_->apply_all();
        polygons_[frame_no_][active_polygon_no_]->draw();
      }
      
      /*if (lvwr_ && frame_no_ >= int(start_frame_) && frame_no_ <= int(end_frame_)) {
        //: draw each polygon in this frame
        for (unsigned int i = 0; i<lvwr_polygons_[frame_no_].size(); i++) {
          lvwr_style_->apply_all();
          lvwr_polygons_[frame_no_][i]->draw();
        }
      }*/
    } else {
      vcl_cout << "Current frame number is not valid!\n";
    }
     
    return false;
  }

  return false;
}

//-----------------------------------------------------------------------------
//: Make and display a dialog box
//-----------------------------------------------------------------------------
bool dbru_labeling_tool::get_labeling_info(dbru_label_sptr label)
{
  vgui_dialog* dlg = new vgui_dialog("Get label info");
  
  bool new_category = false;
  dlg->checkbox("Use new category?", new_category);
  vcl_string user_category = "";
  dlg->field("Name of new category: ", user_category);
  dlg->choice("Choose an existing category:", choices_, choice_);
  dlg->field("motion orientation bin:", label->motion_orientation_bin_);
  dlg->field("view angle bin:", label->view_angle_bin_);
  dlg->field("shadow angle bin: ", label->shadow_angle_bin_);
  dlg->field("shadow length: ", label->shadow_length_);
  
  //dlg->checkbox("Training sample ",label->training_sample_);
  bool return_value = dlg->ask();
  if (new_category) {
    choices_.push_back(user_category);
    label->category_name_ = user_category;
  } else
    label->category_name_ = choices_[choice_];
  
  return return_value;
}

void dbru_labeling_tool::print_object_status() 
{
  vcl_cout << "frame_no: " << frame_no_ << " active_polygon_no: " << active_polygon_no_ << vcl_endl;

  if (active_object_no_ < 0) {
    vcl_cout << "No objetcs set yet!!\n";
  } else {
    vcl_cout << "active object: " << active_object_no_ << " category: " << objects_[active_object_no_]->category_;
    vcl_cout << " start_frame: " << objects_[active_object_no_]->start_frame_;
    vcl_cout << " end_frame: " << objects_[active_object_no_]->end_frame_;
    vcl_cout << vcl_endl;
  }
}

int dbru_labeling_tool::createosl(char *db_object_list_filename, char *video_list_file_name, char *osl_file_name) 
{ 
  vcl_ofstream ofile("log.txt");
  vcl_ifstream fp(video_list_file_name);
  if (!fp) {
    ofile << "Problems in opening video directory list file!\n";
    return 1;
  }

  vcl_map<int, vidl1_movie_sptr> video_directories;
  int id = -1;
  fp >> id;
  while (!fp.eof()) {
    vcl_string dir;
    fp >> dir;
    vidl1_movie_sptr my_movie = vidl1_io::load_movie(dir.c_str());
    
    if (!my_movie) {
      vcl_cout << "problems in loading video ";
      vcl_cout << " with video file name: " << dir << vcl_endl;
      return 2;
    }
    video_directories[id] = my_movie;
    fp >> id;
  }
  fp.close();
  
  ofile<< "loaded " << video_directories.size() << " video dirs from the file\n";
  
  //vbl_array_1d<dbru_object_sptr> *database_objects = reinterpret_cast<vbl_array_1d<dbru_object_sptr> *> (handle);  
  vbl_array_1d<dbru_object_sptr> *database_objects = new vbl_array_1d<dbru_object_sptr>();
  database_objects->clear();

  vcl_ifstream dbfp(db_object_list_filename);
  if (!dbfp) {
    ofile << "Problems in opening db object list file!\n";
    return 3;
  }

  ofile << "reading database objects...\n";
  
  char buffer[1000]; 
  dbfp.getline(buffer, 1000);  // comment 
  vcl_string dummy;
  dbfp >> dummy;   // <contour_segmentation   
  dbfp >> dummy; // object_cnt="23">
  unsigned int size;
  sscanf(dummy.c_str(), "object_cnt=\"%d\">", &size); 

  for (unsigned i = 0; i<size; i++) {
    ofile << "reading database object: " << i << "...\n";
    dbru_object_sptr obj = new dbru_object();
    if (!obj->read_xml(dbfp)) { 
      ofile << "problems in reading database object number: " << i << vcl_endl;
      return 4;
    }

    //: get video file for this database object
    vidl1_movie_sptr my_movie = video_directories[obj->video_id_];
    
    ofile << "read: " << obj << " extracting observations assuming 1 polygon per frame" << vcl_endl;
    
    for (int j = obj->start_frame_; j<=obj->end_frame_; j++) {
      vidl1_frame_sptr frame = my_movie->get_frame(j);
      vil_image_resource_sptr imgr = frame->get_resource();
      vsol_polygon_2d_sptr poly = obj->get_polygon(j-obj->start_frame_);
      int s = poly->size();
      if (s > 0) {

        // get convex hull and use that
        ofile << "!!!creating convex hulled observations\n";
        vcl_vector<vgl_point_2d<double> > ps;
        for(unsigned int pj=0;pj<poly->size();pj++)
        {
          vgl_point_2d<double> p(poly->vertex(pj)->x(),poly->vertex(pj)->y());
          ps.push_back(p);
        }
        vcl_vector<vsol_point_2d_sptr> cps;
        vgl_convex_hull_2d<double> hullp(ps);
        vgl_polygon<double> psg=hullp.hull();
        for(unsigned int k=0;k<psg[0].size();k++)
          cps.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
        vsol_polygon_2d_sptr c_poly = new vsol_polygon_2d(cps);
        ofile << "size before hull: " << s << " size after hull: " << c_poly->size() << vcl_endl;
        //dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, poly, true, true, false);
        dbinfo_observation_sptr obs = new dbinfo_observation(0, imgr, c_poly, true, true, false);
        obs->scan(0, imgr);
        obj->add_observation(obs);
        // output the images
        ofile << " try to save output image into ./v<videoid>_objimages directory\n";
        char buffer[1000];
        vcl_sprintf(buffer, "./v%d_objimages/image_obj%d-poly%d.png",obj->video_id_, i, j-obj->start_frame_);
        vcl_string filename = buffer;

        vcl_ofstream dummy_f(filename.c_str());
        bool create = false;
        if (dummy_f.is_open()) {
          dummy_f.close();
          create = true;
        } else {
          ofile << " cannot create the image file: " << filename << " (directory does not exist) " <<vcl_endl;
        }

        if (create) {
          vil_image_resource_sptr output_sptr = obs->image_cropped();
         vil_save_image_resource(output_sptr, filename.c_str());
        }
      
      } else {
        obj->add_observation(0);  // add null pointer as observation
      }
    }

    if (obj->n_observations() == obj->n_polygons())
      database_objects->push_back(obj);
    else {
      ofile << "problems in object " << *(obj) << " skipping!\n";
      continue;
    }
  }

  if (database_objects->size() > 0) {
    vsl_b_ofstream obfile(osl_file_name);
    vsl_b_write(obfile, database_objects->size());
    for (unsigned i = 0; i<database_objects->size(); i++) {
      (*database_objects)[i]->b_write(obfile);
    }
    obfile.close();
    ofile << "Objects and observations are written to binary output file\n";
  }

  ofile.close();
  return 0;
}

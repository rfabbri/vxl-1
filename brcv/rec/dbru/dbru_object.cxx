// This is  brcv/rec/dbru_object.cxx
//:
// \file
#include <dbru/dbru_object.h>
#include <vcl_iostream.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_cmath.h>
#include <dbru/dbru_label.h>
#include <dbinfo/dbinfo_observation.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbru/dbru_multiple_instance_object.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_vector_io.h>


//------------------------------------------------------------------------
// Constructors
//

dbru_object::
dbru_object(const int video_id,
            const int start_frame,
      const int end_frame) : doc_("")
{
  video_id_ = video_id;      
  start_frame_ = start_frame;    
  end_frame_ = end_frame;
  polygon_per_frame_ = 1;             
 
  polygon_cnt_ = 0;    
  category_ = "null";
}

dbru_object::
dbru_object(const dbru_object& other)
{
  video_id_ = other.video_id_;
  start_frame_ = other.start_frame_;
  end_frame_ = other.end_frame_;
  polygon_per_frame_ = other.polygon_per_frame_;

  polygons_.clear();
  labels_.clear();
  polygon_cnt_ = 0;

  for (unsigned int i = 0; i<other.polygon_cnt_; i++) 
    add_polygon(other.polygons_[i], other.labels_[i]);

  observations_.clear();
  for (unsigned int i = 0; i<(other.observations_).size(); i++) 
    add_observation(other.observations_[i]);

  instances_.clear();
  for (unsigned int i = 0; i<(other.instances_).size(); i++) 
    add_instance(other.instances_[i]);

  category_ = other.category_;
  doc_ = other.doc_;
}

//: set number of polygons per frame
void dbru_object::set_polygon_per_frame(int poly_per_frame) { polygon_per_frame_ = poly_per_frame; }

//: add a polygon (polygons are added without regard to frames, user should keep such records
bool dbru_object::add_polygon(vsol_polygon_2d_sptr poly, dbru_label_sptr label) 
{ 
  if (polygon_cnt_ == 0) 
    category_ = label->category_name_;
  else if (category_ != label->category_name_) {
    vcl_cout << "category mismatch in adding polygon!! Polygon not added!!\n";
    return false;
  }
    
  polygons_.push_back(poly);
  labels_.push_back(label);

  assert(polygons_.size() == labels_.size());
  
  polygon_cnt_ = polygons_.size();

  return true;
}

//: replace an existing polygon with the current one (label stays the same)
void dbru_object::set_polygon(unsigned int i, vsol_polygon_2d_sptr poly)
{
  assert(i >= 0 && i < polygon_cnt_);
  polygons_[i] = poly;
}

//: remove the last added non-empty polygon and all the empty polygons before it
int dbru_object::remove_last_polygon(void) {
  if (!polygons_.size())
    return -1;
  
  int n_removed = 1;
  polygons_.pop_back();
  labels_.pop_back();
  polygon_cnt_ = polygons_.size();

  while (polygons_.size() > 0) {
    vsol_polygon_2d_sptr p = polygons_[polygons_.size()-1];
    if (p->size() != 0) 
      break;
    n_removed++;
    polygons_.pop_back();
    labels_.pop_back();
    polygon_cnt_ = polygons_.size();
  }
  
  return n_removed;
}

//: replace an existing polygon and its label with the given ones
bool dbru_object::set_polygon(unsigned int i, vsol_polygon_2d_sptr poly, dbru_label_sptr label)
{
  if (category_ != label->category_name_) {
    vcl_cout << "category mismatch in setting polygon!! Polygon not set!!\n";
    return false;
  }

  assert(i >= 0 && i < polygon_cnt_);
  polygons_[i] = poly;
  labels_[i] = label;

  return true;
}

//: replace an existing instance
bool dbru_object::set_instances(unsigned int i, dbru_multiple_instance_object_sptr ins) 
{
  assert(i >= 0 && i < instances_.size());
  instances_[i] = ins;
  return true;
}

 //: get the ith instance in the list
dbru_multiple_instance_object_sptr dbru_object::get_instance(unsigned int i)
{
  if (i >= 0 && i < instances_.size()) 
    return instances_[i]; 
  else 
    return 0;
}

//: replace an existing observation
// CAUTION: this is assuming that polygon is also correctly set for this observations
bool dbru_object::set_observation(unsigned int i, dbinfo_observation_sptr obs) 
{
  assert(i >= 0 && i < observations_.size());
  observations_[i] = obs;
  return true;
}

//: replace an existing polygon label with the given ones
bool dbru_object::set_polygon_label(unsigned int i, dbru_label_sptr label)
{
  if (category_ != label->category_name_) {
    vcl_cout << "category mismatch in setting label!! Label not set!!\n";
    return false;
  }

  assert(i >= 0 && i < polygon_cnt_);
  labels_[i] = label;

  return true;
}

//: get the ith polygon in the list
vsol_polygon_2d_sptr dbru_object::get_polygon (unsigned int i)
{
  if (i >= 0 && i < polygon_cnt_)
    return polygons_[i]; 
  else 
    return 0;
}

//: get the kth non-null polygon
vsol_polygon_2d_sptr dbru_object::get_non_null_polygon(unsigned int k, unsigned int&i)
{
  int cnt_k = 0;
  // find polygon number that corresponds to osi th non-null polygon
  i = 0;
  for ( ; i < polygons_.size(); i++) {
    if (polygons_[i]->size() == 0)
      continue;
    if (cnt_k == k)
      break;
    cnt_k++;
  }
  if (i < polygons_.size())
    return polygons_[i];
  else 
    return 0;
}

//: get ith polygon's label
dbru_label_sptr dbru_object::get_label (unsigned int i) 
{ if (i >= 0 && i < polygon_cnt_)
    return labels_[i];
  else
    return 0;
}

dbru_object& dbru_object::operator=(const dbru_object& right) 
{
  if (this != &right) {
    video_id_ = right.video_id_;
    start_frame_ = right.start_frame_;
    end_frame_ = right.end_frame_;

    polygons_.clear();
    labels_.clear();

    for (unsigned int i = 0; i<right.polygon_cnt_; i++) 
      add_polygon(((dbru_object)right).get_polygon(i), ((dbru_object)right).get_label(i));

    observations_.clear();
    for (unsigned int i = 0; i<((dbru_object)right).n_observations(); i++) 
      add_observation(((dbru_object)right).get_observation(i));

    instances_.clear();
    for (unsigned int i = 0; i<((dbru_object)right).n_instances(); i++) 
      add_instance(((dbru_object)right).get_instance(i));
  }

  return *this;
}

//: add an observation
bool dbru_object::add_observation(dbinfo_observation_sptr obs) 
{    
  observations_.push_back(obs);
  return true;
}

//: add an instance
bool dbru_object::add_instance(dbru_multiple_instance_object_sptr ins)
{
  instances_.push_back(ins);
  return true;
}

//: get the ith observation in the list
dbinfo_observation_sptr dbru_object::get_observation(unsigned int i) {
  if (i >= 0 && i < observations_.size()) 
    return observations_[i]; 
  else 
    return 0;
}

//: get the observation with given documentation
dbinfo_observation_sptr dbru_object::get_observation(vcl_string const& obs_doc) {
  for (unsigned i = 0; i<observations_.size(); i++)
    if (observations_[i]->doc() == obs_doc)
      return observations_[i];
  return 0;
}

//: remove the observation and corresponding polygon and label based on polygon_per_frame count
void dbru_object::remove_observation(unsigned int i) 
{
  if (i >= observations_.size()) 
    return;

  vcl_cout << "WARNING in dbru_object::remove_observation() -- this method destroys the consecutiveness of the observations and polygons\n";
  vcl_cout << "                                                start frame and end_frame may not be meaningful after this\n";
               
  vcl_vector< dbinfo_observation_sptr >::iterator iter;
  iter = observations_.begin()+i;
  observations_.erase(iter);

  vcl_vector< dbru_multiple_instance_object_sptr >::iterator iter_i;
  iter_i = instances_.begin()+i;
  instances_.erase(iter_i);

  vcl_vector< vsol_polygon_2d_sptr >::iterator iter_p;
  iter_p = polygons_.begin()+(i*polygon_per_frame_);
  polygons_.erase(iter_p, iter_p+polygon_per_frame_-1);

  vcl_vector< dbru_label_sptr >::iterator iter_l;
  iter_l = labels_.begin()+(i*polygon_per_frame_);
  labels_.erase(iter_l, iter_l+polygon_per_frame_-1);
}

//: null the prototype and observation pointers if they exist
bool dbru_object::null_observation(unsigned i) 
{
  if (i < observations_.size())
    observations_[i] = 0;
  if (i < instances_.size())
    instances_[i] = 0;
  if (i < polygons_.size()) {
    vsol_polygon_2d_sptr empty_poly = new vsol_polygon_2d();
    polygons_[i] = empty_poly;   ///// do not touch the label
    return true;
  } else 
    return false;
}

//: replace an existing observation with given documentation
bool dbru_object::set_observation(vcl_string const& obs_doc, dbinfo_observation_sptr obs) {
  for (unsigned i = 0; i<polygon_cnt_; i++)
    if (observations_[i]->doc() == obs_doc) {
      observations_[i] = obs;
      return true;
    }
  return false;
}

void dbru_object::write_xml(vcl_ostream& os) 
{
  os << "<object videofile_id= \"" << video_id_ << "\" "; 
  os << " start_frame= \"" << start_frame_ << "\" ";
  os << " end_frame= \"" << end_frame_ << "\" ";
  os << " category= \"vehicle\" ";
  os << " subcategory= \"lockheed\" ";
  os << " subject= \"" << category_ << "\">\n";
  
  //os << "\t<series_of_polygon cnt= \"" << polygon_cnt_ << "\" ";
  //os << " polygon_per_frame= \"" << polygon_per_frame_ << "\">\n";
  
  for (unsigned int i = 0; i<polygon_cnt_; i++) {
    vsol_polygon_2d_sptr p = polygons_[i];

    //: create track information
    //<track x="993" y="522" width="47" height="32" x_margin="11" y_margin="8">
    
    vsol_point_2d_sptr cent = new vsol_point_2d(0, 0);
    int width = 2; int height = 2;
    if (p->size() > 0) {
      cent = p->centroid();
      width = (int)vcl_floor(p->get_max_x()-p->get_min_x()+0.5);
      height = (int)vcl_floor(p->get_max_y()-p->get_min_y()+0.5);
    }

    os << "<track x=\"" << cent->x() << "\" y = \"" << cent->y() << "\" ";
    os << "width=\"" << width << "\" ";
    os << "height=\"" << height << "\" ";
    os << "x_margin=\"" << (int)vcl_floor(width/4.0f) << "\" ";
    os << "y_margin=\"" << (int)vcl_floor(height/4.0f) << "\">\n";

    dbru_label_sptr label = labels_[i];
    //os << "\t\t<polygon number_of_vertices= \"" << p->size() << "\" ";
    os << "<Polygon number_of_vertices= \"" << p->size() << "\" ";
    os << " motion_orientation_bin= \"" << label->motion_orientation_bin_ << "\" ";
    os << " view_angle_bin= \"" << label->view_angle_bin_ << "\" ";
    os << " shadow_angle_bin= \"" << label->shadow_angle_bin_ << "\" ";
    os << " shadow_length= \"" << label->shadow_length_ << "\">\n";
    //os << "\t\t\t";
    os << " <outerBoundaryIs>\t\t<LinearRing>\t\t<coordinates> \n";
    for (unsigned int j = 0; j < p->size(); j++)
      os << p->vertex(j)->x() << "," << p->vertex(j)->y() << " ";
    os << "\n";
    os << " </coordinates></LinearRing></outerBoundaryIs></Polygon></track>\n";
    //os << "\t\t</polygon>\n";
  }
  
  //os << "\t</series_of_polygon>\n";
  
  os << "</object>\n";

}

//: my simple reader in our strict format
bool dbru_object::read_xml(vcl_istream& os) 
{
  if (!os || os.eof()) {
    vcl_cout << "Input file is not opened or empty!\n";
    return false;
  }

  vcl_string dummy;
  vcl_string::size_type pos1, pos2;
  char buffer[1000];
  vcl_string line;

  do {
    os.getline(buffer, 1000);

    if (os.eof()) {
      vcl_cout << "Input file is empty! or <object> flag could not be found in this file\n";
      return false;
    }

    line = buffer;
    pos1 = line.find("object", 0);
  
  } while (pos1 == vcl_string::npos);  // if false, assume there is a complete object afterwards 

  pos1 = line.find("\"", 0);
  if (pos1 == vcl_string::npos) {// not found 
    vcl_cout << "Video id could not be found!\n";
    return false;
  }
  
  pos2 = line.find("\"", pos1+1);
  dummy = line.substr(pos1+1, pos2-pos1-1);
  sscanf(dummy.c_str(), "%d", &video_id_);

  pos1 = line.find("\"", pos2+1);
  if (pos1 == vcl_string::npos) {// not found
    vcl_cout << "Start frame could not be found!\n";
    return false;
  }

  pos2 = line.find("\"", pos1+1);
  dummy = line.substr(pos1+1, pos2-pos1-1);
  sscanf(dummy.c_str(), "%d", &start_frame_);

  pos1 = line.find("\"", pos2+1);
  if (pos1 == vcl_string::npos) {// not found
    vcl_cout << "End frame could not be found!\n";
    return false;
  }

  pos2 = line.find("\"", pos1+1);
  dummy = line.substr(pos1+1, pos2-pos1-1);
  sscanf(dummy.c_str(), "%d", &end_frame_);

  pos1 = line.find("\"", pos2+1);  // find category and skip
  if (pos1 == vcl_string::npos) {// not found
    vcl_cout << "Category could not be found!\n";
    return false;
  }

  pos2 = line.find("\"", pos1+1);
  
  pos1 = line.find("\"", pos2+1);  // find subcategory and skip
  if (pos1 == vcl_string::npos) {// not found
    vcl_cout << "Subcategory could not be found!\n";
    return false;
  }

  pos2 = line.find("\"", pos1+1);

  pos1 = line.find("\"", pos2+1);  // find subject and save
  if (pos1 == vcl_string::npos) {// not found
    vcl_cout << "Subject could not be found!\n";
    return false;
  }

  pos2 = line.find("\"", pos1+1);
  category_ = line.substr(pos1+1, pos2-pos1-1);
  
  //os >> dummy; // <series_of_polygon
  //os >> dummy; // cnt= 
  //os >> dummy; 
  //unsigned int cnt;
  //sscanf(dummy.c_str(), "\"%d\"", &cnt); // "polygon_cnt_"
  
  //os >> dummy; // polygon_per_frame= 
  //os >> dummy; 
  //sscanf(dummy.c_str(), "\"%d\">", &polygon_per_frame_); // "polygon_per_frame_">;
  
  //for (unsigned int i = 0; i<cnt; i++) {
  os >> dummy; // <track
  while (dummy != "</object>") {
    
    os.getline(buffer, 1000);  // <track x="993" y="522" width="47" height="32" x_margin="11" y_margin="8">

    os >> dummy; // <Polygon
    os >> dummy; // number_of_vertices= 
    os >> dummy; // \"" << p->size() << "\" ";
    unsigned int size;
    sscanf(dummy.c_str(), "\"%d\"", &size); 

    dbru_label_sptr label = new dbru_label();
    label->category_name_ = category_;

    os >> dummy; // motion_orientation_bin= 
    os >> dummy; //\"" << label->motion_orientation_bin_ << "\" ";
    sscanf(dummy.c_str(), "\"%d\"", &(label->motion_orientation_bin_));

    os >> dummy; // view_angle_bin= 
    os >> dummy; // \"" << label->view_angle_bin_ << "\" ";
    sscanf(dummy.c_str(), "\"%d\"", &(label->view_angle_bin_));

    os >> dummy; // shadow_angle_bin= 
    os >> dummy; // \"" << label->shadow_angle_bin_ << "\" ";
    sscanf(dummy.c_str(), "\"%d\"", &(label->shadow_angle_bin_));

    os >> dummy; // shadow_length= 
    os >> dummy; // \"" << label->shadow_length_ << "\">\n";
    sscanf(dummy.c_str(), "\"%d\">", &(label->shadow_length_));    

    os >> dummy; // <outerBoundaryIs>
    os >> dummy; // <LinearRing>
    os >> dummy; // <coordinates>

    vcl_vector<vsol_point_2d_sptr> vertices;
    for (unsigned int j = 0; j < size; j++) {
      float x, y;
      os >> x; 
      os.get();  // read ,
      os >> y;
      vertices.push_back(new vsol_point_2d(x, y));
    }

    vsol_polygon_2d_sptr poly;
    if (size > 0)
      poly = new vsol_polygon_2d(vertices);
    else 
      poly = new vsol_polygon_2d();

    add_polygon(poly, label);
    
    os >> dummy;  // </coordinates></LinearRing></outerBoundaryIs></Polygon></track>

    os >> dummy; // <track
  }
  
  //os >> dummy; // </serises_of_polygon>
  //os >> dummy; // </object>
  if (dummy != "</object>") {
    vcl_cout << "Problems in parsing!\n";
  }

  polygon_per_frame_ = 1;
  return true;
}

void dbru_object::print(vcl_ostream& os) const
{
  os << "OBJECT video id:\t" << video_id_ << vcl_endl
     << "start frame:\t" << start_frame_ << vcl_endl
     << "end frame:\t" << end_frame_ << vcl_endl
     << "polygons_per_frame:\t" << polygon_per_frame_ << vcl_endl
     << "polygon_cnt:\t" << polygon_cnt_ << vcl_endl;

  for (unsigned int i = 0; i<polygon_cnt_; i++) {
    vsol_polygon_2d_sptr p = polygons_[i];
    os << "polygon " << i << " label: " << *(labels_[i]) << "\tnbr vertices: " << p->size() << "\n"; 
    os << "\t";
    for (unsigned int j = 0; j<p->size(); j++)
      os << " " << (p->vertex(j))->x() << " " << (p->vertex(j))->y();
    os << "\n";
  }
}

//: Binary save self to stream.
void dbru_object::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());

  vsl_b_write(os, this->doc_);

  vsl_b_write(os, this->video_id_);
  vsl_b_write(os, this->start_frame_);
  vsl_b_write(os, this->end_frame_);
  vsl_b_write(os, this->category_);
  vsl_b_write(os, this->polygon_cnt_);
  vsl_b_write(os, this->polygon_per_frame_);
  
  for (unsigned i = 0; i<this->polygon_cnt_; i++) {
    (*(this->labels_[i])).b_write(os);
  }

  for (unsigned i = 0; i<this->polygon_cnt_; i++) {
    (*(this->polygons_[i])).b_write(os);
  }

  // save observations only if they are created for all polygons
  if (observations_.size() != polygon_cnt_) {
    vsl_b_write(os, false);
  } else {
    vsl_b_write(os, true);
    for (unsigned i = 0; i<this->polygon_cnt_; i++) {
      if (!this->observations_[i]) {
        vsl_b_write(os, false);
      } else {
        vsl_b_write(os, true);
        (*(this->observations_[i])).b_write(os);
      }
    }
  }

  // save multiple instances only if they are created for all polygons
  if (instances_.size() != polygon_cnt_) {
    vsl_b_write(os, false);
  } else {
    vsl_b_write(os, true);
    for (unsigned i = 0; i<this->polygon_cnt_; i++) {
      if (!this->instances_[i]) {
        vsl_b_write(os, false);
      } else {
        vsl_b_write(os, true);
        (*(this->instances_[i])).b_write(os);
      }
    }
  }
    
  return;
}

//: Binary load self from stream.
void dbru_object::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 2: { vsl_b_read(is, doc_); }
    case 1:
      {
        vsl_b_read(is, video_id_);
        vsl_b_read(is, start_frame_);
        vsl_b_read(is, end_frame_);
        vsl_b_read(is, category_);
        vsl_b_read(is, polygon_cnt_);
        vsl_b_read(is, polygon_per_frame_);
        
        for (unsigned i = 0; i<this->polygon_cnt_; i++) {
          dbru_label_sptr l = new dbru_label();
          l->b_read(is);
          labels_.push_back(l);
        }

        for (unsigned i = 0; i<this->polygon_cnt_; i++) {
          vsol_polygon_2d_sptr p = new vsol_polygon_2d();
          p->b_read(is);
          polygons_.push_back(p);
        }

        bool obs_available;
        vsl_b_read(is, obs_available);
        if (obs_available) {
          for (unsigned i = 0; i<this->polygon_cnt_; i++) {
            bool ob_available;
            vsl_b_read(is, ob_available);
            if (ob_available) {
              dbinfo_observation_sptr o = new dbinfo_observation();
              o->b_read(is);
              observations_.push_back(o);
            } else {
              observations_.push_back(0);
            }
          }
        }
        break;
      }
    case 3:
      {
        vsl_b_read(is, doc_);
        vsl_b_read(is, video_id_);
        vsl_b_read(is, start_frame_);
        vsl_b_read(is, end_frame_);
        vsl_b_read(is, category_);
        vsl_b_read(is, polygon_cnt_);
        vsl_b_read(is, polygon_per_frame_);
        
        for (unsigned i = 0; i<this->polygon_cnt_; i++) {
          dbru_label_sptr l = new dbru_label();
          l->b_read(is);
          labels_.push_back(l);
        }

        for (unsigned i = 0; i<this->polygon_cnt_; i++) {
          vsol_polygon_2d_sptr p = new vsol_polygon_2d();
          p->b_read(is);
          polygons_.push_back(p);
        }

        bool obs_available;
        vsl_b_read(is, obs_available);
        if (obs_available) {
          for (unsigned i = 0; i<this->polygon_cnt_; i++) {
            bool ob_available;
            vsl_b_read(is, ob_available);
            if (ob_available) {
              dbinfo_observation_sptr o = new dbinfo_observation();
              o->b_read(is);
              observations_.push_back(o);
            } else {
              observations_.push_back(0);
            }
          }
        }

        bool ins_available;
        vsl_b_read(is, ins_available);
        if (ins_available) {
          for (unsigned i = 0; i<this->polygon_cnt_; i++) {
            bool in_available;
            vsl_b_read(is, in_available);
            if (in_available) {
              dbru_multiple_instance_object_sptr o = new dbru_multiple_instance_object();
              o->b_read(is);
              instances_.push_back(o);
            } else {
              instances_.push_back(0);
            }
          }
        }
        break;
      }
  }
}

bool read_objects_from_file(const char *filename, vcl_vector< dbru_object_sptr>& objects)
{
  vcl_ifstream dbfp(filename);
  if (!dbfp) {
    vcl_cout << "Problems in opening objects xml file!\n";
    return false;
  }

  vcl_cout << "reading objects...\n";
  
  char buffer[1000]; 
  dbfp.getline(buffer, 1000);  // comment 
  vcl_string dummy;
  dbfp >> dummy;   // <contour_segmentation   
  
  // there are two types of object files, with object_cnt at the beginning and no object_cnt at the beginning
  //consider both cases

  dbfp >> dummy; // object_cnt="23">

  vcl_string::size_type loc = dummy.find( "object_cnt", 0 );
  if (loc == vcl_string::npos) {
    loc = dummy.find( "</contour_segmentation>", 0 );
    int cnt = 0;
    while (loc == vcl_string::npos) {
      dbfp.putback(' ');
      dbfp.putback('t'); dbfp.putback('c'); dbfp.putback('e'); dbfp.putback('j'); dbfp.putback('b'); dbfp.putback('o');

      dbru_object_sptr obj = new dbru_object();
      if (!obj->read_xml(dbfp)) { 
        vcl_cout << "problems in reading database object number: " << cnt << vcl_endl;
        return false;
      }
      objects.push_back(obj);

      dbfp >> dummy; // if there's another object then <object otherwise </contour_segmentation>
      loc = dummy.find( "</contour_segmentation>", 0 );
      cnt++;
    }
    
  } else {
    unsigned int size;
    sscanf(dummy.c_str(), "object_cnt=\"%d\">", &size); 
 
    // read and extract all objects in this file, if they can be located in poly file and a multiple instance exists then add to OSL
    for (unsigned i = 0; i<size; i++) {
      vcl_cout << "reading database object: " << i << "...\n";
      dbru_object_sptr obj = new dbru_object();
      if (!obj->read_xml(dbfp)) { 
        vcl_cout << "problems in reading database object number: " << i << vcl_endl;
        return false;
      }

      objects.push_back(obj);
    }
  }
  dbfp.close();
  return true;
}

int read_poly_file(vcl_string const& poly_file, vcl_vector<vcl_vector< vsol_polygon_2d_sptr > >& frame_polys)
{
  vcl_ifstream fs(poly_file.c_str());
  
  if (!fs) {
    vcl_cout << "Problems in opening file: " << poly_file << "\n";
    return -1;
  }

  vcl_string dummy;
  int video_id;
  fs >> dummy; // VIDEOID:
  if (dummy != "VIDEOID:" && dummy != "FILEID:" && dummy != "VIDEOFILEID:") {
    vcl_cout << "No video id specified in input file!\n";
    return -1;
  } else {
    fs >> video_id;
    fs >> dummy;   // NFRAMES:
  }
  
  int frame_cnt; 
  fs >> frame_cnt;
  
  //: initialize polygon vector
  for (int i = 0; i<frame_cnt; i++) {
    vcl_vector<vsol_polygon_2d_sptr> tmp;
    frame_polys.push_back(tmp);
  }

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

      vcl_vector< vsol_point_2d_sptr > points;
      for (int k = 0; k<vertex_cnt; k++) {
        vsol_point_2d_sptr p = new vsol_point_2d(x_corners[k], y_corners[k]);
        points.push_back(p);
      }
      vsol_polygon_2d_sptr poly = new vsol_polygon_2d(points);
      frame_polys[i].push_back(poly);
    }
  }
  
  fs.close();
  return video_id;
}

void read_ins_file(const char *filename, vcl_vector< vcl_vector<dbru_multiple_instance_object_sptr> >& frames)
{
  vsl_b_ifstream ifile(filename);
  unsigned numframes=0;
  vsl_b_read(ifile,numframes);
  for(unsigned i=0;i<numframes;i++)
  {
      vcl_vector<dbru_multiple_instance_object_sptr> temp;
      vsl_b_read(ifile,temp);
      vcl_cout << "frame: " << i << " size: " << temp.size() << vcl_endl;
      frames.push_back(temp);
  }
}


#include <vcl_vector.h>
#include <vcl_algorithm.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <vcl_cassert.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbru/dbru_osl.h>
#include <dbru/dbru_label.h>
#include <dbru/dbru_object.h>

dbru_osl::dbru_osl() 
{
}
/*
dbru_osl::dbru_osl(vcl_vector<vcl_string> const& classes,
                       vcl_vector<vcl_vector<dbinfo_observation_sptr> > const& prototypes)
{
  //osl_ = new vcl_vector<dbru_object_sptr>();
  this->add_prototypes(classes, prototypes);
}*/

dbru_osl::~dbru_osl()
{
  for (vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator
         oit = osl_class_map_.begin(); oit != osl_class_map_.end(); oit++)
    delete (*oit).second;
  
  osl_class_map_.clear();
  //delete osl_class_map_;
  
  //osl_->clear();
  osl_.clear();
  
  //delete osl_;
}

//: copy constructor
dbru_osl::dbru_osl(const dbru_osl& rhs)
{
  this->add_objects(rhs.osl_);
}

void dbru_osl::clear()
{
  osl_class_map_.clear();
  //osl_->clear();
  osl_.clear();
  //delete osl_;
  //osl_ = new vcl_vector<dbru_object_sptr>();
  category_id_map_.clear();
}

/*bool dbru_osl::
find_prototype_vector(vcl_string const& cls, 
                      vcl_vector<dbinfo_observation_sptr>*& prototypes )
{
  vcl_map<vcl_string, vcl_vector<dbinfo_observation_sptr>* >::iterator osli;
  osli = osl_.find(cls);
  if (!(osli==osl_.end()))
    {
      vcl_vector<dbinfo_observation_sptr>* protos = (*osli).second;
      if(!protos)
        return false;
   prototypes = protos;
      return true;
    }
  return false;
}*/
   /*       
void dbru_osl::add_prototype(vcl_string const& cls,
                               dbinfo_observation_sptr const& proto)
{
  dbru_label_sptr lbl = new dbru_label(cls);
  dbru_object_sptr obj = new dbru_object();
  obj->set_category(cls);
  if (proto->geometry()->n_polys() == 1) {
    vsol_polygon_2d_sptr poly = proto->geometry()->poly(0);
    obj->add_polygon(poly, lbl);
  } else
    vcl_cout << "WARNING dbru_osl::add_prototype() - number of polygons in the prototype is not 1, polygon is not added to the object\n";
  obj->add_observation(proto);
  
  this->add_object(obj);
}

void dbru_osl::add_prototype(dbru_label_sptr lbl,
                             dbinfo_observation_sptr const& proto)
{
  dbru_object_sptr obj = new dbru_object();
  obj->set_category(lbl->category());
  if (proto->geometry()->n_polys() == 1) {
    vsol_polygon_2d_sptr poly = proto->geometry()->poly(0);
    obj->add_polygon(poly, lbl);
  } else
    vcl_cout << "WARNING dbru_osl::add_prototype() - number of polygons in the prototype is not 1, polygon is not added to the object\n";
  obj->add_observation(proto);
  
  this->add_object(obj);
}

void dbru_osl::
add_prototypes(vcl_vector<vcl_string> const& classes,
               vcl_vector<vcl_vector<dbinfo_observation_sptr> > const& prototypes)
{
  vcl_vector<vcl_string>::const_iterator cit = classes.begin();
  for(vcl_vector<vcl_vector<dbinfo_observation_sptr> >::const_iterator pit = prototypes.begin(); pit != prototypes.end(); ++pit, ++cit)
    for(vcl_vector<dbinfo_observation_sptr>::const_iterator oit = (*pit).begin(); oit != (*pit).end(); ++oit)
      this->add_prototype(*cit, *oit);
}
*/
void dbru_osl::
add_object(dbru_object_sptr const& obj)
{
  //osl_->push_back(obj);
  osl_.push_back(obj);
  //unsigned obj_id = osl_->size()-1;
  unsigned obj_id = osl_.size()-1;

  vcl_string cls = obj->category();
  vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator iter;
  iter = osl_class_map_.find(cls);
  vcl_vector<vcl_pair<unsigned, unsigned> > *vec;
  if (iter == osl_class_map_.end()) {
    vec = new vcl_vector<vcl_pair<unsigned, unsigned> >();
    osl_class_map_[cls] = vec;
  } else 
    vec = iter->second;
  
  for (unsigned i = 0; i<obj->n_observations(); i++)
    vec->push_back(vcl_pair<unsigned, unsigned>(obj_id, i));
}

void dbru_osl::
add_objects(vcl_vector<dbru_object_sptr> const& objects)
{
  for (unsigned i = 0; i<objects.size(); i++)
    this->add_object(objects[i]);

  fill_category_id_map();
}

//: get a particular object
dbru_object_sptr dbru_osl::get_object(unsigned i) {
  if (i<osl_.size())
    return osl_[i];
  return 0;
}

dbru_object_sptr dbru_osl::operator[] (unsigned int i)
{ 
  return get_object(i); 
}

  //: get a particular polygon of a particular object in osl
vsol_polygon_2d_sptr dbru_osl::get_polygon(unsigned i, unsigned j) {
  if (i < osl_.size()) {
    dbru_object_sptr obj = osl_[i];
    return obj->get_polygon(j);
  } else 
    return false;
}

//: get a particular prototype of a particular object in osl
dbinfo_observation_sptr dbru_osl::get_prototype(unsigned i, unsigned j) {
  if (i < osl_.size()) {
    dbru_object_sptr obj = osl_[i];
    return obj->get_observation(j);
  } else 
    return 0;
}

  //: make the observation and polygon pointers null
bool dbru_osl::null_prototype(unsigned i, unsigned j) {
  if (i < osl_.size()) {
    dbru_object_sptr obj = osl_[i];
    return obj->null_observation(j);
  } else 
    return false;
}


//: get a particular label of a particular object in osl
dbru_label_sptr dbru_osl::get_label(unsigned i, unsigned j) {
  dbru_object_sptr obj = get_object(i);
  return obj->get_label(j);
}



bool dbru_osl::prototypes(vcl_string const& cls, 
                            vcl_vector<dbinfo_observation_sptr>& prototypes)
{
  vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator iter;
  iter = osl_class_map_.find(cls);
  vcl_vector<vcl_pair<unsigned, unsigned> > *vec;
  if (iter == osl_class_map_.end()) {
    return false;
  } else 
    vec = iter->second;

  for(vcl_vector<vcl_pair<unsigned, unsigned> >::iterator pit = vec->begin(); pit != vec->end(); ++pit) 
  {
    unsigned obj_id = (*pit).first;
    unsigned proto_id = (*pit).second;
    dbinfo_observation_sptr obs = osl_[obj_id]->get_observation(proto_id);
    prototypes.push_back(obs);
  }
  return true;
}
/*
bool dbru_osl::remove_prototype(vcl_string const& cls,
                                  vcl_string const& obs_doc)
{
  vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator iter;
  iter = osl_class_map_.find(cls);
  vcl_vector<vcl_pair<unsigned, unsigned> > *vec;
  if (iter == osl_class_map_.end()) {
    return false;
  } else 
    vec = iter->second;

  for(vcl_vector<vcl_pair<unsigned, unsigned> >::iterator pit = vec->begin(); pit != vec->end(); ++pit) 
  {
    unsigned obj_id = (*pit).first;
    unsigned proto_id = (*pit).second;
    dbinfo_observation_sptr obs = osl_[obj_id]->get_observation(proto_id);
    if (obs->doc() == obs_doc) {
      //: the following method also removes the polygons and labels based on polygon_per_frame cnt which is usually 1
      osl_[obj_id]->remove_observation(proto_id); 
      vec->erase(vec->end()-1);
      return true;
    }
  }

  return false;
}*/

bool dbru_osl::remove_class(vcl_string const& cls)
{
  bool done = false;
  while (!done) {
    done = true;
    for (vcl_vector<dbru_object_sptr>::iterator iter = osl_.begin(); iter != osl_.end(); iter++)
      if ((*iter)->category() == cls) {
        osl_.erase(iter);
        //osl_->erase(iter);
        done = false;
        break;
      }
  }

  vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator iterm;
  iterm = osl_class_map_.find(cls);
  if (iterm != osl_class_map_.end()) 
    osl_class_map_.erase(iterm);

  return true;
}

//: remove a particular object
bool dbru_osl::remove_object(unsigned i)
{
  if (i < osl_.size()) {
    dbru_object_sptr obj = osl_[i];
    vcl_string cls = obj->category();
    vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::iterator iter;
    iter = osl_class_map_.find(cls);
    vcl_vector<vcl_pair<unsigned, unsigned> > *vec;
    if (iter == osl_class_map_.end()) {
      vcl_cout << "Warning: class " << cls << " is not found\n";
    } else {
      vec = iter->second;
      unsigned nn = 0;
      for (unsigned k = 0; k<i; k++) {
        if (osl_[k]->category() == cls)
          nn += osl_[k]->n_observations();
      }
      vcl_vector<vcl_pair<unsigned, unsigned> >::iterator viter_b = vec->begin()+nn;
      vcl_vector<vcl_pair<unsigned, unsigned> >::iterator viter_e = viter_b+obj->n_observations()-1;
      vec->erase(viter_b, viter_e);
    }  
    
    vcl_vector<dbru_object_sptr>::iterator itero = osl_.begin();
    osl_.erase(itero+i);
    return true;
  } else
    return false;
}

vcl_vector<vcl_string> dbru_osl::classes() const
{
  vcl_vector<vcl_string> clss;
  for (vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::const_iterator
         oit = osl_class_map_.begin(); oit != osl_class_map_.end(); oit++)
    clss.push_back((*oit).first);
  return clss;
}
/*
dbinfo_observation_sptr dbru_osl::prototype(vcl_string const& cls, const unsigned index)
{
  vcl_vector<dbinfo_observation_sptr>* protos = 0;
  if(!find_prototype_vector(cls, protos))
    return 0;
  if(protos&&index<protos->size())
    return (*protos)[index];
  return 0;
}*/

//: the number of prototypes per class
unsigned dbru_osl::n_protos_in_class(vcl_string const& cls) const
{
  vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::const_iterator iter;
  iter = osl_class_map_.find(cls);
  vcl_vector<vcl_pair<unsigned, unsigned> > *vec;
  if (iter == osl_class_map_.end()) {
    return 0;
  } else 
    vec = iter->second;

  return vec->size();
}

//: the vector of number of class prototypes by enumerated by class
vcl_vector<unsigned> dbru_osl::n_prototypes() const
{
  vcl_vector<unsigned> temp;

  for (vcl_map<vcl_string, vcl_vector<vcl_pair<unsigned, unsigned> >* >::const_iterator
         oit = osl_class_map_.begin(); oit != osl_class_map_.end(); oit++)
    if((*oit).second)
      temp.push_back((*oit).second->size());
    else
      temp.push_back(0);
  
  return temp;
}

bool dbru_osl::fill_category_id_map(void) {
  
  // traverse each object once
  if (!osl_.size()) return false;
  category_id_map_.clear();
  int cat_cnt = 0;
  
  for (unsigned i = 0; i < osl_.size(); i++) {
    dbru_object_sptr obj = osl_[i];
    
    vcl_map<vcl_string, int>::iterator iter = category_id_map_.find(obj->category_);
    if (iter == category_id_map_.end()) {
      category_id_map_[obj->category_] = cat_cnt;
      cat_cnt++;
    }

  }

  vcl_cout << "categories in the OSL are found to be:\n";
  for (vcl_map<vcl_string, int>::iterator iter = category_id_map_.begin(); iter != category_id_map_.end(); iter++) {
    vcl_cout << "\t" << iter->first << " id: " << iter->second << "\n";
  }

  return true;

}

void dbru_osl::print(vcl_ostream& os) const
{
  vcl_vector<vcl_string> clss = this->classes();
  unsigned nclasses = clss.size();
  os << this->is_a() << " [\n"
     << "n classes " << nclasses << '\n';
  for (unsigned i = 0; i<nclasses; ++i)
    vcl_cout << clss[i] << ' ';  
  os << " n objects " << this->n_objects();
  os << "\n ]\n";
}

//: Binary save self to stream.
void dbru_osl::b_write(vsl_b_ostream &os) const
{
  //dbru_osl& osl = (dbru_osl&)*this;  
  vsl_b_write(os, version());
  vsl_b_write(os, osl_);
}

//: Binary load self from stream.
void dbru_osl::b_read(vsl_b_istream &is)
{
  //this->clear();
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        vcl_vector<dbru_object_sptr> osl;
        vsl_b_read(is, osl);
        for (unsigned i = 0; i<osl.size(); i++)
          this->add_object(osl[i]);
      }
    }

  fill_category_id_map();
}



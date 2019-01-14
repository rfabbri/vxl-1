#include <vector>
#include <algorithm>
#include <vbl/vbl_ref_count.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <cassert>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_osl.h>

dbinfo_osl::dbinfo_osl()
{
}

dbinfo_osl::dbinfo_osl(std::vector<std::string> const& classes,
                       std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes)
{
  this->add_prototypes(classes, prototypes);
}

dbinfo_osl::~dbinfo_osl()
{
  for (std::map<std::string, std::vector<dbinfo_observation_sptr>* >::iterator
         oit = osl_.begin(); oit != osl_.end(); oit++)
    delete (*oit).second;
}

bool dbinfo_osl::
find_prototype_vector(std::string const& cls, 
                      std::vector<dbinfo_observation_sptr>*& prototypes )
{
  std::map<std::string, std::vector<dbinfo_observation_sptr>* >::iterator osli;
  osli = osl_.find(cls);
  if (!(osli==osl_.end()))
    {
      std::vector<dbinfo_observation_sptr>* protos = (*osli).second;
      if(!protos)
        return false;
   prototypes = protos;
      return true;
    }
  return false;
}
          
void dbinfo_osl::add_prototype(std::string const& cls,
                               dbinfo_observation_sptr const& proto)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  if(find_prototype_vector(cls, protos))
    {
      protos->push_back(proto);
      return;
    }
  //this call is the first time cls has been seen so initiate the 
  //class entry in the osl map
  protos = new std::vector<dbinfo_observation_sptr>;
  protos->push_back(proto);
  osl_[cls]=protos;
}

void dbinfo_osl::
add_prototypes(std::vector<std::string> const& classes,
               std::vector<std::vector<dbinfo_observation_sptr> > const& prototypes)
{
  std::vector<std::string>::const_iterator cit = classes.begin();
  for(std::vector<std::vector<dbinfo_observation_sptr> >::const_iterator pit = prototypes.begin(); pit != prototypes.end(); ++pit, ++cit)
    for(std::vector<dbinfo_observation_sptr>::const_iterator oit = (*pit).begin(); oit != (*pit).end(); ++oit)
      this->add_prototype(*cit, *oit);
}

bool dbinfo_osl::prototypes(std::string const& cls, 
                            std::vector<dbinfo_observation_sptr>& prototypes)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  if(!this->find_prototype_vector(cls, protos))
    return false;
  for(std::vector<dbinfo_observation_sptr>::iterator pit = protos->begin();
          pit != protos->end(); ++pit)
        prototypes.push_back(*pit);
  return true;
}

bool dbinfo_osl::remove_prototype(std::string const& cls,
                                  std::string const& obs_doc)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  if(!this->find_prototype_vector(cls, protos))
    return false;
  if(protos->size()==0)
    return false;
  for(std::vector<dbinfo_observation_sptr>::iterator oit = protos->begin();
      oit != protos->end(); ++oit)
    if((*oit)->doc()==obs_doc)
      {
        protos->erase(oit);
        return true;
      }
  return false;
}

bool dbinfo_osl::remove_class(std::string const& cls)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  std::map<std::string, std::vector<dbinfo_observation_sptr>* >::iterator osli;
  osli = osl_.find(cls);
  if (!(osli==osl_.end()))
    {
      protos = (*osli).second;
      protos->clear();
      osl_.erase(osli);
      delete protos;
      return true;
    }
  return false;
}

std::vector<std::string> dbinfo_osl::classes() const
{
  std::vector<std::string> clss;
  for (std::map<std::string, std::vector<dbinfo_observation_sptr>* >::const_iterator
         oit = osl_.begin(); oit != osl_.end(); oit++)
    clss.push_back((*oit).first);
  return clss;
}

dbinfo_observation_sptr dbinfo_osl::prototype(std::string const& cls, const unsigned index)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  if(!find_prototype_vector(cls, protos))
    return 0;
  if(protos&&index<protos->size())
    return (*protos)[index];
  return 0;
}

//: the number of prototypes per class
unsigned dbinfo_osl::n_protos_in_class(std::string const& cls)
{
  std::vector<dbinfo_observation_sptr>* protos = 0;
  if(find_prototype_vector(cls, protos))
    if(protos)
      return protos->size();
  return 0;
}

//: the vector of number of class prototypes by enumerated by class
std::vector<unsigned> dbinfo_osl::n_prototypes()
{
  std::vector<unsigned> temp;
  std::vector<std::string> clss;
  for (std::map<std::string, std::vector<dbinfo_observation_sptr>* >::const_iterator
         oit = osl_.begin(); oit != osl_.end(); oit++)
    if((*oit).second)
      temp.push_back((*oit).second->size());
    else
      temp.push_back(0);
  return temp;
}

void dbinfo_osl::print(std::ostream& os) const
{
  std::vector<std::string> clss = this->classes();
  unsigned nclasses = clss.size();
  os << this->is_a() << " [\n"
     << "n classes " << nclasses << '\n';
  for (unsigned i = 0; i<nclasses; ++i)
    std::cout << clss[i] << ' ';  
  os << "\n ]\n";
}

//: Binary save self to stream.
void dbinfo_osl::b_write(vsl_b_ostream &os) const
{
  dbinfo_osl& osl = (dbinfo_osl&)*this;  
  vsl_b_write(os, version());
  std::vector<std::string> clss = osl.classes();
  vsl_b_write(os, clss);
  for (std::map<std::string, std::vector<dbinfo_observation_sptr>* >::const_iterator
         oit = osl_.begin(); oit != osl_.end(); oit++)
     vsl_b_write(os, *((*oit).second));
}

//: Binary load self from stream.
void dbinfo_osl::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
        std::vector<std::string> clss;
        vsl_b_read(is, clss);
        std::vector<std::vector<dbinfo_observation_sptr> > prototypes;        
        for(unsigned i = 0; i<clss.size(); ++i)
          {
            std::vector<dbinfo_observation_sptr> protos;
            vsl_b_read(is, protos);
            prototypes.push_back(protos);
          }
        this->add_prototypes(clss, prototypes);
      }
    }
}



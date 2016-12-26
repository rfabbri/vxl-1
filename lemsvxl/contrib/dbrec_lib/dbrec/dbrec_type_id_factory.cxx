//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//


#include "dbrec_type_id_factory.h"

dbrec_type_id_factory* dbrec_type_id_factory::instance_ = 0;
dbrec_type_id_factory_destroyer dbrec_type_id_factory::destroyer_; // definiton and hence instance creation

dbrec_type_id_factory* dbrec_type_id_factory::instance() {
  if (instance_ == 0) {
    instance_ = new dbrec_type_id_factory;
    destroyer_.set_singleton(instance_);
  }
  return instance_;
}

//: it simply makes sure id will already be used up
void dbrec_type_id_factory::register_type(unsigned id) 
{
  if (current_ < id)
    current_ = id;
}

unsigned dbrec_type_id_factory::new_type() 
{
  current_++;
  return current_;
}

dbrec_type_id_factory_destroyer::dbrec_type_id_factory_destroyer(dbrec_type_id_factory* s) 
{
  s_ = s;
}
//: the destructor deletes the instance
dbrec_type_id_factory_destroyer::~dbrec_type_id_factory_destroyer()
{
  if (s_ != 0)
    delete s_;
}

void dbrec_type_id_factory_destroyer::set_singleton(dbrec_type_id_factory *s) 
{
  s_ = s;
}


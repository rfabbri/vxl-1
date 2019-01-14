//:
// \file
// \brief factory interface to produce primitive parts, 
//        concrete factory classes inherits from the base to produce different kinds of primitive parts, eg. gaussian extremas
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 22, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_DBREC_PRIMITIVE_FACTORY_H)
#define _DBREC_PRIMITIVE_FACTORY_H

#include "dbrec_part_sptr.h"
#include <map>
#include <vbl/vbl_ref_count.h>
#include <iostream>

class dbrec_primitive_factory : public vbl_ref_count {

public:
  typedef std::map<unsigned, dbrec_part_sptr>::iterator part_iterator;
  typedef std::map<unsigned, dbrec_part_sptr>::const_iterator part_const_iterator;

  dbrec_part_sptr get_primitive(unsigned type);
  void add_primitive(dbrec_part_sptr p);

  //: create a hiearchy where each class composition node has all the primitives as children (random classifier)
  dbrec_hierarchy_sptr construct_random_classifier(int nclasses);

  part_const_iterator parts_const_begin() const { return flyweights_.begin(); }
  part_const_iterator parts_const_end() const { return flyweights_.end(); }
  part_iterator parts_begin() { return flyweights_.begin(); }
  part_iterator parts_end() { return flyweights_.end(); }
  virtual std::ostream& print(std::ostream& out) const;

protected:
  std::map<unsigned, dbrec_part_sptr> flyweights_;

};

std::ostream & operator<<(std::ostream& out, const dbrec_primitive_factory& p);

#endif  //_DBREC_PRIMITIVE_FACTORY_H

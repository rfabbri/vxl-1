// This is brcv/shp/dbsk2d/dbsk2d_ishock_bnd_key.cxx

//:
// \file

#include <dbsk2d/dbsk2d_ishock_bnd_key.h>

bool dbsk2d_ishock_bnd_key::operator < (const dbsk2d_ishock_bnd_key& other) const
{
  //select fuzzy type depending on the eta type
  if (this->ftype==ANGLE){
    //numerical ordering
    if (AisL(s_eta, other.s_eta)){   
      return true;
    }
  }
  else {
    if (LisL(s_eta, other.s_eta)){ 
      return true;
    }
  }

  //are computed together (from sources)
  //if (eta1==other.eta1)     //topological ordering
  if (AisEq(s_eta, other.s_eta)){ 
    if (type==LEFTCONTACT)    //always at the rightmost end
      return false;
    else if (type==RIGHTCONTACT) //always at the leftmost end
      return true;
    else if (type==LEFT)
      if (other.type==LEFTCONTACT ||
          other.type==RIGHT)
        return true;
      else
        return false;
    else if (type==RIGHT && other.type==LEFTCONTACT)
      return true;
    else                        
      return false;
  }

  return false;
}

//: return a string describing the shock type
vcl_string dbsk2d_ishock_bnd_key::type_string() const
{
  switch(type)
  {
    case LEFT:
      return "L";
    case RIGHT:
      return "R";
    case LEFTCONTACT:
      return "LC";
    case RIGHTCONTACT:
      return "RC";
    default:
      return "N/A";
  }
}


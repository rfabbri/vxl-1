#include "bnld_eno_zerox_label.h"
#include <stdio.h>

#if !VCL_STATIC_CONST_INIT_INT_NO_DEFN
const unsigned bnld_eno_zerox_label::unlabeled
      VCL_STATIC_CONST_INIT_INT_DEFN((unsigned)-1);

const unsigned bnld_eno_zerox_label::maxzeros
      VCL_STATIC_CONST_INIT_INT_DEFN(4);
#endif

bnld_eno_zerox_label::
  bnld_eno_zerox_label(
      const double *data,
      const unsigned *label,
      const bnld_eno_zerox_vector &zc)
  :
  label_(4*zc.size(),unlabeled)
{

  for (unsigned i=0; i<zc.size(); ++i) {

    if (zc[i].number()) {
      if (zc[i].number() == 1)
        label_one_zerox_in_interval(i,zc[i],data,label);
      else if (zc[i].number() == 2)
        label_two_zerox_in_interval(i,zc[i],data,label);
      else
        std::cerr << "This should not be happening: No. of Zerox=" 
          << zc[i].number() << std::endl;
    }
  }
}

void bnld_eno_zerox_label::
label_one_zerox_in_interval(
    unsigned pos,
    const bnld_eno_zero_crossing &, 
    const double *data, 
    const unsigned *data_label)
{
  double start,end;

  start=data[pos];
  end=data[pos+1];

  if (start > 0 && end > 0){
    printf("Both end points can't be +ve with one zerox\n");
    printf("Loc=%d; start=%g; end=%g",pos, start,end);
    label_[maxzeros*pos]=bnld_eno_zerox_label::unlabeled;
  } 
  else if (start > 0)
    label_[maxzeros*pos]=data_label[pos];
  else if (end > 0)
    label_[maxzeros*pos]=data_label[pos+1];
  else {
    printf("Both end points can't be -ve with one zerox\n");
    printf("Loc=%d; start=%g; end=%g",pos, start,end);
    label_[maxzeros*pos] =bnld_eno_zerox_label::unlabeled;
  }
}

void
bnld_eno_zerox_label::
label_two_zerox_in_interval(
    unsigned pos,
    const bnld_eno_zero_crossing &zc_i, 
    const double *data, 
    const unsigned *data_label)
{
  double start,end; 

  start = data[pos];
  end   = data[pos+1];

  /*If both end points are +ve  label the zerox based on the closest
   end point*/
  if (start >= 0 && end >= 0){
    if (zc_i.location(0) < zc_i.location(1)){
      label_[maxzeros*pos]   = data_label[pos];
      label_[maxzeros*pos+1] = data_label[pos+1];
    }
    else{
      std::cerr << "in label_two_zerox_in_interval:: hmm... zero crossings not sorted.." << std::endl;
      label_[maxzeros*pos+1] = data_label[pos];
      label_[maxzeros*pos]   = data_label[pos+1];
    }
  }
  else if (start < 0 || end < 0){
    if (start > 0 || end > 0)
      std::cerr << 
        "label_two_zerox_in_interval: warning: we have two zero xings but function changes sign" 
        << std::endl << "Start: " << start << " End: "
        << end << " Pos: " << pos << std::endl;
    /*If end points are -ve cannot label the zerox*/
    label_[maxzeros*pos]=bnld_eno_zerox_label::unlabeled;
    label_[maxzeros*pos+1]=bnld_eno_zerox_label::unlabeled;
  }
}

//: print alongside bnld_eno_1d and zerox
void bnld_eno_zerox_label::
print(bnld_eno_1d &eno, bnld_eno_zerox_vector &zc, std::ostream& strm) const
{
   strm << "==== Eno 1D with Labeled Zerocrossings ====" << std::endl
        << "len: " << eno.size() << std::endl;

   unsigned i;
   for (i=0; i < eno.size(); ++i) {
      eno[i].print(strm);
      zc[i].print(strm);
      std::cout << "Labels: ";

      for (unsigned k=0; k < zc[i].number(); ++k) {
        std::cout << LABEL_(i,k) << " ";
      }
      std::cout << std::endl << std::endl << std::endl;
   }
}

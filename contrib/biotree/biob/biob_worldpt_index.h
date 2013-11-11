#if !defined(BIOB_WORLDPT_INDEX_H_)
#define BIOB_WORLDPT_INDEX_H_

#include <vsl/vsl_binary_io.h>
#include <vcl_iostream.h>

class biob_worldpt_index {
  //  private:
  public:
    unsigned long int index_;

    biob_worldpt_index(): index_(0) {}
    biob_worldpt_index(int index): index_(index){}
    long unsigned int index() const {return index_;}
    bool operator<(biob_worldpt_index other) const{
      return index_ < other.index_;
    }
    bool operator==(biob_worldpt_index other) const{
      return index_ == other.index_;
    }
};

void vsl_b_write(vsl_b_ostream & os, const biob_worldpt_index & pti);
void vsl_print_summary(vcl_ostream & os, const biob_worldpt_index & pti);
void vsl_b_read(vsl_b_istream & is, biob_worldpt_index& pti);

#endif

// This is basic/dbvidl2/pro/dbvidl2_sink.h
#ifndef dbvidl2_sink_h_
#define dbvidl2_sink_h_

//:
// \file
// \brief A video stream sink
// \author Matt Leotta
// \date 6/1/06
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_string.h>
#include <bprod/bprod_process.h>
#include <vidl/vidl_ostream_sptr.h>

//: Convert a vidl_ostream into a bprod_sink
class dbvidl2_sink : public bprod_sink
{
 public:

  //: Constructor
  dbvidl2_sink(const vidl_ostream_sptr& o) : ostream_(o) {}

  //: Destructor
  virtual ~dbvidl2_sink(){}

  //: Set the ostream
  void set_stream(const vidl_ostream_sptr& o) { ostream_ = o; }

  bool enabled() const { return (ostream_ != NULL); }


  //: Execute this process
  bprod_signal execute();

 private:
  vidl_ostream_sptr ostream_;
};

#endif // dbvidl2_sink_h_

// This is basic/bprod/xio/bprod_xio_sink.h
#ifndef bprod_xio_sink_h_
#define bprod_xio_sink_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief bprod process for streaming XML output
//
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/11/06
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <bprod/bprod_process.h>
#include <string>
#include <fstream>



//: A source for streaming XML to a file
class bprod_xio_sink : public bprod_sink
{
  public:
    //: Constructor
    bprod_xio_sink();

    //: Constructor
    bprod_xio_sink(const std::string& filename);

    //: Destructor
    ~bprod_xio_sink();

    //: Open a new file
    bool open(const std::string& filename);

    //: Runs the process
    bprod_signal execute();

    //: Only execute if the file stream is valid
    // the const_cast here is a workaround for a bug
    // in some versions of gcc that mistakenly define is_open as non-const
    bool enabled() const { return const_cast<std::ofstream&>(os_).is_open(); }

  protected:
    void write_header();
    void write_footer();

  private:
    std::ofstream os_;
};



#endif // bprod_xio_sink_h_

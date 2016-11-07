#if !defined(SLICE_ENGINE_H_)
#define SLICE_ENGINE_H_ 

#include "sliceProcessor.h"
#include <vcl_vector.h>
#include <vcl_fstream.h>
#include <vcl_string.h>

template <class T> class sliceEngine{
  public:
    sliceEngine(vcl_vector<vcl_ifstream*> streams) 
      :save_to_mem_(0),save_to_disk_(0)
      { streamlist_ = streams;
      memlocs_.clear();
      outnames_.clear();

      }
    sliceEngine()
      :save_to_mem_(0),save_to_disk_(0)
      {
      memlocs_.clear();
      outnames_.clear();
      }

    void setStreamlist(vcl_vector<vcl_string> files) {innames_ = files;}
    void addStream(vcl_string f){innames_.push_back(f);}

    inline void clearStreams();
    int nstreams() { return innames_.size();}
    void setSaveToMemory(vcl_vector<T*> memlocs)
    { save_to_mem_ = 1; save_to_disk_ = 0; memlocs_ = memlocs; outnames_.clear();}
    void setSaveToDisk(vcl_vector<vcl_string> fnames)
    { save_to_mem_ = 0; save_to_disk_ = 1; outnames_ = fnames; memlocs_.clear();}

    bool processWith(sliceProcessor<T>* proc);
    
  protected:
  private:
    bool sanity_check(sliceProcessor<T>* proc, int& dimx, int& dimy, int& dimz);
    void cleanup();
    bool save_to_mem_;
    bool save_to_disk_;
    vcl_vector<T*> memlocs_;
    vcl_vector<vcl_string> outnames_;
    vcl_vector<vcl_string> innames_;
    vcl_vector<vcl_ifstream*> streamlist_;
};
#endif 

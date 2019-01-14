#if !defined(SLICE_ENGINE_H_)
#define SLICE_ENGINE_H_ 

#include "sliceProcessor.h"
#include <vector>
#include <fstream>
#include <string>

template <class T> class sliceEngine{
  public:
    sliceEngine(std::vector<std::ifstream*> streams) 
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

    void setStreamlist(std::vector<std::string> files) {innames_ = files;}
    void addStream(std::string f){innames_.push_back(f);}

    inline void clearStreams();
    int nstreams() { return innames_.size();}
    void setSaveToMemory(std::vector<T*> memlocs)
    { save_to_mem_ = 1; save_to_disk_ = 0; memlocs_ = memlocs; outnames_.clear();}
    void setSaveToDisk(std::vector<std::string> fnames)
    { save_to_mem_ = 0; save_to_disk_ = 1; outnames_ = fnames; memlocs_.clear();}

    bool processWith(sliceProcessor<T>* proc);
    
  protected:
  private:
    bool sanity_check(sliceProcessor<T>* proc, int& dimx, int& dimy, int& dimz);
    void cleanup();
    bool save_to_mem_;
    bool save_to_disk_;
    std::vector<T*> memlocs_;
    std::vector<std::string> outnames_;
    std::vector<std::string> innames_;
    std::vector<std::ifstream*> streamlist_;
};
#endif 

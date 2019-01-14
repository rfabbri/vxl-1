#if !defined(SLICEENGINE_TXX_)
#define SLICEENGINE_TXX_
#include "sliceEngine.h"
#include "sliceProcessor.h"
#include "sliceFileManager.h"
#include <string>
#include <cstring>

template <class T>
void sliceEngine<T>::clearStreams(){ 
  for(int stream =0; stream < this->nstreams(); stream++){
    sliceFileManager<T>::closeSliceFile(streamlist_[stream]);
  }
  streamlist_.clear();
}
template <class T>
bool sliceEngine<T>::sanity_check(sliceProcessor<T>* proc, int& dimx, int& dimy, int& dimz)
{
  if(!save_to_mem_ && !save_to_disk_){
    std::cerr << "Error: must call sliceEngine::setSaveToMemory or sliceEngine::setSaveToDisk before processing";
    return false;
  }

  if(save_to_mem_ && proc->noutputs() != memlocs_.size()){
    std::cerr << "Error:  # of memory locations (currently " << memlocs_.size() 
      << " must match # of outputs for for proc: " << proc->name() 
      << ", which is : " << proc->noutputs() << "\n";
    return false;
  }

  if(save_to_disk_ && proc->noutputs() != outnames_.size()){
    std::cerr << "Error:  # of file output names  (currently " << outnames_.size() 
      << " must match # of outputs for for proc: " << proc->name() 
      << ", which is : " << proc->noutputs() << "\n";
    return false;
  }

  if(!(proc->nslices() % 2)){
    std::cerr << "Error:  nslices (currently " << proc->nslices() << " must be odd for proc: " << proc->name() << "\n";
    return false;
  }

  if(this->nstreams() != proc->nstreams()){
    std::cerr << "Error: nstreams must equal proc->nstreams() for proc: " << proc->name() << "\n";
    return false;
  }
  if(this->nstreams() > 0){
    int nextdimx=0; int nextdimy=0; int nextdimz=0;
    streamlist_.push_back(sliceFileManager<T>::openSliceFileForRead(innames_[0],dimx,dimy,dimz));
    for(int i = 1; i < this->nstreams(); i++){
      streamlist_.push_back(sliceFileManager<T>::openSliceFileForRead(innames_[i] ,nextdimx,nextdimy,nextdimz));
      if(nextdimx != dimx || nextdimy != dimy || nextdimz != dimz  )
      {
        std::cerr << "Error: All stream dimensions do not match\n";
        std::cerr << "First " << i << " streams have dimension ";
        std::cerr << dimx << " " << dimy << " " << dimz << "\n";
        std::cerr << "stream " << i+1 << " has dimension ";
        std::cerr << nextdimx << " " << nextdimy << " " << nextdimz << "\n";
        for(int opened = 0; opened < i; opened++) 
                sliceFileManager<T>::closeSliceFile(streamlist_[opened]);
        streamlist_.clear();
        return false;
      }
    }
    if(dimz < proc->nslices()){
    std::cerr << "Data has " << dimz << " slices, proc " << proc->name() << " requires " << proc->nslices() << " slices to process\n";
        return false;
    }
  }

    return true;
}

template <class T>
void sliceEngine<T>::cleanup(){
    //make sure that the user re-initializes the memory locations or filenames
    //if they want to process again
    save_to_mem_ = 0;
    save_to_disk_ = 0;
}


template <class T>
bool sliceEngine<T>::processWith(sliceProcessor<T>* proc)
{
  int dimx,dimy,dimz;
  if(this->sanity_check(proc,dimx,dimy,dimz)){
    int nslices = proc->nslices();
    int nstreams = this->nstreams();
    int noutputs = proc->noutputs();
    int margin = nslices/2;

    //each stream has a corresponding vector of slices (each slice is a T*)
    std::vector< std::vector<T*> > slice_sets(nstreams);

    //set up output memory locations / output file locations
    //------------------------------------------------------------------------
    std::vector<T*> save_offsets;
    std::vector<std::ofstream*> out_streams;
    if(save_to_mem_){
      save_offsets.resize(noutputs);
      for(int i = 0; i < noutputs; i++){
        save_offsets[i] = (T*)memlocs_[i];
      }
    }
    else{
      out_streams.resize(noutputs);
      for(int i = 0; i < noutputs; i++){
        out_streams[i] = sliceFileManager<T>::openSliceFileForWrite(outnames_[i],dimx,dimy,dimz);
      }
    }
    //------------------------------------------------------------------------

    //populate the first set of slices, with zero slices where no data exists
    //------------------------------------------------------------------------
    for(int stream = 0; stream < nstreams; stream++){
      slice_sets[stream].resize(proc->nslices());
      for(int offset = 0; offset < margin; offset++){
        slice_sets[stream][offset] = new T[dimx*dimy];
        std::memset(slice_sets[stream][offset],0,dimx*dimy*sizeof(T));
      }
      for(int offset = margin; offset < nslices; offset++){
        slice_sets[stream][offset] = new T[dimx*dimy];
        sliceFileManager<T>::readOneSlice(streamlist_[stream],slice_sets[stream][offset],dimx,dimy);
      }
    }
    //------------------------------------------------------------------------



    //process the volume, reading/discarding slices as you go, until the point
    //where more zero slices are needed
    //------------------------------------------------------------------------
    std::vector<T*> res; 
    for(int z = 0; z < dimz - margin - 1; z++){
      res = proc->process(slice_sets,dimx,dimy,z);

      if(save_to_mem_){
        for(int out = 0; out < noutputs; out++){
          std::memcpy(save_offsets[out],res[out],dimx*dimy*sizeof(T));
          save_offsets[out]+=dimx*dimy;
          delete [] res[out];
        }
      }
      else{
        for(int out = 0; out < noutputs; out++){
          sliceFileManager<T>::writeOneSlice(out_streams[out],res[out],dimx,dimy);
          delete [] res[out];
        }
      }

      //processing has been done on this slice. 
      //for each input stream, delete top slice, read in new bottom slice
      for(int stream = 0; stream < nstreams; stream++){
        delete [] slice_sets[stream][0];
        for(int i = 0; i < nslices-1; i++){
          slice_sets[stream][i] = slice_sets[stream][i+1];
        }
        slice_sets[stream][nslices-1] = new T[dimx*dimy];
        sliceFileManager<T>::readOneSlice(streamlist_[stream],slice_sets[stream][nslices-1],dimx,dimy);
      }
    }


    //process the remainder, discarding slices and replacing with zero slices as you go
    //------------------------------------------------------------------------
    for(int z = dimz-margin-1; z < dimz; z++){
      res = proc->process(slice_sets,dimx,dimy,z);

      if(save_to_mem_){
        for(int out = 0; out < noutputs; out++){
          std::memcpy(save_offsets[out],res[out],dimx*dimy*sizeof(T));
          save_offsets[out]+=dimx*dimy;
          delete [] res[out];
        }
      }
      else{
        for(int out = 0; out < noutputs; out++){
          sliceFileManager<T>::writeOneSlice(out_streams[out],res[out],dimx,dimy);
          delete [] res[out];
        }
      }

      //processing has been done on this slice. 
      //for each input stream, delete top slice, fill in a zero bottom slice
      for(int stream = 0; stream < nstreams; stream++){
        delete [] slice_sets[stream][0];
        for(int i = 0; i < nslices-1; i++){
          slice_sets[stream][i] = slice_sets[stream][i+1];
        }
        slice_sets[stream][nslices-1] = new T[dimx*dimy];
        std::memset(slice_sets[stream][nslices-1],0,dimx*dimy*sizeof(T));
      }
    } 
    //------------------------------------------------------------------------

    //when all the processing has been done, clean up the slice sets
    //------------------------------------------------------------------------
    for(int stream = 0; stream < nstreams; stream++){
      for(int slice = 0; slice < nslices; slice++){
        delete [] slice_sets[stream][slice];
      }
    }
    this->clearStreams();

    if(save_to_disk_){
      for(int out = 0; out < noutputs; out++){
          sliceFileManager<T>::closeSliceFile(out_streams[out]);
        }
    }
    //------------------------------------------------------------------------
  }
  else{
    return false;
  }

  this->cleanup();
  return true;
}
#define SLICEENGINE_INSTANTIATE(T)\
            template class sliceEngine<T>;

#endif

// This is basic/bprod/bprod_ios_processes.h
#ifndef bprod_ios_processes_h_
#define bprod_ios_processes_h_
//--------------------------------------------------------------------------------
//:
// \file
// \brief Processes that use vcl_iostreams as sources and sinks
//
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/22/08
//
// \verbatim
//  Modifications:
// \endverbatim
//--------------------------------------------------------------------------------


#include <bprod/bprod_process.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>


//=============================================================================
// Sources
//=============================================================================



//: Read data of type T from an istream
template <class T >
class bprod_istream_source : public bprod_source
{
 public:
  bprod_istream_source(const std::istream& istream) : is(istream) {}

  //: Execute the process
  bprod_signal execute()
  {
    if(is.eof()){
      return BPROD_EOS;
    }
    T data;
    is >> data;
    output(0, data);
    return BPROD_VALID;
  }
  std::istream is;
};


//: Read data from an ifstream (base class)
class bprod_ifstream_source_base : public bprod_source
{
  public:
    virtual bool open(const std::string& filename) = 0;
};


//: Read data of type T from an ifstream
template <class T >
class bprod_ifstream_source : public bprod_ifstream_source_base
{
  public:
    bprod_ifstream_source(const std::string& filename) { open(filename.c_str()); }
        
    bool open(const std::string& filename)
    {
      ifs.open(filename.c_str());
      return ifs.is_open();
    }
    
    //: Execute the process
    bprod_signal execute()
    {
      if(ifs.eof()){
        return BPROD_EOS;
      }
      T data;
      ifs >> data;
      output(0, data);
      return BPROD_VALID;
    }
    
    std::ifstream ifs;
};


//: Read data of type T using ifstream from a list of files
template <class T >
class bprod_ifstream_list_source : public bprod_ifstream_source_base
{
  public:
    bprod_ifstream_list_source(const std::string& glob) 
    : index(0) { open(glob); }
  
    bprod_ifstream_list_source(const std::vector<std::string>& files) 
    : filenames(files), index(0) {}
  
    bool open(const std::string& glob)
    {
      filenames.clear();
      index = 0;
      
      for (vul_file_iterator fit=glob; fit; ++fit) {
        // check to see if file is a directory.
        if (vul_file::is_directory(fit()))
          continue;
        filenames.push_back(fit());
      }
      
      // no matching filenames
      if (filenames.empty())
        return false;
      
      // Sort - because the file iterator uses readdir() it does not
      //        iterate over files in alphanumeric order
      std::sort(filenames.begin(),filenames.end());
      
      return true;
    }
    
    //: Execute the process
    bprod_signal execute()
    {
      if(index >= filenames.size()){
        return BPROD_EOS;
      }
      std::ifstream ifs(filenames[index++].c_str());
      if(!ifs.is_open()){
        return BPROD_INVALID;
      }
      T data;
      ifs >> data;
      ifs.close();
      
      output(0, data);
      return BPROD_VALID;
    }
  
    std::vector<std::string> filenames;
    unsigned int index;
};





//=============================================================================
// Sinks
//=============================================================================


//: Write data of type T to an ostream
template <class T >
class bprod_ostream_sink : public bprod_sink
{
 public:
  bprod_ostream_sink(std::ostream& stream) : os(stream) {}

  //: Execute the process
  bprod_signal execute()
  {
    assert(input_type_id(0) == typeid(T));
    os << input<T>(0);
    return BPROD_VALID;
  }
  std::ostream& os;
};




#endif // bprod_ios_processes_h_

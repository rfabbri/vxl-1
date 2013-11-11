#ifndef bvam_memory_chunk_h_
#define bvam_memory_chunk_h_

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

class bvam_memory_chunk : public vbl_ref_count
{
  //: Data
  void *data_;
  
  //: Number of elements (bytes)
  unsigned long size_;
  

 public:
    //: Default constructor
    bvam_memory_chunk();

    //: Allocate n bytes of memory
    bvam_memory_chunk(unsigned long n);

    //: Copy constructor
    bvam_memory_chunk(const bvam_memory_chunk&);

    //: Copy operator
    bvam_memory_chunk& operator=(const bvam_memory_chunk&);

    //: Destructor
    virtual ~bvam_memory_chunk();
    
    //: Pointer to first element of data
    void* data() { return data_;}

    //: Pointer to first element of data
    void* const_data() const { return data_;}

    //: Number of bytes allocated
    unsigned long size() const { return size_; }

    //: Create space for n bytes
    void set_size(unsigned long n);
};

typedef vbl_smart_ptr<bvam_memory_chunk> bvam_memory_chunk_sptr;



#endif


// This is brcv/shp/dbskfg/pro/dbskfg_composite_graph_storage.h
#ifndef dbskfg_composite_graph_storage_h_
#define dbskfg_composite_graph_storage_h_
//:
// \file
// \brief Storage class for composite graph datastructure
// \author Maruthi Narayanan
// \date July 07, 2010
//
// \verbatim
//  Modifications
//
//  Maruthi Narayanan 07/07/10 Initial Version
//
// \endverbatim

#include <bpro1/bpro1_storage.h>
#include <dbskfg/pro/dbskfg_composite_graph_storage_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/algo/dbskfg_detect_transforms_sptr.h>
#include <vil/vil_image_resource_sptr.h>

//: Storage class for dbskfg_icomposite_graph
class dbskfg_composite_graph_storage : public bpro1_storage 
{
public:

  //: Constructor
  dbskfg_composite_graph_storage();

  //: Destructor
  /* virtual */ ~dbskfg_composite_graph_storage();

  /* virtual */ vcl_string type() const { return "composite_graph"; }

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  /* virtual */ bpro1_storage* clone() const;
  
  //: Return a platform independent string identifying the class
  /* virtual */ vcl_string is_a() const 
  { return "dbskfg_composite_graph_storage"; }

  
  //: get the composite graph
  dbskfg_composite_graph_sptr get_composite_graph() { return composite_graph_; }
  //: set the composite graph
  void set_composite_graph( dbskfg_composite_graph_sptr new_composite_graph ) 
  { composite_graph_ = new_composite_graph; }

  //: get the rag graph
  dbskfg_rag_graph_sptr get_rag_graph() { return rag_graph_; }
  //: set the rag graph
  void set_rag_graph( dbskfg_rag_graph_sptr new_rag_graph ) 
  { rag_graph_ = new_rag_graph; }

  //: get the transforms associated with this composite graph
  dbskfg_detect_transforms_sptr get_transforms() { return transforms_; }
  //: set the transforms object
  void set_transforms( dbskfg_detect_transforms_sptr new_transforms ) 
  { transforms_ = new_transforms; }
  
  //: get the image
  vil_image_resource_sptr get_image() { return image_; }
  //: set the image
  void set_image( const vil_image_resource_sptr img ) { image_ = img; }

private:
  
  //: composite graph
  dbskfg_composite_graph_sptr composite_graph_;

  //: rag graph
  dbskfg_rag_graph_sptr rag_graph_;

  //: store all transforms
  dbskfg_detect_transforms_sptr transforms_;
  
  //: store image
  vil_image_resource_sptr image_; 

};

//: Create a smart-pointer to a dbskfg_composite_graph_storage.
struct dbskfg_composite_graph_storage_new : 
    public dbskfg_composite_graph_storage_sptr
{
  typedef dbskfg_composite_graph_storage_sptr base;

  //: Constructor - creates a default dbskfg_composite_graph_storage_sptr.
  dbskfg_composite_graph_storage_new() : 
  base(new dbskfg_composite_graph_storage()) { }
};

#endif //dbskfg_composite_graph_storage_h_

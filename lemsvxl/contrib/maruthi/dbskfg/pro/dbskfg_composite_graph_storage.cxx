// This is brcv/shp/dbskfg/pro/dbskfg_composite_graph_storage.cxx

//:
// \file

#include <dbskfg/pro/dbskfg_composite_graph_storage.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/algo/dbskfg_detect_transforms.h>

//: Constructor
dbskfg_composite_graph_storage::dbskfg_composite_graph_storage():
    bpro1_storage(),
    composite_graph_(0),
    rag_graph_(0),
    transforms_(0),
    image_(0)
{
}

//: Destructor
dbskfg_composite_graph_storage::~dbskfg_composite_graph_storage() 
{ 
    if ( composite_graph_ )
    {

        if ( composite_graph_->get_references() == 1 )
        {
            composite_graph_->clear();
            composite_graph_=0;
        }
        else
        {
            composite_graph_=0;
        }
    }

    if ( rag_graph_ )
    {
        if ( rag_graph_->get_references() == 1 )
        {
            rag_graph_->clear();
            rag_graph_ = 0;
        }
        else
        {
            rag_graph_=0;
        }
    }

    if ( transforms_ )
    {
        if ( transforms_->get_references() == 0 )
        {
            transforms_->destroy_transforms();
        }
        else
        {
            transforms_=0;
        }
    }

    if ( image_ )
    {
        image_ =0;
    }

}

//: Create a copy of the object on the heap.
// The caller is responsible for deletion
bpro1_storage* dbskfg_composite_graph_storage::clone() const
{
  return new dbskfg_composite_graph_storage(*this);
}




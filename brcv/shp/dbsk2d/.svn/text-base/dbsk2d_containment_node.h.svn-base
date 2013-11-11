// This is brcv/shp/dbsk2d/algo/dbsk2d_containment_node.h
#ifndef dbsk2d_containment_node_h_
#define dbsk2d_containment_node_h_
//:
// \file
// \brief This class represents a node in the containment graph
// \author Maruthi Narayanan
// \date 09/05/12
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 09/05/12    Initial version.
//
// \endverbatim 


#include <dbsk2d/dbsk2d_containment_node_sptr.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform_sptr.h>
#include <dbsk2d/algo/dbsk2d_ishock_transform.h>

#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vcl_set.h>
#include <vcl_vector.h>


class dbsk2d_ishock_transform;
class dbsk2d_ishock_bpoint;

//: Represents a node in containment graph
class dbsk2d_containment_node : public vbl_ref_count
{
    
public: 
    //: Constructor
    dbsk2d_containment_node(dbsk2d_ishock_transform_sptr parent_transform,
                            unsigned int depth=0,
                            unsigned int id=0);

    //: Destructor
    ~dbsk2d_containment_node(){parent_transform_=0;children_.clear();}

    //------------------------- Accessors -----------------------------------
    unsigned int get_id(){return id_;}

    unsigned int get_depth(){return depth_;}

    bool get_visited(){return visited_;}

    vcl_set<int> get_key(){return key_;}

    vcl_vector<dbsk2d_containment_node_sptr>& 
        get_children(){return children_;}

    dbsk2d_ishock_transform_sptr& get_parent_transform()
    {return parent_transform_;}

    vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >&
        get_parent_regions(){return parent_regions_;}
        
    bool children_visited(){
        bool flag=true;
        for ( unsigned int i=0; i < children_.size() ; ++i)
        {
            flag=flag&children_[i]->visited_;
        }
        return flag;
    }

    double get_prob(){return prob_;}

    //------------------------- Setters -----------------------------------
    void set_id(unsigned int id){id_=id;}

    void set_depth(unsigned int depth){depth_=depth;}

    void set_visited(bool flag){visited_=flag;}

    void set_key(vcl_set<int> key){key_=key;}

    void set_child_node(dbsk2d_containment_node_sptr& child_node)
    {
        children_.push_back(child_node);     
    }

    void set_prob(double prob){prob_=prob;}

    void set_parent_regions(vcl_vector<dbsk2d_ishock_belm*> region_belms)
    {
        unsigned int index=parent_regions_.size();
        for ( unsigned int i=0; i < region_belms.size() ; ++i)
        {
            this->parent_regions_[index].push_back(region_belms[i]);
        }
    }

    //------------------------- Modifiers -----------------------------------

    //: execute transform
    bool execute_transform(){return parent_transform_->execute_transform();}

    //: destroy transform
    void destroy_transform(){parent_transform_=0;}

    //: overload operator
    inline bool operator==(const dbsk2d_containment_node& other);

    void print_node()
    {
        unsigned int i=1;
        vcl_cout<<"Node "
                <<id_
                <<" "
                <<parent_regions_.size()
                <<" Regions"<<vcl_endl;

        vcl_map<unsigned int,vcl_vector<dbsk2d_ishock_belm*> >::iterator it;
        for ( it = parent_regions_.begin() ; it != parent_regions_.end(); ++it)
        {
            vcl_vector<dbsk2d_ishock_belm*> belms = (*it).second;
            vcl_vector<dbsk2d_ishock_belm*>::iterator sit;
            vcl_cout<<"Region "<<(*it).first<<": ";
            for ( sit = belms.begin() ; sit != belms.end() ; ++sit)
            {
                vcl_cout<<"("
                        <<(*sit)->id()
                        <<","
                        <<(*sit)->get_contour_id()
                        <<") ";
                
            }
            vcl_cout<<vcl_endl;
        }

    }
private: 

    // Keep a set of children
    vcl_vector<dbsk2d_containment_node_sptr> children_;

    // Keep transform that caused this node
    dbsk2d_ishock_transform_sptr parent_transform_;

    // Keep track if visited during depth first traversal
    bool visited_;

    // Keep track of depth in tree of this node
    unsigned int depth_;
    
    // Keep track of id in this node
    unsigned int id_;

    // Keep a unique key describing this node
    vcl_set<int> key_;

    // Keep track of probability of this node
    double prob_;

    // Keep track of regions that spawned this node
    vcl_map< unsigned int,vcl_vector<dbsk2d_ishock_belm*> > parent_regions_;

    // Make copy ctor private
    dbsk2d_containment_node(const dbsk2d_containment_node&);

    // Make assign operator private
    dbsk2d_containment_node& operator
        =(const dbsk2d_containment_node& );
};

#endif //dbsk2d_containment_node_h_

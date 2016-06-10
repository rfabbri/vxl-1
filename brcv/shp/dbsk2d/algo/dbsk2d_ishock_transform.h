// This is brcv/shp/dbsk2d/algo/dbsk2d_ishock_transform.h
#ifndef dbsk2d_ishock_transform_h_
#define dbsk2d_ishock_transform_h_
//:
// \file
// \brief Base class for all shock transforms
// \author Maruthi Narayanan
// \date 07/15/11
// 
// This file contains base methods which are common to gap or loop transforms

// \verbatim
//  Modifications
//   Maruthi Narayanan 07/15/12    Initial version.
//
// \endverbatim 

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/algo/dbsk2d_lagrangian_ishock_detector.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <vcl_map.h>
#include <vcl_string.h>
#include <vcl_iterator.h>
#include <vcl_algorithm.h>

class dbsk2d_lagrangian_ishock_detector;
class dbsk2d_ishock_belm;
class dbsk2d_ishock_edge;

//: Loop Transform Remvol algorithm
// \relates operates on dbsk2d_ishock_graph
class dbsk2d_ishock_transform : public vbl_ref_count
{
    
public: 

    //: Enum
    enum TransformType
    {
        GAP,
        LOOP,
        GAP4
    };

    //: Constructor
    dbsk2d_ishock_transform(
        dbsk2d_ishock_graph_sptr intrinsic_shock_graph,
        TransformType transform_type);

    //: Destructor
    virtual ~dbsk2d_ishock_transform(){
        clear();
        minimal_interacting_elements_.clear();
        min_local_context_.clear();
        ishock_graph_=0;
        boundary_=0; };

    //: Call execute transform
    virtual bool execute_transform(){return false;}

    // Recompute full shock graph
    void recompute_full_shock_graph();

    //: Get boundary elements removed or added
    virtual void get_belms(vcl_set<int>& set)
    {vcl_cerr<<"Error: In base class"<<vcl_endl;}

    // Get transform type
    TransformType get_transform_type(){return transform_type_;}

    //: write boundary
    void write_boundary(vcl_string filename);

    //: write boundary
    void write_shock_boundary(vcl_string filename);

    //: write boundary
    void write_shock_classification(vcl_string filename);

    //: write boundary
    void write_fragments(vcl_string prefix,
                         vcl_vector<vgl_polygon<double> >& polys);

    //: write state
    void write_state(vcl_string filename, 
                     vcl_vector<vgl_polygon<double> >& polys,
		     bool show_shock=true);

    //: get shock graph
    dbsk2d_ishock_graph_sptr get_shock_graph(){return ishock_graph_;}

    //: return whether valid transform
    virtual bool valid_transform(){return true;}

    //: get likelihood of transform
    virtual double likelihood(){return 0.0;}

    //: get contour points defining gap or loop
    virtual vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> 
        get_contour_pair(){vcl_pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*>
            pair(0,0);
        return pair;}

    bool region_affected(vcl_set<int>& region_belms)
    {
        vcl_set<int> intersection;
        vcl_insert_iterator<vcl_set<int> > 
            inserter(intersection,intersection.begin());
        
        vcl_set<int>::iterator out_iterator;
        vcl_set_intersection(minimal_interacting_elements_.begin(),
                             minimal_interacting_elements_.end(),
                             region_belms.begin(),
                             region_belms.end(),
                             inserter);
        int numb_intersections=intersection.size();

        bool flag(false);
        if ( transform_type_ == GAP )
        {
            if ( numb_intersections==1 && region_belms.size() > 1)
            {
                flag=true;
            }
            else
            {
                flag=false;
            }

        }
        else if ( transform_type_ == LOOP )
        {
            if ( minimal_interacting_elements_.size() == 1)
            {
                flag = ( numb_intersections>=1)?true:false;
            }
            else
            {
                flag= ( numb_intersections>=2)?true:false;
            }
        }

        return flag;
    }

    bool region_in_min_local_context(vcl_set<int>& ids)
    {
        vcl_set<int>::iterator it;
        for ( it = ids.begin() ; it != ids.end() ; ++it)
        {
            if ( min_local_context_.count(*it))
            {
                return true;
            }
        }

        return false;

    }

    dbsk2d_ishock_bpoint* endpoint_in_elms(int contour_id)
    {
        vcl_map<unsigned int,dbsk2d_ishock_belm*>::iterator it;
        for ( it = interacting_bnd_elements_.begin() ; 
              it != interacting_bnd_elements_.end(); 
              ++it)
        {
            if ( (*it).second->is_a_line())
            {
                dbsk2d_ishock_bline* bline=(dbsk2d_ishock_bline*)
                    ((*it).second);
                dbsk2d_ishock_bpoint* s_pt=bline->s_pt();
                dbsk2d_ishock_bpoint* e_pt=bline->e_pt();

                if ( s_pt->is_an_end_point() && 
                     bline->get_contour_id() == contour_id)
                {
                    return s_pt;
                }
                else if ( e_pt->is_an_end_point() && 
                          bline->get_contour_id() == contour_id)
                {
                    return e_pt;
                }
            }
        }
        return 0;
    }

protected:

    // Attributes

    // Transform type
    TransformType transform_type_;

    // Private intrinsinc shock graph
    dbsk2d_ishock_graph_sptr ishock_graph_;

    // Private boundary class
    dbsk2d_boundary_sptr boundary_;

    // Class to perform shock computation
    dbsk2d_lagrangian_ishock_detector ishock_detector_;

    // Methods

    // Delete all shocks for belm
    void delete_belm_shocks(dbsk2d_ishock_belm* belm);

    // Delete all shocks for belm
    void delete_shock_and_update(dbsk2d_ishock_edge* cur_edge);

    // Dete shocks
    void delete_shock(dbsk2d_ishock_edge* cur_edge);

    // Compute local shocks
    void local_shock_compute();

    // Delete vertices
    void delete_shock_vertices();

    void form_contact_shocks(dbsk2d_ishock_belm* belm1,
                             dbsk2d_ishock_belm* belm2,
                             dbsk2d_ishock_bpoint* bp1);

    // clear
    void clear(){interacting_bnd_elements_.clear();
                 outer_wavefront_.clear();
                 removal_bnd_elements_.clear();
                 shocks_removed_.clear();
                 ishock_detector_.clear_deleted_elements();}

    // Keep track of all interacting boundary elements
    vcl_map<unsigned int,dbsk2d_ishock_belm*> interacting_bnd_elements_;

    // Keep track of contour elements to remove
    vcl_map<unsigned int,dbsk2d_ishock_belm*> removal_bnd_elements_;

    // Keep track of outer wavefront of local context
    vcl_map<unsigned int,dbsk2d_ishock_node*> outer_wavefront_;
  
    // Keep track of all shocks that have been removed
    vcl_map<unsigned int,vcl_string> shocks_removed_;

    // Keep track of minimal local context contour id
    vcl_set<int> minimal_interacting_elements_;

    // Keep track of minimal local context interacting belm ids
    vcl_map<unsigned int, dbsk2d_ishock_belm*> min_local_context_;

private: 

    // Measure distance from euler spiral
    double distance_from_ess(vcl_vector<dbsk2d_ishock_belm*>& belm_list,
                             vgl_point_2d<double> test_point);

    // Make copy ctor private
    dbsk2d_ishock_transform(const dbsk2d_ishock_transform&);

    // Make assign operator private
    dbsk2d_ishock_transform& operator
        =(const dbsk2d_ishock_transform& );
};

#endif //dbsk2d_ishock_transform_h_

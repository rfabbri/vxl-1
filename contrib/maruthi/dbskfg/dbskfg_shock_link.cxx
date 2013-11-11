// This is brcv/shp/dbskfg/dbskfg_shock_link.cxx

//:
// \file

#include <dbskfg/dbskfg_shock_link.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbskfg/dbskfg_composite_node.h>
#include <dbskfg/dbskfg_contour_link.h>
#include <dbskfg/dbskfg_contour_node.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_rag_node.h>
#include <dbskfg/dbskfg_shock_node.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <dbsk2d/dbsk2d_ishock_bpoint.h>
#include <dbsk2d/dbsk2d_ishock_bline.h>
#include <dbsk2d/dbsk2d_ishock_lineline.h>
#include <dbsk2d/dbsk2d_defines.h>
#include <dbsk2d/dbsk2d_ishock_pointpoint.h>
#include <dbsk2d/dbsk2d_ishock_pointline.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <bsol/bsol_algs.h>
#include <vcl_algorithm.h>

//: Constructor
dbskfg_shock_link::dbskfg_shock_link(dbskfg_composite_node_sptr source,
                                     dbskfg_composite_node_sptr target,
                                     unsigned int id,
                                     dbskfg_shock_link::ShockType shock_type,
                                     dbskfg_utilities::Fragment_Type frag_type) 
    : dbskfg_composite_link(source,target,
                            dbskfg_composite_link::SHOCK_LINK,id),
      shock_link_type_(shock_type),
      fragment_type_(frag_type),
      left_point_(0),
      right_point_(0),
      polygon_boundary_(),
      left_boundary_(),
      right_boundary_(),
      rag_node_(0),
      elm_(0)
{
 
}

//: Destructor
dbskfg_shock_link::~dbskfg_shock_link() 
{
    left_point_ =0;
    right_point_ = 0;
    
    left_contour_links_.clear();
    right_contour_links_.clear();
    degenerate_links_.clear();
    ex_pts_.clear();
    rag_node_=0;

    delete elm_;
    elm_=0;

    // We must delete the lines first to get thru this
    for ( unsigned int k =0 ; k < belm_.size() ; ++k)
    {
        if ( belm_[k]->is_a_line())
        {
            delete belm_[k];
            belm_[k]=0;
        }
    }

    for ( unsigned int k =0 ; k < belm_.size() ; ++k)
    {
        if ( belm_[k])
        {
            delete belm_[k];
            belm_[k]=0;
        }
    }

    belm_.clear();
}

//: Set polygon
void dbskfg_shock_link::set_polygon(vgl_polygon<double> poly)
{

    polygon_boundary_ = poly;

    double G = vcl_pow(2.0,-25.0);

    for (unsigned int s = 0; s < polygon_boundary_.num_sheets(); ++s)
    {
        for (unsigned int p = 0; p < polygon_boundary_[s].size(); ++p)
        { 
            polygon_boundary_[s][p].x()=
                (vcl_floor((polygon_boundary_[s][p].x()/G)+0.5))*G;
            polygon_boundary_[s][p].y()=
                (vcl_floor((polygon_boundary_[s][p].y()/G)+0.5))*G;
        }
    }
}

//: Grab splice cost of this shock link
void dbskfg_shock_link::splice_cost(vcl_vector<double>& costs)
{
    dbsk2d_ishock_graph_sptr ishock_graph;
    dbsk2d_shock_graph_sptr shock_graph;
    dbsk2d_prune_ishock pruner(ishock_graph,shock_graph);
    pruner.compute_shock_saliency(elm_);

    costs.push_back(pruner.dOC(1));
    costs.push_back(pruner.dNC(1));
    costs.push_back(pruner.dPnCost(1));

}

bool dbskfg_shock_link::contour_spawned_shock(
    dbskfg_composite_link_sptr clink,dbskfg_utilities::Orientation& dir)
{


    vcl_vector<dbskfg_composite_link_sptr>::iterator it;

    for ( it = left_contour_links_.begin() ; it != left_contour_links_.end()
              ; ++it)
    {
        if ( clink->id() == (*it)->id())
        {
            dir = dbskfg_utilities::LEFT;
            return true;
        }



    }

    for ( it = right_contour_links_.begin() ; it != right_contour_links_.end()
              ; ++it)
    {
        if ( clink->id() == (*it)->id())
        {
            dir = dbskfg_utilities::RIGHT;
            return true;
        }



    }


    return false;

}
//: Get contour pair
vcl_set<unsigned int> dbskfg_shock_link::get_contour_pair()
{

    vcl_set<unsigned int> contour_pair;

    //Find out type of shock link
    if ( this->shock_compute_type() == dbskfg_utilities::LL )
    {
        dbskfg_contour_node* lcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*left_contour_links_.front()->source()));
        dbskfg_contour_node* rcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*right_contour_links_.front()->source()));

        contour_pair.insert( lcnode->contour_id() );
        contour_pair.insert( rcnode->contour_id() );

        for ( unsigned int k=0; k < left_contour_links_.size() ; ++k)
        {
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &
                    (*left_contour_links_[k]));
            contour_pair.insert(clink->contour_id());
                    
        }
        
        for ( unsigned int k=0; k < right_contour_links_.size() ; ++k)
        {
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &
                    (*right_contour_links_[k]));
               contour_pair.insert(clink->contour_id());
               
        }
        
           
    }
    else if( this->shock_compute_type() == dbskfg_utilities::PP )
    {
        dbskfg_contour_node* cl = left_point_;
        dbskfg_contour_node* cr = right_point_;

        contour_pair.insert(cl->contour_id());
        contour_pair.insert(cr->contour_id());

    }
    else if(this->shock_compute_type() == dbskfg_utilities::LLRP )
    {
        dbskfg_contour_node* lcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*left_contour_links_.front()->source()));
        contour_pair.insert( lcnode->contour_id() );

        for ( unsigned int k=0; k < left_contour_links_.size() ; ++k)
        {
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &
                    (*left_contour_links_[k]));
            contour_pair.insert(clink->contour_id());
                    
        }

        dbskfg_contour_node* cr = right_point_;
        contour_pair.insert(cr->contour_id());


    }
    else // RLLP
    {
        dbskfg_contour_node* rcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*right_contour_links_.front()->source()));
        contour_pair.insert( rcnode->contour_id() );

        for ( unsigned int k=0; k < right_contour_links_.size() ; ++k)
        {
            dbskfg_contour_link* clink=
                dynamic_cast<dbskfg_contour_link*>(
                    &
                    (*right_contour_links_[k]));
            contour_pair.insert(clink->contour_id());
        
       
            
        }
        
        dbskfg_contour_node* cl = left_point_;
        contour_pair.insert(cl->contour_id());
         
    }

    return contour_pair;


}

//: Whether an endpoint spawned this node
bool dbskfg_shock_link::endpoint_spawned()
{

    //Find out type of shock link
    if ( this->shock_compute_type() == dbskfg_utilities::LL )
    {
        return false;

    }
    else if( this->shock_compute_type() == dbskfg_utilities::PP )
    {
     
        dbskfg_contour_node* cl = left_point_;
        dbskfg_contour_node* cr = right_point_;
        
        if ( cl->get_composite_degree() ==1 ||
             cr->get_composite_degree() == 1 )
        {
            return true;
        }
        else
        {
            return false;

        }

    }
    else if(this->shock_compute_type() == dbskfg_utilities::LLRP )
    {
        dbskfg_contour_node* lcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*left_contour_links_.front()->source()));
        dbskfg_contour_node* cr = right_point_;
   

        if ( cr->get_composite_degree() == 1 )
        {
            return true;
        }
        else
        {
            return false;

        }

    }
    else // RLLP
    {

        dbskfg_contour_node* rcnode=dynamic_cast<dbskfg_contour_node*>
            (&(*right_contour_links_.front()->source()));
        dbskfg_contour_node* cl = left_point_;
  

        if ( cl->get_composite_degree() == 1 )
        {
            return true;
        }
        else
        {
            return false;

        }

        
    }

    return false;


}

//: Type of shock compute
dbskfg_utilities::Shock_Compute_Type dbskfg_shock_link::shock_compute_type()
{
    
  if ( left_contour_links_.size() > 0 && right_contour_links_.size() > 0)
  {
      return dbskfg_utilities::LL;
  }

  else if ( left_point_ != 0 && right_point_ != 0 )
  {

      return dbskfg_utilities::PP;
  }

  else if ( left_contour_links_.size() > 0 && right_point_ != 0)
  {
      return dbskfg_utilities::LLRP;
  }

  else if ( right_contour_links_.size() > 0 && left_point_ != 0 )
  {
      return dbskfg_utilities::RLLP;
  }

}

void dbskfg_shock_link::construct_locus()
{
    double source_radius(0);
    double target_radius(0);

    if ( this->source()->node_type() == dbskfg_composite_node::CONTOUR_NODE )
    {
        source_radius = 0;
    }
    else
    {
        dbskfg_shock_node* source_node = dynamic_cast<dbskfg_shock_node*>
            (&(*this->source()));
  
        source_radius = source_node->get_radius(); 
        
    }

    if ( this->target()->node_type() == dbskfg_composite_node::CONTOUR_NODE )
    {

        target_radius=0;
    }
    else
    {
        dbskfg_shock_node* target_node = dynamic_cast<dbskfg_shock_node*>
            (&(*this->target()));
  
        target_radius = target_node->get_radius(); 
   
    }

    dbsk2d_ishock_node parent_node(this->source()->id(),
                                   source_radius,
                                   this->source()->pt());

    if ( left_contour_links_.size() > 0 && right_contour_links_.size() > 0)
    {

        dbsk2d_ishock_bpoint* startleftpt = new dbsk2d_ishock_bpoint(
            left_boundary_.contour_.back().x(),
            left_boundary_.contour_.back().y());


        dbsk2d_ishock_bpoint* endleftpt=new dbsk2d_ishock_bpoint(
            left_boundary_.contour_.front().x(),
            left_boundary_.contour_.front().y());

        dbsk2d_ishock_bline* leftline = new dbsk2d_ishock_bline(
            startleftpt,endleftpt);
     
        dbsk2d_ishock_bpoint* startrightpt= new dbsk2d_ishock_bpoint(
            right_boundary_.contour_.back().x(),
            right_boundary_.contour_.back().y());

        dbsk2d_ishock_bpoint* endrightpt = new dbsk2d_ishock_bpoint(
            right_boundary_.contour_.front().x(),
            right_boundary_.contour_.front().y());

        dbsk2d_ishock_bline* rightline = new dbsk2d_ishock_bline(
            startrightpt,endrightpt);
       
        elm_ = new dbsk2d_ishock_lineline
            (1,               // arbitrary id
             source_radius,   // arbitrary radius
             &parent_node,    // dummy parent node
             leftline,       // left boundary element
             rightline,      // right boundary element
             leftline->len(),  // Left start eta
             0,               // right start eta
             UNCONSTRAINED);  // constraint flag

        elm_->setEndTime(target_radius);
        elm_->setSimTime(target_radius);
        elm_->compute_extrinsic_locus();
       
        ex_pts_=elm_->ex_pts();

        belm_.push_back(startrightpt);
        belm_.push_back(endrightpt);
        belm_.push_back(rightline);
        belm_.push_back(startleftpt);
        belm_.push_back(endleftpt);
        belm_.push_back(leftline);

    }
    else if( left_contour_links_.size() > 0 && right_point_ != 0 )
    {

        // We know we have a left line / right point combination
        dbsk2d_ishock_bpoint* startleftpt = new dbsk2d_ishock_bpoint(
            left_boundary_.contour_.back().x(),
            left_boundary_.contour_.back().y());

        dbsk2d_ishock_bpoint* endleftpt = new dbsk2d_ishock_bpoint(
            left_boundary_.contour_.front().x(),
            left_boundary_.contour_.front().y());

        dbsk2d_ishock_bline* leftline = new dbsk2d_ishock_bline(
            startleftpt,endleftpt);
        dbsk2d_ishock_bline* twinline = new dbsk2d_ishock_bline(endleftpt,
                                                                startleftpt);
        leftline->set_twinLine(twinline);

        dbsk2d_ishock_bpoint* rightpt = new dbsk2d_ishock_bpoint(
            right_point_->pt().x(),
            right_point_->pt().y());
        
        //1) determine the half line the point is interacting with
        //1) determine the half line the point is interacting with
        dbsk2d_ishock_bline* hl;

        if (_isPointAboveLine(rightpt->pt(),leftline->start(), leftline->end()))
        {
            hl = leftline; //point_is_above_GUI_line
        }
        else
        {
            hl = leftline->twinLine();
        }

        //2) compute the projection of the point on to the line
        double eta2 = _deltaPointLine (rightpt->pt(), hl->start(), hl->end(), 
                                       hl->l());

        if (LisL(eta2, 0))
        {
            //compute the Lsp->P vector
            double pp_vec = _vPointPoint (hl->start(), rightpt->pt());
            rightpt->vec_to_eta(angle0To2Pi(pp_vec+vnl_math::pi));
        }
        else if (LisG(eta2, leftline->l()))
        {            
            double pp_vec = _vPointPoint (hl->end(),rightpt->pt());
            rightpt->vec_to_eta(angle0To2Pi(pp_vec+vnl_math::pi));

        }
        else
        {
            rightpt->vec_to_eta( 
                _vPointLine (rightpt->pt(), hl->start(), hl->end()));


        } 

        elm_ = new dbsk2d_ishock_pointline
            (1,               // arbitrary id
             source_radius,   // arbitrary radius
             &parent_node,    // dummy parent node
             leftline,        // left boundary element
             rightpt,         // right boundary element
             leftline->l(),   // Left start eta
             0,               // right start eta
             UNCONSTRAINED);  // constraint flag

        elm_->setReTau(elm_->getRTauFromTime(
                          target_radius));
        elm_->setEndTime(target_radius);
        elm_->compute_extrinsic_locus();
        ex_pts_=elm_->ex_pts();

       
        belm_.push_back(startleftpt);
        belm_.push_back(endleftpt);
        belm_.push_back(leftline);
        belm_.push_back(twinline);
        belm_.push_back(rightpt);

    }
    else if( right_contour_links_.size() > 0 && left_point_ != 0 )
    {

        // We know we have a left line / right point combination
        dbsk2d_ishock_bpoint* startrightpt = new dbsk2d_ishock_bpoint(
            right_boundary_.contour_.back().x(),
            right_boundary_.contour_.back().y());

        dbsk2d_ishock_bpoint* endrightpt = new dbsk2d_ishock_bpoint(
            right_boundary_.contour_.front().x(),
            right_boundary_.contour_.front().y());

        dbsk2d_ishock_bline* rightline = new dbsk2d_ishock_bline(
            startrightpt,endrightpt);
        dbsk2d_ishock_bline* twinline = new dbsk2d_ishock_bline(endrightpt,
                                                                startrightpt);
        rightline->set_twinLine(twinline);

        dbsk2d_ishock_bpoint* leftpt = new dbsk2d_ishock_bpoint(
            left_point_->pt().x(),
            left_point_->pt().y());

        //1) determine the half line the point is interacting with
        //1) determine the half line the point is interacting with
        dbsk2d_ishock_bline* hl;

        if (_isPointAboveLine(leftpt->pt(),rightline->start(),rightline->end()))
        {
            hl = rightline; //point_is_above_GUI_line
        }
        else
        {
            hl = rightline->twinLine();
        }

        //2) compute the projection of the point on to the line
        double eta2 = _deltaPointLine (leftpt->pt(), hl->start(), hl->end(), 
                                       hl->l());

        if (LisL(eta2, 0))
        {
            //compute the Lsp->P vector
            double pp_vec = _vPointPoint (hl->start(), leftpt->pt());
            leftpt->vec_to_eta(angle0To2Pi(pp_vec+vnl_math::pi));
        }
        else if (LisG(eta2, rightline->l()))
        {            
            double pp_vec = _vPointPoint (hl->end(),leftpt->pt());
            leftpt->vec_to_eta(angle0To2Pi(pp_vec+vnl_math::pi));

        }
        else
        {
            leftpt->vec_to_eta( 
                _vPointLine (leftpt->pt(), hl->start(), hl->end()));


        } 

        elm_ = new dbsk2d_ishock_pointline
            (1,               // arbitrary id
             source_radius,   // arbitrary radius
             &parent_node,    // dummy parent node
             leftpt,          // left boundary element
             rightline,       // right boundary element
             0,               // Left start eta
             0,               // right start eta
             UNCONSTRAINED);  // constraint flag

        elm_->setLeTau(elm_->getLTauFromTime(
                          target_radius));
        elm_->setEndTime(target_radius);

        elm_->compute_extrinsic_locus();
        ex_pts_=elm_->ex_pts();
      
        belm_.push_back(startrightpt);
        belm_.push_back(endrightpt);
        belm_.push_back(rightline);
        belm_.push_back(twinline);
        belm_.push_back(leftpt);

    }
    else if ( left_point_ != 0 && right_point_ != 0 )
    {

        dbsk2d_ishock_bpoint* rightpt = new dbsk2d_ishock_bpoint(
            right_point_->pt().x(),
            right_point_->pt().y());
        
        dbsk2d_ishock_bpoint* leftpt = new dbsk2d_ishock_bpoint(
            left_point_->pt().x(),
            left_point_->pt().y());

        //1) compute the etas (actually just vectors)
        double eta1 = _vPointPoint (rightpt->pt(), leftpt->pt());
        double eta2 = angle0To2Pi(eta1+vnl_math::pi);
        leftpt->vec_to_eta(eta2);
        rightpt->vec_to_eta(eta1);
        
        elm_ = new dbsk2d_ishock_pointpoint
            (1,               // arbitrary id
             source_radius,   // arbitrary radius
             &parent_node,    // dummy parent node
             leftpt,         // left boundary element
             rightpt,        // right boundary element
             0,            // Left start eta
             0,            // right start eta
             UNCONSTRAINED);  // constraint flag

        elm_->setLsTau(elm_->getLTauFromTime(
                          source_radius));
        elm_->setRsTau(elm_->getRTauFromTime(
                          source_radius));

        elm_->setLeTau(elm_->getLTauFromTime(
                          target_radius));
        elm_->setReTau(elm_->getRTauFromTime(
                          target_radius));
        elm_->setSimTime(target_radius);
        elm_->setEndTime(target_radius);
     
        elm_->compute_extrinsic_locus();
        ex_pts_=elm_->ex_pts();
          
        belm_.push_back(leftpt);
        belm_.push_back(rightpt);

    }
    
}

void dbskfg_shock_link::form_shock_fragment()
{
    
    if ( fragment_type_ == dbskfg_utilities::DEGENERATE )
    {
        form_degenerate_fragment();
    }
    else
    {
        form_regular_fragment();
    }

    // Regardless of the fragment construct_locus
    construct_locus();

}

void dbskfg_shock_link::form_degenerate_fragment()
{

    // We will make one contour

    // Find out what edges are shared
    dbskfg_composite_link_sptr right_start;
    dbskfg_composite_link_sptr left_start;

    dbskfg_composite_node_sptr shared_node(0);
 
    for (unsigned int i=0; i < left_contour_links_.size() ; ++i)
    {
        shared_node = 
            this->shared_vertex(*left_contour_links_[i]);
        if ( shared_node != 0 )
        {
            left_start=left_contour_links_[i];
            break;
        }

    }
    
    for (unsigned int i=0; i < right_contour_links_.size() ; ++i)
    {
        shared_node = 
            this->shared_vertex(*right_contour_links_[i]);
        if ( shared_node != 0 )
        {
            right_start=right_contour_links_[i];
            break;
        }

    }
    
  
    vcl_vector<vgl_point_2d<double> > right_points;
    vcl_vector<unsigned int> right_ids;
    vcl_vector<unsigned int> left_ids;

    // push back first two points to consider
    unsigned int right_target_id;
    if ( shared_node->id() == right_start->source()->id() )
    {
        right_points.push_back(shared_node->pt());
        right_ids.push_back(shared_node->id());

        right_points.push_back(right_start->target()->pt());
        right_ids.push_back(right_start->target()->id());

        right_target_id=right_start->target()->id();
    
    } 
    else
    {
        right_points.push_back(shared_node->pt());
        right_ids.push_back(shared_node->id());

        right_points.push_back(right_start->source()->pt());
        right_ids.push_back(right_start->source()->id());

        right_target_id=right_start->source()->id();

    }
    
    for ( unsigned int i=1; i < right_contour_links_.size() ; ++i)
    {
        
        vgl_point_2d<double> temp_point;
        unsigned int child_id  = 
            (*(right_contour_links_[i]->target())).id();
        unsigned int parent_id = 
            (*(right_contour_links_[i]->source())).id();
        if ( child_id == right_target_id )
        {
            temp_point=(*(right_contour_links_[i]->source())).pt();
            right_target_id=parent_id;
        }
        else
        {
            temp_point=(*(right_contour_links_[i]->target())).pt();
            right_target_id=child_id;
                
        }
 
        right_points.push_back(temp_point);
        right_ids.push_back(right_target_id);

    }
  

    vcl_vector<vgl_point_2d<double> > left_points;

    // In left case dont push back both ids
    unsigned int left_target_id;
    if ( shared_node->id() == left_start->source()->id() )
    {
        
        left_points.push_back(left_start->target()->pt());
        left_ids.push_back(left_start->target()->id());
        left_target_id=left_start->target()->id();
        
 
    } 
    else
    {
        left_points.push_back(left_start->source()->pt());
        left_ids.push_back(left_start->source()->id());
        left_target_id=left_start->source()->id();

    }

    for ( unsigned int i=1; i < left_contour_links_.size() ; ++i)
    {
        
        vgl_point_2d<double> temp_point;
        unsigned int child_id  = 
            (*(left_contour_links_[i]->target())).id();
        unsigned int parent_id = 
            (*(left_contour_links_[i]->source())).id();
        if ( child_id == left_target_id )
        {
            temp_point=(*(left_contour_links_[i]->source())).pt();
            left_target_id=parent_id;
        }
        else
        {
            temp_point=(*(left_contour_links_[i]->target())).pt();
            left_target_id=child_id;
                
        }

        left_points.push_back(temp_point);
        left_ids.push_back(left_target_id);
    }
    
    vcl_vector<vgl_point_2d<double> >::reverse_iterator rit;
    vcl_vector<vgl_point_2d<double> >::iterator lit;

    // Keep list of points
    vcl_vector<vgl_point_2d<double> > polygon_points;
   
    for ( rit = right_points.rbegin(); rit != right_points.rend(); ++rit)
    {
        vgl_point_2d<double> temp_point=*rit;
       
        polygon_points.push_back(temp_point);
        
        right_boundary_.contour_.push_back(temp_point);
        
    }

    left_boundary_.contour_.push_back(right_boundary_.contour_.back());

    for ( lit = left_points.begin(); lit != left_points.end(); ++lit)
    {
        vgl_point_2d<double> temp_point=*lit;
        polygon_points.push_back(temp_point);
        left_boundary_.contour_.push_back(temp_point);
    }

    // push back shared node
    left_ids.push_back(shared_node->id());

    left_boundary_.contour_ids_ = left_ids;
    vcl_reverse(right_ids.begin(),
                right_ids.end());
    right_boundary_.contour_ids_ = right_ids;

    polygon_points.push_back(this->target()->pt());

    polygon_boundary_.push_back(polygon_points);

}

//: Form shock fragments
void dbskfg_shock_link::form_regular_fragment()
{
    
    // Keep list of points
    vcl_vector<vgl_point_2d<double> > polygon_points;
    
    // Lets check left
    vgl_point_2d<double> startpt = this->source()->pt();
      
    vgl_point_2d<double> midpt = this->target()->pt();

    polygon_points.push_back(startpt);

    // Keep track of start id
    unsigned int first_poly_id = this->source()->id();

    if (left_contour_links_.size() > 0 )
    {
        dbskfg_composite_link_sptr clink=left_contour_links_.front();

        unsigned int left_target_id(1);

        vcl_pair< vgl_point_2d<double>,vgl_point_2d<double> > start_pair=
            find_start_pair_of_polygon(clink,
                                       left_target_id,
                                       dbskfg_utilities::LEFT);
        
        polygon_points.push_back(start_pair.first);
        polygon_points.push_back(start_pair.second);

        left_boundary_.contour_.push_back(start_pair.first);
        left_boundary_.contour_.push_back(start_pair.second);

        // Make a quick dynamic cast to find original contour id
        dbskfg_contour_node* lcnode = dynamic_cast<dbskfg_contour_node*>
            (&(*clink->source()));
        left_boundary_.original_contour_id_= lcnode->contour_id();

        // Grab all ids
        left_boundary_.contour_ids_.push_back(
            (left_target_id==(*clink->target()).id())
            ?(*clink->source()).id():(*clink->target()).id());
        left_boundary_.contour_ids_.push_back(
            left_target_id);

        for ( unsigned int i=1; i < left_contour_links_.size() ; ++i)
        {
        
            vgl_point_2d<double> temp_point;
            unsigned int child_id  = 
                (*(left_contour_links_[i]->target())).id();
            unsigned int parent_id = 
                (*(left_contour_links_[i]->source())).id();
            if ( child_id == left_target_id )
            {
                temp_point=(*(left_contour_links_[i]->source())).pt();
                left_target_id=parent_id;
            }
            else
            {
                temp_point=(*(left_contour_links_[i]->target())).pt();
                left_target_id=child_id;
                
            }
            polygon_points.push_back(temp_point);
            left_boundary_.contour_.push_back(temp_point);
            left_boundary_.contour_ids_.push_back(
                left_target_id);
    
        }

    }
    else
    {
        vgl_point_2d<double> left_point = left_point_->pt();


        left_boundary_.contour_ids_.push_back(
            left_point_->id());
        left_boundary_.original_contour_id_=
            left_point_->contour_id();
        left_boundary_.point_ = left_point;
        polygon_points.push_back(left_point);

    }
    
    polygon_points.push_back(midpt);

    if (right_contour_links_.size() > 0 )
    {
        dbskfg_composite_link_sptr clink=right_contour_links_.back();

        unsigned int right_target_id(1);

        vcl_pair< vgl_point_2d<double>,vgl_point_2d<double> > start_pair=
            find_start_pair_of_polygon(clink,
                                       right_target_id,
                                       dbskfg_utilities::RIGHT);

        polygon_points.push_back(start_pair.first);
        polygon_points.push_back(start_pair.second);

        right_boundary_.contour_.push_back(start_pair.first);
        right_boundary_.contour_.push_back(start_pair.second);

        // Make a quick dynamic cast to find original contour id
        dbskfg_contour_node* rcnode = dynamic_cast<dbskfg_contour_node*>
            (&(*clink->source()));
        right_boundary_.original_contour_id_= rcnode->contour_id();

        // Grab all ids
        right_boundary_.contour_ids_.push_back(
            (right_target_id==(*clink->target()).id())
            ?(*clink->source()).id():(*clink->target()).id());
        right_boundary_.contour_ids_.push_back(
            right_target_id);

        vcl_vector<dbskfg_composite_link_sptr>::reverse_iterator rit
            = right_contour_links_.rbegin();
        rit++;
        for ( ; rit != right_contour_links_.rend(); ++rit)
        {
        
            dbskfg_composite_link_sptr contour_link=*rit;
            vgl_point_2d<double> temp_point;
            unsigned int child_id  = 
                (*contour_link->target()).id();
            unsigned int parent_id = 
                (*contour_link->source()).id();

            if ( child_id == right_target_id )
            {
                temp_point=(*contour_link->source()).pt();
                right_target_id=parent_id;
            }
            else
            {
                temp_point=(*contour_link->target()).pt();
                right_target_id=child_id;
                
            }
            polygon_points.push_back(temp_point);
            right_boundary_.contour_.push_back(temp_point);
            right_boundary_.contour_ids_.push_back(right_target_id);
        }

        if ( right_target_id == first_poly_id )
        {
            // Remove last point
            polygon_points.erase(polygon_points.end());


        }

    }
    else
    {
        vgl_point_2d<double> right_point = right_point_->pt();
        
        polygon_points.push_back(right_point);

        right_boundary_.contour_ids_.push_back(
            right_point_->id());
        right_boundary_.original_contour_id_=
            right_point_->contour_id();
        right_boundary_.point_ = right_point;
    }

    polygon_boundary_.push_back(polygon_points);

}
 




vcl_pair< vgl_point_2d<double>, vgl_point_2d<double> > 
dbskfg_shock_link::find_start_pair_of_polygon(dbskfg_composite_link_sptr clink,
                                              unsigned int& target_id,
                                              dbskfg_utilities::Orientation dir)
{
    vcl_pair< vgl_point_2d<double>,vgl_point_2d<double> > start_pair;
   
    dbskfg_composite_node_sptr source_node = clink->source();
    dbskfg_composite_node_sptr target_node = clink->target();

    vgl_point_2d<double> source_point = source_node->pt();
        
    vgl_point_2d<double> target_point = target_node->pt();
  
    dbskfg_composite_node::edge_iterator eit;
    dbskfg_composite_node::edge_iterator stop_eit;
    
    eit = (dir == dbskfg_utilities::LEFT)? 
        this->source()->out_edges_begin():
        this->target()->out_edges_begin();

    stop_eit = (dir == dbskfg_utilities::LEFT)? 
        this->source()->out_edges_end():
        this->target()->out_edges_end();

    for ( ; eit != stop_eit ; ++eit )
    {
        dbskfg_composite_link_sptr link = *eit;
        
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK)
        {
                 
            dbskfg_shock_link* shock_ray=
                dynamic_cast<dbskfg_shock_link*>(&(*link));

            if ( shock_ray->shock_link_type() == dbskfg_shock_link::SHOCK_RAY )
            {

         
                if ( source_node->id() == shock_ray->target()->id() )
                {
                    start_pair.first  = source_point;
                    start_pair.second = target_point;
                    target_id=target_node->id();
                    break;
                }
                else if ( target_node->id() == shock_ray->target()->id() )
                {
                    start_pair.first  = target_point;
                    start_pair.second = source_point;
                    target_id=source_node->id();
                    break;
                }
            }
        }
        
    }

    return start_pair;
}

void dbskfg_shock_link::print(vcl_ostream& os)
{
    this->dbskfg_composite_link::print(os);
    os<<"Source Id: "
            << this->source()->id() 
            << " Target id: "
            << this->target()->id()
            << vcl_endl;
    
    os<<"Left Contour ids: ";

    if (left_contour_links_.size() > 0 )
    {
     
        for ( unsigned int i=0; i < left_contour_links_.size() ; ++i)
        {
            dbskfg_composite_link_sptr clink=left_contour_links_[i];
            os<<clink->id()<<" ";
     
        }
        os<<vcl_endl;
    }
    else
    {
        if ( left_point_ )
        {
            os<<left_point_->id()<<vcl_endl;
        }
        else
        {

            os<<" NULL "<<vcl_endl;
        }
    }

    os<<"Right Contour ids: ";
    if (right_contour_links_.size() > 0 )
    {
        
        for ( unsigned int i=0; i < right_contour_links_.size() ; ++i)
        {
            dbskfg_composite_link_sptr clink=right_contour_links_[i];
            os<<clink->id()<<" ";
     
        }
        os<<vcl_endl;
        

    }
    else
    {
        if ( right_point_ )
        {
            os<<right_point_->id()<<vcl_endl;
        }
        else
        {
            os<<" NULL "<<vcl_endl;
        }
    }

    // if ( rag_node_ )
    // {
    //     vcl_cout<<"------------ Shock Rag Node Info --------------"<<vcl_endl;
    //     rag_node_->print(os);
    // }
    os<<vcl_endl;

    polygon_boundary_.print(os);
    os<<vcl_endl;

    elm_->getInfo(os);
    if ( rag_node_ )
    {
        vcl_cout<<"rag node id: "<< rag_node_->id()<<vcl_endl;
    }
    left_boundary_.print();
    right_boundary_.print();

}


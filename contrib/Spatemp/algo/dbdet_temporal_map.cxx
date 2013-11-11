#include "dbdet_temporal_map.h"
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <Spatemp/algo/dbdet_temporal_options.h>
#include <Spatemp/algo/dbdet_contour_fragment.h>
#include <Spatemp/gc/GCoptimization.h>
#include <Spatemp/gc/graph.h>
#include <Spatemp/algo/dbdet_spatial_temporal_options.h>
#include <Spatemp/algo/dbdet_temporal_utils.h>
#include <Spatemp/algo/dbdet_region.h>
#include <Spatemp/algo/dbdet_spherical_histogram_sptr.h>

#include <vnl/vnl_random.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>

typedef vcl_list<dbdet_curvelet*>::iterator cvlet_list_iter;
typedef vcl_list<dbdet_curvelet*> cvlet_list;
typedef vcl_list<dbdet_curvelet*>::iterator cvlet_list_iter;

dbdet_temporal_map::dbdet_temporal_map(dbdet_edgemap_sptr  emap):emap_(emap)
{
    if(!emap.ptr()){
    vcl_cout<<"\n invalid edge map ";
    }
    else
    {

    
    }
}

dbdet_temporal_map::dbdet_temporal_map(dbdet_edgemap_sptr  emap, dbdet_temporal_map_params & params):params_(params)
{
    if(!emap.ptr()){
    vcl_cout<<"\n invalid edge map ";
    }
}

dbdet_temporal_map::dbdet_temporal_map(dbdet_curvelet_map &  cmap)
{
        emap_=cmap.EM_;
        for (unsigned i=0;i<emap_->edgels.size();i++)
        {
                dbdet_spatial_temporal_options_sptr temp=new dbdet_spatial_temporal_options();
                for(cvlet_list_iter iter=cmap.map_[i].begin();iter!=cmap.map_[i].end();iter++)
                    temp->st_options_[*iter]=new dbdet_temporal_options(*iter);
                map_.push_back(temp);
                //label_map_.push_back(-1);
        }

        dt_=new dbdet_delaunay_map(emap_->edgels);
}


dbdet_temporal_map::dbdet_temporal_map(dbdet_curvelet_map &  cmap, dbdet_temporal_map_params & params):params_(params)
{
    emap_=cmap.EM_;
    vcl_vector<bool> iscurvelet;

    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        derivatives_map_[emap_->edgels[i]]=new dbdet_spatial_temporal_derivatives();
        //dbdet_spatial_temporal_options_sptr temp=new dbdet_spatial_temporal_options();
        for(cvlet_list_iter iter=cmap.map_[i].begin();iter!=cmap.map_[i].end();iter++)
        {
            derivatives_map_[emap_->edgels[i]]->refcs_.push_back(*iter);
        }
        //map_.push_back(temp);
        //label_map_.push_back(-1);

        if(cmap.map_[i].size()>0)
            iscurvelet.push_back(true);
        else
            iscurvelet.push_back(false);
    }
    dt_=new dbdet_delaunay_map(emap_->edgels,iscurvelet);
}

bool
dbdet_temporal_map::form_bundles()
{
    vcl_cout << "Building pairs ...";
    vcl_cout.flush();

    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator mem_iter;
    int winsize=params_.tneighbor;

    //: iterate over edges in the central edge-map.
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        vcl_cout<<i<<" ";
        //: egdel to form temporal groupings with.
        dbdet_edgel * cur_edgel=emap_->edgels[i];
        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
        {
            dbdet_temporal_options * temporaloption=cv_iter->second;
            //get the grid coordinates of this edgel
            unsigned ii = dbdet_round(cur_edgel->pt.x());
            unsigned jj = dbdet_round(cur_edgel->pt.y());
            int cnt = 0; //count the # of neighboring edgels

            //: iterate over neighboring frames
            vcl_map<int,dbdet_edgemap_sptr>::iterator niter=neighbor_emap_.begin();
            for(;niter!=neighbor_emap_.end();niter++)
            {
                
                //: compute the radius of neighborhood in each frame
                int nrad=vcl_abs(niter->first)*winsize;
                dbdet_edgemap_sptr edgemap=niter->second;
                //iterate over the cell neighborhoods around this edgel that contains the full edgel neighborhood
                for (unsigned xx=ii-nrad; xx<=ii+nrad; xx++){
                    for (unsigned yy=jj-nrad; yy<=jj+nrad ; yy++){
                        if (xx<0 || xx>=edgemap->ncols() || yy<0 || yy>=edgemap->nrows())
                            continue;
                        //: # of edges in each cell whih should not be ususally more than 1
                        unsigned N = edgemap->cell(xx, yy).size();
                        for (unsigned k=0; k<N; k++){
                            dbdet_edgel* eB = edgemap->cell(xx, yy)[k];
                            cnt++;
                            // obtain the curvelets anchored on th edge eB
                            cvlet_list eb_curvlet_list=neighbor_cmap_[niter->first]->curvelets(eB->id);
                            for(cvlet_list_iter cvlistiter=eb_curvlet_list.begin();cvlistiter!=eb_curvlet_list.end();cvlistiter++)
                            {
                                if(params_.type==0)
                                {
                                    //: check has to be performed to see if it is in  
                                    dbdet_temporal_curvature_velocity_model * model;
                                    //: make sure anchor egdel is the first edge for input the function below.
                                    model=new dbdet_temporal_curvature_velocity_model(cv_iter->first,(*cvlistiter),niter->first);
                                    if(model->isvalid())
                                    {
                                        vcl_map<int, dbdet_curvelet* > tbundle;
                                        tbundle[niter->first]=(*cvlistiter);
                                        dbdet_temporal_bundle b( tbundle,model);
                                        b.refc=cv_iter->first;
                                        cv_iter->second->add_temporal_bundle(b);
                                    }
                                }
                                else
                                {
                                    //: check has to be performed to see if it is in  
                                    dbdet_temporal_normal_velocity_model * model;
                                    //: make sure anchor egdel is the first edge for input the function below.
                                    model=new dbdet_temporal_normal_velocity_model(cv_iter->first,(*cvlistiter),niter->first);
                                    model->ref_curvelet=cv_iter->first;
                                    if( model->isvalid())
                                    {
                                        vcl_map<int, dbdet_curvelet* > tbundle;
                                        tbundle[niter->first]=(*cvlistiter);
                                        dbdet_temporal_bundle b( tbundle,model);
                                        b.refc=cv_iter->first;
                                        cv_iter->second->add_temporal_bundle(b);
                                    }
                                }
                            }
                        }
                    }
                }
                if(params_.type==1)
                {
                    cv_iter->second->refine_bundles(niter->first);
                }


            }
         }
    }

    vcl_cout<<"Finsihed forming pairs \n";
    vcl_cout<<"Merging the pairs \n";
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        vcl_cout<<i<<" ";
        //: egdel to form temporal groupings with.
        dbdet_edgel * cur_edgel=emap_->edgels[i];
        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
        {
            cv_iter->second->merge_the_models();
        }
    }
    form_spatial_bundles();
    compute_velocity_3D();
  
    

    return true;
}


void dbdet_temporal_map::update_delaunay_edge_weights()
{
    for(unsigned i=0;i<dt_->delaunay_edges_.size();i++)
    {
        int id1=dt_->delaunay_edges_[i].node1_id_;
        int id2=dt_->delaunay_edges_[i].node2_id_;

        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter1;
        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;

        double incw=0;
        for(iter1=map_[id1]->st_options_.begin();iter1!=map_[id1]->st_options_.end();iter1++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator biter1;
            for(biter1=iter1->second->list_options_.begin();biter1!=iter1->second->list_options_.end();biter1++)
            {
                if(!biter1->used_ && biter1->bundle_.size()>1)
                {
                for(iter2=map_[id2]->st_options_.begin();iter2!=map_[id2]->st_options_.end();iter2++)
                {
                    vcl_list<dbdet_temporal_bundle>::iterator biter2;
                    for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
                    {
                         if(!biter2->used_ && biter2->bundle_.size()>1)
                {
                        if(bundles_intersect((*biter1),(*biter2)))
                            incw+=biter1->nweight_ * biter2->nweight_;
                         }
                    }
                }
                }
            }
        }

        //: not sure if sqrt is a good idea or not ???
        dt_->delaunay_edges_[i].weight_=vcl_sqrt(incw);

    }
}




bool 
dbdet_temporal_map::pass_through_link(vcl_list<dbdet_curvelet*> clist,dbdet_curvelet * c )
{
    //go over all the curvelets
    vcl_list<dbdet_curvelet*>::iterator cv_it = clist.begin();
    for (; cv_it != clist.end(); cv_it++)
    {
      dbdet_curvelet* cvlet = (*cv_it);


        //determine if this is duplicate
        if (cvlet->edgel_chain.size() != c->edgel_chain.size())
          continue;
        
        bool duplicate = true; //reset flag
        for (unsigned k=0; k<cvlet->edgel_chain.size(); k++)
          duplicate = duplicate && (cvlet->edgel_chain[k]==c->edgel_chain[k]);
        if(duplicate)
            return duplicate;
    }

    return false;

}
bool dbdet_temporal_map::form_contour_groupings()
{
    vcl_cout<<"Form Contours : \n";

        return true;
}

void dbdet_temporal_map::extract_one_chains_from_the_link_graph()
{
 
  //now look for edgel chains
  //Rules: 
  //    (a) start from an edgel that is locally legal
  //    (b) trace in both directions until an illegal edgel is reached
  //    (c) prune out short chains

  for (unsigned i=0; i<emap_->edgels.size(); i++)
  {
    edge_to_one_chain[emap_->edgels[i]->id]=-1;
  }
  for (unsigned i=0; i<emap_->edgels.size(); i++)
  {
    dbdet_edgel* first_edgel = emap_->edgels[i];

    //if it's already linked, ignore
    if (elg_.linked[first_edgel->id])
      continue;

    // Check edgel to see if it is legal to start a chain here
    if (elg_.edgel_is_legal_first_edgel(first_edgel)) 
    {
      //start a chain from this edgel
      dbdet_edgel_chain* chain = new dbdet_edgel_chain();

      //add the first edgel to the chain
      chain->push_back(first_edgel);
      elg_.linked[first_edgel->id] = true; //mark it as linked

      //now start tracing FORWARD from its child
      dbdet_edgel* eA = elg_.cLinks[first_edgel->id].front()->ce;
      chain->push_back(eA);

      //trace FORWARD through the link graph until an illegal or terminal edgel is reached
      while (elg_.edgel_is_legal(eA))
      {
        // Mark the last edgel as linked.

        // Note:
        //   By doing this here, we can get the edgels at junctions to be added to the contour 
        //   without marking them as linked. This means that other contours arriving at
        //   the junction can also claim the junction edgel as being on their chains.
        elg_.linked[eA->id] = true;
       
        //is this a terminal edgel?
        if (elg_.cLinks[eA->id].size()==0)
          break; //terminate chain

        //else advance to child node
        eA = elg_.cLinks[eA->id].front()->ce;
        chain->push_back(eA);
      }

      //Note: Junction edgels will still be marked as unlinked after the tracing is done!

      //now start tracing BACKWARD from the first edgel

      //first determine if this is a closed contour
      //with closed contours, the chain might already include the first edgel twice      
      if (eA != first_edgel){
        //not a closed contour, start tracing
        eA = elg_.pLinks[first_edgel->id].front()->pe;
        chain->push_front(eA);
      }

      while (elg_.edgel_is_legal(eA))
      {
        // Mark the last edgel as linked.
        elg_.linked[eA->id] = true;

        //is this a terminal edge?
        if (elg_.pLinks[eA->id].size()==0)
          break; //terminate chain

        //else advance to parent node
        eA = elg_.pLinks[eA->id].front()->pe;
        chain->push_front(eA);
      }

      //save the current chain on the curve fragment graph
      //curve_frag_graph_.insert_fragment(chain);
      vcl_deque<dbdet_edgel*>::iterator lit =chain->edgels.begin();

      for(;lit!=chain->edgels.end();lit++)
        edge_to_one_chain[(*lit)->id]=one_chains.size();

      one_chains.push_back(chain);

    }
          

  }
}




bool dbdet_temporal_map::form_spatial_bundles()
{
    vcl_cout<<"Form Spatial Bundles Along the Link Graph : \n";
    vcl_vector<vcl_map<dbdet_curvelet*, dbdet_temporal_options* > >::iterator iter;
    for(unsigned i=0;i<map_.size();i++)
    {
        vcl_cout<<i<<" ";
        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter_hyp;
        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator link_iter_hyp;

        vcl_list<dbdet_temporal_bundle>::iterator iter_options;
        vcl_list<dbdet_temporal_bundle>::iterator link_iter_options;

        for(iter_hyp=map_[i]->st_options_.begin();iter_hyp!=map_[i]->st_options_.end();iter_hyp++)
        {    
            vcl_list<dbdet_temporal_bundle>::iterator biter;
            //: any arbitrary starting edge
            int id=iter_hyp->first->ref_edgel->id;
            for(iter_options=iter_hyp->second->list_options_.begin();iter_options!=iter_hyp->second->list_options_.end();iter_options++)
            {

                    //iter_hyp->second->st_bundles_.push_back(*biter);

                    dbdet_link_list clinks=elg_.cLinks[id];
                    dbdet_link_list plinks=elg_.pLinks[id];

                    dbdet_link_list::iterator link_iter;
                    //: going through all the child links.
                    for(link_iter=clinks.begin();link_iter!=clinks.end();link_iter++)
                    {
                        int edgeid=(*clinks.begin())->ce->id;
                        if(pass_through_link((*link_iter)->curvelets, link_iter_options->refc))
                        {
                            
                        }
                        //: going through bundle son 2nd edge.
                        //for(link_iter_hyp=map_[edgeid]->st_options_.begin();link_iter_hyp!=map_[edgeid]->st_options_.end();link_iter_hyp++)
                        //{  
                        //    for(link_iter_options=link_iter_hyp->second->list_options_.begin();link_iter_options!=link_iter_hyp->second->list_options_.end();link_iter_options++)
                        //    {
                        //        if(pass_through_link((*link_iter)->curvelets, link_iter_options->refc))
                        //        {
                        //            dbdet_temporal_bundle spatial_b(*iter_options);

                        //            dbdet_temporal_normal_velocity_model * m1,*m2;
                        //            if( m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (iter_options->model_))
                        //                if(m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (link_iter_options->model_))
                        //                {
                        //                    if(m1->compute_beta_s(*m2))
                        //                        iter_hyp->second->st_bundles_.push_back(spatial_b);
                        //                }
                        //        }
                        //    }               
                        //}
                    }
                        
                    for(link_iter=plinks.begin();link_iter!=plinks.end();link_iter++)
                    {
                        int edgeid=(*plinks.begin())->pe->id;
                        //: going through bundle son 2nd edge.
                        for(link_iter_hyp=map_[edgeid]->st_options_.begin();link_iter_hyp!=map_[edgeid]->st_options_.end();link_iter_hyp++)
                        {  
                            for(link_iter_options=link_iter_hyp->second->list_options_.begin();link_iter_options!=link_iter_hyp->second->list_options_.end();link_iter_options++)
                            {
                                if(pass_through_link((*link_iter)->curvelets, link_iter_options->refc))
                                {
                                    dbdet_temporal_bundle spatial_b(*iter_options);

                                    dbdet_temporal_normal_velocity_model * m1,*m2;
                                    if( m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (iter_options->model_))
                                        if(m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (link_iter_options->model_))
                                        {
                                            if(m1->compute_beta_s(*m2))
                                                iter_hyp->second->st_bundles_.push_back(spatial_b);
                                        }
                                }
                            }               
                        }
                    }
                        

                    //for(link_iter=plinks.begin();link_iter!=plinks.end();link_iter++)
                    //{
                    //    int edgeid=(*plinks.begin())->pe->id;
                    //    //: going through bundle son 2nd edge.
                    //    for(link_iter_hyp=map_[edgeid]->st_options_.begin();link_iter_hyp!=map_[edgeid]->st_options_.end();link_iter_hyp++)
                    //    {  
                    //        for(link_iter_options=link_iter_hyp->second->options_.begin();link_iter_options!=link_iter_hyp->second->options_.end();link_iter_options++)
                    //        {
                    //            vcl_list<dbdet_temporal_bundle>::iterator link_biter;
                    //            for(link_biter=link_iter_options->second.begin();link_biter!=link_iter_options->second.end();link_biter++)
                    //            {
                    //                if(link_biter->bundle_.size()>=2)
                    //                {
                    //                    if(pass_through_link((*link_iter)->curvelets, link_biter->refc))
                    //                    {
                    //                        dbdet_temporal_bundle spatial_b=(*biter);
                    //                        dbdet_temporal_normal_velocity_model * m1,*m2;
                    //                        if( m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (biter->model_))
                    //                            if(m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (link_biter->model_))
                    //                            {
                    //                                spatial_b.model_=new dbdet_temporal_normal_velocity_model(*m1,*m2);
                    //                                iter_hyp->second->st_bundles_.push_back(spatial_b);
                    //                            }
                    //                    }
                    //                }
                    //            }
                    //        }
                    //    }
                    //}
                }
            }
        }
    
    return true;
}




bool dbdet_temporal_map::compute_velocity_3D()
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    for(unsigned i=0;i<map_.size();i++)
    {
        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
            vcl_map<int, vcl_list<dbdet_temporal_bundle> >::iterator map_iter;
            //: iterating through all the bundles
            //vcl_cout<<"No of Hypothesis are : "<<cv_iter->second->second_list_options_.size()<<"\n";
            {
                for(list_iter=cv_iter->second->st_bundles_.begin();list_iter!=cv_iter->second->st_bundles_.end();list_iter++)
                {
                    if( dbdet_temporal_normal_velocity_model * m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter->model_))
                    {
//                        m1->compute_V_model();
                            
                    }
                }
            }
        }
    }
    return true;
}

//bool dbdet_temporal_map::compute_RANSAC_V()
//
//{
//    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter_i;
//    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter_j;
//    unsigned num_trials=50000;
//
//    vbl_array_2d<int> hist2d(10,10);
//
//    double thetamin=-vnl_math::pi_over_2;
//    double thetamax=vnl_math::pi_over_2;
//
//    double phimin=0;
//    double phimax=2*vnl_math::pi;
//
//    hist2d.fill(0);
//    for(unsigned k=0;k<num_trials;)
//    {   
//        vnl_random rand;
//        int i=rand.lrand32(0,map_.size()-1);
//        int j=rand.lrand32(0,map_.size()-1);
//
//        for(cv_iter_i=map_[i]->st_options_.begin();cv_iter_i!=map_[i]->st_options_.end();cv_iter_i++) 
//        {
//            for(cv_iter_j=map_[j]->st_options_.begin();cv_iter_j!=map_[j]->st_options_.end();cv_iter_j++)
//            {
//                vcl_list<dbdet_temporal_bundle>::iterator list_iter_i;
//                vcl_list<dbdet_temporal_bundle>::iterator list_iter_j;
//                for(list_iter_i=cv_iter_i->second->st_bundles_.begin();list_iter_i!=cv_iter_i->second->st_bundles_.end();list_iter_i++)
//                {
//                    if( dbdet_temporal_normal_velocity_model * m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_i->model_))
//                    {
//                        for(list_iter_j=cv_iter_j->second->st_bundles_.begin();list_iter_j!=cv_iter_j->second->st_bundles_.end();list_iter_j++)
//                        {
//                            if( dbdet_temporal_normal_velocity_model * m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_j->model_))
//                            {
//                                if(m1->isvalid() && m2->isvalid())
//                                {
//                                float theta=0;
//                                float phi=0;
//                                compute_V(m1,m2,theta,phi);
//                                int indexi=int((theta-thetamin)*10/(thetamax-thetamin));
//                                int indexj=int((phi)*10/phimax);
//                                hist2d(indexi,indexj)++;
//                                k++;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }
//    vcl_cout<<hist2d;
//    return true;
//}
//
bool dbdet_temporal_map::compute_V(dbdet_temporal_normal_velocity_model * m1,dbdet_temporal_normal_velocity_model * m2 , float & theta, float & phi)
{
    for(unsigned i=0;i<1;i++)
    {
        for(unsigned j=0;j<1;j++)
        {
            vnl_matrix<double> M(2,2);
            //: foour cases
            M(0,0)=m1->b[i];M(0,1)=m1->c[i];
            M(1,0)=m2->b[j];M(1,1)=m2->c[j];

            vnl_vector<double> X(2);
            X(0)=-m1->a[i];X(1)=-m2->a[j];
            vnl_svd<double> svd(M);
            vnl_vector<double> sol=svd.solve(X);


            double vx=1;
            double vy=sol[0];
            double vz=sol[1];

            double nc=vcl_sqrt(vx*vx+vy*vy+vz*vz);

            vx/=nc;
            vy/=nc;
            vz/=nc;

            theta=vcl_atan(vz/vcl_sqrt(vx*vx+vy*vy));
            phi=vcl_atan(vy/vx);
            vcl_cout<<"("<<vx<<","<<vy<<","<<vz<<")"<< " ("<<theta<<","<<phi<<")"<<"\n";
        }
    }
    return true;

}


bool dbdet_temporal_map::compute_V(dbdet_temporal_normal_velocity_model * m1, dbdet_temporal_normal_velocity_model * m2,
                                   dbdet_temporal_normal_velocity_model * m3,float & theta, float & phi)
{
    double minerr=1e5;
    double m_vx,m_vy,m_vz;

    for(unsigned i=0;i<2;i++){
        for(unsigned j=0;j<2;j++){
            for(unsigned k=0;k<2;k++)
            {
                vnl_matrix<double> M(3,3);

                M(0,0)=m1->a[i];M(0,1)=m1->b[i];M(0,2)=m1->c[i];
                M(1,0)=m2->a[j];M(1,1)=m2->b[j];M(1,2)=m2->c[j];
                M(2,0)=m3->a[k];M(2,1)=m3->b[k];M(2,2)=m3->c[k];


                vnl_svd<double> svd(M);
                vnl_vector<double> sol=svd.nullvector();


                double vx=sol[0];
                double vy=sol[1];
                double vz=sol[2];

                double err=0;

                double nc=vcl_sqrt(vx*vx+vy*vy+vz*vz);


                vx/=nc;
                vy/=nc;
                vz/=nc;
                
                err+=(m1->a[i]*vx+m1->b[i]*vy+m1->c[i]*vz)*(m1->a[i]*vx+m1->b[i]*vy+m1->c[i]*vz);
                err+=(m2->a[j]*vx+m2->b[j]*vy+m2->c[j]*vz)*(m2->a[j]*vx+m2->b[j]*vy+m2->c[j]*vz);
                err+=(m3->a[k]*vx+m3->b[k]*vy+m3->a[k]*vx+m3->b[k]*vy+m3->c[k]*vz);

                if(err<minerr)
                {
                    minerr=err;
                    m_vx=vx;
                    m_vy=vy;
                    m_vz=vz;     
                }

            }
        }
    }
    if(minerr>0.2)
        return false;
    theta=vcl_atan(m_vz/vcl_sqrt(m_vx*m_vx+m_vy*m_vy));
    phi=vcl_atan(m_vy/m_vx);

    //vcl_cout<<"("<<m_vx<<","<<m_vy<<","<<m_vz<<")\n";
    return true;
}


bool dbdet_temporal_map::bundles_intersect(dbdet_temporal_bundle & b1,dbdet_temporal_bundle & b2)
{
    double err_epsilon=0.35;
    if(dbdet_temporal_curvature_velocity_model * tmodel1=dynamic_cast<dbdet_temporal_curvature_velocity_model *> (b1.model_))
    {
        if(dbdet_temporal_curvature_velocity_model * tmodel2=dynamic_cast<dbdet_temporal_curvature_velocity_model *> (b2.model_))
        {
            if(tmodel1->is_model_intersect(tmodel2, err_epsilon))
            {
                return true;
            }
        }
    }
    return false;
}
vcl_vector<int> dbdet_temporal_map::find_path(dbdet_curvelet * s)
{

    //kthresh=0.15;

    int id=s->ref_edgel->id;

    vcl_vector<int> idsvisited;
    vcl_list<int> idstobevisited;

    //idsvisited.push_back(id);
    idstobevisited.push_back(id);
    bool flag=true;
    while(idstobevisited.size()>0)
    {
        int curr_id=idstobevisited.front();
        dbdet_link_list clinks=elg_.cLinks[curr_id];
        for(dbdet_link_list_iter iter=clinks.begin();iter!=clinks.end();iter++)
        {
            int edgeid=(*iter)->ce->id;
            vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator citer;
            int potentialid=(*iter)->ce->id;
            vcl_vector<int>::iterator iditer = find(idsvisited.begin(), idsvisited.end(), potentialid);
            if(iditer==idsvisited.end())
                idstobevisited.push_back(potentialid);
        }
        vcl_vector<int>::iterator iditer = find(idsvisited.begin(), idsvisited.end(), curr_id);
        if(iditer==idsvisited.end())
            idsvisited.push_back(curr_id);
        idstobevisited.pop_front();
    }

    //: parents
    idstobevisited.push_back(id);
    while(idstobevisited.size()>0)
    {
        int curr_id=idstobevisited.front();
        dbdet_link_list plinks=elg_.pLinks[curr_id];
        for(dbdet_link_list_iter iter=plinks.begin();iter!=plinks.end();iter++)
        {
            int edgeid=(*iter)->pe->id;
            vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator citer;
            int potentialid=(*iter)->pe->id;
            vcl_vector<int>::iterator iditer = find(idsvisited.begin(), idsvisited.end(), potentialid);
            if(iditer==idsvisited.end())
                idstobevisited.push_back(potentialid);
        }
        vcl_vector<int>::iterator iditer = find(idsvisited.begin(), idsvisited.end(), curr_id);
        if(iditer==idsvisited.end())
            idsvisited.push_back(curr_id);

        idstobevisited.pop_front();
    }
    return idsvisited;

}
//: to find out which of the bundles have correpspondence only on one side.
bool dbdet_temporal_map::find_onesided_temporal_bundles()
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    //: iterate over edges in the central edge-map.
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
            {   
                if(!list_iter->used_ && list_iter->bundle_.size()>1)
                {
                    vcl_map<int, dbdet_curvelet *>::iterator mem_iter;  
                    bool negflag=false;
                    bool posflag=false;
                    for(mem_iter=list_iter->bundle_.begin();mem_iter!=list_iter->bundle_.end();mem_iter++)
                    {
                        if(mem_iter->first>0)
                            posflag=true;
                        else
                            negflag=true;
                    }
                    if((posflag^negflag))
                    {
                        list_iter->one_sided_=true;

                    }

                }
            }
        }
    }
    return true;
}

//bool dbdet_temporal_map::pruning_neighbor_links()
//{
//    if(!dt_)
//        return false;
//
//
//    for(unsigned k=0;k<dt_->neighbor_map_.size();k++)
//    {   
//        int edgeid1=k;
//        vcl_vector<int>::iterator iter;
//        vcl_vector<int> temp(0);
//        for(iter=dt_->neighbor_map_[k].begin();iter!=dt_->neighbor_map_[k].end();iter++)
//        {
//
//            int edgeid2=(*iter);    
//            if(edges_intersect(edgeid1,edgeid2))
//            {
//                temp.push_back(edgeid2);
//            }
//                                
//        }
//        valid_neighbor_map_.push_back(temp);
//    }
//
//    return true;
//}
//
bool dbdet_temporal_map::edges_intersect(int id1,int id2)
{
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter1;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;
    
    for(iter1=map_[id1]->st_options_.begin();iter1!=map_[id1]->st_options_.end();iter1++)
    {
        vcl_list<dbdet_temporal_bundle>::iterator biter1;
        for(biter1=iter1->second->list_options_.begin();biter1!=iter1->second->list_options_.end();biter1++)
        {
            for(iter2=map_[id2]->st_options_.begin();iter2!=map_[id2]->st_options_.end();iter2++)
            {
                vcl_list<dbdet_temporal_bundle>::iterator biter2;
                for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
                {
                    if(bundles_intersect((*biter1),(*biter2)))
                        return true;
                }
            }
        }
    }
    return false;

}

//void dbdet_temporal_map::relaxation()
//{
//    //: first step is to initialize the hypotheses per edge
//    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
//    for (unsigned i=0;i<map_.size();i++)
//    {
//        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
//        {
//            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
//            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
//            { 
//                if(list_iter->group_id_>0)
//                    list_iter->weight_=1;//cf_list[list_iter->group_id_].memids.size();
//            }
//            //cv_iter->second->normalize_weights();
//        }
//        map_[i]->normalize_weights();
//    }
//
//    //: second step is to update the hypothesis as per the neighborhood
//    unsigned int maxiter=params_.no_of_relaxation_iterations; 
//    unsigned int niter=0;
//    //while(niter<maxiter)
//    //{
//    //    vcl_cout<<"\nRelaxation iteration # "<<niter+1;
//    //    for(unsigned i=0;i<dt_->delaunay_edges_.size();i++)
//    //    {
//    //        int id1=dt_->delaunay_edges_[i].node1_id_;
//    //        int id2=dt_->delaunay_edges_[i].node2_id_;
//    //        double cnt=0;
//    //        for(cv_iter=map_[id1]->st_options_.begin();cv_iter!=map_[id1]->st_options_.end();cv_iter++)
//    //        {
//    //            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
//    //            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
//    //            {   
//    //                if(!list_iter->used_ && list_iter->bundle_.size()>1) 
//    //                {
//    //                    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;
//    //                    for(iter2=map_[id2]->st_options_.begin();iter2!=map_[id2]->st_options_.end();iter2++)
//    //                    {
//    //                        vcl_list<dbdet_temporal_bundle>::iterator biter2;
//    //                        for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
//    //                        {
//    //                            if(biter2->group_id_!=list_iter->group_id_)
//    //                                if(bundles_intersect((*list_iter),(*biter2)))
//    //                                    list_iter->weight_+=biter2->nweight_;//*(1+dt_->delaunay_edges_[i].weight_);
//    //                            
//
//    //                        }
//    //                    }
//    //                }
//    //            }
//    //        }
//    //    }
//
//    //    //: normalizing 
//    //    for (unsigned i=0;i<map_.size();i++)
//    //    {
//    //        vcl_vector<int> ids=dt_->neighbor_map_[i];
//    //        map_[i]->normalize_weights();
//    //        if(ids.size()>0)
//    //        {
//    //            for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
//    //            {
//
//    //                //cv_iter->second->normalize_weights();
//    //                cv_iter->second->sort_by_weight();
//    //            }
//    //        }
//    //    }
//
//    //    double alpha=0.1;
//    //    //: weight it along the contour.
//    //    for (unsigned i=0;i<map_.size();i++)
//    //    {
//    //        double cnt=0;
//    //        for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
//    //        {
//    //            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
//    //            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
//    //            {   
//    //                if(!list_iter->used_ && list_iter->bundle_.size()>1) 
//    //                {
//    //                    double incw=0;
//    //                    if(list_iter->group_id_>0)
//    //                    {  
//    //                        vcl_vector<int> mems=cf_list[list_iter->group_id_].memids;
//    //                        for(unsigned k=0;k<mems.size();k++)
//    //                        {
//
//    //                            vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;
//    //                            for(iter2=map_[mems[k]]->st_options_.begin();iter2!=map_[mems[k]]->st_options_.end();iter2++)
//    //                            {
//    //                                vcl_list<dbdet_temporal_bundle>::iterator biter2;
//    //                                for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
//    //                                {
//    //                                    if(bundles_intersect((*list_iter),(*biter2)))
//    //                                        incw+=biter2->nweight_;
//    //                                }
//    //                            }
//    //                        }
//
//    //                        incw*=0.01;
//    //                        //if(mems.size()>10)
//    //                        //    incw/=(float)(10);
//    //                        //else 
//    //                            /*if(mems.size()>0)
//    //                            incw/=(float)(mems.size()/2);*/
//    //                    }
//    //                    list_iter->weight_+=incw;
//    //                }
//    //            }
//    //        }
//    //    }
//    //    //double beta=0.1;
//    //    ////: weight it along spatial model.
//    //    //for (unsigned i=0;i<map_.size();i++)
//    //    //{
//    //    //    double cnt=0;
//    //    //    for(cv_iter=map_[i].begin();cv_iter!=map_[i].end();cv_iter++)
//    //    //    {
//    //    //        vcl_list<dbdet_temporal_bundle>::iterator list_iter;
//    //    //        for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
//    //    //        {   
//    //    //            if(!list_iter->used_ && list_iter->bundle_.size()>1) 
//    //    //            {
//    //    //                double incw=0;
//    //    //                for(unsigned k=0;k<list_iter->refc->edgel_chain.size();k++)
//    //    //                    {
//
//    //    //                        vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;
//    //    //                        for(iter2=map_[list_iter->refc->edgel_chain[k]->id].begin();iter2!=map_[list_iter->refc->edgel_chain[k]->id].end();iter2++)
//    //    //                        {
//    //    //                            vcl_list<dbdet_temporal_bundle>::iterator biter2;
//    //    //                            for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
//    //    //                            {
//    //    //                                if(bundles_intersect((*list_iter),(*biter2)))
//    //    //                                    incw+=biter2->nweight_;
//    //    //                            }
//    //    //                        }
//    //    //                    }
//    //    //                list_iter->weight_+=incw;
//    //    //           }
//    //    //        }
//    //    //    }
//    //    //}
//    //    //: normalizing 
//    //    for (unsigned i=0;i<map_.size();i++)
//    //    {
//    //        double cnt=0;
//    //        vcl_vector<int> ids=dt_->neighbor_map_[i];
//    //        map_[i]->normalize_weights();
//    //        if(ids.size()>0)
//    //        {
//    //            for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
//    //            {
//    //                //cv_iter->second->normalize_weights();
//    //                cv_iter->second->sort_by_weight();
//    //            }
//    //        }
//    //    }
//    //    update_delaunay_edge_weights();
//    //    dt_->threshold_delaunay_edges(0.05);
//    //    dt_->recompute_delaunay();
//    //    update_delaunay_edge_weights();
//
//    //    niter++;
//    //}
//
//    while(niter<maxiter)
//    {
//         for (unsigned i=0;i<map_.size();i++)
//        {
//            double cnt=0;
//            for(cv_iter=map_[i]->st_options_.begin();cv_iter!=map_[i]->st_options_.end();cv_iter++)
//            {
//                vcl_list<dbdet_temporal_bundle>::iterator list_iter;
//                for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
//                {   
//                    if(!list_iter->used_ && list_iter->bundle_.size()>1) 
//                    {
//
//                        list_iter->weight_+=get_weight_across_neighbors(&(*list_iter));
//                        list_iter->weight_+=get_weight_along_neighbors(&(*list_iter),7.0);
//
//                        //list_iter->weight_=vcl_pow(list_iter->weight_,1+vcl_pow((niter-1),0.07));
//
//                    }
//                }
//            }
//         }
//         for (unsigned i=0;i<map_.size();i++)
//        {
//            map_[i]->normalize_weights();
//         }
//        update_delaunay_edge_weights();
//        dt_->threshold_delaunay_edges(0.05);
//        dt_->recompute_delaunay();
//        update_delaunay_edge_weights();
//        niter++;
//    }
//
//
//}
//
double
dbdet_temporal_map::get_weight_across_neighbors(dbdet_temporal_bundle * b)
{
    double incw=0;
    int len_contour=5; // might be a parameter.

    vcl_vector<int> dneighbors=dt_->get_neighbors(b->refc->ref_edgel->id);
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;

    // to make sure that edges from the same contour are not accounted lot of times.
    vcl_map<int,double> cost_map;
    vcl_map<int,int> obs_map;
    for(unsigned i=0;i<dneighbors.size();i++)
    {
        for(cv_iter=map_[dneighbors[i]]->st_options_.begin();cv_iter!=map_[dneighbors[i]]->st_options_.end();cv_iter++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator list_iter;
            for(list_iter=cv_iter->second->list_options_.begin();list_iter!=cv_iter->second->list_options_.end();list_iter++)
            {   
                if(!list_iter->used_ && list_iter->bundle_.size()>1) 
                {
                    //: check if the neighbor is on the same contour as of the current hypothesis
                    if(b->group_id_!=list_iter->group_id_ && list_iter->group_id_ >=0)
                        if(cf_list[list_iter->group_id_].memids.size()>len_contour) // check if the hypthesis lie on a significant contour.
                            if(bundles_intersect((*list_iter),(*b)))
                            {
                                if(list_iter->group_id_<0)
                                    incw+=list_iter->nweight_;//*(1+dt_->delaunay_edges_[i].weight_);
                                else
                                {
                                    if(cost_map.find(list_iter->group_id_)!=cost_map.end())
                                    {
                                        cost_map[list_iter->group_id_]+=list_iter->nweight_;
                                        obs_map[list_iter->group_id_]++;
                                    }
                                    else
                                    {
                                        cost_map[list_iter->group_id_]=list_iter->nweight_;
                                        obs_map[list_iter->group_id_]=1;
                                    }
                                }

                            }
                }
            }
        }
        for(vcl_map<int,double>::iterator iter=cost_map.begin();iter!=cost_map.end();iter++)
        {
            incw+=iter->second/(float)obs_map[iter->first];
        }
    }
    return incw;
}

double
dbdet_temporal_map::get_weight_along_neighbors(dbdet_temporal_bundle * b, float scale)
{
    if(b->group_id_<0)
        return 0.0;

    double x0=b->refc->ref_edgel->pt.x();
    double y0=b->refc->ref_edgel->pt.y();

    double scale_sqr=scale*scale;

    double incw=0;
    vcl_vector<int> mems=cf_list[b->group_id_].memids;
    for(unsigned k=0;k<mems.size();k++)
    {
        double x1=emap_->edgels[mems[k]]->pt.x();
        double y1=emap_->edgels[mems[k]]->pt.y();

        if((x0-x1)*(x0-x1)+(y0-y1)*(y0-y1)<scale_sqr)
        {
            vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator iter2;
            for(iter2=map_[mems[k]]->st_options_.begin();iter2!=map_[mems[k]]->st_options_.end();iter2++)
            {
                vcl_list<dbdet_temporal_bundle>::iterator biter2;
                for(biter2=iter2->second->list_options_.begin();biter2!=iter2->second->list_options_.end();biter2++)
                {
                    if(bundles_intersect(*b,(*biter2)))
                        incw+=biter2->nweight_;
                }
            }
        }
    }

    return incw;
}


bool dbdet_temporal_map::optimize()
{
    int num=0;
    vcl_map<int,Graph::node_id> mapping;
    Graph g;
    for(unsigned i=0;i<map_.size();i++)
    {
        vcl_vector<int> ids=dt_->get_neighbors(i);
        if(ids.size()>0)
        {
            mapping[i]=g.add_node();
            g.add_tweights(mapping[i],1,1);
            num++;
        }
    }   


    for(unsigned i=0;i<dt_->delaunay_edges_.size();i++)
        {
            int id1=dt_->delaunay_edges_[i].node1_id_;
            int id2=dt_->delaunay_edges_[i].node2_id_;
            g.add_edge(mapping[id1],mapping[id2],dt_->delaunay_edges_[i].weight_,dt_->delaunay_edges_[i].weight_);
        }
   
    vcl_cout<<"The flow is : \n"<<g.maxflow();

    for(unsigned i=0;i<map_.size();i++)
    {
        if(mapping.find(i)!=mapping.end())
            label_map_[i]=g.what_segment(mapping[i]);
        else
            label_map_[i]=-1;
    }
    return true;
}

bool dbdet_temporal_map::compute_motion_for_triangles()
{
motion_triangle_.clear();
    if(!dt_)
        return false;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter_i;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter_j;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter_k;
        vbl_array_2d<int> hist2d(10,10);

    

    double thetamin=-vnl_math::pi_over_2;
    double thetamax=vnl_math::pi_over_2;

    double phimin=-vnl_math::pi_over_2;
    double phimax=vnl_math::pi_over_2;

    hist2d.fill(0);


    for(unsigned n=0;n<dt_->triangles_.size();n++)
    {
        dbdet_delaunay_triangle c=dt_->triangles_[n];
        int i=c.node1_id_;
        int j=c.node2_id_;
        int k=c.node3_id_;
        vcl_list<dbdet_3D_velocity_model> list_per_tri;
        for(cv_iter_i=map_[i]->st_options_.begin();cv_iter_i!=map_[i]->st_options_.end();cv_iter_i++)
        {
            for(cv_iter_j=map_[j]->st_options_.begin();cv_iter_j!=map_[j]->st_options_.end();cv_iter_j++)
            {
                for(cv_iter_k=map_[k]->st_options_.begin();cv_iter_k!=map_[k]->st_options_.end();cv_iter_k++)
                {
                    vcl_list<dbdet_temporal_bundle>::iterator list_iter_i;
                    vcl_list<dbdet_temporal_bundle>::iterator list_iter_j;
                    vcl_list<dbdet_temporal_bundle>::iterator list_iter_k;

                    for(list_iter_i=cv_iter_i->second->st_bundles_.begin();list_iter_i!=cv_iter_i->second->st_bundles_.end();list_iter_i++)
                    {
                        if( dbdet_temporal_normal_velocity_model * m1=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_i->model_))
                        {
                            for(list_iter_j=cv_iter_j->second->st_bundles_.begin();list_iter_j!=cv_iter_j->second->st_bundles_.end();list_iter_j++)
                            {
                                if( dbdet_temporal_normal_velocity_model * m2=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_j->model_))
                                {
                                    for(list_iter_k=cv_iter_k->second->st_bundles_.begin();list_iter_k!=cv_iter_k->second->st_bundles_.end();list_iter_k++)
                                    {
                                        if( dbdet_temporal_normal_velocity_model * m3=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_k->model_))
                                        {
                                            if(m1->isvalid() && m2->isvalid() && m3->isvalid())
                                            {
                                                float theta=0;
                                                float phi=0;
                                                if(compute_V(m1,m3,m2,theta,phi))
                                                {
                                                int indexi=int((theta-thetamin)*10/(thetamax-thetamin));
                                                int indexj=int((phi-phimin)*10/(phimax-phimin));
                                                hist2d(indexi,indexj)++;
                                                //list_per_tri.push_back(dbdet_3D_velocity_model(theta,phi));
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        motion_triangle_[n]=list_per_tri;

    }
    vcl_cout<<hist2d;
    return true;
}




bool dbdet_temporal_map::confirm_3D_model(dbdet_3D_velocity_model & c, dbdet_temporal_bundle b, float & x1, float & y1 )
{
//    double vx=vcl_cos(c.theta)*vcl_cos(c.phi);double vy=vcl_cos(c.theta)*vcl_sin(c.phi);double vz=vcl_sin(c.theta);
//    
//    double u =(b.refc->ref_edgel->pt.x()-160)/(4*320);
//    double v =(b.refc->ref_edgel->pt.y()-120)/(4*240);
//    double theta =b.refc->ref_edgel->tangent;
//
//    double Ut_x=-vcl_sin(theta);                          double Un_x=vcl_cos(theta);
//    double Ut_y=vcl_cos(theta);                           double Un_y=vcl_sin(theta);
//    double Ut_z=u*vcl_sin(theta)-v*vcl_cos(theta);        double Un_z=-u*vcl_cos(theta)-v*vcl_sin(theta);
//    
//
//    double w=0;
//    if( dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*> (b.model_))
//    {
//        w=m->b0*(Un_x*vx+Un_y*vy+Un_z*vz)/(Ut_x*vx+Ut_y*vy+Ut_z*vz);
//
//        //: edge in a t-1 frame
////        double
//            x1=b.refc->ref_edgel->pt.x()+w*vcl_cos(theta)-m->b0*vcl_sin(theta);
//    //    double 
//            y1=b.refc->ref_edgel->pt.y()+w*vcl_sin(theta)+m->b0*vcl_cos(theta);
//        
//
//
//        //: edge in a t+1 frame
//        double xm1=b.refc->ref_edgel->pt.x()-w*vcl_cos(theta)+m->b0*vcl_sin(theta);
//        double ym1=b.refc->ref_edgel->pt.y()-w*vcl_sin(theta)-m->b0*vcl_cos(theta);
//        if(x1<320 && x1>=0 && y1<240 && y1>=0 && xm1<320 && xm1>=0 && ym1<240 && ym1>=0 )
//        {
//        if(neighbor_emap_[-1]->cell(vcl_floor(x1),vcl_floor(y1)).size()>0 &&
//            neighbor_emap_[1]->cell(vcl_floor(xm1),vcl_floor(ym1)).size()>0 )
//            return true;
//        }
//    }

    return false;
}


bool dbdet_temporal_map::extract_regions()
{
    for(unsigned j=0;j<emap_->edgels.size();j++)
    {
      dbdet_edgel * e=emap_->edgels[j];
      int id=e->id;  
      vcl_vector<int> triangles=dt_->triangle_map_[id];
      
      //: group the triangles which have edges on the same two contours.
      for(unsigned i=0;i<triangles.size();i++)
      {
          dbdet_region region_l;
          dbdet_delaunay_triangle *t1=&dt_->triangles_[triangles[i]];
          if(t1->is_grouped_)
              continue;
          int contour_id1=edge_to_one_chain[t1->node1_id_];
          int contour_id2=edge_to_one_chain[t1->node2_id_];
          int contour_id3=edge_to_one_chain[t1->node3_id_];
          int id1;
          int id2;
          if(contour_id1<0 || contour_id2<0 || contour_id3<0)
              continue;
          if(contour_id1==contour_id2 )
          {
            id1=contour_id1;
            id2=contour_id3;
          }
          else if(contour_id1==contour_id3 )
          {
              id1=contour_id3;
              id2=contour_id2;

          }
          else if(contour_id2==contour_id3 )
          {
              id1=contour_id1;
              id2=contour_id2;
          }
          else 
              continue;
          region_l.c_id1=id1;
          region_l.c_id2=id2;


          if(id1!=id2)
          {
              for(unsigned k=0;k<dt_->triangles_.size();k++)
              {
                  if(dt_->triangles_[k].is_grouped_)
                      continue;
                  dbdet_delaunay_triangle *t=&dt_->triangles_[k];
                  if( edge_to_one_chain[t->node1_id_]==edge_to_one_chain[t->node2_id_]&& edge_to_one_chain[t->node1_id_]==edge_to_one_chain[t->node3_id_])
                      continue;
                  if(edge_to_one_chain[t->node1_id_]==id1 || edge_to_one_chain[t->node1_id_]==id2){
                      if(edge_to_one_chain[t->node2_id_]==id1 || edge_to_one_chain[t->node2_id_]==id2){
                          if(edge_to_one_chain[t->node3_id_]==id1 || edge_to_one_chain[t->node3_id_]==id2)
                          {
                              region_l.triangles_.push_back(t->id_);
                              t->is_grouped_=true;

                          }
                      }
                  }
              }
          }
          else
          {
              for(unsigned k=0;k<dt_->triangles_.size();k++)
              {
                  if(dt_->triangles_[k].is_grouped_)
                      continue;
                  dbdet_delaunay_triangle *t=&dt_->triangles_[k];

                  if( edge_to_one_chain[t->node1_id_]==id1 && edge_to_one_chain[t->node2_id_]==id1 && 
                      edge_to_one_chain[t->node3_id_]==id1)
                  {
                      region_l.triangles_.push_back(t->id_);
                      t->is_grouped_=true;

                  }
              }
          }
          if(region_l.triangles_.size()>0)
          {
              region_l.id_=regions_.size();
              for(unsigned k=0;k<region_l.triangles_.size();k++)
              {
                  region_l.node_ids_.insert(dt_->triangles_[region_l.triangles_[k]].node1_id_);
                  region_l.node_ids_.insert(dt_->triangles_[region_l.triangles_[k]].node2_id_);
                  region_l.node_ids_.insert(dt_->triangles_[region_l.triangles_[k]].node3_id_);
              }
              regions_.push_back(region_l);
          }

      }
}
 return true;
}


int dbdet_temporal_map::left_or_right(dbdet_edgel * b, dbdet_edgel * a , dbdet_edgel * test)
{
    //: b-a
    double xba= b->pt.x()-a->pt.x();
    double yba= b->pt.y()-a->pt.y();


    

    //: test-a
    double xta= test->pt.x()-a->pt.x();
    double yta= test->pt.y()-a->pt.y();

    //: if test -a cross b-a is +/-

    double cp=xba*yta-yba*xta;

    if (cp>=0)
        return 1;
    else 
        return -1;


}

bool dbdet_temporal_map::compute_hough_transform()
{

    unsigned int len=20;
    dbdet_spherical_histogram_sptr hist2d=new dbdet_spherical_histogram();
    //hist2d.set_size(len+1,len+1);
    //hist2d.fill(0.0);

    double dtheta=vnl_math::pi/len;
    double dphi=2*vnl_math::pi/len;

    float thetamin=-vnl_math::pi_over_2;
    float thetamax=vnl_math::pi_over_2;

    float phimin=-vnl_math::pi;
    float phimax=vnl_math::pi;
/*
    for(unsigned k=0;k<regions_.size();k++)
    {
        if(regions_[k].ids_model.size()>0)
        {
            vnl_matrix<double> temp;
            temp.set_size(len+1,len+1);
            temp.fill(0.0);
            unsigned i=0;
            for(float theta=thetamin;theta<=thetamax;i++)
            {
                unsigned j=0;
                for(float phi=phimin;phi<=phimax;j++)
                {
                    temp(i,j)=vcl_min<float>(compute_probabilistic_error(regions_[k],vcl_cos(theta)*vcl_cos(phi),vcl_cos(theta)*vcl_sin(phi),vcl_sin(theta)),1);
                    phi+=dphi;
                }
                theta+=dtheta;
            }
            hist2d=hist2d+temp;
        }
        
    }   */
    for(unsigned k=0;k<regions_.size();k++)
        hist2d->operator +=(*(regions_[k].Vdist.ptr()));

        hist2d->print();
    //vcl_vector<vcl_pair<int,int> >  modes=find_modes(hist2d,3);

    //for(unsigned i=0;i<modes.size();i++)
    //{
    //    float theta_mode=thetamin+dtheta*modes[i].second;
    //    float phi_mode=phimin+dphi*modes[i].first;

    //    vnl_vector<float> v(3);
    //    v(0)=vcl_cos(theta_mode)*vcl_cos(phi_mode);
    //    v(1)=vcl_cos(theta_mode)*vcl_sin(phi_mode);
    //    v(2)=vcl_sin(theta_mode);
    //    v_modes_[i]=v;

    //    vcl_cout<<" Mode "<< v ;
    //    }


    return true;

}



bool dbdet_temporal_map::compute_initial_probability()
{
    for(unsigned k=0;k<regions_.size();k++)
    {
        vcl_map<int, vnl_vector<float> >::iterator iter;
        for(iter=v_modes_.begin();iter!=v_modes_.end();iter++)
        {
//            regions_[k].ps.push_back(computer_error_per_region(regions_[k],iter->second[0],iter->second[1],iter->second[2]));
        }
    }
    return true;
}
vcl_vector<vcl_pair<int,int> > 
dbdet_temporal_map::find_modes(vnl_matrix<float> & m, int bandwidth)
{

    vcl_vector<int> index_i(2*bandwidth+1);
    vcl_vector<int> index_j(2*bandwidth+1);
    vcl_vector<vcl_pair<int,int> > outputmodes;
    int h=bandwidth;
    for(unsigned i=0;i<m.rows();i++)
    {
        for(unsigned j=0;j<m.cols();j++)
        {
            int min_i=i-h;
            int max_i=i+h;    
            int min_j=j-h;
            int max_j=j+h;

            index_i.clear();
            index_j.clear();
           
            for(int p=min_i;p<=max_i;p++)
                if(p<0)
                    index_i.push_back(m.rows()+p);
                else if(p>=m.rows())
                    index_i.push_back(p-m.rows());
                else
                    index_i.push_back(p);
            for(int q=min_j;q<=max_j;q++)
                if(q<0)
                    index_j.push_back(m.cols()+q);
                else if(q>=m.cols())
                    index_j.push_back(q-m.cols());
                else
                    index_j.push_back(q);

            bool flag=true;
            for(unsigned p=0;p<index_i.size();p++)
                for(unsigned q=0;q<index_j.size();q++)
                    if(index_i[p]==i && index_j[q]==j)
                        continue;
                    else
                        if( m(index_i[p],index_j[q])>=m(i,j))
                            flag=false;
            if(flag)
               outputmodes.push_back(vcl_pair<int,int>(i,j));
        }
    }
    return outputmodes;
}
bool dbdet_temporal_map::compute_motion_per_region(double Vx, double Vy, double Vz)
{
    vcl_vector<float> xs;
    vcl_vector<float> ys;
    for(unsigned i=0;i<regions_.size();i++)
        vcl_cout<<computer_error_per_region(regions_[i],Vx,Vy,Vz,xs,ys)<<" ";
    return true;
}


double dbdet_temporal_map::compute_probabilistic_error(dbdet_region r, double Vx, double Vy, double Vz)
{
    //double max_p=0;
    //float epsilon=0.001;
    //vnl_vector<float> v(3);
    //v(0)=Vx;v(1)=Vy;v(2)=Vz;
    //float phi=vcl_atan2(dot_product<float>(v,r.v2),dot_product<float>(v,r.v1));

    //float a=vcl_cos(r.phi_min)*vcl_sin(phi)-vcl_sin(r.phi_min)*vcl_cos(phi);
    //float b=vcl_cos(phi)*vcl_sin(r.phi_max)-vcl_sin(phi)*vcl_cos(r.phi_max);
    //if(a>=0 && b>=0)
    //{
    //    vnl_vector<float> vmean=r.v1*vcl_cos(phi)+r.v2*vcl_sin(phi);
    //    vnl_matrix<float> cov=vcl_cos(phi)*vcl_cos(phi)*r.v1v1+
    //                                2*vcl_cos(phi)*vcl_sin(phi)*r.v1v2+
    //                                vcl_sin(phi)*vcl_sin(phi)*r.v2v2;

    //    vnl_vector<float> lambdas(3);
    //    vnl_matrix<float> Vs(3,3);

    //   if(!vnl_symmetric_eigensystem_compute(cov,Vs,lambdas))
    //     return -1;

    //   float diff_project1=dot_product<float>(Vs.get_column(0),(v-vmean));
    //   float diff_project2=dot_product<float>(Vs.get_column(1),(v-vmean));
    //   float diff_project3=dot_product<float>(Vs.get_column(2),(v-vmean));

    //   //if(lambdas(0)<0.1)
    //   //    lambdas(0)=0.1;
    //   //if(lambdas(1)<0.1)
    //   //    lambdas(1)=0.1;
    //   //if(lambdas(2)<0.1)
    //   //    lambdas(2)=0.1;


    //   vcl_cout<<"\n cov="<<cov<<" vmnea= "<<vmean;
    //   vcl_cout<<"x3= "<<diff_project3<<" lambda3= "<<lambdas(2);

    //   max_p=vcl_exp(-0.5*diff_project3*diff_project3/lambdas(2))/vcl_sqrt(2*vnl_math::pi*lambdas(2));
    //   //max_p=vcl_exp(-0.5*diff_project1*diff_project1/lambdas(0))*vcl_exp(-0.5*diff_project2*diff_project2/lambdas(1))*vcl_exp(-0.5*diff_project3*diff_project3/lambdas(2));


    //}
    //
    //vcl_cout<<"max_p = "<<max_p<<"\n";

    return -1;//max_p;


}
double dbdet_temporal_map::compute_probabilistic_error_1(dbdet_region r, double Vx, double Vy, double Vz)
{
    //double max_p=0;
    //float epsilon=0.001;
    //vnl_vector<float> v(3);
    //v(0)=Vx;v(1)=Vy;v(2)=Vz;
    //float phi=vcl_atan2(dot_product<float>(v,r.v2),dot_product<float>(v,r.v1));

    //float a=vcl_cos(r.phi_min)*vcl_sin(phi)-vcl_sin(r.phi_min)*vcl_cos(phi);
    //float b=vcl_cos(phi)*vcl_sin(r.phi_max)-vcl_sin(phi)*vcl_cos(r.phi_max);
    //if(a>=0 && b>=0)
    //{
    //    vnl_vector<float> vmean=r.v1*vcl_cos(phi)+r.v2*vcl_sin(phi);
    //    vnl_matrix<float> cov=vcl_cos(phi)*vcl_cos(phi)*r.v1v1+
    //                                2*vcl_cos(phi)*vcl_sin(phi)*r.v1v2+
    //                                vcl_sin(phi)*vcl_sin(phi)*r.v2v2;

    //    vnl_vector<float> lambdas(3);
    //    vnl_matrix<float> Vs(3,3);

    //   if(!vnl_symmetric_eigensystem_compute(cov,Vs,lambdas))
    //     return -1;

    //   float diff_project1=dot_product<float>(Vs.get_column(0),(v-vmean));
    //   float diff_project2=dot_product<float>(Vs.get_column(1),(v-vmean));
    //   float diff_project3=dot_product<float>(Vs.get_column(2),(v-vmean));

    //   //if(lambdas(0)<0.1)
    //   //    lambdas(0)=0.1;
    //   //if(lambdas(1)<0.1)
    //   //    lambdas(1)=0.1;
    //   //if(lambdas(2)<0.1)
    //   //    lambdas(2)=0.1;


    //   vcl_cout<<"\n cov="<<cov<<" vmnea= "<<vmean;
    //   vcl_cout<<"x3= "<<diff_project3<<" lambda3= "<<lambdas(2);

    //   max_p=vcl_exp(-0.5*diff_project3*diff_project3/lambdas(2));
    //   //max_p=vcl_exp(-0.5*diff_project1*diff_project1/lambdas(0))*vcl_exp(-0.5*diff_project2*diff_project2/lambdas(1))*vcl_exp(-0.5*diff_project3*diff_project3/lambdas(2));


    //}
    //
    //vcl_cout<<"max_p = "<<max_p<<"\n";

    return -1;


}
double dbdet_temporal_map::compute_geometric_error(dbdet_region r, double Vx, double Vy, double Vz, 
                                                   vcl_vector<double> & xs, vcl_vector<double> & ys)
{
    vcl_set<int>::iterator iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    double tot_err=0;
    for(iter=r.node_ids_.begin();iter!=r.node_ids_.end();iter++)
    {
        double minerr=1e5;
        double minx=1e5;
        double miny=1e5;
        for(cv_iter=map_[*iter]->st_options_.begin();cv_iter!=map_[*iter]->st_options_.end();cv_iter++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator list_iter_i;
            for(list_iter_i=cv_iter->second->st_bundles_.begin();list_iter_i!=cv_iter->second->st_bundles_.end();list_iter_i++)
            {
                if( dbdet_temporal_normal_velocity_model * m1
                    =dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_i->model_))
                {
                    //if(m1->isvalid())
                    {

                        double tx=vcl_cos(m1->ref_curvelet->ref_edgel->tangent);
                        double ty=vcl_sin(m1->ref_curvelet->ref_edgel->tangent);

                        double u=m1->ref_curvelet->ref_edgel->pt.x();
                        double v=m1->ref_curvelet->ref_edgel->pt.y();

                        u=(u-160)/320;
                        v=(v-120)/240;

                        //: V.Un
                        double VUt=-ty*Vx+tx*Vy+Vz*(u*ty-v*tx);
                        //: V.Ut
                        double VUn=tx*Vx+ty*Vy-Vz*(u*tx+v*ty);

                        double vmax=3;

                        if(vcl_fabs(VUt)<0.01)
                        {
                            if(vcl_fabs(m1->b0)<0.01)
                            {
                                double min_tan_err=1e5;
                                double min_tan_e1x;
                                double min_tan_e1y;
                                for(double w=-vmax;w<vmax;)
                                {
                                    double e1x=m1->ref_curvelet->ref_edgel->pt.x()+w*tx ;
                                    double e1y=m1->ref_curvelet->ref_edgel->pt.y()+w*ty ;
                                    double err=compute_error_edge(e1x,e1y,neighbor_emap_[1],m1->ref_curvelet->ref_edgel->tangent);
                                    if(err<min_tan_err){
                                        min_tan_err=err;
                                        min_tan_e1x=e1x;
                                        min_tan_e1y=e1y;
                                    }
                                    w++;         
                                }
                                if(min_tan_err<minerr)
                                {
                                    minerr=min_tan_err;
                                    minx=min_tan_e1x;
                                    miny=min_tan_e1y;
                                }
                            }
                        }
                        else
                        {
                            double w=m1->b0*VUn/VUt;

                            double e1x=m1->ref_curvelet->ref_edgel->pt.x()+w*tx -m1->b0*ty;
                            double e1y=m1->ref_curvelet->ref_edgel->pt.y()+w*ty +m1->b0*tx;

                            double em1x=m1->ref_curvelet->ref_edgel->pt.x()-(w*tx -m1->b0*ty);
                            double em1y=m1->ref_curvelet->ref_edgel->pt.y()-(w*ty +m1->b0*tx);


                            double err=compute_error_edge(e1x,e1y,neighbor_emap_[1],m1->ref_curvelet->ref_edgel->tangent);

                            if(err<minerr)
                            {
                                minerr=err;
                                minx=e1x;
                                miny=e1y;
                            }
                        }

                    }
                }
            }

        }
        if(minerr!=1e5)
        {
            tot_err+=minerr;

            xs.push_back(minx);
            ys.push_back(miny);
        }
    }
    if(xs.size()>0)
        return tot_err/xs.size();
    else
        return 1e5;
}

double dbdet_temporal_map::compute_geometric_error(dbdet_region & r, dbdet_spherical_histogram_sptr Vs)
{
    double tot_err=0;

    double phi_min=-vnl_math::pi;
    double theta_min=-vnl_math::pi_over_2;
    int cnt=0;
    for(unsigned i=0;i<Vs->theta_partitions_;i++)
    {
        float theta=Vs->get_theta(i);

        for(unsigned j=0;j<Vs->phi_partitions_[i];j++)
        {
            if(Vs->hist_[i][j])
            {
                float phi=Vs->get_phi(i,j);
                float vx=vcl_cos(theta)*vcl_cos(phi);
                float vy=vcl_cos(theta)*vcl_sin(phi);
                float vz=vcl_sin(theta);
                vcl_vector<float> xs;
                vcl_vector<float> ys;
                tot_err+=computer_error_per_region(r, vx, vy, vz,xs,ys);
                cnt++;
            }
        }
    }
    if(cnt>0)
        return tot_err/cnt;
    else
        return 1e5;
}
double dbdet_temporal_map::computer_error_per_region(dbdet_region & re, float vx, float vy, float vz,vcl_vector<float> &xs,vcl_vector<float> &ys)
{
    double tot_err=0;
    vcl_set<int>::iterator iter;
    
    for(iter=re.node_ids_.begin();iter!=re.node_ids_.end();iter++)
    {
        //; computing ut,un
        dbdet_edgel * curr_edgel=emap_->edgels[*iter];
        dbdet_spatial_temporal_derivatives * std=derivatives_map_[curr_edgel];
        float ut[3],un[3];
        ut[0]=-std->ty;ut[1]=std->tx;ut[2]=std->x*std->ty-std->y*std->tx;
        un[0]=-std->tx;un[1]=-std->ty;un[2]=std->x*std->tx+std->y*std->ty;

        float r=-(vx*un[0]+vy*un[1]+vz*un[2])/(vx*ut[0]+vy*ut[1]+vz*ut[2]);
        float minerr=1e5;
        float minx=-1000;
        float miny=-1000;
        for(unsigned k=0;k<std->b.size();k++)
        {
            float err=1e5;
            float w=r*std->b[k];
            if((vx*ut[0]+vy*ut[1]+vz*ut[2])/std->b[k]>0)
            {
                float e1x=curr_edgel->pt.x()+(w*std->tx-std->b[k]*std->ty);
                float e1y=curr_edgel->pt.y()+(w*std->ty+std->b[k]*std->tx);

                err=compute_error_edge(e1x,e1y,neighbor_emap_[1],curr_edgel->tangent);
                if(minerr>err)
                {
                    minerr=err;
                    minx=e1x;
                    miny=e1y;
                }

            }
            else
            {
                err=5.0;
                if(minerr>err)
                {
                    minerr=err;
                }
            }

        }
        if(minx!=-1000)
        {
            xs.push_back(minx);
            ys.push_back(miny);
        }
        if(minerr!=1e5)
            tot_err+=minerr;
        else
            tot_err+=1.0;
    }
    return tot_err;

}
double dbdet_temporal_map::compute_error_edge(double x, double y, dbdet_edgemap_sptr emap,double theta)
{
    int winsize=2;
    dbdet_edgel * min_edge=0;
    if(x<emap->ncols() && x>=0 && y<emap->nrows() && y>=0 )
    {
        int indexi=vcl_floor(x);
        int indexj=vcl_floor(y);
        double minerr=1e5;
        for(unsigned k=vcl_max<int>(0,indexi-winsize);k<=vcl_min<int>(indexi+winsize, emap->ncols()-1);k++)
        {

            for(unsigned l=vcl_max<int>(0,indexj-winsize);l<=vcl_min<int>(indexj+winsize, emap->nrows()-1);l++)
            {
                vcl_vector<dbdet_edgel *> *es=&(emap->edge_cells(l,k));
                if(es->size()>0)
                {
                    dbdet_edgel * e=(*es)[0];
                
                double err=((e->pt.x()-x)*(e->pt.x()-x)+(e->pt.y()-y)*(e->pt.y()-y));
                if(err<minerr)
                {
                    minerr=err;
                    min_edge=e;
                }
                }

            }
        }
        //: check if the tangents are enormously different.
        if(min_edge)
            return vcl_sqrt(minerr)*(1+vcl_fabs(vcl_sin(min_edge->tangent-theta)));
        else
            return winsize;

    }
    return 5;
}


bool dbdet_temporal_map::compute_intial_motion()
{
    vcl_set<int>::iterator iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_list<dbdet_temporal_bundle>::iterator list_iter;

    for(unsigned i=0;i<regions_.size();i++)
    {   
        regions_[i].V=new dbdet_3D_velocity_model(10, 20, 0,vnl_math::pi_over_2, -vnl_math::pi, vnl_math::pi);
        for(iter=regions_[i].node_ids_.begin();iter!=regions_[i].node_ids_.end();iter++)
        {
            for(cv_iter=map_[*iter]->st_options_.begin();cv_iter!=map_[*iter]->st_options_.end();cv_iter++)
            {
                for(list_iter=cv_iter->second->st_bundles_.begin();list_iter!=cv_iter->second->st_bundles_.end();list_iter++)
                {
                    if( dbdet_temporal_normal_velocity_model * m=dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter->model_))
                    {
                        double phi0=vcl_atan(-m->a[0]/m->b[0]);
                        double phi1=vcl_atan(-m->a[1]/m->b[1]);

                        regions_[i].V->increment(0,phi0);
                        regions_[i].V->increment(0,phi1);

                    }
                }
            }
        }
    }
    return true;
}




bool dbdet_temporal_map::compute_distribution_per_region_new()
{
    for(unsigned i=0;i<regions_.size();i++)
    {     
        vcl_set<int>::iterator iter;
        for(iter=regions_[i].node_ids_.begin();iter!=regions_[i].node_ids_.end();iter++)
        {
            vcl_vector<dbdet_second_order_velocity_model>::iterator cv_iter;
            dbdet_spherical_histogram_sptr Vmin;
            double minerr=1e5;
            dbdet_edgel * curr_edgel=emap_->edgels[*iter];
            for(cv_iter=derivatives_map_[curr_edgel]->models_.begin();cv_iter!=derivatives_map_[curr_edgel]->models_.end();cv_iter++)
            {
                dbdet_spherical_histogram_sptr V=cv_iter->computeV(curr_edgel);
                double err=compute_geometric_error(regions_[i],V);
                if(err<minerr)
                {
                    Vmin=V;
                    minerr=err;
                }
            }
            if(minerr!=1e5)
                regions_[i].Vs[*iter]=Vmin;              
        }
    }
    for(unsigned i=0;i<regions_.size();i++)
        regions_[i].compute_dist();
    return true;
}
bool dbdet_temporal_map::compute_distribution_per_region()
{
    ////: selecting the best one at each edge
    //for(unsigned i=0;i<regions_.size();i++)
    //{
    //    
    //    double sum_k=0;
    //    vcl_set<int>::iterator iter;
    //    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    //    for(iter=regions_[i].node_ids_.begin();iter!=regions_[i].node_ids_.end();iter++)
    //    {
    //        int minoption=-1;
    //        double minerr=1e5;
    //        double k=-10;
    //        dbdet_temporal_model *min_m=0;
    //        for(cv_iter=map_[*iter]->st_options_.begin();cv_iter!=map_[*iter]->st_options_.end();cv_iter++)
    //        {
    //            vcl_list<dbdet_temporal_bundle>::iterator list_iter_i;
    //            for(list_iter_i=cv_iter->second->st_bundles_.begin();list_iter_i!=cv_iter->second->st_bundles_.end();list_iter_i++)
    //            {
    //                if( dbdet_temporal_normal_velocity_model * m1
    //                    =dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_i->model_))
    //                {
    //                    if(m1->iscomputed_)
    //                    {
    //                        vcl_vector<double> xs;vcl_vector<double> ys;
    //                        double e1,e2;

    //                        e1=compute_geometric_error_for_family(regions_[i],m1->v1_1,m1->v1_2,xs,ys,m1->phi1_min, m1->phi1_max);
    //                        e2=compute_geometric_error_for_family(regions_[i],m1->v2_1,m1->v2_2,xs,ys,m1->phi2_min, m1->phi2_max);
    //                        if(e1<=e2)
    //                        {
    //                            if(minerr>e1)
    //                            {
    //                                minerr=e1;
    //                                minoption=1;
    //                                min_m=list_iter_i->model_;
    //                                k=m1->k;
    //                            }
    //                        }
    //                        else
    //                        {
    //                            if(minerr>e2)
    //                            {
    //                                minerr=e2;
    //                                minoption=2;
    //                                min_m=list_iter_i->model_;
    //                                k=m1->k;
    //                            }
    //                        }
    //                    }
    //                }
    //            }
    //        }
    //        if(min_m)
    //        {
    //            regions_[i].ids_model[*iter]=min_m;
    //            regions_[i].ids_ws[*iter]=minoption;
    //        }
    //        if(k!=-10)
    //            sum_k+=vcl_fabs(k);
    //    }
    //    vcl_map<int,dbdet_temporal_model*>::iterator iter_model;
    //    for(iter_model=regions_[i].ids_model.begin();iter_model!=regions_[i].ids_model.end();iter_model++)
    //    {
    //        if( dbdet_temporal_normal_velocity_model * m
    //            =dynamic_cast<dbdet_temporal_normal_velocity_model*> (iter_model->second))
    //            regions_[i].ids_weights[iter_model->first]=vcl_fabs(m->k)/sum_k;
    //    }  
    //   
    //}
    ////: compute the distribution

    //for(unsigned i=0;i<regions_.size();i++)
    //{
    //    regions_[i].compute_V();
    //    regions_[i].range_for_phi();
    //}

    return true;
}
double 
dbdet_temporal_map::compute_geometric_error_for_family(dbdet_region r, 
                                                       vnl_vector<float> v1,    
                                                       vnl_vector<float> v2,
                                                       vcl_vector<double> & xs, 
                                                       vcl_vector<double> & ys, 
                                                       float phi_min,           
                                                       float phi_max)
{
    vcl_set<int>::iterator iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    double tot_err=0;
    for(iter=r.node_ids_.begin();iter!=r.node_ids_.end();iter++)
    {
        double minerr=1e5;

        vcl_vector<double> minx;
        vcl_vector<double> miny;
        for(cv_iter=map_[*iter]->st_options_.begin();cv_iter!=map_[*iter]->st_options_.end();cv_iter++)
        {
            vcl_list<dbdet_temporal_bundle>::iterator list_iter_i;
            for(list_iter_i=cv_iter->second->st_bundles_.begin();list_iter_i!=cv_iter->second->st_bundles_.end();list_iter_i++)
            {
                if( dbdet_temporal_normal_velocity_model * m1
                    =dynamic_cast<dbdet_temporal_normal_velocity_model*> (list_iter_i->model_))
                {
                    double tx=vcl_cos(m1->tangent);
                    double ty=vcl_sin(m1->tangent);

                    double u=m1->ref_curvelet->ref_edgel->pt.x();
                    double v=m1->ref_curvelet->ref_edgel->pt.y();

                    u=(u-160)/320;
                    v=(v-120)/240;
                    //: heree V = v1\cos\phi+v2\sin\phi
                    //: V.Ut=v1.Ut\cos\phi+v2.Ut\sin\phi
                    //: denoting VUt_a= v1.Ut;
                    //: denoting VUt_b= v2.Ut;
                    double VUt_a=-ty*v1(0)+tx*v1(1)+v1(2)*(u*ty-v*tx);
                    double VUt_b=-ty*v2(0)+tx*v2(1)+v2(2)*(u*ty-v*tx);
                    //: heree V = v1\cos\phi+v2\sin\phi
                    //: V.Un=v1.Un\cos\phi+v2.Un\sin\phi
                    //: denoting VUnt_a= v1.Un;
                    //: denoting VUn_b= v2.Un;
                    double VUn_a=tx*v1(0)+ty*v1(1)-v1(2)*(u*tx+v*ty);
                    double VUn_b=tx*v2(0)+ty*v2(1)-v2(2)*(u*tx+v*ty);

                    double vmax=3;
                    double wmax=vcl_sqrt(vmax*vmax-m1->b0*m1->b0);
                    double cum_err_phi=0;
                    vcl_vector<double> tempx;
                    vcl_vector<double> tempy;
                    for(float phi=phi_min;phi<phi_max;)
                    {
                        double VUt=VUt_a*vcl_cos(phi)+VUt_b*vcl_sin(phi);
                        double VUn=VUn_a*vcl_cos(phi)+VUn_b*vcl_sin(phi);

                        ////: to check if it results in positive depth 
                        //if(vcl_fabs(m1->b0)>0.2 && vcl_fabs(VUt)>0.1&& VUt/m1->b0>0)
                        //{
                        //    cum_err_phi+=2;
                        //}
                        //else
                        {

                            if(vcl_fabs(VUt)<0.1)
                            {
                                //if(vcl_fabs(m1->b0)<0.2)
                                {
                                    double min_tan_err=1e5;
                                    double min_tan_e1x;
                                    double min_tan_e1y;
                                    for(double w=-vmax;w<vmax;)
                                    {
                                        double e1x=m1->ref_curvelet->ref_edgel->pt.x()+w*tx ;
                                        double e1y=m1->ref_curvelet->ref_edgel->pt.y()+w*ty ;
                                        double err=compute_error_edge(e1x,e1y,neighbor_emap_[1],m1->ref_curvelet->ref_edgel->tangent);
                                        if(err<min_tan_err){
                                            min_tan_err=err;
                                            min_tan_e1x=e1x;
                                            min_tan_e1y=e1y;
                                        }
                                        w++;         
                                    }
                                    cum_err_phi+=(min_tan_err+vcl_fabs(m1->b0));
                                    tempx.push_back(min_tan_e1x);
                                    tempy.push_back(min_tan_e1y);
                                }
                            }
                            else
                            {
                                double w=m1->b0*VUn/VUt;
                                double e1x=m1->ref_curvelet->ref_edgel->pt.x()+w*tx -m1->b0*ty;
                                double e1y=m1->ref_curvelet->ref_edgel->pt.y()+w*ty +m1->b0*tx;

                                double em1x=m1->ref_curvelet->ref_edgel->pt.x()-(w*tx -m1->b0*ty);
                                double em1y=m1->ref_curvelet->ref_edgel->pt.y()-(w*ty +m1->b0*tx);

                                cum_err_phi+=compute_error_edge(e1x,e1y,neighbor_emap_[1],m1->ref_curvelet->ref_edgel->tangent);
                                tempx.push_back(e1x);
                                tempy.push_back(e1y);
                            }
                        }
                        phi+=0.2; 
                    }
                    if(cum_err_phi<minerr)
                    {
                        minerr=cum_err_phi;
                        minx=tempx;
                        miny=tempy;
                    }
                }
            }
        }

        if(minerr!=1e5)
        {
            xs.insert(xs.begin(),minx.begin(),minx.end());
            ys.insert(ys.begin(),miny.begin(),miny.end());
            tot_err+=minerr;

        }
    }

    return tot_err;

}



bool dbdet_temporal_map::compute_spatial_temporal_bundles()
{
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        dbdet_edgel* curr_edgel=emap_->edgels[i];
        derivatives_map_[curr_edgel]->compute_models(); 
    }
    return true;
}
bool dbdet_temporal_map::compute_spatial_derivatives()
{
    vcl_cout<<"Form Spatial Bundles Along the Link Graph : \n";
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        //: map of curvelets to beta of the next edge
        vcl_map<dbdet_curvelet*,vcl_vector<float> > k_bs;
        // stores the ds for betas along the curvelets
        vcl_map<dbdet_curvelet*,float > k_bs_ds;
        //
        vcl_map<dbdet_curvelet*,dbdet_edgel* > k_bs_edges;
        dbdet_edgel * curr_edgel=emap_->edgels[i];
        vcl_vector<dbdet_curvelet*>::iterator iter_curvelet=derivatives_map_[ curr_edgel]->refcs_.begin();
        for(;iter_curvelet!=derivatives_map_[curr_edgel ]->refcs_.end();iter_curvelet++)
        {
            dbdet_link_list clinks=elg_.cLinks[curr_edgel->id];
            //: going through all the child links.
            for(dbdet_link_list::iterator link_iter=clinks.begin();link_iter!=clinks.end();link_iter++)
            {
                if(pass_through_link((*link_iter)->curvelets, *iter_curvelet))
                {
                    //: iterating throught the edges till we find one significantly far 
                    vcl_list<dbdet_edgel*> child_chain=(*iter_curvelet)->child_chain();
                    vcl_list<dbdet_edgel*>::iterator eiter=child_chain.begin();
                    for(;eiter!=child_chain.end();eiter++)
                    {
                        float ds2=(curr_edgel->pt-(*eiter)->pt).sqr_length();
                        if(ds2>0.2)
                        {
                            k_bs[*iter_curvelet]=derivatives_map_[(*eiter)]->b;
                            k_bs_ds[*iter_curvelet]=vcl_sqrt(ds2);
                            k_bs_edges[*iter_curvelet]=(*eiter);

                            break;
                        }
                    }
                }
            }
        }
        derivatives_map_[curr_edgel]->compute_bs(k_bs,k_bs_ds,k_bs_edges);
    }
    return true;
}

bool dbdet_temporal_map::compute_temporal_derivatives()
{
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        dbdet_edgel* curr_edgel=emap_->edgels[i];
        derivatives_map_[curr_edgel]->tx=vcl_cos(curr_edgel->tangent);
        derivatives_map_[curr_edgel]->ty=vcl_sin(curr_edgel->tangent);
        derivatives_map_[curr_edgel]->x=(curr_edgel->pt.x()-160)/320;
        derivatives_map_[curr_edgel]->y=(curr_edgel->pt.y()-120)/320;
        derivatives_map_[curr_edgel]->compute_b(beta_edge_map[curr_edgel]);
        derivatives_map_[curr_edgel]->compute_bt(backward_beta_edge_map[curr_edgel]);
    }
    return true;
}



//: function to compute forward beta ( from an  edge to curvelets )
bool dbdet_temporal_map::compute_forward_beta()
{
    vcl_cout << "Forward beta Computation ...\n";
    typedef vcl_pair <double, dbdet_edgel*> dist_edge;
    int frame_forward=1;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator mem_iter;

    int nrad=params_.tneighbor;
    //: iterate over edges in the central edge-map.
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        vcl_cout<<".";
        //: egdel to form temporal groupings with.
        dbdet_edgel * cur_edgel=emap_->edgels[i];
        //get the grid coordinates of this edgel
        unsigned ii = dbdet_round(cur_edgel->pt.x());
        unsigned jj = dbdet_round(cur_edgel->pt.y());
        int cnt = 0; //count the # of neighboring edgels

        double x = cur_edgel->pt.x();
        double y = cur_edgel->pt.y();

        double tx=vcl_cos(cur_edgel->tangent);
        double ty=vcl_sin(cur_edgel->tangent);
        vgl_vector_2d<double> n(-ty,tx);

        dbdet_edgemap_sptr edgemap=neighbor_emap_[frame_forward];
        //: compute the radius of neighborhood in each frame     
        vcl_multimap<double, dbdet_edgel*> pot_edges_;
        vcl_multimap<double, dbdet_edgel*>::iterator pot_iter_;
        vcl_multimap<double, dbdet_edgel*>::iterator pot2_iter_;
        
        //iterate over the cell neighborhoods around this edgel that contains the full edgel neighborhood
        for (unsigned xx=ii-nrad; xx<=ii+nrad; xx++){
            for (unsigned yy=jj-nrad; yy<=jj+nrad ; yy++){
                if (xx<0 || xx>=edgemap->ncols() || yy<0 || yy>=edgemap->nrows())
                    continue;
                //: # of edges in each cell whih should not be ususa lly more than 1
                unsigned N = edgemap->cell(xx, yy).size();
                for (unsigned k=0; k<N; k++){
                    dbdet_edgel* eB = edgemap->cell(xx, yy)[k];
                    pot_edges_.insert(dist_edge(dbdet_temporal_utils::perp_distance(eB->pt-cur_edgel->pt,n),eB));
                }
            }
        }
        for(pot_iter_=pot_edges_.begin();pot_iter_!=pot_edges_.end();pot_iter_++)
        {
            cvlet_list eb_curvlet_list=neighbor_cmap_[frame_forward]->curvelets(pot_iter_->second->id);
            //: going through the curvelets of each of the neighbor edges
            for(cvlet_list_iter cvlistiter=eb_curvlet_list.begin();cvlistiter!=eb_curvlet_list.end();cvlistiter++)
            {
                 bool flag=true;
                 for(pot2_iter_=pot_edges_.begin();pot2_iter_!=pot_iter_;pot2_iter_++)
                      if((*cvlistiter)->ismember(pot2_iter_->second))
                            flag=false;;
                 //:  compute beta
                 if(flag)
                 {
                     double beta=-1e5;
                     if(dbdet_temporal_utils::compute_beta(cur_edgel, (*cvlistiter), beta))
                     {
                        beta_edge_map[cur_edgel].push_back(beta);
                     }
                 }
            }
        }
    }
    return true;
}


//: function to compute backward beta ( from  curvelets to  edge)
bool 
dbdet_temporal_map::compute_backward_beta()
{
    vcl_cout << "Backwarde Beta Computation...\n";
    
    int frame_backward=-1;
    typedef vcl_pair <double, dbdet_edgel*> dist_edge;

    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator cv_iter;
    vcl_map<dbdet_curvelet*, dbdet_temporal_options* >::iterator mem_iter;

    int nrad=params_.tneighbor;

    //: iterate over edges in the central edge-map.
    for (unsigned i=0;i<emap_->edgels.size();i++)
    {
        vcl_cout<<".";
        //: egdel to form temporal groupings with.
        dbdet_edgel * cur_edgel=emap_->edgels[i];
        //get the grid coordinates of this edgel
        unsigned ii = dbdet_round(cur_edgel->pt.x());
        unsigned jj = dbdet_round(cur_edgel->pt.y());
        int cnt = 0; //count the # of neighboring edgels

        double x = cur_edgel->pt.x();
        double y = cur_edgel->pt.y();

        double tx=vcl_cos(cur_edgel->tangent);
        double ty=vcl_sin(cur_edgel->tangent);
        vgl_vector_2d<double> n(-ty,tx);

        dbdet_edgemap_sptr edgemap=neighbor_emap_[frame_backward];
        //: compute the radius of neighborhood in each frame     
        vcl_multimap<double, dbdet_edgel*> pot_edges_;
        vcl_multimap<double, dbdet_edgel*>::iterator pot_iter_;
         vcl_multimap<double, dbdet_edgel*>::iterator pot2_iter_;
        
        //iterate over the cell neighborhoods around this edgel that contains the full edgel neighborhood
         for (unsigned xx=ii-nrad; xx<=ii+nrad; xx++){
             for (unsigned yy=jj-nrad; yy<=jj+nrad ; yy++){
                 if (xx<0 || xx>=edgemap->ncols() || yy<0 || yy>=edgemap->nrows())
                     continue;
                 //: # of edges in each cell whih should not be ususa lly more than 1
                 unsigned N = edgemap->cell(xx, yy).size();
                 for (unsigned k=0; k<N; k++){
                     dbdet_edgel* eB = edgemap->cell(xx, yy)[k];
                     pot_edges_.insert(dist_edge(dbdet_temporal_utils::perp_distance(eB->pt-cur_edgel->pt,n),eB));
                 }
             }
         }
        
       
        for(pot_iter_=pot_edges_.begin();pot_iter_!=pot_edges_.end();pot_iter_++)
        {
            cvlet_list eb_curvlet_list=neighbor_cmap_[frame_backward]->curvelets(pot_iter_->second->id);
            //: going through the curvelets of each of the neighbor edges
            for(cvlet_list_iter cvlistiter=eb_curvlet_list.begin();cvlistiter!=eb_curvlet_list.end();cvlistiter++)
            {
                 bool flag=true;
                 for(pot2_iter_=pot_edges_.begin();pot2_iter_!=pot_iter_;pot2_iter_++)
                      if((*cvlistiter)->ismember(pot2_iter_->second))
                            flag=false;;
                 //:  compute beta
                 if(flag)
                 {
                     double beta=-1e5;
                     vgl_vector_2d<double> p;
                     if(dbdet_temporal_utils::compute_backward_beta( (*cvlistiter),cur_edgel, beta,p))
                     {
                         backward_beta_edge_map[cur_edgel].push_back(beta);
                         backward_vector_edge_map[cur_edgel].push_back(p);
                     }
                 }
            
            }
        }
        
    }
    return true;
}





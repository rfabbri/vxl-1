#include <dbetrk/dbetrk_track.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <dbetrk/dbetrk_edge.h>
#include <vsol/vsol_box_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vnl/vnl_math.h>
#include <vbl/vbl_bounding_box.h>
#include <brip/brip_vil_float_ops.h>
#include <dbctrk/dbctrk_utils.h>
#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
struct less_pair
{
  bool operator()(vcl_pair<double,vcl_vector<dbetrk_edge_sptr > > x, vcl_pair<double,vcl_vector<dbetrk_edge_sptr > > y)
  { return x.first > y.first; }
};
dbetrk_track::dbetrk_track()
{
    std_x=10.0;
    std_y=10.0;
    std_theta=static_cast<float>(vnl_math::pi/4);
    frame_=0;
}
void dbetrk_track::intialize(vcl_vector<dbetrk_edge_sptr>  tracked_dbetrk_edges_)
{
    tracked_dbetrk_edges.push_back(tracked_dbetrk_edges_);
}
bool dbetrk_track::intialize_cells(int w,int h)
{
    cell_matrix.clear();
    vcl_vector<cells> temp;
    for(int i=0;i<(int)(w/20);i++)
    {
        temp.clear();     
        for(int j=0;j<(int)(h/20);j++)
        {
            cells tempcell;
            tempcell.xmin=i*20;
            tempcell.ymin=j*20;

            tempcell.xmax=i*20+20;
            tempcell.ymax=j*20+20;


            temp.push_back(tempcell);

        }
        cell_matrix.push_back(temp);
    }
    return true;
}
bool dbetrk_track::fill_dbetrk_edges(int frameno)
{
    dbetrk_edges.clear();

    intialize_cells(640,480);
    int input_curves_size_int = static_cast<int>(input_curves_.size());
    if(frameno<input_curves_size_int || input_curves_size_int>0)
    {
        //vcl_vector<dbetrk_edge_sptr> curr_frame_nodes_;       
        for(unsigned i=0;i<input_curves_.size();i++)
        {   
            vdgl_edgel_chain_sptr ec=input_curves_[i]->curve()->cast_to_vdgl_digital_curve()->get_interpolator()->get_edgel_chain();
            for(unsigned j=0;j<ec->size();j++)
            {
                dbetrk_edge_sptr newnode=new dbetrk_edge();
                vgl_point_2d<double> point(ec->edgel(j).get_x(),ec->edgel(j).get_y());
                int index_i=(int)(point.x()/20);
                int index_j=(int)(point.y()/20);
                cell_matrix[index_i][index_j].points.push_back(newnode);

                newnode->setpoint(point);
                
                if(j!=ec->size()-1)
                {
                    vdgl_edgel ed = ec->edgel(j);
                    vdgl_edgel en = ec->edgel(j+1);
                    double theta=vcl_atan2(en.y()-ed.y(),en.x()-ed.x());
                    newnode->settheta(theta);
                }
                else
                {
                    newnode->settheta(dbetrk_edges[dbetrk_edges.size()-1]->gettheta());
                }
                newnode->setgrad(ec->edgel(j).get_grad());

                newnode->parent_curve_=input_curves_[i];
                newnode->frame_=frameno;
                dbetrk_edges.push_back(newnode);
            }
        }  
    }
    return true;
}

void dbetrk_track::assign_rgb_values(vil_image_view<float> p0,
                                    vil_image_view<float> p1,
                                    vil_image_view<float> p2, 
                                    int r,int maskradius,
                                    dbetrk_edge_sptr enode)
{
    vbl_array_2d<float> g2d(2*maskradius+1,2*maskradius+1,0);
    utils::gauss2d(1.5,g2d);


    double thetap=enode->gettheta()+vnl_math::pi/2;
    double thetan=enode->gettheta()-vnl_math::pi/2;

    int p_x=(int)(enode->point_.x()+r*vcl_cos(thetap));
    int p_y=(int)(enode->point_.y()+r*vcl_sin(thetap));

    int n_x=(int)(enode->point_.x()+r*vcl_cos(thetan));
    int n_y=(int)(enode->point_.y()+r*vcl_sin(thetan));

   
    vbl_bounding_box<double,2> p_box_temp;
    p_box_temp.update(p_x-maskradius,p_y-maskradius);
    p_box_temp.update(p_x+maskradius,p_y+maskradius);
 
    
    vsol_box_2d_sptr p_box= new vsol_box_2d(p_box_temp);


    vbl_bounding_box<double,2> n_box_temp;
    n_box_temp.update(n_x-maskradius,n_y-maskradius);
    n_box_temp.update(n_x+maskradius,n_y+maskradius);   
    vsol_box_2d_sptr n_box= new vsol_box_2d(n_box_temp);

    vil_image_view<float> img_mask0,img_mask1,img_mask2;
    
    img_mask0.set_size(2*maskradius+1, 2*maskradius+1, 1);
    img_mask1.set_size(2*maskradius+1, 2*maskradius+1, 1);
    img_mask2.set_size(2*maskradius+1, 2*maskradius+1, 1);

    brip_vil_float_ops::chip(p0,p_box,img_mask0);
    brip_vil_float_ops::chip(p1,p_box,img_mask1);
    brip_vil_float_ops::chip(p2,p_box,img_mask2);
 
    enode->pcolor[0]=smooth(img_mask0,g2d);
    enode->pcolor[1]=smooth(img_mask1,g2d);
    enode->pcolor[2]=smooth(img_mask2,g2d); 

    brip_vil_float_ops::chip(p0,n_box,img_mask0);
    brip_vil_float_ops::chip(p1,n_box,img_mask1);
    brip_vil_float_ops::chip(p2,n_box,img_mask2);

    enode->ncolor[0]=smooth(img_mask0,g2d);
    enode->ncolor[1]=smooth(img_mask1,g2d);
    enode->ncolor[2]=smooth(img_mask2,g2d);     

    
 }

double dbetrk_track::smooth(vil_image_view<float> in,vbl_array_2d<float> g2d)
{
   double prodsum=0.0;
   for(unsigned j=0;j<in.nj();j++)
       for(unsigned i=0;i<in.ni();i++)
             prodsum+=g2d(i,j)*in(i,j);

   return prodsum;
}
//: compute edges between two frames frame1 is > frame2 in time
bool dbetrk_track::compute_edges(int frame1,int frame2)
{
         int input_curves_size_int = static_cast<int>(input_curves_.size());
    if((frame1>= input_curves_size_int && frame2 >= input_curves_size_int)
        || input_curves_size_int<0)
        return false;

    vcl_vector<vcl_pair<double,vcl_vector<dbetrk_edge_sptr > > >particledata;
    compute_particles(frame2,particledata,samplenum);
    vcl_vector<dbetrk_edge_sptr> nodes;
    tracked_dbetrk_edges.push_back(nodes);

    for(unsigned i=0;i<tracked_dbetrk_edges[frame1].size();i++)
    {
        for(unsigned j=0;j<particledata.size() && j<10;j++)
        {
            tracked_dbetrk_edges[frame1][i]->particles.push_back(particledata[j].second);
            tracked_dbetrk_edges[frame1][i]->prob_particles.push_back(particledata[j].first);
        }
    }
   
  
    //: insert the maximum 
    for(unsigned i=0;i<tracked_dbetrk_edges[frame1][0]->particles[0].size();i++)
    {
        dbetrk_edge_sptr e=new dbetrk_edge();
        e->setpoint(vgl_point_2d<double>(tracked_dbetrk_edges[frame1][0]->particles[0][i]->point_.x(),
                                         tracked_dbetrk_edges[frame1][0]->particles[0][i]->point_.y()));
        e->settheta(tracked_dbetrk_edges[frame1][0]->particles[0][i]->gettheta());
        e->frame_=frame2;
        tracked_dbetrk_edges[frame2].push_back(e);
    }


 
    
    return true;
}


double dbetrk_track::compute_cost_IHS(dbetrk_edge_sptr e1, dbetrk_edge_sptr e2)
{

    double cost1=vcl_fabs(e1->pcolor[0]-e2->pcolor[0])+vcl_fabs(e1->ncolor[0]-e2->ncolor[0]);
    double cost2=vcl_fabs(e1->ncolor[0]-e2->pcolor[0])+vcl_fabs(e1->pcolor[0]-e2->ncolor[0]);


    if(cost1<cost2)
        return cost1;
    else
        return cost2;

}

double dbetrk_track::compute_cost_dist(dbetrk_edge_sptr e1, dbetrk_edge_sptr e2)
{
   double cost=vgl_distance<double>(e1->point_,e2->point_);
   return cost;

}

vgl_point_2d<double> dbetrk_track::mean_point(vcl_vector<dbetrk_edge_sptr> listofedges)
{
    double accum_x=0;
    double accum_y=0;

    for(unsigned i=0;i<listofedges.size();i++)
    {
        accum_x+=listofedges[i]->point_.x();
        accum_y+=listofedges[i]->point_.y();

    }
    accum_x/=listofedges.size();
    accum_y/=listofedges.size();

    return vgl_point_2d<double>(accum_x, accum_y);

}


void dbetrk_track::compute_particles(int frameno,
                                    vcl_vector<vcl_pair<double,vcl_vector<dbetrk_edge_sptr> > > &particledata, 
                                    int particleno)
{
    
    
    if(frameno>0)
    {
        
        vnl_random normrandom;
      
        vcl_vector<vcl_vector<dbetrk_edge_sptr> > samples;

        if(frameno==1)
           for(int i=0;i<particleno;i++)
            samples.push_back(tracked_dbetrk_edges[frameno-1]);
        else
        {
            for(int i=0;i<particleno;i++)
            {
               samples.push_back(tracked_dbetrk_edges[frameno-2][0]->particles[i%10]);
            }
        }
        
        particledata.clear();
        
        for(int k=0;k<particleno;k++)
        {
           
           vgl_point_2d<double> p=mean_point(samples[k]);
           double tx=(normrandom.drand32()-0.5)*std_x;
           double ty=(normrandom.drand32()-0.5)*std_y;
           double theta=normrandom.normal()*std_theta;

           vcl_vector<dbetrk_edge_sptr > predictedpoints;      
           double toterror=0;
           for(unsigned int i=0;i<samples[k].size();i++)
           {
              double x=samples[k][i]->point_.x();
              double y=samples[k][i]->point_.y();
              double newx=(x-p.x())*vcl_cos(theta)-(y-p.y())*vcl_sin(theta)+p.x() +tx;
              double newy=(x-p.x())*vcl_sin(theta)+(y-p.y())*vcl_cos(theta)+p.y() +ty;
              //: function to compute cost for each point
              int xmin=static_cast<int>(newx);
              int ymin=static_cast<int>(newy);
              int xmax=static_cast<int>(newx);
              int ymax=static_cast<int>(newy);
              
              double thetanew=samples[k][i]->gettheta()-theta;
              if(thetanew>vnl_math::pi/2)
                  thetanew-=vnl_math::pi;
              if(thetanew<(-vnl_math::pi/2))
                    thetanew+=vnl_math::pi;
                        
                double min_error=1e8;
                dbetrk_edge_sptr min_e;
                for(unsigned j=0;j<cell_matrix.size();j++)
                   for(unsigned k=0;k<cell_matrix[j].size();k++)
                    {
                    if(xmin<=cell_matrix[j][k].xmax && xmax>=cell_matrix[j][k].xmin
                    && ymin<=cell_matrix[j][k].ymax && ymax>=cell_matrix[j][k].ymin)
                    {
                        for(unsigned int l=0;l<cell_matrix[j][k].points.size();l++)
                        {
                          double error1=vgl_distance<double>(vgl_point_2d<double>(newx,newy),cell_matrix[j][k].points[l]->point_);
                          double error2=vcl_fabs(thetanew-cell_matrix[j][k].points[l]->gettheta());

                          double error=error1*weight1+error2*weight2;
                          if(error<min_error)
                          {
                              min_error=error;
                              min_e=cell_matrix[j][k].points[l];
                            
                          }
                        }
                      }
                    
                     }
                    
                    
                dbetrk_edge_sptr epoint=new dbetrk_edge();
                epoint->setpoint(vgl_point_2d<double>(newx,newy));
                epoint->settheta(thetanew);
                predictedpoints.push_back(epoint);

                toterror+=min_error;   

            }
            toterror/=predictedpoints.size();
            toterror=vcl_exp(-toterror);
            particledata.push_back(vcl_make_pair(toterror,predictedpoints));
 
        }
        
        vcl_sort(particledata.begin(),particledata.end(),less_pair());
       
    }
}



double dbetrk_track::error_grad(dbetrk_edge_sptr e1,dbetrk_edge_sptr e2, double theta)
{
    double theta1=e1->gettheta();
    double theta2=e2->gettheta();

    if(theta1<0)
        theta1+=(vnl_math::pi/2);

    if(theta2<0)
        theta2+=(vnl_math::pi/2);


    theta1-=theta;

    return vcl_fabs(theta1-theta2);
        
}
    /*for(int i=0;i<dbetrk_edges[frame1].size();i++)
    {
        for(int j=0;j<dbetrk_edges[frame2].size();j++)
        {
            //double distance=vgl_distance<double>(dbetrk_edges[frame1][i]->point_,dbetrk_edges[frame2][j]->point_);
            double x1=dbetrk_edges[frame1][i]->point_.x();
            double y1=dbetrk_edges[frame1][i]->point_.y();

            double x2=dbetrk_edges[frame2][j]->point_.x();
            double y2=dbetrk_edges[frame2][j]->point_.y(); 

            if(x2<x1+motion_ )
                if(x2>x1-motion_)
                    if(y2<y1+motion_)
                        if(y2>y1-motion_)
                         {
                              edge_arc * arcptr= new edge_arc();
                              arcptr->set_dbetrk_edge_to(dbetrk_edges[frame1][i]);
                              arcptr->set_dbetrk_edge_from(dbetrk_edges[frame2][j]);
                              arcptr->setcost(compute_cost_IHS(dbetrk_edges[frame1][i],dbetrk_edges[frame2][j]));

                              dbetrk_edges[frame1][i]->in_edges.push_back(arcptr);

                          }
         }
        vcl_sort(dbetrk_edges[frame1][i]->in_edges.begin(),dbetrk_edges[frame1][i]->in_edges.end(),less_val());
    }*/

#include <dbctrk/dbctrk_tracker_curve.h>
#include <dbctrk/dbctrk_algs.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vcl_cmath.h>
#include <vsl/vsl_binary_io.h>
#include <vsl/vsl_map_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vnl/io/vnl_io_matrix.h>
#include <vnl/io/vnl_io_matrix_fixed.h>
#include <dbctrk/dbctrk_curve_description.h>
#include <vgl/io/vgl_io_vector_2d.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vsl/vsl_vector_io.h>
#include <vsl/vsl_map_io.h>
#include <dbctrk/dbctrk_curve_description.h>
#include <dbctrk/dbctrk_curve_matching.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_box_2d.h>

match_data::match_data(dbctrk_tracker_curve_sptr c,match_data_sptr m)
{
  match_curve_set.push_back(c);
  curve_set=0;
  R_=m->R_;
  T_=m->T_;
  scale_=m->scale_;
  Tbar=m->Tbar;
  energy_=m->energy_;
  euc_=m->euc_;
  cost_=m->cost_;
  mapping_=m->mapping_;
  tail1_=m->tail1_;
  tail2_=m->tail2_;

}

short match_data::version() const
{
  return 1;
}
   

//: Binary save self to stream.
void match_data::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, this->match_curve_set);
  vsl_b_write(os, this->curve_set);
  vsl_b_write(os, this->R_);
  vsl_b_write(os, this->T_);
  vsl_b_write(os, this->scale_);
  vsl_b_write(os, this->Tbar);
  vsl_b_write(os, this->energy_);
  vsl_b_write(os, this->euc_);
  vsl_b_write(os, this->cost_);
  vsl_b_write(os, this->mapping_);
  vsl_b_write(os, this->tail1_);
  vsl_b_write(os, this->tail2_);

}
  
//: Binary load self from stream.
void match_data::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
      vsl_b_read(is, this->match_curve_set);
      vsl_b_read(is, this->curve_set);
      vsl_b_read(is, this->R_);
      vsl_b_read(is, this->T_);
      vsl_b_read(is, this->scale_);
      vsl_b_read(is, this->Tbar);
      vsl_b_read(is, this->energy_);
      vsl_b_read(is, this->euc_);
      vsl_b_read(is, this->cost_);
      vsl_b_read(is, this->mapping_);
      vsl_b_read(is, this->tail1_);
      vsl_b_read(is, this->tail2_);
  
  break;

  default:
    vcl_cerr << "I/O ERROR: dbctrk_tracker_curve::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
vcl_string match_data::is_a() const
{
  return vcl_string("match_data");
}

bool match_data::is_class(vcl_string const& string) const
{
  if(string==is_a())
    return true;
  else
    return false;
}



void vsl_b_write(vsl_b_ostream & os, const match_data * p)
{
  if (!p){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}
void vsl_b_read(vsl_b_istream &is,match_data * &p)
{
  delete(p);
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    p = new match_data();
    p->b_read(is);
  }
  else
    p = 0;
}
void match_data::print_summary(vcl_ostream &os) const
{
  os<<"\n the energy of the match is "<<this->energy_;
}
dbctrk_tracker_curve  :: dbctrk_tracker_curve()
{
  id_=-1;
  match_id_=id_;
  group_id_=id_;
  track_id_=-1;
  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  ismovingobject_=false;
  frame_number=-1;

  desc=0;
  best_match_next_=0;
  best_match_prev_=0;
  isnextclosure_=-100;
  isprevclosure_=-100; 
}

void dbctrk_tracker_curve  ::init_set(vtol_edge_2d_sptr const& c,int id)
{
  c_=c;
  id_=id;

  match_id_=id_;
  group_id_=id_;

  next_.clear();
  prev_.clear();

  best_match_next_=0;
  best_match_prev_=0;

  isreal_=true;
  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  ismovingobject_=false;
  isnextclosure_=-100;
  isprevclosure_=-100; 
  vdgl_edgel_chain_sptr  ec;
  vdgl_edgel_chain_sptr ec_sub=new vdgl_edgel_chain;

 
  ec=c->curve()->cast_to_vdgl_digital_curve()->get_interpolator()->get_edgel_chain();
  vcl_vector<vsol_point_2d_sptr> vsolpoints;
  
// vcl_cout<<"\n"<<ec->size();
  //subsampling the curves so as to cut the computational time short
  for (unsigned int i=0; i<ec->size(); )
  {
    ec_sub->add_edgel(ec->edgel(i));
    vsol_point_2d_sptr temp=new vsol_point_2d(ec->edgel(i).x(),ec->edgel(i).y());
    vsolpoints.push_back(temp);
    i+=1;
  }
  dc_=new vsol_digital_curve_2d(vsolpoints);
  desc= new dbctrk_curve_description(ec_sub);
  ec_=ec_sub;

}

void dbctrk_tracker_curve  ::init_set(vcl_vector<vgl_point_2d<double> > p,int id)
{
  c_=0;
  id_=id;
  match_id_=id_;
  group_id_=id_;
  next_.clear();
  prev_.clear();
  best_match_next_=0;
  best_match_prev_=0;

  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  isreal_=true;
  ismovingobject_=false;
  isnextclosure_=-100;
  isprevclosure_=-100; 
  vdgl_digital_curve_sptr dc;
  vdgl_edgel_chain_sptr  ec;

  dc=dbctrk_algs::create_digital_curves(p);
  ec=dc->get_interpolator()->get_edgel_chain();

  desc= new dbctrk_curve_description(ec);
}
void dbctrk_tracker_curve  ::init_set(dbctrk_tracker_curve_sptr c,vcl_vector<int> ks,int id)
{
  c_=0;
  id_=id;
  match_id_=id_;
  group_id_=id_;
  next_.clear();
  prev_.clear();
  best_match_next_=0;
  best_match_prev_=0;

  ismatchedprev_=false;
  ismatchednext_=false;
  isreliable_=false;
  isreal_=false;

  isnextclosure_=-100;
  isprevclosure_=-100;  

  vdgl_edgel_chain_sptr  ec;
  vdgl_edgel_chain_sptr ec_sub=new vdgl_edgel_chain;
  vtol_edge_2d_sptr vc=c->get_curve();
  ec=vc->curve()->cast_to_vdgl_digital_curve()->get_interpolator()->get_edgel_chain();
  for (unsigned int i=0;i<ks.size();i++)
  {
    ec_sub->add_edgel(c->ec_->edgel(ks[i]));
  }
  desc= new dbctrk_curve_description(ec_sub);
}
double dbctrk_tracker_curve::compute_mean(vcl_vector<double> t)
{
  double sum=0;
  for (unsigned int i=0; i<t.size(); ++i)
    sum+=t[i];
  sum=sum/t.size();
  return sum;
}

void dbctrk_tracker_curve
        ::compute_transformation(vcl_vector<vgl_point_2d<double> > p,
                                 vcl_vector<vgl_point_2d<double> > & transformed_curve,
                                 vnl_matrix<double> R,vnl_matrix<double> T)
{
  vcl_vector<double> x;
  vcl_vector<double> y;

  for (unsigned int i=0; i<p.size(); ++i)
  {
    x.push_back(p[i].x());
    y.push_back(p[i].y());
  }

  double x_cen=compute_mean(x);
  double y_cen=compute_mean(y);

  for (unsigned int i=0; i<p.size(); ++i)
  {
    double tempx=p[i].x()-x_cen;
    double tempy=p[i].y()-y_cen;

    double tx=R(0,0)*tempx + R(0,1)*tempy + T(0,0)+x_cen;
    double ty=R(1,0)*tempx + R(1,1)*tempy + T(1,0)+y_cen;

    vgl_point_2d<double> temp(tx,ty);
    transformed_curve.push_back(temp);
  }
}

double dbctrk_tracker_curve ::compute_euclidean_distance(vnl_matrix<double> R,vnl_matrix<double> T,double s)
{
  if(get_best_match_prev())
  {
  double cost=0;
  vcl_map<int,int>::iterator iter;
  vcl_vector<vgl_point_2d<double> > curve1;
  vcl_vector<vgl_point_2d<double> > tcurve1;
  vcl_vector<vgl_point_2d<double> > curve2;
  for(iter=get_best_match_prev()->mapping_.begin();
    iter!=get_best_match_prev()->mapping_.end();
    iter++)
  {
    double tempx1=get_best_match_prev()->match_curve_set[0]->desc->curve_->point((*iter).first).x();
    double tempy1=get_best_match_prev()->match_curve_set[0]->desc->curve_->point((*iter).first).y();
    vgl_point_2d<double> point1(tempx1,tempy1);
    curve1.push_back(point1);

    double tempx2=desc->curve_->point((*iter).second).x();
    double tempy2=desc->curve_->point((*iter).second).y();
    vgl_point_2d<double> point2(tempx2,tempy2);
    curve2.push_back(point2);
  }
  compute_transformation(curve1,tcurve1,R,T);
  unsigned int min_index=0;
  for(unsigned int i=0;i<tcurve1.size();i++)
  {
    double min_dist=1e6;
    for(unsigned int j=0;j<curve2.size();j++)
    {
      double dist=vcl_sqrt((tcurve1[i].x()-curve2[j].x())*(tcurve1[i].x()-curve2[j].x())
                +(tcurve1[i].y()-curve2[j].y())*(tcurve1[i].y()-curve2[j].y()));
      if(min_dist>dist)
      {
        min_dist=dist;
        min_index=j;
      }
    }
    if(min_dist<1e6)
      cost+=min_dist;
  }
  cost/=get_best_match_prev()->mapping_.size();
  return cost;
  }
  else
  {
    return -1;
  }


}

double dbctrk_tracker_curve ::compute_euclidean_distance_next(vnl_matrix<double> R,vnl_matrix<double> T,double s)
{
  if (get_best_match_next())
  {
   double x1,y1,x2,y2;
   double dist=0;
   vcl_map<int,int> alignment= get_best_match_next()->mapping_;
   vcl_map<int,int>::iterator iter1;
   double H[2]={0,0};
   for (iter1 = alignment.begin(); iter1!=alignment.end(); iter1++)
   {

   H[0]=desc->curve_->point((*iter1).first).x();
   H[1]=desc->curve_->point((*iter1).first).y();
   vnl_matrix<double> X (H, 2, 1);
   vnl_matrix<double> Xt=R*X+T;

   x1=Xt(0,0);
   y1=Xt(1,0);

   x2=get_best_match_next()->match_curve_set[0]->desc->curve_->point((*iter1).second).x();
   y2=get_best_match_next()->match_curve_set[0]->desc->curve_->point((*iter1).second).y();
   dist+=vcl_sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
    
   }

  dist/=alignment.size();
  return dist;
  }
  else
    return -1.0;
}


short dbctrk_tracker_curve::version() const
{
  return 1;
}


//: Binary save self to stream.
void dbctrk_tracker_curve::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
 /* vsl_b_write(os, this->next_.size());
    for(int i=0;i<this->next_.size();i++)
      vsl_b_write(os, this->next_[i]);

    vsl_b_write(os, this->prev_.size());
    for(int i=0;i<this->prev_.size();i++)
      vsl_b_write(os, this->prev_[i]);

    vsl_b_write(os, this->seg_prev_.size());
    for(int i=0;i<this->seg_prev_.size();i++)
      vsl_b_write(os, this->seg_prev_[i]);
*/
 //  vsl_b_write(os, this->next_);
  vsl_b_write(os, this->prev_);
 // vsl_b_write(os, this->best_match_next_);
  vsl_b_write(os, this->best_match_prev_);
  vsl_b_write(os, this->id_);
  vsl_b_write(os, this->match_id_);
  vsl_b_write(os, this->group_id_);
  vsl_b_write(os, this->track_id_);
  vsl_b_write(os, this->frame_number);
  vsl_b_write(os, this->ismatchedprev_);
  vsl_b_write(os, this->ismatchednext_);
  vsl_b_write(os, this->ismovingobject_);
  vsl_b_write(os, this->grouped_);
  vsl_b_write(os, this->isreliable_);
  vsl_b_write(os, this->isreal_);
  vsl_b_write(os, this->spatialsig);
  vsl_b_write(os, this->desc);
  vsl_b_write(os, this->neighbors_);

}
  
//: Binary load self from stream.
void dbctrk_tracker_curve::b_read(vsl_b_istream &is)
{
  if (!is) return;

  short ver;
  vsl_b_read(is, ver);
  switch(ver)
  {
  case 1:
  /*  int n;
    vsl_b_read(is, n);
    this->next_.clear();
    for(int i=0;i<n;i++)
      {
        match_data_sptr temp;
        vsl_b_read(is,temp);
        this->next_.push_back(temp);
      }
    vsl_b_read(is, n);
    this->prev_.clear();
    for(int i=0;i<n;i++)
      {
        match_data_sptr temp;
        vsl_b_read(is,temp);
        this->prev_.push_back(temp);
      }
    vsl_b_read(is, n);
    this->seg_prev_.clear();
    for(int i=0;i<n;i++)
      {
        match_data_sptr temp;
        vsl_b_read(is,temp);
        this->seg_prev_.push_back(temp);
      }
*/
 //   vsl_b_read(is, this->next_);
    vsl_b_read(is, this->prev_);
 //   vsl_b_read(is, this->best_match_next_);
    vsl_b_read(is, this->best_match_prev_);

  if(this->best_match_prev_)
  {
    match_data_sptr temp=new match_data(this,this->best_match_prev_);
    this->best_match_prev_->match_curve_set[0]->set_best_match_next(temp);  
  }
    vsl_b_read(is, this->id_);
    vsl_b_read(is, this->match_id_);
    vsl_b_read(is, this->group_id_);
    vsl_b_read(is, this->track_id_);
    vsl_b_read(is, this->frame_number);
    vsl_b_read(is, this->ismatchedprev_);
    vsl_b_read(is, this->ismatchednext_);
    vsl_b_read(is, this->ismovingobject_);
    vsl_b_read(is, this->grouped_);
    vsl_b_read(is, this->isreliable_);
    vsl_b_read(is, this->isreal_);
    vsl_b_read(is, this->spatialsig);
    vsl_b_read(is, this->desc);
    vsl_b_read(is, this->neighbors_);

  
  for(unsigned int i=0;i<this->prev_.size();i++)
  {
    match_data_sptr temp=new match_data(this,this->prev_[i]);
    
    this->prev_[i]->match_curve_set[0]->next_.push_back(temp);
//    vcl_cout<<"\n the frame num is "<<temp->match_curve_set[0]->frame_number;
    vcl_sort(this->prev_[i]->match_curve_set[0]->next_.begin(),this->prev_[i]->match_curve_set[0]->next_.end(),less_cost());
   }
    break;

  default:
    vcl_cerr << "I/O ERROR: dbctrk_tracker_curve::b_read(vsl_b_istream&)\n"
             << "           Unknown version number "<< ver << '\n';
    is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
    return;
  }
}
vcl_string dbctrk_tracker_curve::is_a() const
{
  return vcl_string("dbctrk_tracker_curve");
}

bool dbctrk_tracker_curve::is_class(vcl_string const& string) const
{
  if(string==is_a())
    return true;
  else
    return false;
}



void vsl_b_write(vsl_b_ostream & os, const dbctrk_tracker_curve* p)
{
  if (!p){
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}
void vsl_b_read(vsl_b_istream &is, dbctrk_tracker_curve* &p)
{
  delete(p);
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr){
    p = new dbctrk_tracker_curve();
    p->b_read(is);
  }
  else
    p = 0;
}

void dbctrk_tracker_curve::print_summary(vcl_ostream &os) const
{
  os<<"\n the id of the curve is "<<this->id_;
}


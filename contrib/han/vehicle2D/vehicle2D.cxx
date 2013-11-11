#include "vehicle2D.h"
#include <vcl_cmath.h> // for vcl_sqrt()


//constructor from 2 sets of points.. 
vehicle2D::vehicle2D(vcl_vector<vnl_double_2 >p0,vcl_vector<vnl_double_2 >p1)
{
  for (unsigned i=0;i<p0.size();i++)
    p0_.push_back(p0[i]);
  for (unsigned i=0;i<p1.size();i++)
    p1_.push_back(p1[i]);

  double max_z0=-10000000000.0,max_z1=-1000000000.0;
  for (unsigned i=0;i<p0.size();i++)
  {

    if (p0[i][1]>max_z0) {
      max_z0=p0[i][1]; 
    }

  }
  for (unsigned i=0;i<p1.size();i++)
  {
    if (p1[i][1]>max_z1) {
      max_z1=p1[i][1]; 
    }
  }
  double max_z;
  if (max_z1>max_z0) max_z=max_z0; //need smaller one(joint)
  else max_z=max_z1;

  double min_z0=1000000000000.0;
  for (unsigned i=0;i<p0.size();i++)
  {

    if (p0[i][1]<min_z0) {
      min_z0=p0[i][1]; 
    }
  }
  water_level_step_=vcl_fabs(p0[1][1]-p0[0][1]);
  water_level_min_=min_z0;
  water_level_max_=max_z;
  //
}


//
void vehicle2D::print()
{
  vcl_cout<<"*******************************"<<vcl_endl;
  vcl_cout<<"max  z: "<<water_level_max_<<vcl_endl;
  vcl_cout<<"min  z: "<<water_level_min_<<vcl_endl;
  vcl_cout<<"step z: "<<water_level_step_<<vcl_endl;
  vcl_cout<<"-------------------------------"<<vcl_endl;
  for (unsigned i=0;i<p0_.size();i++)
    vcl_cout<<(p0_[i][0]-p1_[i][0])<<vcl_endl;
  vcl_cout<<"*******************************"<<vcl_endl;

}

//void 

void vehicle2D::com()  //center of mass
{
  vcl_cout<<"*******************************"<<vcl_endl;
  vcl_cout<<"max  z: "<<water_level_max_<<vcl_endl;
  vcl_cout<<"min  z: "<<water_level_min_<<vcl_endl;
  vcl_cout<<"step z: "<<water_level_step_<<vcl_endl;
  vcl_cout<<"-------------------------------"<<vcl_endl;
  double percentage=65;
  double sum0=0.0; int counter=0;
  for (unsigned i=0;i<percentage/100.0*p0_.size();i++)
  {

    sum0+=(p0_[i][0]+p1_[i][0])/2.0;
    vcl_cout<<(p0_[i][0]+p1_[i][0])/2.0<<" "<<(p0_[i][0]-p1_[i][0])<<vcl_endl;
    counter++;
  }

  vcl_cout<<"lower part sum: "<<sum0/counter/50<<vcl_endl;
  vcl_cout<<"-------------------------------"<<vcl_endl;
  double sum1=0.0;
  counter=0;
  for (unsigned i=unsigned (percentage/100.0*p0_.size());i<(p0_.size()-2);i++) //don't use top 2...
  {
    sum1+=(p0_[i][0]+p1_[i][0])/2.0;
    vcl_cout<<(p0_[i][0]+p1_[i][0])/2.0<<" "<<(p0_[i][0]-p1_[i][0])<<vcl_endl;
    counter++;
  }

  vcl_cout<<"cabin part sum: "<<sum1/counter/50<<vcl_endl;
  vcl_cout<<"-------------------------------"<<vcl_endl;

  vcl_cout<<"*******************************"<<vcl_endl;

}

// length of given stripes and center  of given stripes.
vnl_double_4 vehicle2D::sum(double z_lower_bound, double z_upper_bound)
{// bounds are percentage 1~100%

  double sum_center=0.0;
  double sum_p0=0,sum_p1=0;
  double counter_center=0;
  for (unsigned i=unsigned (z_lower_bound/100.0*p0_.size());i<(p0_.size()-1)&& i<z_upper_bound/100.0*p0_.size();i++) //don't use top 2...
  {
    sum_center+=(p0_[i][0]+p1_[i][0])/2.0;
    //vcl_cout<<(p0_[i][0]+p1_[i][0])/2.0<<" "<<(p0_[i][0]-p1_[i][0])<<vcl_endl;
    counter_center++;

    sum_p0+=p0_[i][0];
    sum_p1+=p1_[i][0];
  }
  sum_center/=counter_center;
  sum_p0/=counter_center;
  sum_p1/=counter_center;

  double sum_length=0.0;
  double counter_length=0;
  for (unsigned i=unsigned (z_lower_bound/100.0*p0_.size());i<(p0_.size()-1)&&i<z_upper_bound/100.0*p0_.size();i++) //don't use top 2...
  {
    sum_length+=vcl_fabs((p0_[i][0]-p1_[i][0]));

    counter_length++;
  }
  sum_length/=counter_length;

  vnl_double_4 a(sum_length,sum_center,sum_p0,sum_p1);
  vcl_cout<<z_lower_bound<<" "<<z_upper_bound<<" :"<<"length: "<<a[0]<<" center: "<<a[1]
  <<" p0: "<<a[2]<<" p1: "<<a[3]<< " #:"<<counter_center<<vcl_endl;
  return a;

}
//search around at 65 % level.. 
// using
// 1. multi scale: concavity..
// 2. 
// 3.

void vehicle2D::find_probe_8() 
{

   vcl_cout<<"--- find probe 8 ---"<<vcl_endl;
  // first, find best difference between cabin and body susing banded filter.
  double percentage=65;
  double sum0=0.0; int counter=0;
  double diff_max=-10000.0;
  double diff_max_perc=-1.0;
  for (double perc=25; perc<=75; perc+=2.5)
  {
    vcl_cout<<"--- "<<perc-30<<"~"<<perc<<"~"<<perc+30<< " ---"<<vcl_endl;
    vnl_double_4 body=sum(perc-30, perc);
    vnl_double_4 high=sum(perc, perc+30);
    double diff=body[0]-high[0];
    vcl_cout<<diff<<vcl_endl;

    if (diff>diff_max) {
      diff_max=diff;
      diff_max_perc=perc;
    }
  }


  //second, find the front probe 7
  double body_min=10000.0;
  double body_min_perc=-1.0;
  double body_max=-10000.0;
  double body_max_perc=-1.0;

  for (double perc=20; perc<=50; perc+=2.5)
  {
    vcl_cout<<"--- "<<perc-20<<"~"<<perc<< " ---"<<vcl_endl;
    vnl_double_4 body=sum(perc-20, perc);
    //vnl_double_4 high=sum(perc, perc+30);

    if (body[2]<body_min) { // body [2] is probe 7
      body_min=body[2];
      body_min_perc=perc;
    }

    
    if (body[3]>body_max) { // body [3] is probe 0
      body_max=body[3];
      body_max_perc=perc;
    }
  }
  vnl_double_2 probe_7(body_min/50,perc_translator(body_min_perc)/50);
  vnl_double_2 probe_0(body_max/50,perc_translator(body_max_perc)/50);

  //third, find canbin top

  vnl_double_4 cabin=sum(diff_max_perc,diff_max_perc+20); //20 is good?
  vcl_cout<<"cabin "<<cabin<<vcl_endl;
  vnl_double_2 probe_8(cabin[2]/50,perc_translator(diff_max_perc)/50);


  vcl_cout <<"probe_7: "<< probe_7<<" --probe_8: "<<probe_8<<" --probe_0: "<<probe_0<<" perc 0% is "<<perc_translator(0)/50<<vcl_endl;
  vcl_cout <<"ratio(cabin/length) "<< (probe_8[0]-probe_0[0])/(probe_7[0]-probe_0[0])<<vcl_endl;

  vcl_cout <<"vehicle height is "<<water_level_max_/50-perc_translator(0)/50+.5<<vcl_endl;

  vcl_cout<<"--- find probe 8 end ---"<<vcl_endl;
}

//translate percentage to feet
double vehicle2D::perc_translator(double perc)
{
  double min_z=water_level_min_;
  double max_z=water_level_max_;
  return ((max_z-min_z)*perc/100+min_z);
}

//constructor for model.. 
vehicle2D::vehicle2D(double total_l,  double bonnet_l, double trunk_l, //double cabin_l,
                     double total_h, double front_top_h, double front_windshield_h,double sp)//start point            
{


  //test
  water_level_min_=303.468;
  height_=total_h+water_level_min_;
  water_level_step_=5;

  bl_=bonnet_l;
  tl_=trunk_l;

  //body 

  for (double w=water_level_min_;w<height_-50; w+=water_level_step_) //zt is height of trunk
  {
    vnl_double_2 p0(0+sp,w);
    p0_.push_back(p0);

    vnl_double_2 p1(total_l+sp,w);
    p1_.push_back(p1);
  }


  //cabin

  double bl=bl_; // bonnet length
  double tl=tl_;  //trunk length
  for (double w=height_-50;w<height_; w+=water_level_step_) //zt is height of trunk
  {


    vnl_double_2 p0(bl+sp,w);
    p0_.push_back(p0);
    bl-=.05; //front windshield slope.

    vnl_double_2 p1(total_l-tl+sp,w);
    p1_.push_back(p1);
    tl+=.01; 
  }


}

//checking similarity of two vehicle 2D
double vehicle2D::sim_check(vehicle2D A, vehicle2D B)
{

  double sum=0.0;
  for (unsigned i=0;i<A.p0_.size()&&i<A.p1_.size()&&B.p0_.size()&&B.p1_.size();i++)
  {
    sum+=overlap(A.p0_[i],A.p1_[i],B.p0_[i],B.p1_[i]);
  }
  return sum;

}


//checking similarity of two vehicle 2D
void vehicle2D::shift(double sh)
{

  for (unsigned i=0;i<p0_.size() ;i++)
    p0_[i][0]+=sh;
  for (unsigned i=0;i<p1_.size() ;i++)
    p1_[i][0]+=sh;

}

//1d overlapping distance.
double vehicle2D::overlap(vnl_double_2 a, vnl_double_2 b,vnl_double_2 c, vnl_double_2 d)
{
  //a[0]<b[0] by definition c[0]<d[0]


  double ab=vcl_fabs(a[0]-b[0]);
  double cd=vcl_fabs(c[0]-d[0]);

  if (ab==0||cd==0) return 0;

  if (d[0]<=a[0]) return 0; //  c----d  a-----b case
  if (b[0]<=c[0]) return 0; //  a----b  c-----d case
  if (c[0]<=a[0]&&b[0]<=d[0]) return ab;  //   c---a===b---d case
  if (c[0]>=a[0]&&b[0]>=d[0]) return cd;  //   a---c===d---b case

  if (a[0]<=c[0]&&c[0]<=b[0]&& b[0]<=d[0]) return vcl_abs(b[0]-c[0]);

  return -.1;
}

vehicle2D::vehicle2D(vcl_vector<vgl_point_3d<double> >pts, int pts_flag)
{
  if (pts_flag==0) {//all pts
    pts_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_.push_back(*it);
    }
  }
  else if (pts_flag==1) //front pts
  {
    pts_front_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_front_.push_back(*it);
    }
  }
  else if (pts_flag==2) //rear pts
  {
    pts_rear_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_rear_.push_back(*it);
    }
  }
  else ;

}

/*void vehicle2D::multiscale_angle(const vcl_vector <vnl_double_2> a)
{
for (unsigned i=0; i<a.size();i++) 
{




}


}*/
////////////
///////////
///////////
///////////

void vehicle2D::set(vcl_vector<vgl_point_3d<double> >pts, int pts_flag)
{
  if (pts_flag==0) {//all pts
    pts_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_.push_back(*it);
    }
  }
  else if (pts_flag==1) //front pts
  {
    pts_front_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_front_.push_back(*it);
    }
  }
  else if (pts_flag==2) //rear pts
  {
    pts_rear_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts.begin(); it != pts.end(); ++it)
    {
      pts_rear_.push_back(*it);
    }
  }
  else ;

}



double vehicle2D::curvature(double scale,int pts_flag) 
{
  vcl_vector<vgl_point_3d<double> > pts_here=copy_out(pts_flag);

  for (unsigned i=1;i<pts_here.size()-1;i++) { //avoid first and end points
    bool flag=true;
    vgl_sphere_3d<double> u(pts_here[i] , scale);
    vgl_point_3d <double>psp,pep,nep,nsp,p1,p2,Ppos,Pneg; //previous start point, next end point ..etc
    unsigned j=0;
    nsp=pts_here[i];
    nep=pts_here[i+1];
    //vcl_cout<<nsp<<nep<<vcl_endl;
    while (u.contains(nep)) {
      j++;
      if (i+j>pts_here.size()-1 || i+j-1<0 ) {
        flag=false; 
        break;
      }
      nep=pts_here[i+j];
      nsp=pts_here[i+j-1];
    }
    vgl_line_3d_2_points<double> ln(nsp,nep);
    u.clip(ln, p1, p2);
    double d1=vgl_distance(nep,p1);
    double d2=vgl_distance(nep,p2);
    if (d1<d2) Ppos=p1;
    else Ppos=p2;

    unsigned k=0;
    psp=pts_here[i];
    pep=pts_here[i-1];
    while (u.contains(pep)) {
      k--;
      if (i+k>pts_here.size()-1 || i+k-1<0 ) {
        flag=false;
        break; 
      }
      psp=pts_here[i+k];
      pep=pts_here[i+k-1];
    }
    vgl_line_3d_2_points<double> lp(psp,pep);
    u.clip(lp, p1, p2);
    double d3=vgl_distance(pep,p1);
    double d4=vgl_distance(pep,p2);
    if (d3<d4) Pneg=p1;
    else Pneg=p2;


    double C=local_curvature(Ppos,pts_here[i],Pneg);
    vcl_cout<<i<<" "<<C<<vcl_endl;
  }
  return 0.0;
}


vcl_vector<vgl_point_3d<double> > vehicle2D::copy_out(int pts_flag)
{
  vcl_vector<vgl_point_3d<double> >pts_here;
  if (pts_flag==0) {//all pts
    //pts_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts_.begin(); it != pts_.end(); ++it)
    {
      pts_here.push_back(*it);
    }
  }
  else if (pts_flag==1) //front pts
  {
    //pts_front_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts_front_.begin(); it != pts_front_.end(); ++it)
    {
      pts_here.push_back(*it);
    }
  }
  else if (pts_flag==2) //rear pts
  {
    //pts_rear_.clear();
    vcl_vector<vgl_point_3d<double> >::const_iterator it;
    for (it=pts_rear_.begin(); it != pts_rear_.end(); ++it)
    {
      pts_here.push_back(*it);
    }
  }
  else ;

  return pts_here;

}

vcl_vector <double> vehicle2D::distance(int flag)
{

  vcl_vector<vgl_point_3d<double> >pts=copy_out(flag);
  vcl_vector <double> dl;
  for (unsigned i=1;i<pts.size();i++) {
    double d=vgl_distance(pts[i-1],pts[i]);
    dl.push_back(d);
  }

  return dl;
}



// usage:    eg.) read_wrl_file(pts_f,"delf.wrl",true);
int vehicle2D::read_wrl_file(vcl_vector <vgl_point_3d<double> > & ptl,vcl_string argv,bool rotation){
  vcl_ifstream in(argv.c_str());
  //in >> cam_matrix;
  if(in.fail()){
    vcl_cerr << "Failed to read file "<<argv << vcl_endl;
    exit(-4);
    return -1;
  }

  // in
  vcl_string hhh="";
  double x,y,z;
  //vcl_vector<vgl_point_3d<double> >ptl;
  while (!in.eof()) {

    in>>hhh;
    if (hhh=="translation") {
      in>>x>>y>>z;
      vgl_point_3d<double> p3d(x,y,z);
      ptl.push_back(p3d);
      /////##comented out 10-2-2006##/// vcl_cout<<p3d<<vcl_endl;
    }
  }
  vcl_cerr<<"vrml points size: "<<ptl.size()<<vcl_endl;
  if (0&&ptl.size()<20) {
    vcl_cout<<"Not enough points for recognition"<<vcl_endl;
    ////  vcl_cerr<<"Not enough points for recognition"<<vcl_endl;
    exit(555);
  }

  if (!rotation) return 1;
  vcl_cout<<R_<<vcl_endl;
  vcl_vector<vnl_double_3> pts_z;
  for (unsigned i=0;i<ptl.size();i++) {
    vnl_double_3 p(ptl[i].x(),ptl[i].y(),ptl[i].z());

    pts_z.push_back(R_*p);
    /////##comented out 10-2-2006##///vcl_cout<<R_*p<<vcl_endl;
  }

  // ///##comented out 10-2-2006##/// vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  ptl.clear();

  for (unsigned i=0;i<pts_z.size();i++) {
    vgl_point_3d <double> p(pts_z[i](0),pts_z[i](1),pts_z[i](2));

    ptl.push_back(p);
    /////##comented out 10-2-2006##/// vcl_cout<<p<<vcl_endl;
  }
  // ///##comented out 10-2-2006##/// vcl_cout<<"------------------------------------------------------"<<vcl_endl;
  in.close();

  return 1;
}

//int vehicle2D::read_bb_box_file(vcl_vector <vgl_point_3d<double> > & ptl,char* argv){
int vehicle2D::read_bb_box_file(vcl_string argv){
  vcl_ifstream in(argv.c_str());
  //in >> cam_matrix;
  if(in.fail()){
    vcl_cerr << "Failed to read file "<<argv << vcl_endl;
    return -1;
  }

  // in
  vcl_string hhh="";
  double x,y,z;
  double dummy;
  //vcl_vector<vgl_point_3d<double> >ptl;

  for (unsigned j=0;j<3;j++) {
    in >>hhh;
    vcl_cout<<hhh<<vcl_endl;
  }

  double temp;
  vnl_double_4x4 RT(0.0);
  for (unsigned i=0;i<4;i++) {
    for (unsigned j=0;j<4;j++) {
      in >> temp;
      RT[i][j]=temp;
    }
  }



  R_[0][0]=RT[0][0];R_[0][1]=RT[0][1];R_[0][2]=RT[0][2];
  R_[1][0]=RT[1][0];R_[1][1]=RT[1][1];R_[1][2]=RT[1][2];
  R_[2][0]=RT[2][0];R_[2][1]=RT[2][1];R_[2][2]=RT[2][2];

  R_.normalize_columns();
  vcl_cout<<R_<<vcl_endl;

  while (!in.eof()) {

    in>>hhh;
    /*  if (hhh=="Camera") {

    for (unsigned j=0;j<13;j++) in >>dummy;

    }
    else {
    x=atof(hhh.c_str());
    in>>y>>z;
    vgl_point_3d<double> p3d(x,y,z);
    ptl.push_back(p3d);
    vcl_cout<<p3d<<vcl_endl; 
    //exit(1);
    }*/
  }
  //vcl_cout<<"size: "<<ptl.size()<<vcl_endl;
  in.close();
  //exit(0);
  return 1;
}

// pts generation from pts_f and pts_r
void vehicle2D::pts_from_pts_f_and_r() 
{

}

vgl_polygon<double> vehicle2D::get_XY_con(int flag)
{
  vcl_vector <vgl_point_3d<double> >  pts=copy_out(flag);

  vcl_vector<vgl_point_2d<double> > ps;
  for(unsigned i=0;i<pts.size();i++)
  {
    vgl_point_2d<double> p(pts[i].x(),pts[i].z());
    ps.push_back(p);
  }

  vgl_convex_hull_2d<double> hullp(ps);
  vgl_polygon<double> psg=hullp.hull();
  vgl_polygon<double> p(ps);
  return p;
}








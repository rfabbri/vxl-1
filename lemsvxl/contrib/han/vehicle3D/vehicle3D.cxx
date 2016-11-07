#include "vehicle3D.h"
#include <vcl_cmath.h> // for vcl_sqrt()


vehicle3D::vehicle3D(vcl_vector<vgl_point_3d<double> >pts, int pts_flag)
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

void vehicle3D::set(vcl_vector<vgl_point_3d<double> >pts, int pts_flag)
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



double vehicle3D::curvature(double scale,int pts_flag) 
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

/*double vehicle3D::local_curvature(vgl_point_3d<double> p0,vgl_point_3d<double> p1,vgl_point_3d<double> p2)
{
  return 1.0;
}*/

vcl_vector<vgl_point_3d<double> > vehicle3D::copy_out(int pts_flag)
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

vcl_vector <double> vehicle3D::distance(int flag)
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
int vehicle3D::read_wrl_file(vcl_vector <vgl_point_3d<double> > & ptl,vcl_string argv,bool rotation){
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

//int vehicle3D::read_bb_box_file(vcl_vector <vgl_point_3d<double> > & ptl,char* argv){
int vehicle3D::read_bb_box_file(vcl_string argv){
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
void vehicle3D::pts_from_pts_f_and_r() 
{

}

vgl_polygon<double> vehicle3D::get_XY_con(int flag)
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








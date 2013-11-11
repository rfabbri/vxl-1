#include <dt/dbrl_clough_tocher_patch.h>
#include <vgl/vgl_vector_3d.h>

#include <vgl/vgl_triangle_test.h>

dbrl_clough_tocher_patch::dbrl_clough_tocher_patch()
{
}
dbrl_clough_tocher_patch::~dbrl_clough_tocher_patch()
{
}
void dbrl_clough_tocher_patch::set_points(vgl_point_3d<double> pt1,
                    vgl_point_3d<double> pt2,
                    vgl_point_3d<double> pt3)

{
    p1=pt1;
    p2=pt2;
    p3=pt3;
}
void dbrl_clough_tocher_patch::set_partials(vgl_vector_2d<double> fxy1,
                  vgl_vector_2d<double> fxy2,
                  vgl_vector_2d<double> fxy3)
{
    f1=fxy1;
    f2=fxy2;
    f3=fxy3;

}
void dbrl_clough_tocher_patch::set_cross_bnd_derivative(double f1nxy,double f2nxy,double f3nxy)
{
    f1n=f1nxy;
    f2n=f2nxy;
    f3n=f3nxy;

}

bool dbrl_clough_tocher_patch::intriangle(vgl_point_2d<double> p) const
{

    return vgl_triangle_test_inside(p1.x(), p1.y(),
                                            p2.x(), p2.y(),
                                            p3.x(), p3.y(),
                                            p.x(), p.y()) ;
}

bool dbrl_clough_tocher_patch::compute_cubic_control_points()
{
    //: p1p2c

    vgl_point_3d<double> pc=centre(p1,p2,p3);
    

    coeffs1[0]=p1.z();
    coeffs1[3]=p2.z();

    coeffs1[1]=p1.z()+((p2.x()-p1.x())*f1.x()+(p2.y()-p1.y())*f1.y())/3;
    coeffs1[2]=p2.z()+((p1.x()-p2.x())*f2.x()+(p1.y()-p2.y())*f2.y())/3;

    coeffs1[8]=p1.z()+((pc.x()-p1.x())*f1.x()+(pc.y()-p1.y())*f1.y())/3;
    coeffs1[4]=p2.z()+((pc.x()-p2.x())*f2.x()+(pc.y()-p2.y())*f2.y())/3;
    
    vgl_vector_2d<double> n12(p2.y()-p1.y(),p1.x()-p2.x());
    n12=normalize(n12);

    vgl_point_3d<double> temp1_210((2*p1.x()+p2.x())/3,(2*p1.y()+p2.y())/3,0.0);
    vgl_point_3d<double> temp1_111((p1.x()+p2.x()+pc.x())/3,(p1.y()+p2.y()+pc.y())/3,0.0);

    coeffs1[9]=(f1n*(temp1_111.x()-temp1_210.x())*(temp1_111.y()-temp1_210.y())+
        coeffs1[1]*(n12.x()*(temp1_111.y()-temp1_210.y())+n12.y()*(temp1_111.x()-temp1_210.x())))/
        (n12.x()*(temp1_111.y()-temp1_210.y())+n12.y()*(temp1_111.x()-temp1_210.x()));
    temp1_111.set(temp1_111.x(),temp1_111.y(),coeffs1[9]);


    //: p2p3c
    coeffs2[0]=p2.z();
    coeffs2[3]=p3.z();

    coeffs2[1]=p2.z()+((p3.x()-p2.x())*f2.x()+(p3.y()-p2.y())*f2.y())/3;
    coeffs2[2]=p3.z()+((p2.x()-p3.x())*f3.x()+(p2.y()-p3.y())*f3.y())/3;

    coeffs2[8]=p2.z()+((pc.x()-p2.x())*f2.x()+(pc.y()-p2.y())*f2.y())/3;
    coeffs2[4]=p3.z()+((pc.x()-p3.x())*f3.x()+(pc.y()-p3.y())*f3.y())/3;


    vgl_vector_2d<double> n23(p3.y()-p2.y(),p2.x()-p3.x());
    n23=normalize(n23);

    vgl_point_3d<double> temp2_210((2*p2.x()+p3.x())/3,(2*p3.y()+p3.y())/3,0.0);
    vgl_point_3d<double> temp2_111((p2.x()+p3.x()+pc.x())/3,(p2.y()+p3.y()+pc.y())/3,0.0);

    coeffs2[9]=(f2n*(temp2_111.x()-temp2_210.x())*(temp2_111.y()-temp2_210.y())+
        coeffs2[1]*(n23.x()*(temp2_111.y()-temp2_210.y())+n23.y()*(temp2_111.x()-temp2_210.x())))/
        (n23.x()*(temp2_111.y()-temp2_210.y())+n23.y()*(temp2_111.x()-temp2_210.x()));
    temp2_111.set(temp2_111.x(),temp2_111.y(),coeffs2[9]);

    //: p3p1c
    coeffs3[0]=p3.z();
    coeffs3[3]=p1.z();

    coeffs3[1]=p3.z()+((p1.x()-p3.x())*f3.x()+(p1.y()-p3.y())*f3.y())/3;
    coeffs3[2]=p1.z()+((p3.x()-p1.x())*f1.x()+(p3.y()-p1.y())*f1.y())/3;

    coeffs3[8]=p3.z()+((pc.x()-p3.x())*f3.x()+(pc.y()-p3.y())*f3.y())/3;
    coeffs3[4]=p1.z()+((pc.x()-p1.x())*f1.x()+(pc.y()-p1.y())*f1.y())/3;

    vgl_vector_2d<double> n31(p1.y()-p3.y(),p3.x()-p1.x());
    n31=normalize(n31);

    vgl_point_3d<double> temp3_210((2*p3.x()+p1.x())/3,(2*p1.y()+p1.y())/3,0.0);
    vgl_point_3d<double> temp3_111((p3.x()+p1.x()+pc.x())/3,(p3.y()+p1.y()+pc.y())/3,0.0);

    coeffs3[9]=(f3n*(temp3_111.x()-temp3_210.x())*(temp3_111.y()-temp3_210.y())+
        coeffs3[1]*(n31.x()*(temp3_111.y()-temp3_210.y())+n31.y()*(temp3_111.x()-temp3_210.x())))/
        (n31.x()*(temp3_111.y()-temp3_210.y())+n31.y()*(temp3_111.x()-temp3_210.x()));

    temp3_111.set(temp3_111.x(),temp3_111.y(),coeffs3[9]);



    //: rest of the coefficients are computed here

    //temp1_111
    //temp3_111

    vgl_point_3d<double> temp1_201(p1.x()*2/3+pc.x()/3,p1.y()*2/3+pc.y()/3,coeffs1[8]);
    vgl_vector_3d<double> n3d_13=cross_product(temp1_111-temp1_201,temp3_111-temp1_201);

    vgl_point_3d<double> temp1_102(p1.x()*1/3+pc.x()*2/3,p1.y()/3+pc.y()*2/3,0.0);
    double temp1_102_z=-((temp1_102.x()-temp1_201.x())*n3d_13.x()+(temp1_102.y()-temp1_201.y())*n3d_13.y())/n3d_13.z() + temp1_201.z();
    temp1_102.set(temp1_102.x(),temp1_102.y(),temp1_102_z);


    vgl_point_3d<double> temp2_201(p2.x()*2/3+pc.x()/3,p2.y()*2/3+pc.y()/3,coeffs2[8]);
    vgl_vector_3d<double> n3d_12=cross_product(temp2_111-temp2_201,temp2_111-temp1_201);

    vgl_point_3d<double> temp2_102(p2.x()*1/3+pc.x()*2/3,p2.y()/3+pc.y()*2/3,0.0);
    double temp2_102_z=-((temp2_102.x()-temp2_201.x())*n3d_12.x()+(temp2_102.y()-temp2_201.y())*n3d_12.y())/n3d_12.z() + temp2_201.z();
    temp2_102.set(temp2_102.x(),temp2_102.y(),temp2_102_z);

    vgl_point_3d<double> temp3_201(p3.x()*2/3+pc.x()/3,p3.y()*2/3+pc.y()/3,coeffs3[8]);
    vgl_vector_3d<double> n3d_23=cross_product(temp3_111-temp3_201,temp3_111-temp2_201);

    vgl_point_3d<double> temp3_102(p3.x()*1/3+pc.x()*2/3,p3.y()/3+pc.y()*2/3,0.0);
    double temp3_102_z=-((temp3_102.x()-temp3_201.x())*n3d_23.x()+(temp3_102.y()-temp3_201.y())*n3d_23.y())/n3d_23.z() + temp3_201.z();
    temp3_102.set(temp3_102.x(),temp3_102.y(),temp3_102_z);
    
    vgl_point_3d<double> temp_111=centre(temp1_102,temp2_102,temp3_102);

    coeffs1[7]=coeffs3[5]=temp1_102.z();
    coeffs2[7]=coeffs1[5]=temp2_102.z();
    coeffs3[7]=coeffs2[5]=temp3_102.z();
    
    coeffs1[6]=coeffs2[6]=coeffs3[6]=temp_111.z();



    return true;
}
double dbrl_clough_tocher_patch::interpolate(vgl_point_2d<double> p)const
{
    vgl_vector_3d<double> pbc=convert_to_barycentric(p,vgl_point_2d<double>(p1.x(),p1.y()),
                                                       vgl_point_2d<double>(p2.x(),p2.y()),
                                                       vgl_point_2d<double>(p3.x(),p3.y()));
                                                       
    int triindex=find_mini_triangle(pbc);
    vgl_point_3d<double> pcentre=centre(p1,p2,p3);
    if(triindex>0 && triindex<=3)
    {
      double fp;
      switch(triindex)
      {
      case 1:  
          pbc=convert_to_barycentric(p,vgl_point_2d<double>(p1.x(),p1.y()),
                                                       vgl_point_2d<double>(p2.x(),p2.y()),
                                                       vgl_point_2d<double>(pcentre.x(),pcentre.y()));
          fp=compute_value(coeffs1,pbc);
          break;
      case 2:
          pbc=convert_to_barycentric(p,vgl_point_2d<double>(p2.x(),p2.y()),
                                                       vgl_point_2d<double>(p3.x(),p3.y()),
                                                       vgl_point_2d<double>(pcentre.x(),pcentre.y()));
          fp=compute_value(coeffs2,pbc);
          break;
      case 3:
          pbc=convert_to_barycentric(p,vgl_point_2d<double>(p3.x(),p3.y()),
                                                       vgl_point_2d<double>(p1.x(),p1.y()),
                                                       vgl_point_2d<double>(pcentre.x(),pcentre.y()));
          fp=compute_value(coeffs3,pbc);
          break;

      }
      return fp;
    }
    else
        return -1.0;
}
int dbrl_clough_tocher_patch::find_mini_triangle(vgl_vector_3d<double> p) const
{
 if(p.x()>=p.z() && p.y()>=p.z())
     return 1;
 else if(p.y()>=p.x() && p.z()>=p.x())
     return 2;
 else if(p.z()>=p.y() && p.x()>=p.y())
     return 3;
    
}

double dbrl_clough_tocher_patch::compute_value(const double coeffs[10],vgl_vector_3d<double> bcp) const
{
    
    double val=bcp.x()*bcp.x()*bcp.x()*coeffs[0]+3*bcp.x()*bcp.x()*bcp.y()*coeffs[1]+
        3*bcp.x()*bcp.y()*bcp.y()*coeffs[2]+bcp.y()*bcp.y()*bcp.y()*coeffs[3]+
        3*bcp.y()*bcp.y()*bcp.z()*coeffs[4]+bcp.y()*bcp.z()*bcp.z()*coeffs[5]+
        bcp.z()*bcp.z()*bcp.z()*coeffs[6]+3*bcp.x()*bcp.z()*bcp.z()*coeffs[7]+
        3*bcp.x()*bcp.x()*bcp.z()*coeffs[8]+6*bcp.x()*bcp.y()*bcp.z()*coeffs[9];
       
    return val;
}
vgl_vector_3d<double> dbrl_clough_tocher_patch::convert_to_barycentric(vgl_point_2d<double> p,
                                                                       vgl_point_2d<double> p1,
                                                                       vgl_point_2d<double> p2,
                                                                       vgl_point_2d<double> p3) const
{

    double tarea=vcl_fabs(cross_product(p2-p1,p3-p1));
    double u=vcl_fabs(cross_product(p2-p,p3-p))/tarea;
    double v=vcl_fabs(cross_product(p3-p,p1-p))/tarea;
    double w=vcl_fabs(cross_product(p1-p,p2-p))/tarea;

    return vgl_vector_3d<double>(u,v,w);
    
}

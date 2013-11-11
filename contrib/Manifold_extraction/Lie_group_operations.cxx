#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include "vehicle_model.h"
#include "Lie_group_operations.h"



void set_transformation_matrices(vnl_matrix<double>& m1,vnl_matrix<double>& m2,vnl_matrix<double>& m3,vehicle_model M,
                                 double s1x,double t1x,double s1y,double t1y,double s2x,double s2y,double s3x,double s3y,
                                 double t3x,double t3y)
    {

    m1.put(0,0,s1x);
    m1.put(1,1,s1y);
    m1.put(0,2,t1x);
    m1.put(1,2,t1y);
    m1.put(2,2,1);

    m2.put(0,0,s2x);
    m2.put(1,1,s2y);
    m2.put(2,2,1);

    m3.put(0,0,s3x);
    m3.put(1,1,s3y);
    m3.put(0,2,t3x);
    m3.put(1,2,t3y);
    m3.put(2,2,1);
    }




// only for m of size 3x3
//only scales and translates the box
void transform_box(vnl_matrix<double> m,vsol_rectangle_2d box)
    {

    vsol_point_2d_sptr v1 = box.p0();
    vsol_point_2d_sptr v2 = box.p1();
    vsol_point_2d_sptr v3 = box.p2();
    vsol_point_2d_sptr v4 = box.p3();

    double x,y;

    x = v1->x();
    y = v1->y();

    v1->set_x(m.get(0,0)*x+m.get(0,2));
    v1->set_y(m.get(1,1)*y+m.get(1,2));

    x = v2->x();
    y = v2->y();

    v2->set_x(m.get(0,0)*x+m.get(0,2));
    v2->set_y(m.get(1,1)*y+m.get(1,2));

    x = v3->x();
    y = v3->y();

    v3->set_x(m.get(0,0)*x+m.get(0,2));
    v3->set_y(m.get(1,1)*y+m.get(1,2));

    x = v4->x();
    y = v4->y();

    v4->set_x(m.get(0,0)*x+m.get(0,2));
    v4->set_y(m.get(1,1)*y+m.get(1,2));
    }



// only scales and translates the model 
//model should be aligned wrt x and y axes
vehicle_model transform_model(vehicle_model M,vnl_matrix<double> m1,vnl_matrix<double> m2,vnl_matrix<double> m3)
    {
    // translating the boxes so that the y coordinate of 
    // centroid becomes zero,then transform the boxes
    //with the supplied matrices and translate them back
    vsol_rectangle_2d tr_engine(M.engine());
    vsol_rectangle_2d tr_body(M.body());
    vsol_rectangle_2d tr_rear(M.rear());

    vcl_cout << m1 << vcl_endl;
    vcl_cout << m2 << vcl_endl;
    vcl_cout << m3 << vcl_endl;

    transform_box(m1,tr_engine);
    transform_box(m2,tr_body);
    transform_box(m3,tr_rear);

    vehicle_model tr_model(tr_engine,tr_body,tr_rear);
    return tr_model;
    }

void rotate_box(vnl_matrix<double> m,vsol_rectangle_2d box)
    {

    vsol_point_2d_sptr v1 = box.p0();
    vsol_point_2d_sptr v2 = box.p1();
    vsol_point_2d_sptr v3 = box.p2();
    vsol_point_2d_sptr v4 = box.p3();

    double x,y,z;

    // since only scaling and translation are done,the order of vertices would be preserved
    x = m.get(0,0)*v1->x()+m.get(0,1)*v1->y()+m.get(0,2);
    y = m.get(1,0)*v1->x()+m.get(1,1)*v1->y()+m.get(1,2);
    z =  m.get(2,2);

    v1->set_x(x/z);
    v1->set_y(y/z);

    x = m.get(0,0)*v2->x()+m.get(0,1)*v2->y()+m.get(0,2);
    y = m.get(1,0)*v2->x()+m.get(1,1)*v2->y()+m.get(1,2);

    v2->set_x(x/z);
    v2->set_y(y/z);

    x = m.get(0,0)*v3->x()+m.get(0,1)*v3->y()+m.get(0,2);
    y = m.get(1,0)*v3->x()+m.get(1,1)*v3->y()+m.get(1,2);

    v3->set_x(x/z);
    v3->set_y(y/z);

    x = m.get(0,0)*v4->x()+m.get(0,1)*v4->y()+m.get(0,2);
    y = m.get(1,0)*v4->x()+m.get(1,1)*v4->y()+m.get(1,2);

    v4->set_x(x/z);
    v4->set_y(y/z);

    }

vehicle_model rotate_model(vehicle_model M,double angle)
    {

    vnl_matrix<double>R(3,3,0.0); 

    R.put(0,0,vcl_cos(angle));
    R.put(0,1,-1*vcl_sin(angle));
    R.put(1,0,vcl_sin(angle));
    R.put(1,1,vcl_cos(angle));
    R.put(2,2,1);

    vsol_rectangle_2d rotated_engine(M.engine());
    vsol_rectangle_2d rotated_body(M.body());
    vsol_rectangle_2d rotated_rear(M.rear());

    rotate_box(R,rotated_engine);
    rotate_box(R,rotated_body);
    rotate_box(R,rotated_rear);

    vehicle_model rotated_model(rotated_engine,rotated_body,rotated_rear);

    return rotated_model;
    }


vnl_matrix<double> get_transformation_matrix(vsol_rectangle_2d from_box,vsol_rectangle_2d to_box)
    {
    vnl_matrix<double> from_points(3,4,1);
    vnl_matrix<double> to_points(3,4,1);

    from_points.put(0,0,from_box.p0()->x());
    from_points.put(1,0,from_box.p0()->y());

    from_points.put(0,1,from_box.p1()->x());
    from_points.put(1,1,from_box.p1()->y());

    from_points.put(0,2,from_box.p2()->x());
    from_points.put(1,2,from_box.p2()->y());

    from_points.put(0,3,from_box.p3()->x());
    from_points.put(1,3,from_box.p3()->y());

    to_points.put(0,0,to_box.p0()->x());
    to_points.put(1,0,to_box.p0()->y());

    to_points.put(0,1,to_box.p1()->x());
    to_points.put(1,1,to_box.p1()->y());

    to_points.put(0,2,to_box.p2()->x());
    to_points.put(1,2,to_box.p2()->y());

    to_points.put(0,3,to_box.p3()->x());
    to_points.put(1,3,to_box.p3()->y());

    vnl_matrix<double> transformation_matrix = (to_points*from_points.transpose())* vnl_matrix_inverse<double>(from_points*from_points.transpose());
    return transformation_matrix;

    }

double matrix_log_dist(vnl_matrix<double> M1,vnl_matrix<double> M2)
    {
    vnl_matrix<double>resultant = M1*vnl_matrix_inverse<double>(M2);


    double sx = vcl_log(resultant.get(0,0));
    double sy = vcl_log(resultant.get(1,1));
    double tx = resultant.get(0,2);
    double ty = resultant.get(1,2);
    double distance = 1e10;

    // in order to take the log of the scaling factors,they have to be non negative
    sx = vcl_abs(sx);
    sy = vcl_abs(sy);

    if (~(sx == 1)&&~(sy == 1))
        {
        distance = vcl_sqrt(vcl_pow((sx),2) + vcl_pow((sy),2) +
            vcl_pow((tx*sx)/(resultant.get(0,0)-1),2) + vcl_pow((ty*sy)/(resultant.get(1,1)-1),2));
        }
    else
        {
        distance = vcl_sqrt(vcl_pow((sx),2) + vcl_pow((sy),2) +
            vcl_pow(tx,2) + vcl_pow(ty,2));
        }


    return distance;
    }

double calculate_Lie_distance_between_vectors
(vcl_vector<vsol_rectangle_2d >model1,vcl_vector<vsol_rectangle_2d  >model2,vcl_vector<vsol_rectangle_2d  >ref_model)
    {
    vnl_matrix<double> T11 = get_transformation_matrix(model1[0],ref_model[0]);
    vnl_matrix<double> T21 = get_transformation_matrix(model1[1],ref_model[1]);
    vnl_matrix<double> T31 = get_transformation_matrix(model1[2],ref_model[2]);

    vnl_matrix<double> T12 = get_transformation_matrix(model2[0],ref_model[0]);
    vnl_matrix<double> T22 = get_transformation_matrix(model2[1],ref_model[1]);
    vnl_matrix<double> T32 = get_transformation_matrix(model2[2],ref_model[2]);

    // vcl_cout << T11 << vcl_endl;

    double d = matrix_log_dist(T11,T12) + matrix_log_dist(T21,T22) + matrix_log_dist(T31,T32);
    //  double d = 0;
    return d;
    }

double calculate_Lie_distance(vehicle_model M1,vehicle_model M2,vehicle_model RM)
    {
    vnl_matrix<double> T11 = get_transformation_matrix(M1.engine(),RM.engine());
    vnl_matrix<double> T21 = get_transformation_matrix(M1.body(),RM.body());
    vnl_matrix<double> T31 = get_transformation_matrix(M1.rear(),RM.rear());

    vnl_matrix<double> T12 = get_transformation_matrix(M2.engine(),RM.engine());
    vnl_matrix<double> T22 = get_transformation_matrix(M2.body(),RM.body());
    vnl_matrix<double> T32 = get_transformation_matrix(M2.rear(),RM.rear());

    // vcl_cout << T11 << vcl_endl;

    double d = matrix_log_dist(T11,T12) + matrix_log_dist(T21,T22) + matrix_log_dist(T31,T32);
    //  double d = 0;
    return d;
    }

void get_transformation_matrices(vehicle_model M1,vehicle_model M2,vnl_matrix<double> &G1,
                                 vnl_matrix<double> &G2,vnl_matrix<double> &G3)
    {
    G1 = get_transformation_matrix(M1.engine(),M2.engine());
    G2 = get_transformation_matrix(M1.body(),M2.body());
    G3 = get_transformation_matrix(M1.rear(),M2.rear());

    }

vnl_matrix<double> get_Lie_algebra(vnl_matrix<double> G)             
    {
    // 3x3 Lie algebra element
    vnl_matrix<double> g(3,3,0.0);

    double g11,g12,g21,g22,g13,g23,det_val,tol = 1e-6;;
    //2x2 Identity element
    vnl_matrix<double> I(2,2,0.0);
    I.put(0,0,1);
    I.put(1,1,1);

    //2x2 rotation matrix 
    vnl_matrix<double> R(2,2,0.0);
    //2x2 scaling matrix
    vnl_matrix<double> S(2,2,0.0);
    vnl_matrix<double> P(2,2,0.0);

    //Lie algebra element corresponding to 2x2 rotation matrix
    vnl_matrix<double> A(2,2,0.0);

    double sin_theta,cos_theta,sx,sy,tx,ty;

    //get the scaling,rotation angle and translation values from G
    sx = vcl_sqrt(G.get(0,0)*G.get(0,0)+G.get(0,1)*G.get(0,1));
    sy = vcl_sqrt(G.get(1,0)*G.get(1,0)+G.get(1,1)*G.get(1,1));

    if (sy > tol)
        {
        sin_theta = G.get(1,0)/sy;
        cos_theta = vcl_sqrt(1-sin_theta*sin_theta);
        }
    else if (sx > tol)
        {
        cos_theta = G.get(0,0)/sx;
        sin_theta = vcl_sqrt(1-cos_theta*cos_theta);
        }
    else
        {
        cos_theta = 1;
        sin_theta = 0;
        }

    tx = G.get(0,2);
    ty = G.get(1,2);

    //setting the rotation matrix
    R.put(0,0,cos_theta);
    R.put(0,1,-sin_theta);
    R.put(1,0,sin_theta);
    R.put(1,1,cos_theta);

    //setting the entries of S
    S.put(0,0,sx);
    S.put(1,1,sy);

    //setting the entries of the Lie algebra element
    A.put(0,1,-1*vcl_acos(cos_theta));
    A.put(1,0,vcl_acos(cos_theta));

    if (sx > tol)
        g11 = vcl_log(sx);
    else
        g11 = 0;


    g12 = A.get(0,1);
    g21 = A.get(1,0);

    if (sy > tol)
        g22 = vcl_log(sy);
    else
        g22 = 0;


    //setting the entries of P
    P.put(0,0,g11);
    P.put(0,1,g12);
    P.put(1,0,g21);
    P.put(1,1,g22);

    det_val = P.get(0,0)*P.get(1,1) - P.get(0,1)*P.get(1,0);

    vnl_matrix<double> Pinv(2,2,0.0);

    if (vcl_abs(det_val) > 1e-6 )
        Pinv = vnl_matrix_inverse<double>(P);

    vnl_matrix<double> result1 = S*R;
    vnl_matrix<double> result2 = result1-I;
    vnl_matrix<double> res_inv = vnl_matrix_inverse<double>(result2);
    vnl_matrix<double> res = res_inv*P;

    if (((vcl_abs(sx -1) < tol)||(vcl_abs(sy -1) < tol))&&(vcl_abs(sin_theta) < tol))
        {
        g13 = tx;
        g23 = ty;
        }  
    else
        {
        g13 = res.get(0,0)*tx+ res.get(0,1)*ty;
        g23 = res.get(1,0)*tx+ res.get(1,1)*ty;
        }


    g.put(0,0,g11);
    g.put(0,1,g12);
    g.put(0,2,g13);
    g.put(1,0,g21);
    g.put(1,1,g22);
    g.put(1,2,g23);
    g.put(2,2,0);

    return g;
    }

vnl_matrix<double> get_Lie_algebra_9x9(vnl_matrix<double> G)
    {
    vnl_matrix<double>G1(3,3,0.0),G2(3,3,0.0),G3(3,3,0.0);
    vnl_matrix<double>g1,g2,g3,g(9,9,0.0);

    int num_rows = 3,num_cols = 3,i,j;

    for (i = 0;i<3;i++)
        {
        for (j = 0;j<3;j++)
            {
            G1.put(i,j,G.get(i,j));
            G2.put(i,j,G.get(i+num_rows,j+num_cols));
            G3.put(i,j,G.get(i+2*num_rows,j+2*num_cols));
            }
        }


    g1 = get_Lie_algebra(G1);
    g2 = get_Lie_algebra(G2);
    g3 = get_Lie_algebra(G3);

    for (i = 0;i<3;i++)
        {
        for (j = 0;j<3;j++)
            {
            g.put(i,j,g1.get(i,j));
            g.put(i+num_rows,j+num_cols,g2.get(i,j));
            g.put(i+2*num_rows,j+2*num_cols,g3.get(i,j));
            }
        }




    return g;
    }

vnl_matrix<double> get_Lie_group(vnl_matrix<double> g)   
    {
    double sx,sy,thet,det_val,tol = 1e-6;
    int i,j;
    vnl_matrix<double> G(3,3,0.0),I(2,2,0.0),
        S(2,2,0.0),R(2,2,0.0),rot_scale(2,2,0.0),P(2,2,0.0),Pinv(2,2,0.0),res(2,2,0.0);

    sx = vcl_pow(vcl_exp(1.0),g.get(0,0));
    sy = vcl_pow(vcl_exp(1.0),g.get(1,1));

    // thet = g.get(1,0);

    thet = (vcl_abs(g.get(1,0))+vcl_abs(g.get(0,1)))/2;

    R.put(0,0,vcl_cos(thet));
    R.put(0,1,-1*vcl_sin(thet));
    R.put(1,0,vcl_sin(thet));
    R.put(1,1,vcl_cos(thet));

    S.put(0,0,sx);
    S.put(1,1,sy);

    I.put(0,0,1);
    I.put(1,1,1);

    rot_scale = S*R;

    // vcl_cout << rot_scale << vcl_endl;

    for (i = 0;i<2;i++)
        for (j = 0;j<2;j++)
            G.put(i,j,rot_scale(i,j));

    for (i = 0;i<2;i++)
        for (j = 0;j<2;j++)
            P.put(i,j,g.get(i,j));

    det_val = P.get(0,0)*P.get(1,1) - P.get(0,1)*P.get(1,0);

    if (vcl_abs(det_val) > tol )
        Pinv = vnl_matrix_inverse<double>(P);

    res = Pinv*(rot_scale - I);

    double G13,G23;

    if (((vcl_abs(sx -1) < tol) || (vcl_abs(sy -1) < tol))&&(vcl_abs(thet) < tol))
        {
        G13 = g.get(0,2);
        G23 = g.get(1,2);
        }
    else
        {
        G13 = res.get(0,0)*g.get(0,2)+ res.get(0,1)*g.get(1,2);
        G23 = res.get(1,0)*g.get(0,2)+ res.get(1,1)*g.get(1,2);
        }

    G.put(0,2,G13);
    G.put(1,2,G23);
    G.put(2,2,1);

    return G;
    }

vnl_matrix<double> get_Lie_algebra_3d(vnl_matrix<double> G)
    {
  double sx,sy,sz,tol = 1e-6;
    int i,j;

    vnl_matrix<double> g(4,4,0.0);

   sx = vcl_log(G.get(0,0));
   sy = vcl_log(G.get(1,1));
   sz = vcl_log(G.get(2,2));

    g.put(0,0,sx);  
    g.put(1,1,sy); 
    g.put(2,2,sz); 
    g.put(3,3,0); 

    return g;
    }

//returns the Lie group element of an algebra element representing a 
//3d transformation

vnl_matrix<double> get_Lie_group_3d(vnl_matrix<double> g)   
    {
    double sx,sy,sz,tol = 1e-6;
    int i,j;

    vnl_matrix<double> G(4,4,0.0);

   sx = vcl_pow(vcl_exp(1.0),g.get(0,0));
   sy = vcl_pow(vcl_exp(1.0),g.get(1,1));
   sz = vcl_pow(vcl_exp(1.0),g.get(2,2));

    G.put(0,0,sx);  
    G.put(1,1,sy); 
    G.put(2,2,sz); 
    G.put(3,3,1); 

    return G;

    }

//this assumes that the 9x9 matrices whose lie group element is sought are 
//of the form G1 0 0
//            0 G2 0
//            0 0 G3
// where G1,G2 and G3 are 3x3 matrices 
vnl_matrix<double> get_Lie_group_9x9(vnl_matrix<double> g)   
    {
    vnl_matrix<double>g1(3,3,0.0),g2(3,3,0.0),g3(3,3,0.0);
    vnl_matrix<double>G1,G2,G3,G(9,9,0.0);

    int num_rows = 3,num_cols = 3,i,j;

    for (i = 0;i<3;i++)
        {
        for (j = 0;j<3;j++)
            {
            g1.put(i,j,g.get(i,j));
            g2.put(i,j,g.get(i+num_rows,j+num_cols));
            g3.put(i,j,g.get(i+2*num_rows,j+2*num_cols));
            }
        }


    G1 = get_Lie_group(g1);
    G2 = get_Lie_group(g2);
    G3 = get_Lie_group(g3);

    for (i = 0;i<3;i++)
        {
        for (j = 0;j<3;j++)
            {
            G.put(i,j,G1.get(i,j));
            G.put(i+num_rows,j+num_cols,G2.get(i,j));
            G.put(i+2*num_rows,j+2*num_cols,G3.get(i,j));
            }
        }

    return G;
    }

//the coupled transfomations lead to taking the projection of each of the elements
//onto a particular sub manifold 

void calculate_coupled_intrinsic_mean(vcl_vector<vnl_matrix<double> > box_one_elements,
                                      vcl_vector<vnl_matrix<double> > box_two_elements,
                                      vcl_vector<vnl_matrix<double> > box_three_elements,
                                      double M1_x2,double M1_x4,
                                      vnl_matrix<double> &In_mean_B1,
                                      vnl_matrix<double> &In_mean_B2,
                                      vnl_matrix<double> &In_mean_B3)
    {
    //create a 3x3 intrinsic mean element and initialize it to Identity element
    vnl_matrix<double> inv_In_mean_B1(3,3,0.0),inv_In_mean_B2(3,3,0.0),inv_In_mean_B3(3,3,0.0);
    vnl_matrix<double> S_1(3,3,0.0),S_2(3,3,0.0),S_3(3,3,0.0);
    vnl_matrix<double> S_tot_B1(3,3,0.0),S_tot_B2(3,3,0.0),S_tot_B3(3,3,0.0);
    vnl_matrix<double> S_sub_1(3,3,0.0),S_sub_2(3,3,0.0),S_sub_3(3,3,0.0);

    double tol = 1e-2,frob_norm = 1,num_elements = box_one_elements.size(),count = 0,tx;
    int i,j,num_iterations = 0;



    In_mean_B1 = *(box_one_elements.begin());
    In_mean_B2 = *(box_two_elements.begin());
    In_mean_B3 = *(box_three_elements.begin());


    while ((frob_norm > tol)& (num_iterations <= 1e5))
        {
        vcl_vector<vnl_matrix<double> >::iterator iter_B1 = box_one_elements.begin();
        vcl_vector<vnl_matrix<double> >::iterator iter_B2 = box_two_elements.begin();
        vcl_vector<vnl_matrix<double> >::iterator iter_B3 = box_three_elements.begin();

        inv_In_mean_B1 = vnl_matrix_inverse<double>(In_mean_B1);
        inv_In_mean_B2 = vnl_matrix_inverse<double>(In_mean_B2);
        inv_In_mean_B3 = vnl_matrix_inverse<double>(In_mean_B3);

        for (i = 0;i<3;i++)
            for (j = 0;j<3;j++)
                S_tot_B1.put(i,j,0.0);

        S_tot_B2 = S_tot_B1;
        S_tot_B3 = S_tot_B1;

        for (i=0;i<num_elements;i++)
            {
            S_1 = inv_In_mean_B1*(box_one_elements[i]);
            S_2 = inv_In_mean_B2*(box_two_elements[i]);
            S_3 = inv_In_mean_B3*(box_three_elements[i]);

            S_sub_1 = S_1;
            S_sub_2 = S_2;
            S_sub_3 = S_3;

            tx = M1_x2*(S_sub_2.get(0,0) - S_sub_1.get(0,0));
            S_sub_1.put(0,2,tx);


            tx = M1_x4*(S_sub_2.get(0,0) - S_sub_3.get(0,0));
            S_sub_3.put(0,2,tx);


            S_tot_B1 = S_tot_B1 + get_Lie_algebra(S_sub_1);
            S_tot_B2 = S_tot_B2 + get_Lie_algebra(S_sub_2);
            S_tot_B3 = S_tot_B3 + get_Lie_algebra(S_sub_3);

            tx = M1_x2*(S_tot_B2.get(0,0) - S_tot_B1.get(0,0));
            S_tot_B1.put(0,2,tx);


            tx = M1_x4*(S_tot_B2.get(0,0) - S_tot_B3.get(0,0));
            S_tot_B3.put(0,2,tx);


            }

        S_tot_B1 = S_tot_B1*(1/num_elements);
        S_tot_B2 = S_tot_B2*(1/num_elements);
        S_tot_B3 = S_tot_B3*(1/num_elements);



        frob_norm = S_tot_B1.frobenius_norm() + S_tot_B2.frobenius_norm() + S_tot_B3.frobenius_norm();

        In_mean_B1 = In_mean_B1*get_Lie_group(S_tot_B1);
        In_mean_B2 = In_mean_B2*get_Lie_group(S_tot_B2);
        In_mean_B3 = In_mean_B3*get_Lie_group(S_tot_B3);

        tx = M1_x2*(In_mean_B2.get(0,0) - In_mean_B1.get(0,0));

        In_mean_B1.put(0,2,tx);

        tx = M1_x4*(In_mean_B2.get(0,0) - In_mean_B3.get(0,0));

        In_mean_B3.put(0,2,tx);

        num_iterations++;

        vcl_cout << "iteration: " << num_iterations << vcl_endl;

        }
    if (num_iterations == 1e5)
        vcl_cout << "intrinsic mean not converged ! " << vcl_endl;

    }


vnl_matrix<double> calculate_intrinsic_mean(vcl_vector<vnl_matrix<double> > group_elements)
    {
    //create a 3x3 intrinsic mean element and initialize it to identity element
    vnl_matrix<double> in_mean(3,3,0.0),inv_in_mean(3,3,0.0),s(3,3,0.0),s_tot(3,3,0.0);

    double tol = 1e-2,frob_norm = 1,num_elements = group_elements.size();
    int i,j,num_iterations = 0;


    in_mean = *(group_elements.begin());


    vcl_cout << in_mean << vcl_endl;

    while ((frob_norm > tol)& (num_iterations <= 1e5))
        {
        inv_in_mean = vnl_matrix_inverse<double>(in_mean);

        for (i = 0;i<3;i++)
            for (j = 0;j<3;j++)
                s_tot.put(i,j,0.0);

        vcl_vector<vnl_matrix<double> >::iterator iter = group_elements.begin();


        for (iter;iter != group_elements.end();iter++)
            {
            s = inv_in_mean*(*iter);

            vcl_cout << " lie algebra of s " << vcl_endl;
            vcl_cout << get_Lie_algebra(s) << vcl_endl;

            s_tot = s_tot + get_Lie_algebra(s);
            }

        vcl_cout << " s tot " << vcl_endl;
        vcl_cout << s_tot << vcl_endl;

        s_tot = s_tot*(1/num_elements);

        vcl_cout << " s tot after averaging: " << vcl_endl;
        vcl_cout << s_tot << vcl_endl;

        frob_norm = s_tot.frobenius_norm();

        in_mean = in_mean*get_Lie_group(s_tot);

        vcl_cout << in_mean << vcl_endl;

        num_iterations++;
        vcl_cout << "iteration: " << num_iterations << vcl_endl;


        }
    if (num_iterations == 1e5)
        vcl_cout << "intrinsic mean not converged ! " << vcl_endl;
    return in_mean;
    }

vnl_matrix<double> calculate_intrinsic_mean_3d(vcl_vector<vnl_matrix<double> > group_elements)
    {
    //create a 3x3 intrinsic mean element and initialize it to identity element
    vnl_matrix<double> in_mean(4,4,0.0),inv_in_mean(4,4,0.0),s(4,4,0.0),s_tot(4,4,0.0);

    double tol = 1e-2,frob_norm = 1,num_elements = group_elements.size();
    int i,j,num_iterations = 0;


    in_mean = *(group_elements.begin());


    vcl_cout << in_mean << vcl_endl;

    while ((frob_norm > tol)& (num_iterations <= 1e5))
        {
        inv_in_mean = vnl_matrix_inverse<double>(in_mean);

        for (i = 0;i<4;i++)
            for (j = 0;j<4;j++)
                s_tot.put(i,j,0.0);

        vcl_vector<vnl_matrix<double> >::iterator iter = group_elements.begin();


        for (iter;iter != group_elements.end();iter++)
            {
            s = inv_in_mean*(*iter);

            vcl_cout << " lie algebra of s " << vcl_endl;
            vcl_cout << get_Lie_algebra_3d(s) << vcl_endl;

            s_tot = s_tot + get_Lie_algebra_3d(s);
            }

        vcl_cout << " s tot " << vcl_endl;
        vcl_cout << s_tot << vcl_endl;

        s_tot = s_tot*(1/num_elements);

        vcl_cout << " s tot after averaging: " << vcl_endl;
        vcl_cout << s_tot << vcl_endl;

        frob_norm = s_tot.frobenius_norm();

        in_mean = in_mean*get_Lie_group_3d(s_tot);

        vcl_cout << in_mean << vcl_endl;

        num_iterations++;
        vcl_cout << "iteration: " << num_iterations << vcl_endl;


        }
    if (num_iterations == 1e5)
        vcl_cout << "intrinsic mean not converged ! " << vcl_endl;
    return in_mean;
    }


//bring all the group elements close to identity by multiplying with inverse of intrinsic mean
//and then find out the variance
double get_variance(vcl_vector<vnl_matrix<double> > group_elements,vnl_matrix<double> intrinsic_mean)
    {
    double var = 0;
    vnl_matrix<double>S,lie_alg;
    vnl_matrix<double>inv_In_mean = vnl_matrix_inverse<double>(intrinsic_mean);

    for (vcl_vector<vnl_matrix<double> >::iterator iter = group_elements.begin();iter != group_elements.end();iter++)
        {
        S = inv_In_mean*(*iter);
        lie_alg = get_Lie_algebra(S);
        var += lie_alg.frobenius_norm();
        }
    var = var/group_elements.size();
    return var;
    }

projection_vnl_least_squares_function::projection_vnl_least_squares_function(vnl_matrix<double> G1,vnl_matrix<double>sub_manifold_gen)
:vnl_least_squares_function(1,9,no_gradient),group_element_(G1),sub_manifold_generator_(sub_manifold_gen)
    {

    }

void projection_vnl_least_squares_function::f(vnl_vector<double> const& params, vnl_vector<double>& residuals)
    {
    int i = 0,count = 0,j;
    vnl_matrix<double> inv_group_element,point_on_sub_manifold,S,lie_alg;
    double scalar_val = params[i];

    inv_group_element  = vnl_matrix_inverse<double>(group_element_);
    point_on_sub_manifold = get_Lie_group(scalar_val*sub_manifold_generator_);

    S = inv_group_element*point_on_sub_manifold;
    lie_alg = get_Lie_algebra(S);

    for (i = 0;i<lie_alg.rows();i++)
        for (j = 0;j<lie_alg.columns();j++)
            residuals[count++] = lie_alg.get(i,j);
    }
// number of unknowns is group_elements.size()+9

geodesic_vnl_least_squares_function::geodesic_vnl_least_squares_function(vcl_vector<vnl_matrix<double> > group_elements,
                                                                         vnl_vector<double> params_freeze,vnl_vector<double> fixed_param_values,
                                                                         int num_params)                                                                         
                                                                         :vnl_least_squares_function(num_params,9*group_elements.size(),no_gradient),
                                                                         group_elements_(group_elements),params_freeze_(params_freeze),fixed_param_values_(fixed_param_values),
                                                                         num_params_(num_params)
    {

    }

void geodesic_vnl_least_squares_function::f(vnl_vector<double> const& params, vnl_vector<double>& residuals)
    {
    int i,j,count,k,param_num ;

    vnl_matrix<double> inv_group_element,lie_alg,S,point_on_sub_manifold,sub_manifold_generator(3,3,0.0);
    double scalar_val;

    k = group_elements_.size();

    param_num = 0;

    for (i = 0;i<3;i++)
        for (j = 0;j<3;j++)
            {
            if (params_freeze_[k] == 0)
                sub_manifold_generator.put(i,j,params[param_num++]);
            else
                sub_manifold_generator.put(i,j,fixed_param_values_[k]);

            k += 1;
            }

        vcl_cout << "sub manifold generator: " << vcl_endl;
        vcl_cout << sub_manifold_generator << vcl_endl;

        k = 0;
        count = 0;
        param_num = 0;

        for (vcl_vector<vnl_matrix<double> >::iterator it = group_elements_.begin();it != group_elements_.end();it++)
            {
            if (params_freeze_[k] == 0)
                scalar_val = params[param_num++];
            else
                scalar_val = fixed_param_values_[k];

            inv_group_element  = vnl_matrix_inverse<double>(*it);
            point_on_sub_manifold = get_Lie_group(scalar_val*sub_manifold_generator);

            S = inv_group_element*point_on_sub_manifold;
            lie_alg = get_Lie_algebra(S);

            for (i = 0;i<lie_alg.rows();i++)
                for (j = 0;j<lie_alg.columns();j++)
                    residuals[count++] = lie_alg.get(i,j);


            k++;
            }

    }

geodesic_vnl_least_squares_function_9x9::geodesic_vnl_least_squares_function_9x9(vcl_vector<vnl_matrix<double> > group_elements,
                                                                                 vnl_vector<double> params_freeze,vnl_vector<double> fixed_param_values,
                                                                                 int num_params)                                                                         
                                                                                 :vnl_least_squares_function(num_params,81*group_elements.size(),no_gradient),
                                                                                 group_elements_(group_elements),params_freeze_(params_freeze),fixed_param_values_(fixed_param_values),
                                                                                 num_params_(num_params)
    {

    }

void geodesic_vnl_least_squares_function_9x9::f(vnl_vector<double> const& params, vnl_vector<double>& residuals)
    {
    int i,j,count,k,param_num ;

    vnl_matrix<double> inv_group_element,lie_alg,S,point_on_sub_manifold,inv_point_on_sub_manifold,sub_manifold_generator(9,9,0.0);
    double scalar_val;

    k = group_elements_.size();

    param_num = 0;

    for (i = 0;i<9;i++)
        {
        for (j = 0;j<9;j++)
            {
            if (params_freeze_[k] == 0)
                sub_manifold_generator.put(i,j,params[param_num++]);
            else
                sub_manifold_generator.put(i,j,fixed_param_values_[k]);

            k++;
            }
        }

        vcl_cout << "sub manifold generator: " << vcl_endl;
        vcl_cout << sub_manifold_generator << vcl_endl;

        k = 0;
        count = 0;
        param_num = 0;

        vcl_cout <<"printing out the frobenius norms" << vcl_endl;

        for (vcl_vector<vnl_matrix<double> >::iterator it = group_elements_.begin();it != group_elements_.end();it++)
            {
            if (params_freeze_[k] == 0)
                scalar_val = params[param_num++];
            else
                scalar_val = fixed_param_values_[k];

            inv_group_element  = vnl_matrix_inverse<double>(*it);

            point_on_sub_manifold = get_Lie_group_9x9(scalar_val*sub_manifold_generator);

             vcl_cout << "point_on_sub_manifold " << point_on_sub_manifold  << vcl_endl;

            inv_point_on_sub_manifold = vnl_matrix_inverse<double>(point_on_sub_manifold);

            // S = inv_group_element*point_on_sub_manifold;
            S = inv_point_on_sub_manifold*(*it);
            vcl_cout << S << vcl_endl;

            lie_alg = get_Lie_algebra_9x9(S);
            
            for (i = 0;i<lie_alg.rows();i++)
                for (j = 0;j<lie_alg.columns();j++)
                    residuals[count++] = lie_alg.get(i,j);

            vcl_cout << "frob norm " << lie_alg.frobenius_norm() << vcl_endl;

            k += 1;
            }

    }

//get the optimal geodesic and for debugging purposes return the scalars associated with
//projection of each of the points

vnl_matrix<double> get_geodesic(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> initial_params,
                                vnl_vector<double> params_freeze,vnl_vector<double> &params)                              
    {
    double scalar_val,n;
    int i,j,k,num_params,count; 
    vnl_matrix<double> sub_manifold_gen(3,3,0.0);
    vnl_vector<double> parameters;

    num_params = 0;

    for (i=0;i<params_freeze.size();i++)
        {
        if (params_freeze[i] == 0)
            num_params++;
        }
    parameters.set_size(num_params);

    for (i=0,count = 0;i<params_freeze.size();i++)
        {
        if (params_freeze[i] == 0)
            parameters[count++] = initial_params[i];
        }

    //during the iterations if some of the parameter values are desired to be kept constant,then these 
    //values are read from the fixed_param values vector 
    vnl_vector<double> fixed_param_values(initial_params);


    geodesic_vnl_least_squares_function geodesic_lsqr_fn(group_elements,params_freeze,fixed_param_values,num_params);
    vnl_levenberg_marquardt lm (geodesic_lsqr_fn);

    lm.set_x_tolerance(lm.get_x_tolerance());
    lm.set_trace(true);

    //initializing the starting values with parameters vector
    lm.minimize_without_gradient(parameters);

    lm.diagnose_outcome();

    n = group_elements.size();
    k = n;

    for (i=0;i<3;i++)
        for (j=0;j<3;j++)
            sub_manifold_gen.put(i,j,initial_params[k++]);

    params.set_size(n);

    for (i = 0;i<n;i++)
        params[i] = parameters[i];

    return  sub_manifold_gen;
    }

vnl_matrix<double> get_geodesic_9x9(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> initial_params,
                                    vnl_vector<double> params_freeze,vnl_vector<double> &params)                              
    {
    double scalar_val,n;
    int i,j,k,num_params,count; 
    vnl_matrix<double> sub_manifold_gen(9,9,0.0);
    vnl_vector<double> parameters;

    num_params = 0;

    for (i=0;i<params_freeze.size();i++)
        {
        if (params_freeze[i] == 0)
            num_params++;
        }
    parameters.set_size(num_params);

    for (i=0,count = 0;i<params_freeze.size();i++)
        {
        if (params_freeze[i] == 0)
            parameters[count++] = initial_params[i];
        }

    //during the iterations if some of the parameter values are desired to be kept constant,then these 
    //values are read from the fixed_param values vector 
    vnl_vector<double> fixed_param_values(initial_params);


    geodesic_vnl_least_squares_function_9x9 geodesic_lsqr_fn_9x9(group_elements,params_freeze,fixed_param_values,num_params);
    vnl_levenberg_marquardt lm (geodesic_lsqr_fn_9x9);

    lm.set_x_tolerance(lm.get_x_tolerance());
    lm.set_trace(true);

    //initializing the starting values with parameters vector
    lm.minimize_without_gradient(parameters);

    lm.diagnose_outcome();

    count = 0;

    n = group_elements.size();
    

    params.set_size(n);

    for (i = 0;i<n;i++)
        {
        if (params_freeze[i] == 0)
            params[i] = parameters[count++];
        else
            params[i] = fixed_param_values[i];
        }

    k = n;

    for (i=0;i<9;i++)
        {
        for (j=0;j<9;j++)
            {
            if (params_freeze[k] == 0)
                sub_manifold_gen.put(i,j,params[count++]);
            else
            sub_manifold_gen.put(i,j,initial_params[k]);
            k++;
            }
        }

    return  sub_manifold_gen;
    }

#if 0
vnl_matrix<double> get_geodesic(vcl_vector<vnl_matrix<double> > group_elements,vnl_vector<double> initial_params,
                                vnl_vector<double> params_freeze,vnl_vector<double> &params)                              
    {
    double scalar_val,n;
    int i,j,k,num_params;
    vnl_vector<double> parameters,params_freeze;
    vnl_matrix<double> sub_manifold_gen(3,3,0.0);

    n = group_elements.size();

    //   the number of unknowns is n+9.so,set the parameter size to n+9.
    parameters.set_size(n+9);
    params_freeze.set_size(n+9);

    //   initialize all the parameters corresponding to projections with 1.
    for (i =0;i<n;i++)
        parameters[i] = 1;

    //   assume that the generator corresponding to optimum 1-D sub manifold is of 
    //   the form:
    //             1 0 1
    //             0 1 1
    //             0 0 0

    //   initialize the parameters corresponding to the scalars associated with generators
    i = n;
    parameters[i++] = 1;
    parameters[i++] = 0;
    parameters[i++] = 1;
    parameters[i++] = 0;
    parameters[i++] = 1;
    parameters[i++] = 1;
    parameters[i++] = 0;
    parameters[i++] = 0;
    parameters[i++] = 0;

    num_params = 0;

    for (i=0;i<params_freeze.size();i++)
        {
        params_freeze[i] = 0;
        //keep a count of active parameters
        num_params++;
        }

    vnl_vector<double> fixed_param_values(parameters);


    geodesic_vnl_least_squares_function geodesic_lsqr_fn(group_elements,params_freeze,fixed_param_values,num_params);
    vnl_levenberg_marquardt lm (geodesic_lsqr_fn);

    lm.set_x_tolerance(lm.get_x_tolerance());
    lm.set_trace(true);
    lm.minimize_without_gradient(parameters);

    k = n;

    for (i=0;i<3;i++)
        for (j=0;j<3;j++)
            sub_manifold_gen.put(i,j,parameters[k++]);

    params.set_size(n);

    for (i = 0;i<n;i++)
        params[i] = parameters[i];

    return  sub_manifold_gen;
    }
#endif

vnl_matrix<double> get_projection(vnl_matrix<double> G1,vnl_matrix<double> sub_manifold_gen)
    {
    double scalar_val;
    vnl_vector<double> parameters;
    vnl_matrix<double> projection_of_G1;

    //initialize the scalar value with 1.
    parameters.put(0,1);

    projection_vnl_least_squares_function lsqr_fn(G1,sub_manifold_gen);
    vnl_levenberg_marquardt lm (lsqr_fn);

    lm.set_x_tolerance(lm.get_x_tolerance());
    lm.set_trace(true);
    lm.minimize_without_gradient(parameters);

    scalar_val = parameters[0];

    projection_of_G1 = (vnl_matrix_inverse<double>(G1))*(scalar_val*sub_manifold_gen);
    return  projection_of_G1;
    }


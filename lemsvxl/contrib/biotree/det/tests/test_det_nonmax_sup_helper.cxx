//: 
// \file     test_det_nonmax_sup_helper
// \brief    testing for det_nonmax_sup_helper class
// \author   K. Kang
// \date     2005-09-30
// 

#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <det/det_nonmax_sup_helper.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>
#include <vgl/vgl_point_3d.h>
#include <vbl/vbl_array_3d.h>

void test_intersect()
{
  const unsigned dim = 5;
  unsigned center = dim/2;
  assert(dim%2!=0); // dimension should be an odd number

  // set up the vertice

  int nv = dim + 1;
  vcl_vector<vgl_point_3d<double> > vertice(nv*nv*nv);

  for(int i=0; i<nv; i++)
    for(int j=0; j<nv; j++)
      for(int k=0; k<nv; k++)
        vertice[i*nv*nv + j*nv + k].set(-(double)(dim)/2.0+i, -double(dim)/2.0+j, -double(dim)/2.0+k);

  vcl_vector<vbl_array_3d<int> > vertice_map;
  
  // setup the vertice map
  for(int i=0; i<8; i++)
    vertice_map.push_back(vbl_array_3d<int>(dim, dim, dim));
  
  for(int i = 0; i< dim; i++)
    for(int j=0; j< dim; j++)
      for(int k=0; k< dim; k++)
      {
        // push 8 vertice index into the map
        vertice_map[0].operator()(i,j,k) = i*nv*nv + j*nv + k;
        vertice_map[1].operator()(i,j,k) = i*nv*nv + j*nv + k+ 1;
        vertice_map[2].operator()(i,j,k) = i*nv*nv + (j+1)*nv + k;
        vertice_map[3].operator()(i,j,k) = (i+1)*nv*nv + j*nv + k;
        vertice_map[4].operator()(i,j,k) = (i+1)*nv*nv + (j+1)*nv + (k+1);
        vertice_map[5].operator()(i,j,k) = (i+1)*nv*nv + (j+1)*nv + k;
        vertice_map[6].operator()(i,j,k) = (i+1)*nv*nv + j*nv + (k+1);
        vertice_map[7].operator()(i,j,k) = i*nv*nv + (j+1)*nv + k+1;
      }
  
  vgl_vector_3d<double> dir(0, 1.0, 1.0);


  vbl_array_3d<bool>  flags = det_nonmax_sup_helper::intersection_flags(dir, vertice, vertice_map);

  for(int i=0; i<dim; i++)
    for(int j=0; j<dim; j++)
      for(int k=0; k<dim; k++)
      {
        if( k + j == dim -1 )
          TEST("testing on voxels", flags[i][j][k], true);
        else
          TEST("testing on voxels", flags[i][j][k], false);
      }

  
}

void test_projection()
{

  det_cylinder_map cm(15, 15, 15);

  for(int i = 0; i<15; i++)
    for(int j = 0; j<15; j++)
      for(int k=0; k<15; k++)
      {
        cm[i][j][k].location_ = vgl_point_3d<double>(0, 0, 0);
        cm[i][j][k].dir_ = vgl_vector_3d<double>(0, 1.0, 1.0);
        cm[i][j][k].strength_ = i*15*15 + j*15 + k;
      }

  const unsigned dim = 5;
  unsigned center = dim/2;
  assert(dim%2!=0); // dimension should be an odd number

  // set up the vertice

  int nv = dim + 1;
  vcl_vector<vgl_point_3d<double> > vertice(nv*nv*nv);

  for(int i=0; i<nv; i++)
    for(int j=0; j<nv; j++)
      for(int k=0; k<nv; k++)
        vertice[i*nv*nv + j*nv + k].set(-(double)(dim)/2.0+i, -double(dim)/2.0+j, -double(dim)/2.0+k);

  vcl_vector<vbl_array_3d<int> > vertice_map;
  
  // setup the vertice map
  for(int i=0; i<8; i++)
    vertice_map.push_back(vbl_array_3d<int>(dim, dim, dim));
  
  for(int i = 0; i< dim; i++)
    for(int j=0; j< dim; j++)
      for(int k=0; k< dim; k++)
      {
        // push 8 vertice index into the map
        vertice_map[0].operator()(i,j,k) = i*nv*nv + j*nv + k;
        vertice_map[1].operator()(i,j,k) = i*nv*nv + j*nv + k+ 1;
        vertice_map[2].operator()(i,j,k) = i*nv*nv + (j+1)*nv + k;
        vertice_map[3].operator()(i,j,k) = (i+1)*nv*nv + j*nv + k;
        vertice_map[4].operator()(i,j,k) = (i+1)*nv*nv + (j+1)*nv + (k+1);
        vertice_map[5].operator()(i,j,k) = (i+1)*nv*nv + (j+1)*nv + k;
        vertice_map[6].operator()(i,j,k) = (i+1)*nv*nv + j*nv + (k+1);
        vertice_map[7].operator()(i,j,k) = i*nv*nv + (j+1)*nv + k+1;
      }
  
  vcl_cout <<"---------------------------------------------------------\n";
  vcl_cout <<"Starting Testing projection from plane (0, 1, 1, 0) to x-z plane\n";
  vcl_cout <<"---------------------------------------------------------\n";

  vgl_vector_3d<double> dir(0, 1., 1.);
  
  vbl_array_3d<bool> flags = det_nonmax_sup_helper::intersection_flags(dir, vertice, 
      vertice_map);
  
  vbl_array_2d<double> proj = det_nonmax_sup_helper::proj_axis_plane(cm, 7, 7, 7, dir, flags);

  for(int i =0; i<dim; i++)
    for(int j=0; j<dim; j++)
      TEST("testing projection", proj[i][j], (i+7-2)*225+(15-1-(j+7-2))*15+(j+7-2));

  vcl_cout <<"---------------------------------------------------------\n";
  vcl_cout <<"Starting Testing projection from plane ( 1, 0, 1, 0) to y-z plane\n";
  vcl_cout <<"---------------------------------------------------------\n";

  dir.set(1.0, 0, 1.);
  
  flags = det_nonmax_sup_helper::intersection_flags(dir, vertice, 
      vertice_map);
 
  proj = det_nonmax_sup_helper::proj_axis_plane(cm, 7, 7, 7, dir, flags);

  for(int i =0; i<dim; i++)
    for(int j=0; j<dim; j++)
      TEST("testing projection", proj[i][j], (15-1-(j+7-2))*225+(i+7-2)*15+(j+7-2));

  vcl_cout <<"---------------------------------------------------------\n";
  vcl_cout <<"Starting Testing projection from plane ( 1, 1, 0, 0) to y-z plane\n";
  vcl_cout <<"---------------------------------------------------------\n";

  dir.set(1.0, 1., 0.);
  
  flags = det_nonmax_sup_helper::intersection_flags(dir, vertice, 
      vertice_map);
 
  proj = det_nonmax_sup_helper::proj_axis_plane(cm, 7, 7, 7, dir, flags);

  for(int i =0; i<dim; i++)
    for(int j=0; j<dim; j++)
      TEST("testing projection", proj[i][j], (15-1-(i+7-2))*225+(i+7-2)*15+(j+7-2));

}

static void test_det_nonmax_sup_helper()
{
  vcl_cout << "---test nonmaxium_suppression_helper ---" << vcl_endl;

  test_intersect();

  test_projection();
}

TESTMAIN(test_det_nonmax_sup_helper);

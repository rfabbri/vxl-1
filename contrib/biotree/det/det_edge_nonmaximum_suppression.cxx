#include "det_edge_nonmaximum_suppression.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/algo/vnl_qr.h>
#include <vbl/vbl_array_2d.h>

#include <vnl/vnl_math.h>

det_edge_nonmaximum_suppression::det_edge_nonmaximum_suppression()
{
  construct_vertex_offset_indices_lookup_table();
  construct_intersection_face_lookup_table();
}

void det_edge_nonmaximum_suppression::construct_vertex_offset_indices_lookup_table()
{
  table1_[0] = vnl_int_3(-1, -1, -1);
  table1_[1] = vnl_int_3(0, -1, -1);
  table1_[2] = vnl_int_3(1, -1, -1);
  table1_[3] = vnl_int_3(-1, 0, -1);
  table1_[4] = vnl_int_3(0, 0, -1);
  table1_[5] = vnl_int_3(1, 0, -1);
  table1_[6] = vnl_int_3(-1, 1, -1);
  table1_[7] = vnl_int_3(0, 1, -1);
  table1_[8] = vnl_int_3(1, 1, -1);
  table1_[9] = vnl_int_3(-1, -1, 0);
  table1_[10] = vnl_int_3(0, -1, 0);
  table1_[11] = vnl_int_3(1, -1, 0);
  table1_[12] = vnl_int_3(-1, 0, 0);
  table1_[13] = vnl_int_3(1, 0, 0);
  table1_[14] = vnl_int_3(-1, 1, 0);
  table1_[15] = vnl_int_3(0, 1, 0);
  table1_[16] = vnl_int_3(1, 1, 0);
  table1_[17] = vnl_int_3(-1, -1, 1);
  table1_[18] = vnl_int_3(0, -1, 1);
  table1_[19] = vnl_int_3(1, -1, 1);
  table1_[20] = vnl_int_3(-1, 0, 1);
  table1_[21] = vnl_int_3(0, 0, 1);
  table1_[22] = vnl_int_3(1, 0, 1);
  table1_[23] = vnl_int_3(-1, 1, 1);
  table1_[24] = vnl_int_3(0, 1, 1);
  table1_[25] = vnl_int_3(1, 1, 1);
}

void det_edge_nonmaximum_suppression::construct_intersection_face_lookup_table()
{
  // setting face indices
  for(int i=0; i<=23; i++)
    table2_[i].face_index = i;
  // setting voxel indices associated with the intersected face
  table2_[0].voxels[0] = 21; table2_[0].voxels[1] = 22; table2_[0].voxels[2] = 24; table2_[0].voxels[3] = 25;
  table2_[1].voxels[0] = 20; table2_[1].voxels[1] = 21; table2_[1].voxels[2] = 23; table2_[1].voxels[3] = 24;
  table2_[2].voxels[0] = 17; table2_[2].voxels[1] = 18; table2_[2].voxels[2] = 20; table2_[2].voxels[3] = 21;
  table2_[3].voxels[0] = 18; table2_[3].voxels[1] = 19; table2_[3].voxels[2] = 21; table2_[3].voxels[3] = 22;
  table2_[4].voxels[0] = 13; table2_[4].voxels[1] = 16; table2_[4].voxels[2] = 22; table2_[4].voxels[3] = 25;
  table2_[5].voxels[0] = 15; table2_[5].voxels[1] = 16; table2_[5].voxels[2] = 24; table2_[5].voxels[3] = 25;
  table2_[6].voxels[0] = 14; table2_[6].voxels[1] = 15; table2_[6].voxels[2] = 23; table2_[6].voxels[3] = 24;
  table2_[7].voxels[0] = 12; table2_[7].voxels[1] = 14; table2_[7].voxels[2] = 20; table2_[7].voxels[3] = 23;
  table2_[8].voxels[0] = 9; table2_[8].voxels[1] = 12; table2_[8].voxels[2] = 17; table2_[8].voxels[3] = 20;
  table2_[9].voxels[0] = 9; table2_[9].voxels[1] = 10; table2_[9].voxels[2] = 17; table2_[9].voxels[3] = 18;
  table2_[10].voxels[0] = 10; table2_[10].voxels[1] = 11; table2_[10].voxels[2] = 18; table2_[10].voxels[3] = 19;
  table2_[11].voxels[0] = 11; table2_[11].voxels[1] = 13; table2_[11].voxels[2] = 19; table2_[11].voxels[3] = 22;
  table2_[12].voxels[0] = 5; table2_[12].voxels[1] = 8; table2_[12].voxels[2] = 13; table2_[12].voxels[3] = 16;
  table2_[13].voxels[0] = 7; table2_[13].voxels[1] = 8; table2_[13].voxels[2] = 15; table2_[13].voxels[3] = 16;
  table2_[14].voxels[0] = 6; table2_[14].voxels[1] = 7; table2_[14].voxels[2] = 14; table2_[14].voxels[3] = 15;
  table2_[15].voxels[0] = 3; table2_[15].voxels[1] = 6; table2_[15].voxels[2] = 12; table2_[15].voxels[3] = 14;
  table2_[16].voxels[0] = 0; table2_[16].voxels[1] = 3; table2_[16].voxels[2] = 9; table2_[16].voxels[3] = 12;
  table2_[17].voxels[0] = 0; table2_[17].voxels[1] = 1; table2_[17].voxels[2] = 9; table2_[17].voxels[3] = 10;
  table2_[18].voxels[0] = 1; table2_[18].voxels[1] = 2; table2_[18].voxels[2] = 10; table2_[18].voxels[3] = 11;
  table2_[19].voxels[0] = 2; table2_[19].voxels[1] = 5; table2_[19].voxels[2] = 11; table2_[19].voxels[3] = 13;
  table2_[20].voxels[0] = 4; table2_[20].voxels[1] = 5; table2_[20].voxels[2] = 7; table2_[20].voxels[3] = 8;
  table2_[21].voxels[0] = 3; table2_[21].voxels[1] = 4; table2_[21].voxels[2] = 6; table2_[21].voxels[3] = 7;
  table2_[22].voxels[0] = 0; table2_[22].voxels[1] = 1; table2_[22].voxels[2] = 3; table2_[22].voxels[3] = 4;
  table2_[23].voxels[0] = 1; table2_[23].voxels[1] = 2; table2_[23].voxels[2] = 4; table2_[23].voxels[3] = 5;
  // setting the coordinate plane
  strcpy(table2_[0].coord_plane, "+z"); 
  strcpy(table2_[1].coord_plane, "+z"); 
  strcpy(table2_[2].coord_plane, "+z"); 
  strcpy(table2_[3].coord_plane, "+z");
  strcpy(table2_[4].coord_plane, "+x"); 
  strcpy(table2_[5].coord_plane, "+y"); 
  strcpy(table2_[6].coord_plane, "+y"); 
  strcpy(table2_[7].coord_plane, "-x");
  strcpy(table2_[8].coord_plane, "-x"); 
  strcpy(table2_[9].coord_plane, "-y"); 
  strcpy(table2_[10].coord_plane, "-y"); 
  strcpy(table2_[11].coord_plane, "+x");
  strcpy(table2_[12].coord_plane, "+x"); 
  strcpy(table2_[13].coord_plane, "+y"); 
  strcpy(table2_[14].coord_plane, "+y"); 
  strcpy(table2_[15].coord_plane, "-x");
  strcpy(table2_[16].coord_plane, "-x"); 
  strcpy(table2_[17].coord_plane, "-y"); 
  strcpy(table2_[18].coord_plane, "-y"); 
  strcpy(table2_[19].coord_plane, "+x");
  strcpy(table2_[20].coord_plane, "-z"); 
  strcpy(table2_[21].coord_plane, "-z"); 
  strcpy(table2_[22].coord_plane, "-z"); 
  strcpy(table2_[23].coord_plane, "-z");
}

det_edge_map det_edge_nonmaximum_suppression::apply(det_edge_map const& input)
{
  int n = 3; //suppression box size is 3
  int margin = (n-1)/2;

  int nx = input.nx();
  int ny = input.ny();
  int nz = input.nz();

  // store the resulting edge map
  det_edge_map output(nx, ny, nz);
  for(int i=margin; i<nx-margin; i++)
  {
    for(int j=margin; j<ny-margin; j++)
    {
      for(int k=margin; k<nz-margin; k++)
      {
        output(i,j,k).strength_ = 0.0;
        output(i,j,k).location_ = vgl_point_3d<double>(0.0, 0.0, 0.0);
        output(i,j,k).dir_ = vgl_vector_3d<double>(0.0, 0.0, 0.0);
        // operations in positive ray direction
        vgl_vector_3d<double> direction(input(i,j,k).dir_);
        normalize(direction);
        if(direction != vgl_vector_3d<double> (0.0, 0.0, 0.0))
        {
          vnl_double_2 angles1 = find_angles(direction);
          int face1 = find_intersected_face_index(angles1);
          assert(face1 != -1);
          double s_plus = find_s_parameter(face1, direction);
          vgl_point_3d<double> intersection_point_plus(direction.x()*s_plus, direction.y()*s_plus, direction.z()*s_plus);
          double f_plus = interpolate_value_on_face_intersection_point(face1, intersection_point_plus, input, i, j, k);
          // operations in negative ray direction
//          vnl_double_2 angles2 = find_angles(-direction);
//          int face2 = find_intersected_face_index(angles2);
          int face2 = find_opposite_intersected_face_index(face1);
          assert(face2 != -1);
          double s_minus = find_s_parameter(face2, direction);
          vgl_point_3d<double> intersection_point_minus(direction.x()*s_minus, direction.y()*s_minus, direction.z()*s_minus);
          double f_minus = interpolate_value_on_face_intersection_point(face2, intersection_point_minus, input, i, j, k);

          double s[3] = {s_minus, 0, s_plus};
          double f[3] = {f_minus, input(i,j,k).strength_, f_plus};

          if(f[1] > f[0] && f[1] > f[2])
          {
            double subpixel_s;
            double subpixel_strength;
            find_subvoxel_maximum_s_parameter(s,f,subpixel_s,subpixel_strength);
            vgl_point_3d<double> subpixel_maximum(direction.x() * subpixel_s, direction.y() * subpixel_s, direction.z() * subpixel_s);

            double x = subpixel_maximum.x(); double y = subpixel_maximum.y(); double z = subpixel_maximum.z();

            // if the subpixel maximum is inside the cube sorrounding the voxel, then there is a maximum
            if(x < 0.5 && x > -0.5 && y < 0.5 && y > -0.5 && z < 0.5 && z > -0.5)
            {
              output(i,j,k).strength_ = subpixel_strength;
              output(i,j,k).dir_ = input(i,j,k).dir_;
              output(i,j,k).location_ = vgl_point_3d<double>(x, y, z); // CHANGE THIS TO SUBPIXEL LOCATION (RELATIVE OR ABSOLUTE?)
            }
          }
        }
      }
    }
  }

  return output;
}

vnl_double_2 det_edge_nonmaximum_suppression::find_angles(vgl_vector_3d<double> direction)
{
  vnl_double_2 angles;

  double n1 = direction.x();
  double n2 = direction.y();
  double n3 = direction.z();
  // theta is in the range of [0, pi]
  double theta = vcl_acos(n3);
  double phi;
  if(n1 == 0)
    phi = vnl_math::pi_over_2;
  else
    phi = vcl_atan(n2/n1);
  
  if(n1>0 && n2>0) //1st quadrant
    1;
  else if(n1<0 && n2>0) //2nd quadrant
    phi = phi + vnl_math::pi;
  else if(n1<0 && n2<0) //3rd quadrant
    phi = phi + vnl_math::pi;
  else if(n1>0 && n2<0) //4th quadrant
    phi = phi + 2*vnl_math::pi;
  else if(n1<0 && n2==0)
    phi = phi + vnl_math::pi;
  else if(n1==0 && n2<0)
    phi = phi + vnl_math::pi;

  angles[0] = theta;
  angles[1] = phi;

  return angles;
}

int det_edge_nonmaximum_suppression::find_intersected_face_index(vnl_double_2 &angles)
{
  double theta = angles[0];
  double phi = angles[1];
  double a45 = vnl_math::pi_over_4;

  if(phi >= 0 && phi <= a45)
  {
    double phi_in = phi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 0;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 4;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 12;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 20;
  }
  else if(phi >= a45 && phi <= 2*a45)
  {
    double phi_in = vnl_math::pi_over_2 - phi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 0;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 5;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 13;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 20;
  }
  else if(phi >= 2*a45 && phi <= 3*a45)
  {
    double phi_in = phi - vnl_math::pi_over_2;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 1;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 6;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 14;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 21;
  }
  else if(phi >= 3*a45 && phi <= 4*a45)
  {
    double phi_in = vnl_math::pi - phi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 1;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 7;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 15;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 21;
  }
  else if(phi >= 4*a45 && phi <= 5*a45)
  {
    double phi_in = phi - vnl_math::pi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 2;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 8;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 16;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 22;
  }
  else if(phi >= 5*a45 && phi <= 6*a45)
  {
    double phi_in = 3*vnl_math::pi_over_2 - phi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 2;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 9;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 17;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 22;
  }
  else if(phi >= 6*a45 && phi <= 7*a45)
  {
    double phi_in = phi - 3*vnl_math::pi_over_2;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 3;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 10;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 18;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 23;
  }
  else if(phi >= 7*a45 && phi <= 8*a45)
  {
    double phi_in = 2*vnl_math::pi - phi;
    double theta_0 = atan(1/(cos(phi_in)));
    assert(theta_0 >= 0 && phi_in >= 0);

    if(theta >= 0 && theta <= theta_0)
      return 3;
    else if(theta >= theta_0 && theta <= 2*a45)
      return 11;
    else if(theta >= 2*a45 && theta <= vnl_math::pi - theta_0)
      return 19;
    else if(theta >= vnl_math::pi - theta_0 && theta <= 4*a45)
      return 23;
  }
  return -1;
}

int det_edge_nonmaximum_suppression::find_opposite_intersected_face_index(int face_index)
{
  switch ( face_index )
  {
  case 0:
    return 22;
    break;
  case 1:
    return 23;
    break;
  case 2:
    return 20;
    break;
  case 3:
    return 21;
    break;
  case 4:
    return 16;
    break;
  case 5:
    return 17;
    break;
  case 6:
    return 18;
    break;
  case 7:
    return 19;
    break;
  case 8:
    return 12;
    break;
  case 9:
    return 13;
    break;
  case 10:
    return 14;
    break;
  case 11:
    return 15;
    break;
  case 12:
    return 8;
    break;
  case 13:
    return 9;
    break;
  case 14:
    return 10;
    break;
  case 15:
    return 11;
    break;
  case 16:
    return 4;
    break;
  case 17:
    return 5;
    break;
  case 18:
    return 6;
    break;
  case 19:
    return 7;
    break;
  case 20:
    return 2;
    break;
  case 21:
    return 3;
    break;
  case 22:
    return 0;
    break;
  case 23:
    return 1;
    break;

  default:
    return -1;
  }
}

double det_edge_nonmaximum_suppression::find_s_parameter(int face_index, vgl_vector_3d<double> direction)
{
  double n1 = direction.x();
  double n2 = direction.y();
  double n3 = direction.z();
  char coord_plane[3];
  strcpy(coord_plane, table2_[face_index].coord_plane);
  if(strcmp(coord_plane, "+x") == 0)
    return 1/n1;
  else if(strcmp(coord_plane, "-x") == 0)
    return -1/n1;
  else if(strcmp(coord_plane, "+y") == 0)
    return 1/n2;
  else if(strcmp(coord_plane, "-y") == 0)
    return -1/n2;
  else if(strcmp(coord_plane, "+z") == 0)
    return 1/n3;
  else if(strcmp(coord_plane, "-z") == 0)
    return -1/n3;
  else
  {
    vcl_cout << "Something is wrong with find_s_parameter function\n";
    exit(-1);
  }
}

double det_edge_nonmaximum_suppression::interpolate_value_on_face_intersection_point(int face_index, vgl_point_3d<double> point, 
                                                                                     det_edge_map const& input, int i, int j, int k)
{
  char coord_plane[3];
  strcpy(coord_plane, table2_[face_index].coord_plane);

  vnl_int_4 voxel_indices(table2_[face_index].voxels);
  
  vnl_int_3 vox1_indices(table1_[voxel_indices[0]]);
  vnl_int_3 vox2_indices(table1_[voxel_indices[1]]);
  vnl_int_3 vox3_indices(table1_[voxel_indices[2]]);
  vnl_int_3 vox4_indices(table1_[voxel_indices[3]]);

  // values should be taken from the input
  double val1 = input(i+vox1_indices[0], j+vox1_indices[1], k+vox1_indices[2]).strength_;
  double val2 = input(i+vox2_indices[0], j+vox2_indices[1], k+vox2_indices[2]).strength_;
  double val3 = input(i+vox3_indices[0], j+vox3_indices[1], k+vox3_indices[2]).strength_;
  double val4 = input(i+vox4_indices[0], j+vox4_indices[1], k+vox4_indices[2]).strength_;
  
  if(strcmp(coord_plane, "+x") == 0 || strcmp(coord_plane, "-x") == 0)
  {
    double tempval1 = vcl_fabs(point.y() - vox1_indices[1]) * val2 + vcl_fabs(point.y() - vox2_indices[1]) * val1;
    double tempval2 = vcl_fabs(point.y() - vox3_indices[1]) * val4 + vcl_fabs(point.y() - vox4_indices[1]) * val3;
    return vcl_fabs(point.z() - vox1_indices[2]) * tempval2 + vcl_fabs(point.z() - vox3_indices[2]) * tempval1;
  }
  else if(strcmp(coord_plane, "+y") == 0 || strcmp(coord_plane, "-y") == 0)
  {
    double tempval1 = vcl_fabs(point.x() - vox1_indices[0]) * val2 + vcl_fabs(point.x() - vox2_indices[0]) * val1;
    double tempval2 = vcl_fabs(point.x() - vox3_indices[0]) * val4 + vcl_fabs(point.x() - vox4_indices[0]) * val3;
    return vcl_fabs(point.z() - vox1_indices[2]) * tempval2 + vcl_fabs(point.z() - vox3_indices[2]) * tempval1;
  }
  else if(strcmp(coord_plane, "+z") == 0 || strcmp(coord_plane, "-z") == 0)
  {
    double tempval1 = vcl_fabs(point.x() - vox1_indices[0]) * val2 + vcl_fabs(point.x() - vox2_indices[0]) * val1;
    double tempval2 = vcl_fabs(point.x() - vox3_indices[0]) * val4 + vcl_fabs(point.x() - vox4_indices[0]) * val3;
    return vcl_fabs(point.y() - vox1_indices[1]) * tempval2 + vcl_fabs(point.y() - vox3_indices[1]) * tempval1;
  }
  return -1;
}

void det_edge_nonmaximum_suppression::find_subvoxel_maximum_s_parameter(double s[3], double f[3],
                                                                        double &subpixel_s,
                                                                        double &subpixel_strength)
{
  double A = f[2] / ((s[2]-s[0])*(s[2]-s[1]));
  double B = f[1] / ((s[1]-s[0])*(s[1]-s[2]));
  double C = f[0] / ((s[0]-s[1])*(s[0]-s[2]));

  subpixel_s = ((A+B)*s[0] + (A+C)*s[1] + (B+C)*s[2]) / (2*(A+B+C));
  subpixel_strength = A * (subpixel_s-s[0]) * (subpixel_s-s[1]) + 
                      B * (subpixel_s-s[0]) * (subpixel_s-s[2]) + 
                      C * (subpixel_s-s[1]) * (subpixel_s-s[2]);
}

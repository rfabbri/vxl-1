// This is contrib/mleotta/modrec/modrec_vehicle_mesh.h
#ifndef modrec_vehicle_mesh_h_
#define modrec_vehicle_mesh_h_

//:
// \file
// \brief Functions for generating and manipulated the vehicle mesh
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 5/8/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <imesh/imesh_mesh.h>
#include <map>
#include <string>


//: Read the vehicle parameters from a file
void modrec_read_vehicle_params(const std::string filename,
                                std::map<std::string,double>& params);

//: Return the mean vehicle params
std::map<std::string,double> modrec_read_vehicle_params();


//: Generate the complete vehicle mesh
void modrec_generate_vehicle(const std::map<std::string,double>& params,
                             imesh_mesh& mesh);


//: Generate the mesh faces for the vehicle body
std::auto_ptr<imesh_face_array_base>
modrec_generate_vehicle_body_faces();


//: Generate the mesh vertices for the vehicle body
std::auto_ptr<imesh_vertex_array<3> >
modrec_generate_vehicle_body_verts(const std::map<std::string,double>& params);


//: Generate the texture coords for the vehicle body
std::vector<vgl_point_2d<double> >
modrec_generate_vehicle_body_tex();


//: Generate the mesh faces for the vehicle wheel (with \param rs radial samples)
std::auto_ptr<imesh_face_array_base>
modrec_generate_vehicle_wheel_faces(unsigned int rs = 16);


//: Generate the mesh vertices for the vehicle wheel
//  \params r1 is the wheel radius
//  \params r2 is the tire outer radius
//  \params r3 is the tire width
std::auto_ptr<imesh_vertex_array<3> >
modrec_generate_vehicle_wheel_verts(double r1, double r2, double w, unsigned int rs = 16);


//: Generate the texture coords for the vehicle wheel
// if index == 0, center the wheel at the origin
// else position the wheel in each of the four corners of the unit square
std::vector<vgl_point_2d<double> >
modrec_generate_vehicle_wheel_tex(unsigned int index = 0, unsigned int rs = 16);

// =============================================================================
// Dodecahedral mesh

//: Generate the dodecahedral vehicle mesh
void modrec_generate_dodec_vehicle(const std::map<std::string,double>& params,
                                   imesh_mesh& mesh);

//: Generate the mesh vertices for the dodecahedral body
std::auto_ptr<imesh_vertex_array<3> >
modrec_generate_dodec_vehicle_verts(const std::map<std::string,double>& params);

//: Generate the mesh faces for the dodecahedral body
std::auto_ptr<imesh_face_array_base>
modrec_generate_dodec_vehicle_faces();

// =============================================================================
// Ferryman mesh
// J. M. Ferryman, A. D. Worrall, G. D. Sullivan, and K. D. Baker
// "A Generic Deformable Model for Vehicle Recognition", BMVC 1995

//: Generate the ferryman vehicle mesh
void modrec_generate_ferryman_vehicle(const std::map<std::string,double>& params,
                                      imesh_mesh& mesh);

//: Generate the mesh vertices for the Ferryman body
std::auto_ptr<imesh_vertex_array<3> >
modrec_generate_ferryman_vehicle_verts(const std::map<std::string,double>& params);

//: Generate the mesh faces for the Ferryman body
std::auto_ptr<imesh_face_array_base>
modrec_generate_ferryman_vehicle_faces();


#endif // modrec_vehicle_mesh_h_

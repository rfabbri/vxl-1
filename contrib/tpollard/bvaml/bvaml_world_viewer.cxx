#ifndef _bvaml_world_viewer_cxx_
#define _bvaml_world_viewer_cxx_

#include "bvaml_world_viewer.h"
#include "bvaml_log_writer.h"

#include <vcl_cmath.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>


//--------------------------------------------------
void 
bvaml_world_viewer::write_raw_world(
  bvaml_world* world,
  const vcl_string& filename )
{
  // Write header.
  vcl_ofstream ofs( filename.c_str(), vcl_ofstream::binary );
  unsigned char t = 0; ofs.write( (char*)&t, 1 );
  unsigned int nx = params_->num_voxels().x(); ofs.write( (char*)&nx, 4 );
  unsigned int ny = params_->num_voxels().y(); ofs.write( (char*)&ny, 4 );
  unsigned int nz = params_->num_voxels().z(); ofs.write( (char*)&nz, 4 );

  // Write every voxel.
  for( int x = 0; x < params_->num_voxels().x(); x++ ){
    for( int y = 0; y < params_->num_voxels().y(); y++ ){
      for( int z = 0; z < params_->num_voxels().z(); z++ ){
        bvaml_voxel v( params_ );
        world->get_voxel( vgl_point_3d<int>(x,y,z), v, true, false, false, NULL );
        unsigned char vo = (unsigned char)floor((*v.occupancy_prob)*255);
        ofs.write( (char*)&vo, 1 );
      }
    }
  }
  ofs.close();
};


//--------------------------------------------------
void 
bvaml_world_viewer::write_x3d_world(
  bvaml_world* world,
  const vcl_string& filename,
  float threshold,
  float scale )
{
  world->occupancy_to_heights(0);
};


/* OLD X3D WRITING
//--------------------------------------------------
void 
bvaml_world_viewer::write_x3d_world(
  bvaml_world* world,
  const vcl_string& filename,
  float threshold,
  float scale )
{
  scale = (float)(1.0/scale);
  bvaml_log_writer log( "bvaml_world_viewer::write_world" );

  vcl_ofstream file_stream( filename.c_str() );
  file_stream <<
    "<X3D version='3.0' profile='Immersive'>\n" <<
    " <Scene>\n" <<
    " <Background skyColor='0 0 1' />\n";
  draw_world_axis( file_stream );

  // Write each supervoxel.
  for(int svx = 0; svx < params_->num_supervoxels.x(); svx++ ){
    for(int svy = 0; svy < params_->num_supervoxels.y(); svy++ ){
      for(int svz = 0; svz < params_->num_supervoxels.z(); svz++ ){

        vcl_stringstream svname; svname << svx << ' ' << svy << ' ' << svz;
        log.print_msg( "writing supervoxel " + svname.str() );
        
        vgl_point_3d<int> corner( params_->supervoxel_length*svx, 
          params_->supervoxel_length*svy, params_->supervoxel_length*svz );
        int svi = world->supervoxel_index( vgl_point_3d<int>( svx, svy, svz ) );
        world->supervoxels_[ svi ].load_occupancies();

        file_stream << 
          "  <Shape>\n" <<
          "   <IndexedFaceSet colorPerVertex='false' coordIndex='";

        // Write all of the indices
        int index = 0;
        for( float x = 0; x < params_->supervoxel_length; x+=scale ){
          for( float y = 0; y < params_->supervoxel_length; y+=scale ){
            for( float z = 0; z < params_->supervoxel_length; z+=scale ){
              
              bvaml_voxel this_voxel( params_ );
              world->supervoxels_[ svi ].get_voxel( 
                vgl_point_3d<int>( (int)floor(x), (int)floor(y), (int)floor(z) ), this_voxel, 
                true, false, true, NULL );

              if( (*this_voxel.occupancy_prob) > threshold ){
                file_stream << '\n' << index << ' ' << index+1 << ' ' <<
                  index+2 << ' ' << index+3 << " -1,";
                index += 4;
              }
            }
          }
        }
        file_stream << " ' >\n" << 
          "    <Color color='";

        // Write the colors.
        for( float x = 0; x < params_->supervoxel_length; x+=scale ){
          for( float y = 0; y < params_->supervoxel_length; y+=scale ){
            for( float z = 0; z < params_->supervoxel_length; z+=scale ){

              bvaml_voxel this_voxel( params_ );
              world->supervoxels_[ svi ].get_voxel( 
                vgl_point_3d<int>( (int)floor(x), (int)floor(y), (int)floor(z) ), this_voxel, 
                true, false, true, NULL );

              if( (*this_voxel.occupancy_prob) > threshold ){
                float this_color = this_voxel.appearance->expected_color(vnl_vector<float>());
                if( this_color > 1 ) this_color = 1;
                if( this_color < 0 ) this_color = 0;
                file_stream << '\n' << this_color << ' ' << this_color <<
                  ' ' << this_color << ',';
              }
            }
          }
        }
        file_stream << " ' />\n" << 
          "    <Coordinate point='";

        // Write the coordinates.
        for( float x = 0; x < params_->supervoxel_length; x+=scale ){
          for( float y = 0; y < params_->supervoxel_length; y+=scale ){
            for( float z = 0; z < params_->supervoxel_length; z+=scale ){
              
              bvaml_voxel this_voxel( params_ );
              world->supervoxels_[ svi ].get_voxel( 
                vgl_point_3d<int>( (int)floor(x), (int)floor(y), (int)floor(z) ), this_voxel, 
                true, false, true, NULL );

              if( (*this_voxel.occupancy_prob) > threshold ){
                vgl_point_3d<float> c( corner.x()+floor(x),
                  corner.y()+floor(y), corner.z()+floor(z) );
                vgl_vector_3d<float> n(0,0,1);
                vgl_point_3d<float> p1, p2, p3, p4;
                get_face_points( c, n, p1, p2, p3, p4 );
                file_stream << '\n' <<
                  p1.x() << ' ' << p1.y() << ' ' << p1.z() << ", " <<
                  p2.x() << ' ' << p2.y() << ' ' << p2.z() << ", " <<
                  p3.x() << ' ' << p3.y() << ' ' << p3.z() << ", " <<
                  p4.x() << ' ' << p4.y() << ' ' << p4.z() << ", ";
              }

            }
          }
        }
        file_stream << " ' />\n" << 
          "   </IndexedFaceSet>\n" <<
          "  </Shape>\n";
      } // vz
    } // vy
  } // vx

  file_stream <<
    " </Scene>\n" <<
    "</X3D>";
}; */

/* SLICE WRITING
//-----------------------------------------------------
void 
bvaml_world_viewer::write_world(
    bvaml_world* world,
    const vcl_string& slice_dir,
    float scale )
{
  int nx_scaled = (int)ceil(params_->num_voxels().x()*scale);
  int ny_scaled = (int)ceil(params_->num_voxels().y()*scale);
  int nz_scaled = (int)ceil(params_->num_voxels().z()*scale);
  int current_sv = -1;

  bvaml_log_writer log( "bvaml_world_viewer::write_world" );

  for( float z = 0; z < nz_scaled; z++ ){
    int vz = (int)floor(z/scale);

    // Unload all cached supervoxels if no longer needed.
    vgl_point_3d<int> this_sv_index, this_vinsv_index; 
    world->get_voxel_location( 
      vgl_point_3d<int>(0,0,vz), this_sv_index, this_vinsv_index );
    if( this_sv_index.z() != current_sv ){
      for( int sv = 0; sv < world->supervoxels_.size(); sv++ )
        world->supervoxels_[sv].empty_cache();
      current_sv = this_sv_index.z();
    }

    // Set up the image for this slice.
    vil_image_view<vxl_byte> this_slice( nx_scaled, ny_scaled, 1 );
    vcl_stringstream slice_name;
    slice_name << slice_dir << "/";
    if( z < 10 ) slice_name << '0';
    if( z < 100 ) slice_name << '0';
    slice_name << z << ".tif";
    log.print_msg( "writing slice to " + slice_name.str() );

    // Fill in the image.
    for( float x = 0; x < nx_scaled; x++ ){
      for( float y = 0; y < ny_scaled; y++ ){
        int vx = (int)floor(x/scale);
        int vy = (int)floor(y/scale);
        bvaml_voxel this_voxel;
        world->get_voxel( this_voxel, vgl_point_3d<int>( vx, vy, vz ) );
        int this_prob = (int)floor( 255*this_voxel.occupancy_prob );
        if( !(this_prob <= 255 ) ) this_prob = 255;
        if( !(this_prob >= 0 ) ) this_prob = 0;
        this_slice(x,y) = this_prob;
      }
    }

    vil_save( this_slice, slice_name.str().c_str() );
  }

};
*/

//-----------------------------------------------------
void 
bvaml_world_viewer::start_inspection_file()
{
  inspection_file_.clear();
  inspection_file_ <<
    "<X3D version='3.0' profile='Immersive'>\n" <<
    " <Scene>\n" <<
    " <Background skyColor='0 0 1' />\n";
  draw_world_axis( inspection_file_ );
};


//------------------------------------------------
void 
bvaml_world_viewer::add_inspection_ray(
  const vcl_vector< vgl_point_3d<int> >& voxel_indices,
  const bvaml_ray& ray,
  bool before_update )
{
  bvaml_log_writer log( "bvaml_world_viewer::add_inspection_ray" );
  log.print_msg( "adding ray" );
  assert( voxel_indices.size() == ray.voxels.size() );
  if( voxel_indices.size() == 0 ) return;

  // Write all of the indices
  inspection_file_ << 
    "  <Shape>\n" <<
    "   <IndexedFaceSet colorPerVertex='false' coordIndex='";
  int index = 0;
  for( unsigned i = 0; i < voxel_indices.size(); i++ ){
    if( before_update ) inspection_file_ << '\n' << 
      4*i+0 << ' ' << 4*i+1 << ' ' << 4*i+2 << ' ' << 4*i+3 << " -1,";
    if( !before_update ) inspection_file_ << '\n' << 
      4*i+3 << ' ' << 4*i+2 << ' ' << 4*i+1 << ' ' << 4*i+0 << " -1,";
  }

  // Write the colors.
  inspection_file_ << " ' >\n" << 
    "    <Color color='";
  for( unsigned i = 0; i < voxel_indices.size(); i++ ){
    vgl_vector_3d<float> n(0,0,1);
    float this_color = (float)(.5+.5*(*ray.voxels[i]->occupancy_prob));
    if( this_color > 1 ) this_color = 1;
    if( before_update ) inspection_file_ << '\n' << this_color << " 0 0,";
    else inspection_file_ << "\n0 " << this_color << " 0,";
  }

  // Write the coordinates.
  inspection_file_ << " ' />\n" << 
    "    <Coordinate point='";
  for( unsigned i = 0; i < voxel_indices.size(); i++ ){
    vgl_point_3d<float> c( (float)voxel_indices[i].x(), 
      (float)voxel_indices[i].y(), (float)voxel_indices[i].z() );
    vgl_vector_3d<float> n(0,0,1);
    //if( params_->appearance_model != 0 )
      //n.set( ray.voxels[i].appearance_model[1],
        //ray.voxels[i].appearance_model[2], ray.voxels[i].appearance_model[3] );
    vgl_point_3d<float> p1, p2, p3, p4;
    get_face_points( c, n, p1, p2, p3, p4 );
    inspection_file_ << '\n' <<
      p1.x() << ' ' << p1.y() << ' ' << p1.z() << ", " <<
      p2.x() << ' ' << p2.y() << ' ' << p2.z() << ", " <<
      p3.x() << ' ' << p3.y() << ' ' << p3.z() << ", " <<
      p4.x() << ' ' << p4.y() << ' ' << p4.z() << ", ";
  }

  inspection_file_ << " ' />\n" << 
    "   </IndexedFaceSet>\n" <<
    "  </Shape>\n";
};


//---------------------------------------------
void 
bvaml_world_viewer::save_inspection_file(
  const vcl_string& filename )
{
  bvaml_log_writer log( "bvaml_world_viewer::save_inspection_file" );
  log.print_msg( "saving" );

  inspection_file_ <<
    " </Scene>\n" <<
    "</X3D>";

  vcl_ofstream file_stream( filename.c_str() );
  file_stream << inspection_file_.str();
};


//--------------------------------------------
void 
bvaml_world_viewer::draw_world_axis(
  vcl_ostream& stream )
{
  stream << 
  "  <Transform translation='" << params_->num_voxels().x()/2.0 << " 0 0'>\n" <<
  "   <Shape>\n" <<
  "    <Box size = '" << params_->num_voxels().x() << " .5 .5' />\n" <<
  "    <Appearance>\n" <<
  "      <Material diffuseColor='1 0 0'/>\n" <<
  "    </Appearance>\n" <<
  "   </Shape>\n" <<
  "  </Transform>\n" <<

  "  <Transform translation='0 " << params_->num_voxels().y()/2.0 << " 0'>\n" <<
  "   <Shape>\n" <<
  "    <Box size = '.5 " << params_->num_voxels().y() << " .5' />\n" <<
  "    <Appearance>\n" <<
  "      <Material diffuseColor='0 1 0'/>\n" <<
  "    </Appearance>\n" <<
  "   </Shape>\n" <<
  "  </Transform>\n" <<

  "  <Transform translation='0 0 " << params_->num_voxels().z()/2.0 << "'>\n" <<
  "   <Shape>\n" <<
  "    <Box size = '.5 .5 " << params_->num_voxels().z() << "' />\n" <<
  "    <Appearance>\n" <<
  "      <Material diffuseColor='.5 .5 0'/>\n" <<
  "    </Appearance>\n" <<
  "   </Shape>\n" <<
  "  </Transform>\n";
};


//--------------------------------------------
void 
bvaml_world_viewer::get_face_points(
  const vgl_point_3d<float>& center,
  const vgl_vector_3d<float>& normal,
  vgl_point_3d<float>& p1,
  vgl_point_3d<float>& p2, 
  vgl_point_3d<float>& p3, 
  vgl_point_3d<float>& p4 )
{
  if( fabs(normal.x())+fabs(normal.y())+fabs(normal.z()) < .0001 ){
    p1 = center + vgl_vector_3d<float>(.5,.5,0);
    p2 = center + vgl_vector_3d<float>(.5,-.5,0);
    p3 = center + vgl_vector_3d<float>(-.5,-.5,0);
    p4 = center + vgl_vector_3d<float>(-.5,.5,0);
    return;
  }
  int biggest_normal_dir = 0;
  float biggest_normal_val = fabs( normal.x() );
  if( fabs( normal.y() ) > biggest_normal_val ){
    biggest_normal_val = fabs( normal.y() ); biggest_normal_dir = 1; }
  if( fabs( normal.z() ) > biggest_normal_val ){
    biggest_normal_val = fabs( normal.z() ); biggest_normal_dir = 2; }

  vgl_vector_3d<float> v1;
  if( biggest_normal_dir == 0 )
    v1 = vgl_vector_3d<float>( -(normal.y()+normal.z())/normal.x(), 1, 1 );
  if( biggest_normal_dir == 1 )
    v1 = vgl_vector_3d<float>( 1, -(normal.z()+normal.x())/normal.y(), 1 );
  if( biggest_normal_dir == 2 )
    v1 = vgl_vector_3d<float>( 1, 1, -(normal.x()+normal.y())/normal.z() );

  vgl_vector_3d<float> v2 = cross_product( normal, v1 );
  normalize( v1 ); normalize( v2 );

  p1.set( center.x()+(float).5*v1.x(), center.y()+(float).5*v1.y(), center.z()+(float).5*v1.z() );
  p2.set( center.x()+(float).5*v2.x(), center.y()+(float).5*v2.y(), center.z()+(float).5*v2.z() );
  p3.set( center.x()-(float).5*v1.x(), center.y()-(float).5*v1.y(), center.z()-(float).5*v1.z() );
  p4.set( center.x()-(float).5*v2.x(), center.y()-(float).5*v2.y(), center.z()-(float).5*v2.z() );
};


#endif // _bvaml_world_viewer_cxx_

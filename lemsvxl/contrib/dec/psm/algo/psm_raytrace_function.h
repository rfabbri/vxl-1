#ifndef psm_raytrace_function_h_
#define psm_raytrace_function_h_

#include <vcl_cassert.h>

#include <hsds/hsds_fd_tree_node_index.h>
#include <hsds/hsds_fd_tree.h>

#include <psm/psm_apm_traits.h>
#include <psm/psm_aux_traits.h>
#include <psm/psm_scene.h>
#include <psm/psm_aux_scene.h>
#include <psm/psm_cell_id.h>

#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_backproject.h>


#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>

#include "psm_raytrace_operations.h"
#include "psm_block_visibility_iterator.h"


template<class F, psm_apm_type APM, psm_aux_type AUX_T = PSM_AUX_NULL>
class psm_raytrace_function
{
public:
  //: constructor
  psm_raytrace_function(psm_scene<APM> &scene, const vpgl_camera<double> *cam, unsigned int ni, unsigned int nj, bool reverse_traversal = false, unsigned int i0 = 0, unsigned int j0 = 0)
    :scene_(scene), aux_scene_ptr_(new psm_aux_scene<AUX_T>()), cam_(cam), img_ni_(ni), img_nj_(nj), use_aux_(false), debug_lvl_(0), reverse_traversal_(reverse_traversal), img_i0_(i0), img_j0_(j0) 
  {}

  psm_raytrace_function(psm_scene<APM> &scene, psm_aux_scene_base_sptr aux_scene, const vpgl_camera<double> *cam, unsigned int ni, unsigned int nj, bool reverse_traversal = false, unsigned int i0 = 0, unsigned int j0 = 0)
    :scene_(scene), aux_scene_ptr_(aux_scene), cam_(cam), img_ni_(ni), img_nj_(nj), use_aux_(true), debug_lvl_(0), reverse_traversal_(reverse_traversal), img_i0_(i0), img_j0_(j0)
  {}

  //: run for a single floating (sub) pixel
  bool run_single(F& step_functor, float i, float j)
  {
    psm_block_visibility_iterator<APM> block_vis_it(scene_, cam_, (unsigned int)i, (unsigned int)j, 1, 1, reverse_traversal_);

    bool continue_trace = true;

    psm_aux_scene<AUX_T> *aux_scene = dynamic_cast<psm_aux_scene<AUX_T>*>(aux_scene_ptr_.ptr());
    if (!aux_scene) {
      vcl_cerr << "error: psm_parallel_raytrace_function: failed to cast aux_scene to correct type." << vcl_endl;
      return false;
    }

    while(block_vis_it.next())
    {
      vcl_vector<vgl_point_3d<int> > vis_blocks;
      block_vis_it.current_blocks(vis_blocks);
      // traverse each visible block one at a time
      vcl_vector<vgl_point_3d<int> >::const_iterator block_it = vis_blocks.begin();
      for (; block_it != vis_blocks.end(); ++block_it) {
        const vgl_point_3d<int> block_idx = *block_it;

        if (debug_lvl_ > 0) vcl_cout << "processing block at index (" << block_idx.x() << ", " << block_idx.y() << ", " << block_idx.z() << ")" << vcl_endl;
        // make sure block projects to inside of image
        vbl_bounding_box<double,3> block_bb = scene_.block_bounding_box(block_idx);
        if (!cube_visible(block_bb, cam_, (unsigned int)i, (unsigned int)j, 1, 1, false)) {
          continue;
        }

        hsds_fd_tree<psm_sample<APM>,3> &block = scene_.get_block(block_idx);
        hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype,3> &aux_block = aux_scene->get_block(block_idx);

        vbl_bounding_box<unsigned int,2> img_bb;

        // initialize ray_origin() function for this block
        if (!generate_ray_init(block_bb, img_bb)) {
          continue;
        }

        // get direction of vector and enter point - this depends on which camera type we have
        vgl_point_3d<double> ray_origin;
        vgl_vector_3d<double> direction;
        vgl_point_3d<double> enter_pt(0.0,0.0,0.0);
        vcl_vector<vgl_point_3d<double> > plane_intersections(6);
        generate_ray(i,j,block_bb, ray_origin, direction);

        // compute intersection of each plane with ray
        double lambda[6];
        lambda[0] = (block_bb.xmin() - ray_origin.x())/direction.x();
        lambda[1] = (block_bb.xmax() - ray_origin.x())/direction.x();
        lambda[2] = (block_bb.ymin() - ray_origin.y())/direction.y();
        lambda[3] = (block_bb.ymax() - ray_origin.y())/direction.y();
        lambda[4] = (block_bb.zmin() - ray_origin.z())/direction.z();
        lambda[5] = (block_bb.zmax() - ray_origin.z())/direction.z();
        for (unsigned int face=0; face<6; ++face) {
          plane_intersections[face] = ray_origin + (direction * lambda[face]);
        }

        // determine which point is the entrance point based on direction
        const double epsilon = 1e-6; // use in place of zero to avoid badly conditioned lambdas
        if ( (plane_intersections[5].x() >= block_bb.xmin()) && (plane_intersections[5].x() <= block_bb.xmax()) &&
          (plane_intersections[5].y() >= block_bb.ymin()) && (plane_intersections[5].y() <= block_bb.ymax()) &&
          (direction.z() < -epsilon) )
        {
          // ray intersects the zmax plane
          // check zmax first since it is probably the most common 
          enter_pt = plane_intersections[5];
        }
        else if ( (plane_intersections[0].y() >= block_bb.ymin()) && (plane_intersections[0].y() <= block_bb.ymax()) &&
          (plane_intersections[0].z() >= block_bb.zmin()) && (plane_intersections[0].z() <= block_bb.zmax()) &&
          (direction.x() > epsilon) )
        {
          // ray intersects the xmin plane
          enter_pt = plane_intersections[0];
        }
        else if ( (plane_intersections[1].y() >= block_bb.ymin()) && (plane_intersections[1].y() <= block_bb.ymax()) &&
          (plane_intersections[1].z() >= block_bb.zmin()) && (plane_intersections[1].z() <= block_bb.zmax()) &&
          (direction.x() < -epsilon) )
        {
          // ray intersects the xmax plane
          enter_pt = plane_intersections[1];
        }
        else if ( (plane_intersections[2].x() >= block_bb.xmin()) && (plane_intersections[2].x() <= block_bb.xmax()) &&
          (plane_intersections[2].z() >= block_bb.zmin()) && (plane_intersections[2].z() <= block_bb.zmax()) &&
          (direction.y() > epsilon) ) 
        {
          // ray intersects the ymin plane
          enter_pt = plane_intersections[2];
        }
        else if ( (plane_intersections[3].x() >= block_bb.xmin()) && (plane_intersections[3].x() <= block_bb.xmax()) &&
          (plane_intersections[3].z() >= block_bb.zmin()) && (plane_intersections[3].z() <= block_bb.zmax()) &&
          (direction.y() < -epsilon) ) 
        {
          // ray intersects the ymax plane
          enter_pt = plane_intersections[3];
        }
        else if ( (plane_intersections[4].x() >= block_bb.xmin()) && (plane_intersections[4].x() <= block_bb.xmax()) &&
          (plane_intersections[4].y() >= block_bb.ymin()) && (plane_intersections[4].y() <= block_bb.ymax()) &&
          (direction.z() > epsilon) ) 
        {
          // ray intersects the zmin plane
          enter_pt = plane_intersections[4];
        }
        else {
          // no entry point into this block found
          continue;
        }
        // step through cells, starting at enter_pt
        hsds_fd_tree_node_index<3> cell_index = block.index_at(vnl_vector_fixed<double,3>(enter_pt.x(),enter_pt.y(),enter_pt.z()));

        while (continue_trace) {
          vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(cell_index);
          // find exit point of cell
          vgl_point_3d<double> exit_pt;
          unsigned int step_dim = 0;
          bool step_positive = false;
          bool found_exit = cube_exit_point(cell_bb, enter_pt, direction, exit_pt, step_dim, step_positive);
          if (!found_exit) {
            vcl_cerr << "error: could not find cell exit point" << vcl_endl;
            vcl_cerr << "   enter_pt = [" << enter_pt.x() << ", " << enter_pt.y() << ", " << enter_pt.z() << "]" << vcl_endl;
            vcl_cerr << "   direction = [" << direction.x() << ", " << direction.y() << ", " << direction.z() << "] " << vcl_endl;
            vcl_cerr << "   cell_bb = [" << cell_bb.xmin() <<", " << cell_bb.xmax() << "]  [" << cell_bb.ymin() << ", " << cell_bb.ymax() << "]  [" << cell_bb.zmin() << ", " << cell_bb.zmax() <<"] " << vcl_endl;
            assert(found_exit);
            break;
          }
          psm_sample<APM> &cell_val = block[cell_index];
          psm_aux_traits<AUX_T>::sample_datatype &aux_val = aux_block[cell_index];
          continue_trace = step_functor.step_cell(i,j, enter_pt, exit_pt, cell_val, aux_val, psm_cell_id(block_idx,cell_index));

          // determine next cell
          hsds_fd_tree_node_index<3> exit_idx = block.full_index_at(cell_index,vnl_vector_fixed<double,3>(exit_pt.x(),exit_pt.y(),exit_pt.z()));
          hsds_fd_tree_node_index<3> neighbor;
          bool found_neighbor = block.neighbor_cell(exit_idx,step_dim,step_positive,neighbor);
          if (!found_neighbor) {
            // we have reached the boundary of this block. stop trace.
            break;
          }
          enter_pt = exit_pt;
          cell_index = neighbor;
        }
      }
    }
    return true;

  }



  //: run the function
  bool run(F& step_functor) 
  {
    psm_block_visibility_iterator<APM> block_vis_it(scene_, cam_, img_i0_, img_j0_, img_ni_, img_nj_, reverse_traversal_);

    vil_image_view<bool> continue_trace(img_ni_, img_nj_);
    continue_trace.fill(true);

    psm_aux_scene<AUX_T> *aux_scene = dynamic_cast<psm_aux_scene<AUX_T>*>(aux_scene_ptr_.ptr());
    if (!aux_scene) {
      vcl_cerr << "error: psm_parallel_raytrace_function: failed to cast aux_scene to correct type." << vcl_endl;
      return false;
    }

    while(block_vis_it.next())
    {
      vcl_vector<vgl_point_3d<int> > vis_blocks;
      block_vis_it.current_blocks(vis_blocks);
      // traverse each visible block one at a time
      vcl_vector<vgl_point_3d<int> >::iterator block_it = vis_blocks.begin();
      for (; block_it != vis_blocks.end(); ++block_it) {
        const vgl_point_3d<int> block_idx = *block_it;
        if (debug_lvl_ > 0) vcl_cout << "processing block at index (" << block_idx.x() << ", " << block_idx.y() << ", " << block_idx.z() << ")" << vcl_endl;
        // make sure block projects to inside of image
        vbl_bounding_box<double,3> block_bb = scene_.block_bounding_box(block_idx);
        if (!cube_visible(block_bb, cam_, img_i0_, img_j0_, img_ni_, img_nj_, false)) {
          continue;
        }

        hsds_fd_tree<psm_sample<APM>,3> &block = scene_.get_block(block_idx);
        hsds_fd_tree<typename psm_aux_traits<AUX_T>::sample_datatype,3> &aux_block = aux_scene->get_block(block_idx);

        vbl_bounding_box<unsigned int,2> img_bb;

        // initialize ray_origin() function for this block
        if (!generate_ray_init(block_bb, img_bb)) {
          continue;
        }

        // for each image pixel
        for (unsigned int i=img_bb.xmin(); i < img_bb.xmax(); ++i) {
          if (debug_lvl_ > 1) {
            if (!(i % 10))
              vcl_cout << ".";
          }
          for (unsigned int j=img_bb.ymin(); j < img_bb.ymax(); ++j) {
            if (!continue_trace(i - img_i0_ , j - img_j0_)) {
              continue;
            }
            // get direction of vector and enter point - this depends on which camera type we have
            vgl_point_3d<double> ray_origin;
            vgl_vector_3d<double> direction;
            vgl_point_3d<double> enter_pt(0.0,0.0,0.0);
            vcl_vector<vgl_point_3d<double> > plane_intersections(6);

            // add 0.5 to get center of pixel
            generate_ray(i + 0.5f, j + 0.5f, block_bb, ray_origin, direction);

            // compute intersection of each plane with ray
            double lambda[6];
            lambda[0] = (block_bb.xmin() - ray_origin.x())/direction.x();
            lambda[1] = (block_bb.xmax() - ray_origin.x())/direction.x();
            lambda[2] = (block_bb.ymin() - ray_origin.y())/direction.y();
            lambda[3] = (block_bb.ymax() - ray_origin.y())/direction.y();
            lambda[4] = (block_bb.zmin() - ray_origin.z())/direction.z();
            lambda[5] = (block_bb.zmax() - ray_origin.z())/direction.z();
            for (unsigned int face=0; face<6; ++face) {
              plane_intersections[face] = ray_origin + (direction * lambda[face]);
            }

            // determine which point is the entrance point based on direction
            const double epsilon = 1e-6; // use in place of zero to avoid badly conditioned lambdas
            if ( (plane_intersections[5].x() >= block_bb.xmin()) && (plane_intersections[5].x() <= block_bb.xmax()) &&
              (plane_intersections[5].y() >= block_bb.ymin()) && (plane_intersections[5].y() <= block_bb.ymax()) &&
              (direction.z() < -epsilon) )
            {
              // ray intersects the zmax plane
              // check zmax first since it is probably the most common 
              enter_pt = plane_intersections[5];
            }
            else if ( (plane_intersections[0].y() >= block_bb.ymin()) && (plane_intersections[0].y() <= block_bb.ymax()) &&
              (plane_intersections[0].z() >= block_bb.zmin()) && (plane_intersections[0].z() <= block_bb.zmax()) &&
              (direction.x() > epsilon) )
            {
              // ray intersects the xmin plane
              enter_pt = plane_intersections[0];
            }
            else if ( (plane_intersections[1].y() >= block_bb.ymin()) && (plane_intersections[1].y() <= block_bb.ymax()) &&
              (plane_intersections[1].z() >= block_bb.zmin()) && (plane_intersections[1].z() <= block_bb.zmax()) &&
              (direction.x() < -epsilon) )
            {
              // ray intersects the xmax plane
              enter_pt = plane_intersections[1];
            }
            else if ( (plane_intersections[2].x() >= block_bb.xmin()) && (plane_intersections[2].x() <= block_bb.xmax()) &&
              (plane_intersections[2].z() >= block_bb.zmin()) && (plane_intersections[2].z() <= block_bb.zmax()) &&
              (direction.y() > epsilon) ) 
            {
              // ray intersects the ymin plane
              enter_pt = plane_intersections[2];
            }
            else if ( (plane_intersections[3].x() >= block_bb.xmin()) && (plane_intersections[3].x() <= block_bb.xmax()) &&
              (plane_intersections[3].z() >= block_bb.zmin()) && (plane_intersections[3].z() <= block_bb.zmax()) &&
              (direction.y() < -epsilon) ) 
            {
              // ray intersects the ymax plane
              enter_pt = plane_intersections[3];
            }
            else if ( (plane_intersections[4].x() >= block_bb.xmin()) && (plane_intersections[4].x() <= block_bb.xmax()) &&
              (plane_intersections[4].y() >= block_bb.ymin()) && (plane_intersections[4].y() <= block_bb.ymax()) &&
              (direction.z() > epsilon) ) 
            {
              // ray intersects the zmin plane
              enter_pt = plane_intersections[4];
            }
            else {
              // no entry point into this block found
              continue;
            }
            // step through cells, starting at enter_pt
            hsds_fd_tree_node_index<3> cell_index = block.index_at(vnl_vector_fixed<double,3>(enter_pt.x(),enter_pt.y(),enter_pt.z()));

            while (continue_trace(i-img_i0_, j-img_j0_)) {
              vbl_bounding_box<double,3> cell_bb = block.cell_bounding_box(cell_index);
              // find exit point of cell
              vgl_point_3d<double> exit_pt;
              unsigned int step_dim = 0;
              bool step_positive = false;
              bool found_exit = cube_exit_point(cell_bb, enter_pt, direction, exit_pt, step_dim, step_positive);
              if (!found_exit) {
                vcl_cerr << "error: could not find cell exit point" << vcl_endl;
                vcl_cerr << "   enter_pt = [" << enter_pt.x() << ", " << enter_pt.y() << ", " << enter_pt.z() << "]" << vcl_endl;
                vcl_cerr << "   direction = [" << direction.x() << ", " << direction.y() << ", " << direction.z() << "] " << vcl_endl;
                vcl_cerr << "   cell_bb = [" << cell_bb.xmin() <<", " << cell_bb.xmax() << "]  [" << cell_bb.ymin() << ", " << cell_bb.ymax() << "]  [" << cell_bb.zmin() << ", " << cell_bb.zmax() <<"] " << vcl_endl;
                assert(found_exit);
                break;
              }
              psm_sample<APM> &cell_val = block[cell_index];
              psm_aux_traits<AUX_T>::sample_datatype &aux_val = aux_block[cell_index];
              continue_trace(i-img_i0_, j-img_j0_) = step_functor.step_cell(i,j, enter_pt, exit_pt, cell_val, aux_val, psm_cell_id(block_idx,cell_index));

              // determine next cell
              hsds_fd_tree_node_index<3> exit_idx = block.full_index_at(cell_index,vnl_vector_fixed<double,3>(exit_pt.x(),exit_pt.y(),exit_pt.z()));
              hsds_fd_tree_node_index<3> neighbor;
              bool found_neighbor = block.neighbor_cell(exit_idx,step_dim,step_positive,neighbor);
              if (!found_neighbor) {
                // we have reached the boundary of this block. stop trace.
                break;
              }
              enter_pt = exit_pt;
              cell_index = neighbor;
            }
          }
        }
      }
    }
    return true;
  }

protected:

  bool generate_ray_init(vbl_bounding_box<double,3> const& block_bb, vbl_bounding_box<unsigned int,2> &img_bb)
  {
    // determine intersection of block bounding box projection and image bounds
    vbl_bounding_box<double,2> img_bounds;
    img_bounds.update(img_i0_,img_j0_);
    img_bounds.update(img_i0_ + img_ni_ - 1, img_j0_ + img_nj_ - 1);

    vbl_bounding_box<double,2> block_projection;
    double u,v;
    cam_->project(block_bb.xmin(),block_bb.ymin(),block_bb.zmin(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmin(),block_bb.ymin(),block_bb.zmax(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmin(),block_bb.ymax(),block_bb.zmin(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmin(),block_bb.ymax(),block_bb.zmax(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmax(),block_bb.ymin(),block_bb.zmin(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmax(),block_bb.ymin(),block_bb.zmax(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmax(),block_bb.ymax(),block_bb.zmin(),u,v);
    block_projection.update(u,v);
    cam_->project(block_bb.xmax(),block_bb.ymax(),block_bb.zmax(),u,v);
    block_projection.update(u,v);

    img_bb.reset();
    if (disjoint(img_bounds,block_projection)) {
      img_bb.update(0,0);
      return false;
    }
    else {
      unsigned int xmin = (unsigned int)vcl_max(img_bounds.xmin(), block_projection.xmin());
      unsigned int ymin = (unsigned int)vcl_max(img_bounds.ymin(), block_projection.ymin());
      img_bb.update(xmin,ymin);
      unsigned int xmax = (unsigned int)vcl_min(img_bounds.xmax(), block_projection.xmax()) + 1;
      unsigned int ymax = (unsigned int)vcl_min(img_bounds.ymax(), block_projection.ymax()) + 1;
      img_bb.update(xmax,ymax);
    }
    if (vpgl_perspective_camera<double> const* pcam = dynamic_cast<vpgl_perspective_camera<double> const*>(cam_)) {
      cam_is_rational_ = false;
    }
    else if (vpgl_rational_camera<double> const* rcam = dynamic_cast<vpgl_rational_camera<double> const*>(cam_)) {
      cam_is_rational_ = true;

      // assume that the ray intersects the two z faces, and is a above the scene 
      vcl_vector<vgl_homg_point_2d<double> > corners_img;
      vcl_vector<vgl_homg_point_2d<double> > corners_top;
      vcl_vector<vgl_homg_point_2d<double> > corners_bot;

      // create vectors containing four corners of grid, and their projections into the image

      vgl_point_3d<double> top_intersect, bot_intersect;
      // 0,0
      corners_img.push_back(vgl_homg_point_2d<double>(img_bb.xmin(),img_bb.ymin()));
      top_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmax());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmin(),img_bb.ymin()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmax()),top_intersect,top_intersect);
      corners_top.push_back(vgl_homg_point_2d<double>(top_intersect.x(),top_intersect.y()));
      bot_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmin());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmin(),img_bb.ymin()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmin()),bot_intersect,bot_intersect);
      corners_bot.push_back(vgl_homg_point_2d<double>(bot_intersect.x(),bot_intersect.y()));

      // 0,nj
      corners_img.push_back(vgl_homg_point_2d<double>(img_bb.xmin(),img_bb.ymax()));
      top_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmax());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmin(),img_bb.ymax()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmax()),top_intersect,top_intersect);
      corners_top.push_back(vgl_homg_point_2d<double>(top_intersect.x(),top_intersect.y()));
      bot_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmin());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmin(),img_bb.ymax()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmin()),bot_intersect,bot_intersect);
      corners_bot.push_back(vgl_homg_point_2d<double>(bot_intersect.x(),bot_intersect.y()));

      // ni,0
      corners_img.push_back(vgl_homg_point_2d<double>(img_bb.xmax(),img_bb.ymin()));
      top_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmax());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmax(),img_bb.ymin()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmax()),top_intersect,top_intersect);
      corners_top.push_back(vgl_homg_point_2d<double>(top_intersect.x(),top_intersect.y()));
      bot_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmin());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmax(),img_bb.ymin()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmin()),bot_intersect,bot_intersect);
      corners_bot.push_back(vgl_homg_point_2d<double>(bot_intersect.x(),bot_intersect.y()));

      // ni,nj
      corners_img.push_back(vgl_homg_point_2d<double>(img_bb.xmax(),img_bb.ymax()));
      top_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmax());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmax(),img_bb.ymax()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmax()),top_intersect,top_intersect);
      corners_top.push_back(vgl_homg_point_2d<double>(top_intersect.x(),top_intersect.y()));
      bot_intersect = vgl_point_3d<double>(block_bb.xmin(),block_bb.ymin(),block_bb.zmin());
      vpgl_backproject::bproj_plane(*rcam,vgl_point_2d<double>(img_bb.xmax(),img_bb.ymax()),vgl_plane_3d<double>(0.0,0.0,1.0,-block_bb.zmin()),bot_intersect,bot_intersect);
      corners_bot.push_back(vgl_homg_point_2d<double>(bot_intersect.x(),bot_intersect.y()));

      vgl_h_matrix_2d_compute_linear comp_4pt;
      if (!comp_4pt.compute(corners_img,corners_top, H_img_to_top_)) {
        vcl_cerr << "ERROR computing homography from image to zmax plane.\n";
        return false;
      }
      if (!comp_4pt.compute(corners_img,corners_bot, H_img_to_bot_)) {
        vcl_cerr << "ERROR computing homography from voxel slice to zmin plane.\n";
        return false;
      }
    }
    else {
      vcl_cerr << "Error: Unsupported Camera type! " << vcl_endl;
      return false;
    }
    return true;
  }

  void generate_ray(float i, float j, vbl_bounding_box<double,3> const& block_bb, vgl_point_3d<double> &ray_origin, vgl_vector_3d<double> &direction)
  {
    if (cam_is_rational_) {
      // get coordinates of ray intersection with top and bottom plane 
      vnl_vector_fixed<double,3> img_pt(i,j,1.0);
      vnl_vector_fixed<double,3> top_pt_2d =  H_img_to_top_.get_matrix() * img_pt;
      vgl_point_3d<double> top_pt(top_pt_2d[0]/top_pt_2d[2], top_pt_2d[1]/top_pt_2d[2], block_bb.zmax());
      vnl_vector_fixed<double,3> bot_pt_2d =  H_img_to_bot_.get_matrix() * img_pt;
      vgl_point_3d<double> bot_pt(bot_pt_2d[0]/bot_pt_2d[2], bot_pt_2d[1]/bot_pt_2d[2], block_bb.zmin());

      // direction is normalize vector top - bottom
      direction = normalize(bot_pt - top_pt);

      // use top point as origin
      ray_origin = top_pt;

    } else {
      vpgl_perspective_camera<double> const* pcam = static_cast<vpgl_perspective_camera<double> const*>(cam_);
      // backproject image point to a ray
      ray_origin = pcam->camera_center();
      vgl_line_3d_2_points<double> cam_ray = pcam->backproject(vgl_homg_point_2d<double>((double)i,(double)j));
      direction = normalize(cam_ray.direction());
    }
  }

  bool cam_is_rational_;
  vgl_h_matrix_2d<double> H_img_to_top_; // only used in case of rational camera
  vgl_h_matrix_2d<double> H_img_to_bot_; // only used in case of rational camera

  psm_scene<APM> &scene_;
  psm_aux_scene_base_sptr aux_scene_ptr_;

  bool reverse_traversal_;

  const vpgl_camera<double>* cam_;
  unsigned int img_i0_;
  unsigned int img_j0_;
  unsigned int img_ni_;
  unsigned int img_nj_;

  bool use_aux_;
  unsigned int debug_lvl_;


};

#endif

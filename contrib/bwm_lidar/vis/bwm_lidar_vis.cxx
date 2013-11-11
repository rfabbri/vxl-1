//: This is bwm_lidar_vis.cxx
//  lidar image meshing and visualization.
//  MingChing Chang
//  Dec 13, 2007.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vul/vul_sprintf.h>
#include <vul/vul_file.h>

#include <vcl_string.h>
#include <vcl_iomanip.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>

#include <vpgl/vpgl_proj_camera.h>

#include <vil/vil_crop.h>
#include <vil/vil_save.h>
#include <vil/vil_copy.h>

#include <vsol/vsol_box_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vil/algo/vil_binary_closing.h>
#include <vil/algo/vil_structuring_element.h>

#include <dbmsh3d/dbmsh3d_textured_mesh_mc.h>
#include <dbmsh3d/dbmsh3d_face_mc.h>
#include <dbmsh3d/dbmsh3d_textured_face_mc.h>
#include <dbmsh3d/dbmsh3d_vertex.h>

#include <dbmsh3d/algo/dbmsh3d_mesh_bnd.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_utils.h>
#include <dbmsh3d/vis/dbmsh3d_vis_vertex.h>
#include <dbmsh3d/vis/dbmsh3d_vis_edge.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>

#include <dbsol/algo/dbsol_curve_algs.h>


#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoBaseColor.h>
#include <Inventor/nodes/SoGroup.h>
#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoIndexedFaceSet.h>
///#include <Inventor/nodes/SoIndexedLineSet.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoCoordinate3.h>
///#include <Inventor/nodes/SoSwitch.h>

///#include <Inventor/sensors/SoSensor.h>
///#include <Inventor/sensors/SoTimerSensor.h>

///#include <Inventor/engines/SoTimeCounter.h>
///#include <Inventor/engines/SoCalculator.h>

#include "bwm_lidar_vis.h"

SoSeparator* draw_range_pixel_cube (lidar_pixel* p, int shift_x, int shift_y, int shift_z, float ground_height)
{
  SoSeparator* root = new SoSeparator;
  
  //color
  SoBaseColor *basecolor = new SoBaseColor;
  basecolor->rgb = SbColor (p->color_r_, p->color_g_, p->color_b_);
  root->addChild (basecolor);

  SoTranslation* trans = new SoTranslation;
  trans->translation.setValue (SbVec3f(shift_x, shift_y, shift_z + (p->height_ + ground_height)/2));
  root->addChild (trans);

  SoCube* cube = new SoCube ();
  cube->width = 1;
  cube->height = 1;
  cube->depth = p->height_ - ground_height;
  root->addChild (cube);

  return root;
}

//mesh and visualize
SoSeparator* vis_lidar_data (lidar_range_data* LRD, int shift_x, int shift_y, int shift_z, float ground_height)
{
  SoSeparator* root = new SoSeparator;

  for (int y=0; y<LRD->data_.size(); y++) {
    for (int x=0; x<LRD->data_[y].size(); x++) {
      lidar_pixel* p = LRD->data_[y][x];
      if (p->label_ == 0 && p->color_r_==0 && p->color_g_ == 0 && p->color_b_ == 0)
        continue;
      root->addChild (draw_range_pixel_cube (p, shift_x + x, shift_y + y, shift_z, ground_height));
    }
  }

  return root;
}

void median_filter_array_3x3 (vnl_matrix<double>& height, const int ni, const int nj)
{  
  for (int j=0; j<nj; j++)
    for (int i=0; i<ni; i++)
      height(i, j) = median_3x3 (height, ni, nj, i, j);
}

SoSeparator* vis_lidar_pixel_color (vil_image_view<int>& labels, 
                                    vil_image_view<unsigned char>& colors, 
                                    vnl_matrix<double>& height, 
                                    vnl_matrix<int>& occupied,
                                    const double& ground_height)
{
  SoSeparator* root = new SoSeparator;

  int shift_x = 0;
  int shift_y = 0;
  int shift_z = 0;

  
  //median filiter the height array.
  median_filter_array_3x3 (height, labels.ni(), labels.nj());

  for (int j=0; j<labels.nj(); j++) {
    for (int i=0; i<labels.ni(); i++) {
      int l = labels(i,j);
      if (l == 0) /// && colors(i,j,0)==0 && colors(i,j,1)==0 && colors(i,j,2)==0)
        continue; //skip all ground pixels.
      lidar_pixel p (l, height(i,j), float(colors(i,j,0))/255, float(colors(i,j,1))/255, float(colors(i,j,2))/255);
      //skip flat cube
      if (vcl_fabs (height(i,j) - ground_height) < 0.1)
        continue;
      root->addChild (draw_range_pixel_cube (&p, shift_x + i, shift_y + j, shift_z, ground_height));
    }
  }

  return root;
}

//######################################################################

//: This function takes the 32 views and camera positions and a building mesh
//  and produce textured mapping buildings as a VRML file.
void generate_texture_bld (vcl_vector<vil_image_view<vxl_byte> >& views, 
                           vcl_vector<vnl_matrix_fixed<double,3,4> >& camera_matrices,
                           vcl_vector<vnl_vector_fixed<double,3> >& camera_center,
                           vcl_vector<vnl_vector_fixed<double,3> >& camera_view_direction,
                           dbmsh3d_textured_mesh_mc* M,
                           const char* out_file)
{

}


//######################################################################


#include <vil/vil_save.h>
#include <../seg/dbdet/tracer/dbdet_contour_tracer.h>

#include <dbsol/dbsol_file_io.h>

bool save_bw_image (vil_image_view<bool>& img, vcl_string filename)
{
  vil_image_view<vxl_byte> tmp (img.ni(), img.nj(), 1);
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++) 
      if (img(i,j,0))
        tmp (i, j, 0) = 255;
      else
        tmp (i, j, 0) = 0;
  return vil_save (tmp, filename.c_str());
}


SoSeparator* draw_filled_polygon (vcl_vector<vsol_point_2d_sptr>& poly_points, const float height,
                                  const SbColor& color, const float fTransparency)
{
  SoSeparator* root = new SoSeparator;

  //Put shapehint for possible concave polygon.
  SoShapeHints* hints = new SoShapeHints();
  hints->vertexOrdering = SoShapeHints::CLOCKWISE;
  hints->shapeType = SoShapeHints::SOLID;
  ///hints->vertexOrdering = SoShapeHints::COUNTERCLOCKWISE;
  ///hints->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
  hints->faceType = SoShapeHints::UNKNOWN_FACE_TYPE; //this may slow down the rendering!
  root->addChild (hints);

  //Create a SoMaterial to draw color in detail.
  SoMaterial *material = new SoMaterial;
  material->diffuseColor.setValue (color);
  material->emissiveColor.setValue(color/2);
  material->transparency = fTransparency;
  root->addChild (material);

  unsigned int nVertices = poly_points.size();
  SbVec3f* verts = new SbVec3f[nVertices];
  for (unsigned int i=0; i<nVertices; i++)
    verts[i] = SbVec3f (poly_points[i]->x(), poly_points[i]->y(), height);

  _draw_filled_polygon_geom (root, verts, nVertices);

  delete[] verts;
  return root;
}

double estimate_ground_height (vil_image_view<vxl_byte>& img_view_label, 
                               vil_image_view<vxl_byte>& img_view_max)
{  
  //Estimate the ground height and draw the ground plane.
  vcl_vector<double> gnd_height;
  for (unsigned int j=0; j<img_view_label.nj(); j++)
    for (unsigned int i=0; i<img_view_label.ni(); i++)
      if (img_view_label(i,j,0) == 0) {
        double height = img_view_max(i, j);
          gnd_height.push_back (height);
      }

  //Estimate the median ground height.
  assert (gnd_height.size() != 0);
  vcl_nth_element (gnd_height.begin(),
                   gnd_height.begin() + int(gnd_height.size()/2), 
                   gnd_height.end());
  double ground_height = *(gnd_height.begin() + int(gnd_height.size()/2));
  return ground_height;
}

SoSeparator* draw_buildings (vil_image_view<vxl_byte>& img_view_label, 
                             vil_image_view<vxl_byte>& img_view_max,
                             const double ground_height)
{
  SoSeparator* root = new SoSeparator;

  //Put the label of buildings to bld_label (gray-scale image).
  //Also put all labels to a set.
  vil_image_view<int> bld_label_img (img_view_label.ni(), img_view_label.nj());
  bld_label_img.fill (0);
  vcl_set<int> bld_label_set;

  //Go through the labeled image and generate a building labeling image.
  for (unsigned int j=0; j<img_view_label.nj(); j++)
    for (unsigned int i=0; i<img_view_label.ni(); i++) {
      if (img_view_label(i,j,0)) { //building: red != 0
        int label = img_view_label(i,j,0);
        bld_label_img(i, j) = label;
        //add to set.
        if (bld_label_set.find(label) == bld_label_set.end())
          bld_label_set.insert (label);
      }
    }    
  vul_printf (vcl_cout, "  building labeling image generated.\n");

  //Go through each building label and generate a binary image for contour tracing.
  vcl_set<int>::iterator it = bld_label_set.begin(); 
  int count = 0;
  vul_printf (vcl_cout, "    drawing building: ");
  for (; it != bld_label_set.end(); it++, count++) {
    vul_printf (vcl_cout, "%d ", count);
    int label = *it;
    vil_image_view<bool> bld_bin (bld_label_img.ni(), bld_label_img.nj());
    bld_bin.fill (false);

    //Create building binary image.
    //Also estimate the average building height from lidar_image_max.
    vcl_vector<double> bld_height;

    //get building color from the first non-zero pixel.
    SbColor color (0, 0, 0);

    for (unsigned int j=0; j<bld_label_img.nj(); j++)
      for (unsigned int i=0; i<bld_label_img.ni(); i++)
        if (bld_label_img(i,j) == label) {
          bld_bin(i,j) = true;
          double height = img_view_max(i, j);
          bld_height.push_back (height);

          if (color == SbColor (0,0,0)) {
            double r = img_view_label(i, j, 0);
            double g = img_view_label(i, j, 1);
            double b = img_view_label(i, j, 2);
            color = SbColor (float(r/255), float(g/255), float(b/255));
          }
        }

    //save bld_bin to debug.
    ///save_bw_image (bld_bin, "bld.png");

    //Contour tracing this building.    
    //invoke the tracer
    float sigma=0, beta=0;
    int nsteps=0, threshold=0;
    int intensity_threshold = 127;

    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(sigma);
    ctracer.set_curvature_smooth_nsteps(nsteps);
    ctracer.set_curvature_smooth_beta(beta);
    ctracer.trace(bld_bin);

    //Get the largest contour points.
    ///vcl_vector<vsol_point_2d_sptr> poly_points = ctracer.largest_contour();

    //Go through all contours which is not too small.
    for (unsigned int c=0; c<ctracer.contours().size(); c++) {
      vcl_vector<vsol_point_2d_sptr> poly_points_0 = ctracer.contours()[c];

      //line fitting.
      vcl_vector<vsol_point_2d_sptr> poly_points; 
      fit_lines_to_contour (poly_points_0, 0.05, poly_points);

      if (poly_points.size() < 3) //10
        continue;

    //Debug: save this con file
    ///if (label == 229)
      ///dbsol_save_con_file("bld_229.con", poly_points, true);

      //perturb a little (to remove the degenerate problem in visualization).
      for (unsigned i=0; i<poly_points.size(); i++) {
        double x = poly_points[i]->x();
        double y = poly_points[i]->y();              
        double random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_x (x + random * 0.001);
        random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_y (y + random * 0.001);
      }
      
      //Estimate the median building height.
      assert (bld_height.size() != 0);
      vcl_nth_element (bld_height.begin(),
                       bld_height.begin() + int(bld_height.size()/2), 
                       bld_height.end());
      double median_height = *(bld_height.begin() + int(bld_height.size()/2));

      //mesh the building using polygon points and height.
      SoSeparator* bld_vis = new SoSeparator;    
      root->addChild (bld_vis);
      bld_vis->addChild (draw_filled_polygon (poly_points, float(median_height), color, 0.0f));

      //draw each vertical face of the building from median_height to the ground_height.
      /*SbVec3f* face = new SbVec3f[4];
      for (unsigned int i=0; i<poly_points.size(); i++) {
        int j = (i+1) % poly_points.size();
        face[0].setValue (poly_points[i]->x(), poly_points[i]->y(), median_height);
        face[1].setValue (poly_points[j]->x(), poly_points[j]->y(), median_height);
        face[2].setValue (poly_points[j]->x(), poly_points[j]->y(), ground_height);
        face[3].setValue (poly_points[i]->x(), poly_points[i]->y(), ground_height);
        bld_vis->addChild (draw_filled_polygon (face, 4, color, 0));
      }   
      delete face;*/
    }
    bld_height.clear();
  }
  vul_printf (vcl_cout, "\n");

  return root;
}

SoSeparator* draw_vegetation (vil_image_view<vxl_byte>& img_view_label, 
                              vil_image_view<vxl_byte>& img_view_max,
                              const double ground_height)
{
  SoSeparator* root = new SoSeparator;

  //Put the label of vegetation to veg_label (gray-scale image).
  //Also put all labels to a set.
  vil_image_view<int> veg_label_img (img_view_label.ni(), img_view_label.nj());
  veg_label_img.fill (0);
  vcl_set<int> veg_label_set;

  //Go through the labeled image and generate a building labeling image.
  for (unsigned int j=0; j<img_view_label.nj(); j++)
    for (unsigned int i=0; i<img_view_label.ni(); i++) {
      if (img_view_label(i,j,1)) { //vegetation: green != 0
        int label = img_view_label(i,j,1);

        veg_label_img(i, j) = label;

        //add to set.
        if (veg_label_set.find(label) == veg_label_set.end())
          veg_label_set.insert (label);
      }
    }
  vul_printf (vcl_cout, "  vegetation labeling image generated.\n");

  //Go through each vegetation label and generate a binary image for contour tracing.
  vcl_set<int>::iterator it = veg_label_set.begin(); 
  int count = 0;
  vul_printf (vcl_cout, "    drawing vegetation: ");
  for (; it != veg_label_set.end(); it++, count++) {
    vul_printf (vcl_cout, "%d ", count);
    int label = *it;
    vil_image_view<bool> veg_bin (veg_label_img.ni(), veg_label_img.nj());
    veg_bin.fill (false);

    //Create vegetation binary image.
    //Also estimate the average vegetation height from lidar_image_max.
    vcl_vector<double> veg_height;

    //get vegetation color from the first non-zero pixel.
    SbColor color (0, 0, 0);

    for (unsigned int j=0; j<veg_label_img.nj(); j++)
      for (unsigned int i=0; i<veg_label_img.ni(); i++)
        if (veg_label_img(i,j) == label) {
          veg_bin(i,j) = true;
          double height = img_view_max(i, j);
          veg_height.push_back (height);

          if (color == SbColor (0,0,0)) {
            double r = img_view_label(i, j, 0);
            double g = img_view_label(i, j, 1);
            double b = img_view_label(i, j, 2);
            color = SbColor (float(r/255), float(g/255), float(b/255));
          }
        }

    //Contour tracing this vegetation.    
    //invoke the tracer
    float sigma=0, beta=0;
    int nsteps=0, threshold=0;
    int intensity_threshold = 127;

    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(sigma);
    ctracer.set_curvature_smooth_nsteps(nsteps);
    ctracer.set_curvature_smooth_beta(beta);
    ctracer.trace(veg_bin);

    //Get the largest contour's polygon points.
    ///vcl_vector<vsol_point_2d_sptr> poly_points = ctracer.largest_contour();

    //Go through all contours which is not too small.
    for (unsigned int c=0; c<ctracer.contours().size(); c++) {
      vcl_vector<vsol_point_2d_sptr> poly_points = ctracer.contours()[c];
      if (poly_points.size() < 10)
        continue;

      //perturb a little (to remove the degenerate problem in visualization).
      for (unsigned i=0; i<poly_points.size(); i++) {
        double x = poly_points[i]->x();
        double y = poly_points[i]->y();              
        double random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_x (x + random * 0.001);
        random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_y (y + random * 0.001);
      }

      //Debug: save this con file
      if (label == 240) {
        save_bw_image (veg_bin, "veg_240.png");
        dbsol_save_con_file("veg_240.con", poly_points, true);
      }
      
      //Estimate the median building height.
      assert (veg_height.size() != 0);
      vcl_nth_element (veg_height.begin(),
                       veg_height.begin() + int(veg_height.size()/2), 
                       veg_height.end());
      double median_height = *(veg_height.begin() + int(veg_height.size()/2));

      //mesh the building using polygon points and height.
      SoSeparator* veg_vis = new SoSeparator;    
      root->addChild (veg_vis);
      veg_vis->addChild (draw_filled_polygon (poly_points, float(median_height), color, 0.0f));

      //draw each vertical face of the building from median_height to the ground_height.
      SbVec3f* face = new SbVec3f[4];
      for (unsigned int i=0; i<poly_points.size(); i++) {
        int j = (i+1) % poly_points.size();
        face[0].setValue (poly_points[i]->x(), poly_points[i]->y(), median_height);
        face[1].setValue (poly_points[j]->x(), poly_points[j]->y(), median_height);
        face[2].setValue (poly_points[j]->x(), poly_points[j]->y(), ground_height);
        face[3].setValue (poly_points[i]->x(), poly_points[i]->y(), ground_height);
        veg_vis->addChild (draw_filled_polygon (face, 4, color, 0));
      }   
      delete face;
    }
    veg_height.clear();
  }
  vul_printf (vcl_cout, "\n");

  return root;
}

void _check_add_v (const double v, vcl_vector<double>& value)
{
  if (v)
    value.push_back (v);
}

int median_3x3 (vil_image_view<vxl_byte>& img_view_max, 
                const int i, const int j, const int plane)
{
  vcl_vector<double> value;
  double v;
  if (i>0) {
    if (j>0)
      _check_add_v (img_view_max(i-1, j-1), value);
    _check_add_v (img_view_max(i-1,j), value);
    if (j<img_view_max.nj()-1)
      _check_add_v (img_view_max(i-1,j+1), value);
  }

  if (j>0)
    _check_add_v (img_view_max(i, j-1), value);
  _check_add_v (img_view_max(i,j), value);
  if (j<img_view_max.nj()-1)
    _check_add_v (img_view_max(i,j+1), value);

  if (i<img_view_max.ni()-1) {
    if (j>0)
      _check_add_v (img_view_max(i+1, j-1), value);
    _check_add_v (img_view_max(i+1,j), value);
    if (j<img_view_max.nj()-1)
      _check_add_v (img_view_max(i+1,j+1), value);
  }

  //Estimate the median ground height.
  assert (value.size() != 0);
  vcl_nth_element (value.begin(),
                   value.begin() + int(value.size()/2), 
                   value.end());
  int median = *(value.begin() + int(value.size()/2));
  return median;
}

SoSeparator* draw_vegetation_pixel (vil_image_view<vxl_byte>& img_view_label, 
                                    vil_image_view<vxl_byte>& img_view_max,
                                    const double ground_height)
{
  SoSeparator* root = new SoSeparator;

  float shift_x = 0;
  float shift_y = 0;
  float shift_z = 0;
  for (unsigned int j=0; j<img_view_label.nj(); j++)
    for (unsigned int i=0; i<img_view_label.ni(); i++)
      if (img_view_label(i,j,1)) { //vegetation: green != 0
        double r = img_view_label(i, j, 0);
        double g = img_view_label(i, j, 1);
        double b = img_view_label(i, j, 2);
        float x = i;
        float y = j;

        //Median filtering the pixel height..
        int median_height = median_3x3 (img_view_max, i, j, 1);
        ///int median_height = img_view_max(i,j);

        ///SbColor color = SbColor (float(r/255), float(g/255), float(b/255));
        lidar_pixel p (img_view_label(i,j,1), median_height, float(r/255), float(g/255), float(b/255));
        root->addChild (draw_range_pixel_cube (&p, shift_x + x, shift_y + y, shift_z, ground_height));
      }  

  return root;
}

SoSeparator* vis_lidar_labeled_image (vil_image_resource_sptr lidar_image_max, 
                                      vil_image_resource_sptr lidar_image_min, 
                                      vil_image_resource_sptr lidar_image_labeled)
{
  vul_printf (vcl_cout, "vis_lidar_labeled_image(): image size %dx%d.\n",
              lidar_image_labeled->get_view()->ni(), lidar_image_labeled->get_view()->nj());
  SoSeparator* root = new SoSeparator;

  vil_image_view<vxl_byte> img_view_max = lidar_image_max->get_view();
  vil_image_view<vxl_byte> img_view_min = lidar_image_min->get_view();
  vil_image_view<vxl_byte> img_view_label = lidar_image_labeled->get_view();

  //Remove the bordering pixel (r,g,b) of the lidar_image_labeled.
  for (unsigned int j=0; j<img_view_label.nj(); j++) {
    img_view_label(0, j, 0) = 0;
    img_view_label(0, j, 1) = 0;
    img_view_label(0, j, 2) = 0;
    img_view_label(img_view_label.ni()-1, j, 0) = 0;
    img_view_label(img_view_label.ni()-1, j, 1) = 0;
    img_view_label(img_view_label.ni()-1, j, 2) = 0;
  }
  for (unsigned int i=0; i<img_view_label.ni(); i++) {
    img_view_label(i, 0, 0) = 0;
    img_view_label(i, 0, 1) = 0;
    img_view_label(i, 0, 2) = 0;
    img_view_label(i, img_view_label.nj()-1, 0) = 0;
    img_view_label(i, img_view_label.nj()-1, 1) = 0;
    img_view_label(i, img_view_label.nj()-1, 2) = 0;
  }

  //Estimate ground height.
  double ground_height = estimate_ground_height (img_view_label, img_view_max);

  //draw the ground plane.
  SbVec3f* ground_plane = new SbVec3f[4];
  ground_plane[0].setValue (0, 0, ground_height);
  ground_plane[1].setValue (img_view_label.ni(), 0, ground_height);
  ground_plane[2].setValue (img_view_label.ni(), img_view_label.nj(), ground_height);
  ground_plane[3].setValue (0, img_view_label.nj(), ground_height);  
  root->addChild (draw_filled_polygon (ground_plane, 4, SbColor(0.5f, 0.5f, 0.5f), 0));
  delete ground_plane;

  //Draw the buildings.
  root->addChild (draw_buildings (img_view_label, img_view_max, ground_height));

  //Draw the vegetation.
  root->addChild (draw_vegetation (img_view_label, img_view_max, ground_height));
  root->addChild (draw_vegetation_pixel (img_view_label, img_view_max, ground_height));

  return root;
}

//########################################################################

SoSeparator* vis_lidar_labeled_data (vil_image_view<int>& labels,
                                     vil_image_view<unsigned char>& labels_colored,
                                     const bool use_labels_colored,
                                     vil_image_view<unsigned char>& colors,
                                     vnl_matrix<double>& height,
                                     vnl_matrix<int>& occupied,
                                     const double& ground_height,
                                     vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec)
{
  vul_printf (vcl_cout, "vis_lidar_labeled_data(): image size %dx%d.\n",
              labels.ni(), labels.nj());
  SoSeparator* root = new SoSeparator;

  //Remove the bordering pixel (r,g,b) of the labels.
  for (unsigned int j=0; j<labels.nj(); j++) {
    labels(0, j) = 0;
    labels(labels.ni()-1, j) = 0;
  }
  for (unsigned int i=0; i<labels.ni(); i++) {
    labels(i, 0) = 0;
    labels(i, labels.nj()-1) = 0;
  }

  //Draw the buildings.
  root->addChild (draw_buildings (labels, height, labels_colored, colors, 
                                  use_labels_colored, ground_height, 
                                  M_vec));

  ///dbmsh3d_save_ply2 (M, "bld.ply2");

  //Draw the vegetation.
  ///root->addChild (draw_vegetation (img_view_label, img_view_max, ground_height));
  ///root->addChild (draw_vegetation_pixel (labels, height, labels_colored, ground_height));

  return root;
}
                                    
double estimate_ground_height (vil_image_view<int>& labels, 
                               vnl_matrix<double>& height)
{  
  //Estimate the ground height and draw the ground plane.
  vcl_vector<double> gnd_height;
  for (unsigned int j=0; j<labels.nj(); j++)
    for (unsigned int i=0; i<labels.ni(); i++)
      if (labels(i,j) == 0) {
        double h = height(i, j);
          gnd_height.push_back (h);
      }

  //Estimate the median ground height.
  assert (gnd_height.size() != 0);
  vcl_nth_element (gnd_height.begin(),
                   gnd_height.begin() + int(gnd_height.size()/2), 
                   gnd_height.end());
  double ground_height = *(gnd_height.begin() + int(gnd_height.size()/2));
  return ground_height;
}

//: if use_labels_colored == false,
//  draw each building using the average color.

SoSeparator* draw_buildings (vil_image_view<int>& labels, 
                             vnl_matrix<double>& height,
                             vil_image_view<unsigned char>& labels_colored,
                             vil_image_view<unsigned char>& colors,
                             const bool use_labels_colored,
                             const double ground_height,
                             vcl_vector<dbmsh3d_textured_mesh_mc*>& M_vec)
{
  SoSeparator* root = new SoSeparator;

  //Put the label of buildings to bld_label (gray-scale image).
  //Also put all labels to a set.
  vil_image_view<int> bld_label_img (labels.ni(), labels.nj());
  bld_label_img.fill (0);
  vcl_set<int> bld_label_set;

  //Go through the labeled image and generate a building labeling image.
  for (unsigned int j=0; j<labels.nj(); j++)
    for (unsigned int i=0; i<labels.ni(); i++) {
      if (labels(i,j) > 0) { //building: red != 0
        int l = labels(i,j);
        bld_label_img(i, j) = l;
        //add to set.
        if (bld_label_set.find(l) == bld_label_set.end())
          bld_label_set.insert (l);
      }
    }    
  vul_printf (vcl_cout, "  building labeling image generated.\n");

  vil_structuring_element disk;
  disk.set_to_disk(1.5);

  //Go through each building label and generate a binary image for contour tracing.
  vcl_set<int>::iterator it = bld_label_set.begin(); 
  int count = 0;
  vul_printf (vcl_cout, "    drawing building: ");
  for (; it != bld_label_set.end(); it++, count++) {
    vul_printf (vcl_cout, "%d ", count);
    int label = *it;
    vil_image_view<bool> bld_bin (labels.ni(), labels.nj());
    bld_bin.fill (false);

    //Create building binary image.
    //Also estimate the average building height from lidar_image_max.
    vcl_vector<double> bld_height;

    //get building color from the first non-zero pixel.
    SbColor color (0, 0, 0);

    double r, g, b;
    for (unsigned int j=0; j<labels.nj(); j++)
      for (unsigned int i=0; i<labels.ni(); i++)
        if (labels(i,j) == label) {
          bld_bin(i,j) = true;
          double h = height(i, j);
          bld_height.push_back (h);

          if (color == SbColor (0,0,0)) {
            r = labels_colored(i, j, 0);
            g = labels_colored(i, j, 1);
            b = labels_colored(i, j, 2);
            color = SbColor (float(r/255), float(g/255), float(b/255));
          }
        }

    //run morphological closing on bld_bin.
    vil_image_view<bool> bld_bin_closing (labels.ni(), labels.nj());
    vil_binary_closing (bld_bin, bld_bin_closing, disk);

    //save bld_bin to debug.
    save_bw_image (bld_bin_closing, "bld_closing.png");

    //estimate an average color for this building
    if (use_labels_colored == false) {
      r = 0;
      g = 0;
      b = 0;
      int count = 0;
      for (unsigned int j=0; j<bld_bin.nj(); j++)
        for (unsigned int i=0; i<bld_bin.ni(); i++)
          if (bld_bin(i,j)) {
            r += colors(i,j,0);
            g += colors(i,j,1);
            b += colors(i,j,2);
            count++;
          }
      r /= count;
      g /= count;
      b /= count;
      color = SbColor (float(r/255), float(g/255), float(b/255));
    }

    //Contour tracing this building.    
    //invoke the tracer
    float sigma=0, beta=0;
    int nsteps=0, threshold=0;
    int intensity_threshold = 127;

    dbdet_contour_tracer ctracer;
    ctracer.set_sigma(sigma);
    ctracer.set_curvature_smooth_nsteps(nsteps);
    ctracer.set_curvature_smooth_beta(beta);
    ctracer.trace(bld_bin_closing); //bld_bin

    //Get the largest contour points.
    ///vcl_vector<vsol_point_2d_sptr> poly_points = ctracer.largest_contour();

    //Go through all contours which is not too small.
    for (unsigned int c=0; c<ctracer.contours().size(); c++) {
      vcl_vector<vsol_point_2d_sptr> poly_points_0 = ctracer.contours()[c];

      if (poly_points_0.size() < 10)
        continue;

      //line fitting.
      vcl_vector<vsol_point_2d_sptr> poly_points; 
      fit_lines_to_contour (poly_points_0, 0.05, poly_points);

      if (poly_points.size() < 3) //10)
        continue;

      //Debug: save this con file
      ///if (label == 15)
        ///dbsol_save_con_file("bld-15.con", poly_points, true);

      //perturb a little (to remove the degenerate problem in visualization).
      for (unsigned i=0; i<poly_points.size(); i++) {
        double x = poly_points[i]->x();
        double y = poly_points[i]->y();              
        double random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_x (x + random * 0.001);
        random = (double)rand() / (double)RAND_MAX - 0.5;
        poly_points[i]->set_y (y + random * 0.001);
      }
      
      //Estimate the median building height.
      assert (bld_height.size() != 0);
      vcl_nth_element (bld_height.begin(),
                       bld_height.begin() + int(bld_height.size()/2), 
                       bld_height.end());
      double median_height = *(bld_height.begin() + int(bld_height.size()/2));

      //mesh the building using polygon points and height.
      SoSeparator* bld_vis = new SoSeparator;    
      root->addChild (bld_vis);
      bld_vis->addChild (draw_filled_polygon (poly_points, float(median_height), color, 0.0f));

      dbmsh3d_textured_mesh_mc* M = new dbmsh3d_textured_mesh_mc;
      M_vec.push_back (M);

      //Add vertices of this building into M.
      add_building_faces (M, poly_points, median_height, ground_height);

      //draw each vertical face of the building from median_height to the ground_height.
      SbVec3f* face = new SbVec3f[4];
      for (unsigned int i=0; i<poly_points.size(); i++) {
        int j = (i+1) % poly_points.size();
        face[0].setValue (poly_points[i]->x(), poly_points[i]->y(), median_height);
        face[1].setValue (poly_points[j]->x(), poly_points[j]->y(), median_height);
        face[2].setValue (poly_points[j]->x(), poly_points[j]->y(), ground_height);
        face[3].setValue (poly_points[i]->x(), poly_points[i]->y(), ground_height);
        bld_vis->addChild (draw_filled_polygon (face, 4, color, 0));
      }
      delete face;
    }
    bld_height.clear();
  }
  vul_printf (vcl_cout, "\n %d building meshes created.\n", M_vec.size());

  return root;
}


double median_3x3 (vnl_matrix<double>& height, const int ni, const int nj, const int i, const int j)
{
  vcl_vector<double> value;
  double v;
  if (i>0) {
    if (j>0)
      _check_add_v (height(i-1, j-1), value);
    _check_add_v (height(i-1,j), value);
    if (j<nj-1)
      _check_add_v (height(i-1,j+1), value);
  }

  if (j>0)
    _check_add_v (height(i, j-1), value);
  _check_add_v (height(i,j), value);
  if (j<nj-1)
    _check_add_v (height(i,j+1), value);

  if (i<ni-1) {
    if (j>0)
      _check_add_v (height(i+1, j-1), value);
    _check_add_v (height(i+1,j), value);
    if (j<nj-1)
      _check_add_v (height(i+1,j+1), value);
  }

  //Estimate the median ground height.
  if (value.size() == 0)
    return -1;
  vcl_nth_element (value.begin(),
                   value.begin() + int(value.size()/2), 
                   value.end());
  double median = *(value.begin() + int(value.size()/2));
  return median;
}

//Add vertices of this building into M.
void add_building_faces (dbmsh3d_textured_mesh_mc* M,
                         const vcl_vector<vsol_point_2d_sptr>& poly_points, 
                         const double median_height, const double ground_height)
{
  //add the top face.
  dbmsh3d_textured_face_mc* Ft = M->_new_face ();
  M->_add_face (Ft);

  //add vertices of the top face.
  for (unsigned int i=0; i<poly_points.size(); i++) {
    double x = poly_points[i]->x();
    double y = poly_points[i]->y();
    double z = median_height;

    dbmsh3d_vertex* V = M->_new_vertex ();
    V->set_pt (x, y, z);
    M->_add_vertex (V);
    
    vgl_point_2d<double> tex_coord (x, y);
    Ft->_add_vertex (V, tex_coord);
  }

  //add the bottom face.
  dbmsh3d_textured_face_mc* Fb = M->_new_face ();
  M->_add_face (Fb);

  //add vertices of the bottom face.
  for (unsigned int i=0; i<poly_points.size(); i++) {
    double x = poly_points[i]->x();
    double y = poly_points[i]->y();
    double z = ground_height;

    dbmsh3d_vertex* V = M->_new_vertex ();
    V->set_pt (x, y, z);
    M->_add_vertex (V);
    vgl_point_2d<double> tex_coord (x, y);
    Fb->_add_vertex (V, tex_coord);
  }

  //add the side faces.
  for (unsigned int i=0; i<poly_points.size(); i++) {
    int j = (i+1) % poly_points.size();

    dbmsh3d_textured_face_mc* Fs = M->_new_face ();
    M->_add_face (Fs);
    Fs->_add_vertex (Ft->vertices(i));
    Fs->_add_vertex (Ft->vertices(j));
    Fs->_add_vertex (Fb->vertices(j));
    Fs->_add_vertex (Fb->vertices(i));
  }
}

SoSeparator* draw_vegetation_pixel (vil_image_view<int>& labels,
                                    vnl_matrix<double>& height,
                                    vil_image_view<unsigned char>& labels_colored,
                                    const double ground_height)
{
  SoSeparator* root = new SoSeparator;

  float shift_x = 0;
  float shift_y = 0;
  float shift_z = 0;
  for (unsigned int j=0; j<labels.nj(); j++)
    for (unsigned int i=0; i<labels.ni(); i++)
      if (labels(i,j) < 0) { //vegetation: green != 0
        double r = labels_colored(i, j, 0);
        double g = labels_colored(i, j, 1);
        double b = labels_colored(i, j, 2);
        float x = i;
        float y = j;

        //Median filtering the pixel height..
        double median_height = median_3x3 (height, labels.ni(), labels.nj(), i, j);
        if (median_height == -1)
          continue;

        ///SbColor color = SbColor (float(r/255), float(g/255), float(b/255));
        lidar_pixel p (labels(i,j), median_height, float(r/255), float(g/255), float(b/255));
        root->addChild (draw_range_pixel_cube (&p, shift_x + x, shift_y + y, shift_z, ground_height));
      }  

  return root;
}


bool texturemap_meshes(vcl_vector<dbmsh3d_textured_mesh_mc*>& meshes, vcl_vector<vil_image_view<vxl_byte> > const& images, vcl_vector<vpgl_proj_camera<double> > const& cameras, vcl_string texture_image_dir)
{
  // based on world_model/texture_map_generator::generate_texture_map(obj_observable* obj, vcl_string texture_filename, bgeo_lvcs lvcs)


  for(unsigned mesh_idx = 0; mesh_idx < meshes.size(); mesh_idx++) {
    // each mesh gets its own texture file
    vcl_stringstream texture_filename_stream;
    texture_filename_stream << texture_image_dir << "/texmap_" << vcl_setw(5) << vcl_setfill('0') << mesh_idx << ".jpg";
    vcl_string texture_filename = texture_filename_stream.str();

    if (images.size() == 0) {
      vcl_cerr << "Error: Cannot create texture map, zero observers!\n";
      return false;
    }
    if (images.size() != cameras.size()) {
      vcl_cerr << "Error: image and camera vectors have different sizes.\n";
      return false;
    }



    // initialize the bounding boxes and the image sizes
    vcl_vector<vgl_point_2d<int> > img_sizes;
    vcl_vector<vsol_box_2d> bounding_box;

    for (unsigned cam_idx = 0; cam_idx < cameras.size(); cam_idx++) {
      vgl_point_2d<int> img_size(images[cam_idx].ni(),images[cam_idx].nj());
      img_sizes.push_back(img_size);
      vsol_box_2d box;
      bounding_box.push_back(box);
    }

    dbmsh3d_textured_mesh_mc* mesh = meshes[mesh_idx];

    // go through these steps so we end up with an IFS mesh that has oriented face normals.
    // not sure if there is a more efficient way of doing this. -DEC
    ///mesh->build_IFS_mesh();
    ///mesh->IFS_to_MHE();
    mesh->orient_face_normals();

    vcl_string file = vul_sprintf("bld_.ply2");
    dbmsh3d_save_ply2 (mesh, ("fixed1_" + file).c_str());

    mesh->build_IFS_mesh();
    
    file = vul_sprintf("bld_.ply2");
    dbmsh3d_save_ply2 (mesh, ("fixed2_" + file).c_str());

    // find best camera for each mesh face
    vcl_map<int, int> best_face_observer_idx;

    // project all mesh vertices with each camera
    vcl_map<int, dbmsh3d_vertex*> mesh_verts = mesh->vertexmap();
    vcl_vector<vcl_map<int, vgl_point_2d<double> > > vert_projections;

    for (unsigned cam_idx = 0; cam_idx < cameras.size(); cam_idx++) {
      vcl_map<int, vgl_point_2d<double> > camera_vert_projections;

      vcl_map<int, dbmsh3d_vertex*>::iterator vit;
      for (vit = mesh_verts.begin(); vit!=mesh_verts.end(); vit++) {
        dbmsh3d_vertex* vert = (dbmsh3d_vertex*)vit->second;
        vgl_point_3d<double> world_pt = vert->pt();
        double u,v;
        cameras[cam_idx].project(world_pt.x(), world_pt.y(), world_pt.z(), u, v);
        vgl_point_2d<double> image_pt(u,v);
        camera_vert_projections[vert->id()] = image_pt;
      } //for each vertex
      vert_projections.push_back(camera_vert_projections);
    }// for each observer

    // find best observer for each face
    vcl_map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit != mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* tex_face = (dbmsh3d_textured_face_mc*)fit->second;
      tex_face->set_tex_map_uri(texture_filename);

      vcl_vector<dbmsh3d_vertex*> face_vertices; ///tex_face->vertices();
      tex_face->_get_bnd_Vs_MHE (face_vertices);

      int best_observer_idx = -1;
      double best_observer_score = 0.0;

      for (unsigned cam_idx = 0; cam_idx < cameras.size(); cam_idx++) {

        // test each vertex for visibility, assume face visibility is AND of all tests
        bool is_visible = true;
        for (unsigned j=0; j < face_vertices.size(); j++){
          vgl_point_2d<double> vert = vert_projections[cam_idx][face_vertices[j]->id()];
          // make sure vertex projection is within image
          if ( (vert.x() < 0) || (vert.x() >= img_sizes[cam_idx].x()) ||
            (vert.y() < 0) || (vert.y() >= img_sizes[cam_idx].y()) ) {
              is_visible = false;
              break;
          }
        }
        //
        // TODO: check if face is occluded
        //
        if (is_visible) {
          // compute normal in lvcs coordinates!
          //vgl_vector_3d<double> face_normal = compute_face_normal_lvcs(tex_face,lvcs);

          vgl_vector_3d<double> face_normal = compute_normal_ifs(face_vertices);
          //vgl_vector_3d<double> face_normal_phe = tex_face->compute_normal();

          //vcl_cout << "face normal     = " << face_normal << vcl_endl;
          //vcl_cout << "face normal phe = " << face_normal_phe << vcl_endl;

          face_normal = face_normal / face_normal.length(); // not gauranteed to be normalized
          //vgl_vector_3d<double> camera_direction = observers_[obs_idx]->camera_direction_rational(lvcs);
          vgl_homg_point_3d<double> cam_center_homg = cameras[cam_idx].camera_center();

          vgl_point_3d<double> cam_center(cam_center_homg);
          //vul_printf (vcl_cout, "cam %d center: (%lf, %lf, %lf).\n", 
                      //cam_idx, cam_center.x(), cam_center.y(), cam_center.z());

          vgl_point_3d<double> c = tex_face->compute_center_pt();
            //vul_printf (vcl_cout, "face %d center: (%lf, %lf, %lf).\n", 
                      //tex_face->id(), c.x(), c.y(), c.z());

          vgl_vector_3d<double> camera_direction = (cam_center - c);
          
          // normalize
          camera_direction = camera_direction / camera_direction.length();
          //vul_printf (vcl_cout, "cam %d dir: (%lf, %lf, %lf).\n", 
                      //cam_idx, camera_direction.x(), camera_direction.y(), camera_direction.z());

          // just use angle for now, maybe incorporate distance to camera later?
          double score = dot_product(face_normal,camera_direction);

          if (score > best_observer_score) {
            best_observer_score = score;
            best_observer_idx = cam_idx;
          }
        }
      }

      if (best_observer_score > 0) {
        vcl_cout << " Face " << tex_face->id() << "  visible. " << vcl_endl;
        for (unsigned j=0; j < face_vertices.size(); j++){
          dbmsh3d_vertex* face_vert = (dbmsh3d_vertex*)face_vertices[j];
          vgl_point_2d<double> vert_proj = vert_projections[best_observer_idx][face_vert->id()];
          // update bounding box of mesh
          bounding_box[best_observer_idx].add_point(vert_proj.x(),vert_proj.y());
          best_face_observer_idx[tex_face->id()] = best_observer_idx;
        }
      }
      else {
        // use -1 to indicate face not visible from any observer
        vcl_cout << "Face " << tex_face->id() << " not visible from any observer!" << vcl_endl;
        best_face_observer_idx[tex_face->id()] = -1;
      }

    } // for each face

    // determine crop region for each observers image
    vcl_vector<vgl_point_2d<int> > crop_points;
    vcl_vector<vgl_point_2d<int> > crop_sizes;

    // cropped images will be stacked horizontally, so
    // calculate max height for image and x offsets for each region
    int tex_width = 0, tex_height = 0;
    vcl_vector<int> x_offsets;
    x_offsets.push_back(0); // for beginning of first image

    for (unsigned cam_idx = 0; cam_idx < cameras.size(); cam_idx++) {
      if (!bounding_box[cam_idx].empty()) {
        int min_x = vcl_floor(bounding_box[cam_idx].get_min_x());
        int max_x = vcl_ceil(bounding_box[cam_idx].get_max_x());
        int min_y = vcl_floor(bounding_box[cam_idx].get_min_y());
        int max_y = vcl_ceil(bounding_box[cam_idx].get_max_y());
        vgl_point_2d<int> crop_point(min_x,min_y);
        vgl_point_2d<int> crop_size(max_x - min_x, max_y - min_y);

        if (crop_size.y() > tex_height) {
          tex_height = crop_size.y();
        }
        x_offsets.push_back(x_offsets.back() + crop_size.x());

        crop_points.push_back(crop_point);
        crop_sizes.push_back(crop_size);
      }
      else {
        x_offsets.push_back(x_offsets.back());

        vgl_point_2d<int> zero(0,0);
        crop_points.push_back(zero);
        crop_sizes.push_back(zero);
      }
    }
    tex_width = x_offsets.back();

    // now loop through each face's tex coords and normalize 
    for (fit = mesh->facemap().begin(); fit != mesh->facemap().end(); fit++) {

      dbmsh3d_textured_face_mc* tex_face = (dbmsh3d_textured_face_mc*)fit->second;
      vcl_vector<dbmsh3d_vertex*> face_vertices = tex_face->vertices();
      int best_obs = best_face_observer_idx[tex_face->id()];

      if (best_obs >= 0) {
        for (unsigned v=0; v<face_vertices.size(); v++) {
          vgl_point_2d<double> vert_proj = vert_projections[best_obs][face_vertices[v]->id()];
          double x = (vert_proj.x() - crop_points[best_obs].x() + x_offsets[best_obs]) / (double)tex_width;
          double y = 1 - ((vert_proj.y() - crop_points[best_obs].y()) / (double)tex_height);

          vgl_point_2d<double> vert_proj_norm(x,y);

          // add coord to texture coordinate list
          tex_face->set_tex_coord(face_vertices[v]->id(), vert_proj_norm);
        }
      }
      else {
        // set all texture coordinates for this face to (0,0)
        for (unsigned v=0; v<face_vertices.size(); v++) {
          vgl_point_2d<double> vert_proj(0.0,0.0);
          tex_face->set_tex_coord(face_vertices[v]->id(), vert_proj);
        }
      }
    }

    // crop out bounding box from original images and copy to the texture image
    //vil_image_view_base_sptr tex_map_view = vil_crop(img_orig,bounding_box.get_min_x(),bounding_box.width(),bounding_box.get_min_y(),bounding_box.height());
    //Make a one pixel image!!
    if (tex_width==0)
      tex_width = 1;
    if (tex_height==0)
      tex_height = 1;
      

    vil_image_view<vxl_byte> tex_map_view(tex_width,tex_height,3);
    for (unsigned cam_idx = 0; cam_idx < cameras.size(); cam_idx++) {
      if ( (crop_sizes[cam_idx].x() > 0) && (crop_sizes[cam_idx].y() > 0) ) {
        //vgui_image_tableau_sptr img_tab = observers_[obs_idx]->get_image_tableau();
        //vil_image_resource_sptr img_orig_res = img_tab->get_image_resource();

        //if (img_orig_res->pixel_format() != VIL_PIXEL_FORMAT_BYTE) {
        //  vcl_cerr << "texture_map generator: unsupported image type "<<img_orig_res->pixel_format()<<vcl_endl;
        //  continue;
        // }
        vil_image_view<vxl_byte> cropped_view = vil_crop(images[cam_idx],crop_points[cam_idx].x(),crop_sizes[cam_idx].x(),
          crop_points[cam_idx].y(),crop_sizes[cam_idx].y());
        // vil_image_resource_sptr img_orig_cropped = vil_crop(img_orig_res,
        //                                                 crop_points[obs_idx].x(),crop_sizes[obs_idx].x(),
        //                                                 crop_points[obs_idx].y(),crop_sizes[obs_idx].y());


        //vil_image_view<vxl_byte> cropped_view = img_orig_cropped->get_view();
        vcl_cout << "cropped_view nplanes = "<<cropped_view.nplanes()<<vcl_endl;
        vcl_cout << "tex_map_view nplanes = "<<tex_map_view.nplanes()<<vcl_endl;
        vil_copy_to_window(cropped_view,tex_map_view,x_offsets[cam_idx],0);
      }
    }
    vil_save(tex_map_view,texture_filename.c_str());
  }

  return true;
}



bool save_mesh_vrml(vcl_string filename, vcl_vector<dbmsh3d_textured_mesh_mc*> &meshes, vgl_point_3d<double> virtual_camera_center)
{

  FILE* fp;
  if ((fp = vcl_fopen(filename.c_str(), "w")) == NULL) {
    vcl_fprintf (stderr, "Can't open vrml file %s to write.\n", filename.c_str());
    return false; 
  }

  vcl_fprintf(fp, "#VRML V2.0 utf8\n");
  //vcl_fprintf(fp, "PROFILE Immersive\n\n");

  vcl_fprintf(fp,"NavigationInfo {\n Headlight TRUE \n} \n");
  vcl_fprintf(fp,"Transform {\n  translation %f %f %f \n children [\n",-virtual_camera_center.x(),-virtual_camera_center.y(),-virtual_camera_center.z());

  vcl_vector<dbmsh3d_textured_mesh_mc*>::iterator it;
  int obj_count = 0;
  for (it = meshes.begin(); it != meshes.end(); it++, obj_count++) {

    // assume object is texture mapped
    dbmsh3d_textured_mesh_mc* mesh = (*it);

    // texture image is actually defined per face in the dbmsh3d structure, but we will 
    // assume that it is the same for every face of the mesh here.
    dbmsh3d_textured_face_mc* first_face = (dbmsh3d_textured_face_mc*)mesh->facemap().begin()->second;
    // just want filename, not full path
    vcl_string texmap_url = vul_file::strip_directory(first_face->tex_map_uri());
  
    vcl_fprintf(fp, "Transform {\n");
    vcl_fprintf(fp, "  children\n");
    vcl_fprintf(fp, "  Shape {\n");
    vcl_fprintf(fp, "    appearance Appearance {\n");
    vcl_fprintf(fp, "      material Material{}\n");
    vcl_fprintf(fp, "      texture ImageTexture {\n");
    vcl_fprintf(fp, "        url \"%s\"\n",texmap_url.c_str());
    vcl_fprintf(fp, "      }\n");
    vcl_fprintf(fp, "    }\n");
    vcl_fprintf(fp, "    geometry IndexedFaceSet {\n");
    vcl_fprintf(fp, "      coord Coordinate {\n");
    vcl_fprintf(fp, "        point [\n");

    // map vertex ID's to indices.
    vcl_map<int,int> vert_indices;

    vcl_map<int, dbmsh3d_vertex*>::iterator vit;
    int idx = 0;
    for (vit = mesh->vertexmap().begin(); vit != mesh->vertexmap().end(); vit++, idx++) {
      dbmsh3d_vertex* v = (dbmsh3d_vertex*)vit->second;
      vert_indices[v->id()] = idx;
      vgl_point_3d<double> vert_pt = v->pt();
      //lvcs_->global_to_local(v->pt().x(),v->pt().y(),v->pt().z(),bgeo_lvcs::wgs84,x,y,z,bgeo_lvcs::DEG,bgeo_lvcs::METERS);
      vcl_fprintf(fp,"       %0.8f %0.8f %0.8f,\n",vert_pt.x(), vert_pt.y(), vert_pt.z());
    }
    vcl_fprintf(fp, "        ]\n");
    vcl_fprintf(fp, "      }\n");
    vcl_fprintf(fp, "      coordIndex[\n");
    
    vcl_map<int, dbmsh3d_face*>::iterator fit;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      vcl_fprintf(fp, "             ");
      face->_ifs_track_ordered_vertices ();
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        vcl_fprintf( fp, "%d ",vert_indices[v->id()]);
      }
      vcl_fprintf(fp, "-1,\n");
    }
    vcl_fprintf(fp, "      ]\n\n");

    vcl_fprintf(fp, "      texCoord TextureCoordinate {\n");
    vcl_fprintf(fp, "        point [\n");

    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      for (unsigned j=0; j<face->vertices().size(); j++) {
        dbmsh3d_vertex* v = (dbmsh3d_vertex*) face->vertices(j);
        vgl_point_2d<double> pt = face->tex_coords(v->id());
        vcl_fprintf(fp, "           %0.8f %0.8f,\n",pt.x(),pt.y());
      }
    }
    vcl_fprintf(fp, "        ]\n");
    vcl_fprintf(fp, "      }\n\n");  

    vcl_fprintf(fp, "      texCoordIndex[\n");
    int tex_coord_idx = 0;
    for (fit = mesh->facemap().begin(); fit!= mesh->facemap().end(); fit++) {
      dbmsh3d_textured_face_mc* face = (dbmsh3d_textured_face_mc*)fit->second;
      vcl_fprintf(fp, "                ");    
      for (unsigned j=0; j < face->vertices().size(); j++) {
        vcl_fprintf(fp, "%d ",tex_coord_idx++);
      }
      vcl_fprintf(fp, "-1,\n");
    }
    vcl_fprintf(fp, "      ]\n\n");
    vcl_fprintf(fp, "      solid TRUE\n");
    vcl_fprintf(fp, "      convex FALSE\n");
    vcl_fprintf(fp, "      creaseAngle 0\n");
    vcl_fprintf(fp, "    }\n");
    vcl_fprintf(fp, "  }\n");
    vcl_fprintf(fp, "}\n\n\n");
  }

  vcl_fprintf(fp,"\n]}\n");

  vcl_fclose(fp);
  return true;




}




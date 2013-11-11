#include <vgl/vgl_point_2d.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include "bwm_lidar_algo.h"

//###################################################################

//Convert labelled lidar images to mesh.
dbmsh3d_mesh* lidar_to_mesh (vil_image_view<vxl_byte>& img1, 
                             vil_image_view<vxl_byte>& img2, 
                             vil_image_view<vxl_byte>& label_img)
{  
  dbmsh3d_mesh* M = new dbmsh3d_mesh;

  //segment the lidar into ground plane, vegitation, and building points.
  unsigned int ni = img1.ni();
  unsigned int nj = img1.nj();
  vil_image_view<bool> gnd (ni,nj,1);
  vil_image_view<bool> veg (ni,nj,1);
  vil_image_view<bool> bld (ni,nj,1);
  convert_label_image (label_img, gnd, veg, bld);

  //determine ground height: as median of all ground pixels in img2
  //gnd_height = median(img2(gnd(:)));
  double gnd_height;
  bool r = median_image_value (img2, gnd_height);
  assert (r);

  //disp('creating ground plane');
  //create a single polygon for the ground plane
  //meshes(1).Vertices = [0 0 gnd_height; size(img1,2) 0 gnd_height; size(img1,2) size(img1,1) gnd_height; 0 size(img1,1) gnd_height];
  //meshes(1).Faces = [1 2 3 4];
  //patch(meshes(1),'faceColor','blue','faceAlpha',0.3);
  dbmsh3d_face* gndF = M->_new_face ();
  M->_add_face (gndF);
  dbmsh3d_vertex* V1 = M->_new_vertex ();
  M->_add_vertex (V1);
  V1->set_pt (0, 0, gnd_height);
  gndF->_ifs_add_bnd_V (V1);
  dbmsh3d_vertex* V2 = M->_new_vertex ();
  gndF->_ifs_add_bnd_V (V2);
  V2->set_pt (img1.ni(), 0, gnd_height);
  M->_add_vertex (V2);
  dbmsh3d_vertex* V3 = M->_new_vertex ();
  M->_add_vertex (V3);
  V3->set_pt (img1.ni(), img1.nj(), gnd_height);
  gndF->_ifs_add_bnd_V (V3);
  dbmsh3d_vertex* V4 = M->_new_vertex ();
  M->_add_vertex (V4);
  V4->set_pt (0, img1.nj(), gnd_height);
  gndF->_ifs_add_bnd_V (V4);  

  //Save mesh of ground plane as 'ground.ply')
  dbmsh3d_save_ply2 (M, "ground.ply2");

  //disp('extracting horizontal surfaces');
  vcl_vector<vcl_vector<vgl_point_2d<double> > > polys;
  vcl_vector<double> heights;
  vil_image_view<int> plane_mask (bld.ni(), bld.nj(), 1);
  extract_horizontal_polygons (img1, bld, polys, heights, plane_mask);

  //lidar_simp(plane_mask > 0) = heights(plane_mask(plane_mask > 0));


  //create an extruded mesh for each polygon
  for (unsigned int i=0; i<polys.size(); i++) {
    //Create IFS mesh for each building.
    /*nverts = size(polys{i},2);
    poly_top = [polys{i}(1,:); polys{i}(2,:); ones(1,nverts)*heights(i)];
    poly_bot = [polys{i}(1,:); polys{i}(2,:); ones(1,nverts)*gnd_height];
    verts = [poly_top'; poly_bot'];
    
    faces = ones(nverts+2,max(4,nverts))*NaN;
    
    faces(1,1:nverts) = [1:nverts];
    faces(2,1:nverts) = [nverts+1:2*nverts];
    for f=1:nverts-1
        faces(f+2,1:4) = [f f+1 f+1+nverts f+nverts];
    end
    faces(nverts+2,1:4) = [nverts 1 1+nverts 2*nverts];
    
    mesh.Vertices = verts;
    mesh.Faces = faces;
    patch(mesh,'faceColor','red','faceAlpha',0.75);
    meshes(end+1) = mesh;*/

    //draw the polyline as a closed mesh face at the heights[i].
    dbmsh3d_face* bldF = M->_new_face ();
    M->_add_face (bldF);
    dbmsh3d_face* botF = M->_new_face ();
    M->_add_face (botF);

    //Go through each polyline vertex and add to bldF.
    for (unsigned int j=0; j<polys[i].size(); j++) {
      dbmsh3d_vertex* V = M->_new_vertex ();
      M->_add_vertex (V);
      V->set_pt (polys[i][j].x(), polys[i][j].y(), heights[i]);
      bldF->_ifs_add_bnd_V (V);

      dbmsh3d_vertex* Vb = M->_new_vertex ();
      M->_add_vertex (Vb);
      Vb->set_pt (polys[i][j].x(), polys[i][j].y(), gnd_height);
      botF->_ifs_add_bnd_V (Vb);
    }

    //Go through each polyline edge and add a mesh face as the side of the building.
    for (unsigned int j=0; j<polys[i].size()-1; j++) {
      dbmsh3d_face* sideF = M->_new_face ();
      M->_add_face (sideF);
      V1 = bldF->vertices(j);
      V2 = bldF->vertices(j+1);
      V3 = botF->vertices(j);
      V4 = botF->vertices(j+1);
      sideF->_ifs_add_bnd_V (V1);
      sideF->_ifs_add_bnd_V (V2);
      sideF->_ifs_add_bnd_V (V4);
      sideF->_ifs_add_bnd_V (V3);
    }
    
  }

  //option to delete the ground plane from M
  M->remove_face (0);

  //Save mesh of ground plane as 'ground.ply')
  dbmsh3d_save_ply2 (M, "buildings.ply2");

  //disp('tesselating remaining lidar points');
  // tesselate lidar points for vegetation
  /*blob_mask = ~(gnd | (plane_mask > 0));
  lidar_simp(blob_mask) = img1(blob_mask);
  % dilate mask to ensure no gaps between blobs and ground/buildings
  blob_mask = imdilate(blob_mask,ones(5));
  [verts,faces] = triangulate_blob(img1,blob_mask);
  veg_mesh.Vertices = verts;
  veg_mesh.Faces = faces;
  meshes(end+1) = veg_mesh;
  patch(veg_mesh,'faceColor','green','faceAlpha',0.5,'edgeColor','none');
  axis equal*/

  return M;
}

//segment the lidar into ground plane, vegitation, and building points.
void convert_label_image (vil_image_view<vxl_byte>& label_img, 
                          vil_image_view<bool>& gnd, 
                          vil_image_view<bool>& veg, 
                          vil_image_view<bool>& bld)
{
  
  //gnd = rgb2gray(label_img) == 0;
  for (unsigned int j=0; j<label_img.nj(); j++)
    for (unsigned int i=0; i<label_img.ni(); i++)
      gnd (i,j,0) = label_img(i,j,0) == 0;

  //the reddish pixels are the buildings
  //bld = label_img(:,:,1) ~= 0;
  for (unsigned int j=0; j<label_img.nj(); j++)
    for (unsigned int i=0; i<label_img.ni(); i++)
      bld (i,j,0) = label_img(i,j,0) != 0; //red


  //the greenish pixels are the vegetation regions
  //veg = label_img(:,:,2) ~= 0;
  for (unsigned int j=0; j<label_img.nj(); j++)
    for (unsigned int i=0; i<label_img.ni(); i++)
      veg (i,j,0) = label_img(i,j,1) != 0; //green

  //make sure no building blobs touch the edges of the image.
  //bld(:,1:2) = 0;
  //bld(:,end-1:end) = 0;
  //bld(1:2,:) = 0;
  //bld(end-1:end,:) = 0;
  for (unsigned int j=0; j<bld.nj(); j++) {
    bld (0,j,0) = 0;
    bld (1,j,0) = 0;
    bld (bld.ni()-1,j,0) = 0;
    bld (bld.ni()-2,j,0) = 0;
  }
  for (unsigned int i=0; i<bld.ni(); i++) {
    bld (i,0,0) = 0;
    bld (i,1,0) = 0;
    bld (i,bld.nj()-1,0) = 0;
    bld (i,bld.nj()-2,0) = 0;
  }
}

bool median_image_value (vil_image_view<vxl_byte>& img, double& median)
{
  int np = img.nplanes();
  vcl_vector<double> tmp;
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++) {
      if (img(i,j,0)) {
        double pixel = 0;
        for (unsigned int k=0; k<np; k++)
          pixel += img(i,j,k);
        pixel /= np;
        tmp.push_back (pixel);
      }
    }
  if (tmp.size() == 0)
    return false;

  vcl_nth_element (tmp.begin(),
                   tmp.begin() + int(tmp.size()/2), 
                   tmp.end());
  median = *(tmp.begin() + int(tmp.size()/2));
  tmp.clear();
  return true;
}

// disp('extracting horizontal surfaces');
//    polys: vector<vgl_point_2d<double>>
//    heights: vector<double>
//    plane_mask: vil_image
void extract_horizontal_polygons (vil_image_view<vxl_byte>& img1, 
                                  vil_image_view<bool>& bld,
                                  vcl_vector<vcl_vector<vgl_point_2d<double> > >& polys,
                                  vcl_vector<double>& plane_heights,
                                  vil_image_view<int>& plane_mask)
{
  //plane_mask = zeros(size(mask));
  for (unsigned int j=0; j<plane_mask.nj(); j++)
    for (unsigned int i=0; i<plane_mask.ni(); i++)
      plane_mask(i,j,0) = 0;

  //connected component labelling on the bld image.
  //need total number of regions and label of each region.
  vil_image_view<int> labeled (bld.ni(), bld.nj(), 1);
  unsigned int nregions = conn_label (bld, labeled);

  //debug: save bld image
  save_bw_image (bld, "bld.png");

  //debug: save the conn_label image to debug.
  save_int_image (labeled, "labeled.png");

  for (int b=1; b<=nregions; b++) {
    vil_image_view<bool> planes (bld.ni(), bld.nj(), 1);
    vcl_vector<double> heights;
    segment_horizontal_planes (img1, labeled, b, planes, heights);

    //Update the plane_mask image.
    //plane_mask(planes > 0) = planes(planes > 0) + max(plane_mask(:));
    int max = 0;
    for (unsigned int j=0; j<plane_mask.nj(); j++)
      for (unsigned int i=0; i<plane_mask.ni(); i++)
        if (plane_mask(i,j,0)>max)
          max = plane_mask(i,j,0);
    for (unsigned int j=0; j<plane_mask.nj(); j++)
      for (unsigned int i=0; i<plane_mask.ni(); i++)
        if (planes(i,j,0) > 0) {
          plane_mask(i,j,0) = planes(i,j,0) + max;
        }
    
    //debug: save image for a specific building.
    save_bw_image (planes, "planes.png");

    for (unsigned int i=0; i<heights.size(); i++) {
      //For each extracted plane, create polygon contour.
      ///polyline_big = extract_region_perimeter(planes == i);
      vcl_vector<vgl_point_2d<double> > poly_big;
      extract_region_perimeter (planes, i, poly_big);

      //push the current reduced polygon contour into the vector of polys[]
      ///polys{end+1} = reduce_verts(polyline_big,1);
      vcl_vector<vgl_point_2d<double> > poly;
      reduce_verts (poly_big, poly);
      polys.push_back (poly);

      //add the current plane_height to height[].
      ///heights(end+1) = plane_heights(i);
      plane_heights.push_back (heights[i]);
    }
  }
}

//return total number of regions and label of each region in labeled.
unsigned int conn_label (vil_image_view<bool>&bld, 
                         vil_image_view<int>& labeled) 
{
  unsigned int nregions = 0;
    
  //connected component labelling on the bld image.
  int label = 0;
  for (unsigned int j=0; j<bld.nj(); j++)
    for (unsigned int i=0; i<bld.ni(); i++) {
      if (bld(i,j,0) != 0)
        labeled(i,j,0) = label++;
      else
        labeled(i,j,0) = 0;
    }

  bool change;
  int iter = 0;
  do {
    change = false;
    iter++;
    //top-down pass
    for (unsigned int j=0; j<bld.nj(); j++)
      for (unsigned int i=0; i<bld.ni(); i++) {
        if (labeled(i,j,0) != 0) {
          label = labeled(i,j,0);
          if (i>0) //top
            if (labeled(i-1,j,0) != 0)
              label = vcl_min (label, labeled(i-1,j,0));
          if (j>0) //left
            if (labeled(i,j-1,0) != 0)
              label = vcl_min (labeled(i,j-1,0), label);
          if (i>0 && j>0) //top-left
            if (labeled(i-1,j-1,0) != 0)
              label = vcl_min (labeled(i-1,j-1,0), label);
          if (i>0 && j<bld.nj()-1) //top-right
            if (labeled(i-1,j+1,0) != 0)
              label = vcl_min (labeled(i-1,j+1,0), label);
          if (i<bld.ni()-1 && j>0) //bottom-left
            if (labeled(i+1,j-1,0) != 0)
              label = vcl_min (labeled(i+1,j-1,0), label);

          if (label != labeled(i,j,0)) {
            change = true;
            labeled(i,j,0) = label;
          }
        }
      }

    //bottom-up pass
    for (int j=bld.nj()-1; j>=0; j--)
      for (int i=bld.ni()-1; i>=0; i--) {
        if (labeled(i,j,0) != 0) {
          label = labeled(i,j,0);
          if (i<bld.ni()-1) //bottom
            if (labeled(i+1,j,0) != 0)
              label = vcl_min (labeled(i+1,j,0), label);
          if (j<bld.nj()-1) //right
            if (labeled(i,j+1,0) != 0)
              label = vcl_min (labeled(i,j+1,0), label);
          if (i<bld.ni()-1 && j<bld.nj()-1) //bottom-right
            if (labeled(i+1,j+1,0) != 0)
              label = vcl_min (labeled(i+1,j+1,0), label);
          if (i>0 && j<bld.nj()-1) //top-right
            if (labeled(i-1,j+1,0) != 0)
              label = vcl_min (labeled(i-1,j+1,0), label);
          if (i<bld.ni()-1 && j>0) //bottom-left
            if (labeled(i+1,j-1,0) != 0)
              label = vcl_min (labeled(i+1,j-1,0), label);

          if (label != labeled(i,j,0)) {
            change = true;
            labeled(i,j,0) = label;
          }
        }
      }
  }
  while (change);
  vcl_cout << "total iterations in c-c labelling: " << iter << vcl_endl;

  //need to re-order all nregions into 1, 2, 3, ...
  vcl_set<int> region_labels;
  for (unsigned int j=0; j<labeled.nj(); j++)
    for (unsigned int i=0; i<labeled.ni(); i++) {
      if (labeled(i,j,0) !=0) {
        label = labeled(i,j,0);
        if (region_labels.find(label) == region_labels.end())
          region_labels.insert (label);
      }
    }

  int reset_l = 0;
  
  while (region_labels.size() != 0) {
    vcl_set<int>::iterator it = region_labels.begin();
    int l = *it;
    region_labels.erase (it);
    reset_l++;

    for (unsigned int j=0; j<labeled.nj(); j++)
      for (unsigned int i=0; i<labeled.ni(); i++) {
        if (labeled(i,j,0) == l) {
          //reset the label l to reset_l.
          labeled(i,j,0) = reset_l;
        }
      }
  }

  vcl_cout << "total regions (from label 1, 2, ... to n): " << reset_l << vcl_endl;

  return reset_l;
}

bool save_int_image (vil_image_view<int>& img, vcl_string filename)
{
  vil_image_view<vxl_byte> tmp (img.ni(), img.nj(), 1);
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++) 
      tmp (i, j, 0) = (char) img(i,j,0);
  return vil_save (tmp, filename.c_str());
}

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

//: go through the labelled image for building b.
//  result: 
//   image: planes
//   heights: vector of double
void segment_horizontal_planes (vil_image_view<vxl_byte>& img1,
                                vil_image_view<int>& labeled, 
                                const unsigned int b,
                                vil_image_view<bool>& planes, 
                                vcl_vector<double>& heights)
{
  
  double bin_size = 1.5;
  double max_height_range = 1.5;
  double min_area = 16;

  //lidar: img1
  //mask: for building in labeled[b] == b

  //create a histogram and search for peaks corresponding to horizontal planes
  //lidar_v = lidar(mask(:));
  //bins = [min(lidar_v)-0.01:bin_size:max(lidar_v)+0.01];
  vcl_vector<vxl_byte> lidar_v;
  double min = DBL_MAX;
  double max = 0;
  for (unsigned int j=0; j<img1.nj(); j++)
    for (unsigned int i=0; i<img1.ni(); i++) 
      if (labeled(i,j,0) == b) {
        lidar_v.push_back (img1(i,j,0));
        if (min > img1(i,j,0))
          min = img1(i,j,0);
        if (max < img1(i,j,0))
          max = img1(i,j,0);
      }

  vcl_vector<double> bins;
  for (double d=min-0.01; d<=max+0.01; d+=bin_size) {
    bins.push_back (d);
  }

  //check for the case where just one bin is returned. we need at least two
  //for hist to work properly.
  //if (length(bins) == 1)
    //bins = [(bins - bin_size) bins];
  //end
  if (bins.size() == 1) {
    bins.insert (bins.begin(), bins[0] - bin_size);
  }

  //h = hist(lidar_v,bins);    
  //N = HIST(Y,X), where X is a vector, returns the distribution of Y
  //among bins with centers specified by X.
  vcl_vector<int> h;
  hist (lidar_v, bins, h);

  //[max_val, max_i] = max(h);
  int max_val = 0;
  for (unsigned int i=0; i<h.size(); i++)
    if (h[i] > max_val)
      max_val = h[i];

  //find peaks
  //h_prev = [0 h(1:end-1)];
  vcl_vector<int> h_prev;
  h_prev.push_back (0);
  for (unsigned int i=0; i<h.size()-1; i++)
    h_prev.push_back (h[i]);

  //h_next = [h(2:end) 0];
  vcl_vector<int> h_next;  
  for (unsigned int i=1; i<h.size(); i++)
    h_next.push_back (h[i]);
  h_next.push_back (0);

  //peaks = find((h > h_prev) & (h > h_next));
  //npeaks = length(peaks);
  vcl_vector<int> peaks;
  for (unsigned int i=0; i<h.size(); i++) {
    if (h[i] > h_prev[i] && h[i] > h_next[i])
      peaks.push_back (i);
  }

  //regions = zeros(size(lidar));
  //nregions = 0;
  //regions is the image planes to return.
  vil_image_view<vxl_byte> in_range (planes.ni(), planes.nj(), 1);
  planes.fill (0);
  in_range.fill (0);
  
  for (int i=0; i<peaks.size(); i++) {
    double bin_center = bins[peaks[i]];
    ///in_range = (vcl_fabs(lidar - bin_center) < bin_size/2) & mask;
    //find all pixel value of this building (masked by labeled[b] == b) that is in the bin range to in_range.
    for (unsigned int j=0; j<planes.nj(); j++)
      for (unsigned int i=0; i<planes.ni(); i++) {
        double tmp = double(img1(i,j,0)) - bin_center;
        if (labeled(i,j,0) == b)
          if (vcl_fabs(tmp) < bin_size/2)
            in_range(i,j,0) = img1(i,j,0);
      }

    ///assuming we dont have two planes within a single bin here!
    ///med_height = median(lidar(in_range(:)));
    double med_height;
    bool r = median_image_value (in_range, med_height);
    assert (r);
    
    ///plane = (abs(lidar - med_height) <= max_height_range) & mask;
    for (unsigned int j=0; j<planes.nj(); j++)
      for (unsigned int i=0; i<planes.ni(); i++) {
        double tmp = double(img1(i,j,0)) - med_height;
        if (labeled(i,j,0) == b)
          if (vcl_fabs(tmp) <= max_height_range)
            planes(i,j,0) = true;
      }
    
    //segment in case multiple planes have same height
    //labeled = bwlabel(plane);
    //props = regionprops(labeled,'Area');
    //int nblobs = 0; ///length(props);
    //for (int r=0; r<=nblobs; r++) {
      ///if (props(r).Area > min_area)
        //  nregions = nregions + 1;
        //  regions(labeled == r) = nregions;
        //  heights(nregions) = med_height;
      ///}
    //}

    //Now we are outputing a portion of a building of a particular height.
    //Need to handle the case that such height are shared by several planes (not likely)!!
    //Add the assumed single component of non-zero plane (i,j,0) as a single component to heights[]. 
    heights.push_back (med_height);
  }
}

//: histogram
//  input: value, bins.
//  output: h.
void hist (vcl_vector<vxl_byte>& value, 
           vcl_vector<double>& bins, 
           vcl_vector<int>& h)
{
  const int sz = bins.size(); 
  assert (sz >=2);
  h.resize (sz);
  for (unsigned int i=0; i<h.size(); i++)
    h[i] = 0;

  //Determine threshold values for bins.
  //1) if lidar_v[i] < bin[0]
  //2) if value[i] >= bin[sz-1]
  //3) if bin[j] <= value[i] < bin[j+1]

  vcl_vector<int> th;
  for (int j=0; j<bins.size()-1; j++) {
    double t = (bins[j] + bins[j+1])/2;
    th.push_back (t);
  }

  int count = 0;
  for (unsigned int i=0; i<value.size(); i++) {
    for (unsigned int j=0; j<th.size(); j++) {
      if (value[i] < th[j]) {
        h[j]++;
        count++;
        break;
      }
    }
    if (value[i] >=th[th.size()-1]) {
      h[th.size()]++;
      count++;
    }
  }

  assert (count == value.size());
}

void extract_region_perimeter (vil_image_view<bool>& planes, 
                               const unsigned int i,
                               vcl_vector<vgl_point_2d<double> >& poly_big)
{
  //bw = imdilate(bw,ones(3));  
  vil_structuring_element disk;
  disk.set_to_disk(1.5);
  vcl_cout << "str" << disk << vcl_endl;
  vil_image_view<bool> planes2;
  vil_binary_dilate (planes, planes2, disk);
  
  //debug: save the conn_label image to debug.
  save_bw_image (planes2, "planes2.png");

  //bw = imfill(bw,'holes');
  bw_fill_holes (planes2);

  save_bw_image (planes2, "planes2.png");

  //perim = bwperim(bw);
  //Find perimeter pixels in binary image.
  vil_image_view<bool> perim (planes.ni(), planes.nj(), 1);
  get_bw_perim (planes2, perim);

  //debug: save the perim image to debug.
  save_bw_image (perim, "perim.png");

  
  //: The following part can be replaced with the existing contour tracing code.
  //  Trace the contour into a closed polyline in vgl_point_2d<double>

  //8 neighbors in the 3x3 area.
  //search_r = [-1 0 1  0 -1 -1 1  1];
  int search_i[] = {-1, 0, 1, 0, -1, -1, 1, 1};

  //search_c = [ 0 1 0 -1 -1  1 1 -1];
  int search_j[] = {0, 1, 0, -1, -1, 1, 1, -1};

  //[perim_r,perim_c] = find(perim);
  //Now start to trace the contour and add to the polyline.
  //cur_r = perim_r(1);
  //cur_c = perim_c(1);

  //Find (x,y) coord of the first contour pixel.
  int start_i = -1, start_j = -1;
  for (unsigned int j=0; j<planes.nj(); j++)
    for (unsigned int i=0; i<planes.ni(); i++) {
      if (planes(i,j,0) !=0) {
        start_i = i;
        start_j = j;
        break;
      }
    }
  assert (start_i >= 0);

  //polyline = zeros(2,0);
  poly_big.clear();

  int cur_i = start_i;
  int cur_j = start_j;

  //while (any(perim(:)))
  ///while (non_zero (perim)) {
  while (1) {
    //Add this point (x, y) = (cur_c, cur_r) to the polyline.
    //polyline(:,end+1) = [cur_c cur_r]';
    poly_big.push_back (vgl_point_2d<double> (cur_i, cur_j));

    //Set the current contour pixel value to 0.
    //perim(cur_r,cur_c) = 0;
    perim (cur_i, cur_j, 0) = 0;

    //found_next = logical(0);
    //for i=1:8 //for the 8 neighbors in the 3x3 kernel.
        //If pixel is non-zero (at contour)
        //if (perim(cur_r + search_r(i),cur_c + search_c(i)))

            //cur_r = cur_r + search_r(i);
            //cur_c = cur_c + search_c(i);
            //found_next = 1;
            //break;
        //end
    //end
    //if (~found_next)
        //% end of polyline
        //break;
    //end

    //found next contour pixel until can't found such pixel (finished).
    bool found = false;
    for (int i=0; i<8; i++) {      
      if (perim (cur_i + search_i[i], cur_j + search_j[i], 0)) {        
        cur_i = cur_i + search_i[i];
        cur_j = cur_j + search_j[i];
        found = true;
        break;
      }
    }
    if (found == false) {
      //finish contour tracer
      break;
    }
  }

}

void bw_fill_holes (vil_image_view<bool>& img)
{
  //Assume all boundary points are 0 and propagate to label background pixels to be -1;
  vil_image_view<bool> bg (img.ni(), img.nj(), 1);
  bg.fill (0);
  
  bg(0,0,0) = true;
  bg(bg.ni()-1, bg.nj()-1,0) = true;

  //label the connected component of background.
  bool change;
  int iter = 0;
  do {
    change = false;
    iter++;
    //top-down pass
    for (unsigned int j=0; j<bg.nj(); j++)
      for (unsigned int i=0; i<bg.ni(); i++) {
        if (img(i,j,0) == 0) {
          if (i>0) //top
            if (bg(i-1,j,0) != 0)
              bg(i,j,0) = true;
          if (j>0) //left
            if (bg(i,j-1,0) != 0)
              bg(i,j,0) = true;          

          if (bg(i,j,0))
            change = true;
        }
      }

    //bottom-up pass
    for (int j=bg.nj()-1; j>=0; j--)
      for (int i=bg.ni()-1; i>=0; i--) {
        if (img(i,j,0) == 0) {
          if (i<bg.ni()-1) //bottom
            if (bg(i+1,j,0) != 0)
              bg(i,j,0) = true;
          if (j<bg.nj()-1) //right
            if (bg(i,j+1,0) != 0)
              bg(i,j,0) = true;

          if (bg(i,j,0))
            change = true;
        }
      }
  }
  while (change);
  vcl_cout << "total iterations in background labelling: " << iter << vcl_endl;

  //look for all non-bg pixels and fill it.
  int count = 0;
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++) {
      if (bg(i,j,0)==0) {
        img(i,j,0) = true;
        count++;
      }
    }

  //debug: save image for a specific building.
  save_bw_image (bg, "bg.png");

  vcl_cout << "number of hole pixels filled: " << count << vcl_endl;
}

void get_bw_perim (vil_image_view<bool>& img, vil_image_view<bool>& perim)
{
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++)
      perim(i,j,0) = 0;

  //A pixel is part of the perimeter if it nonzero and it is connected to at least one zero-valued pixel.
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++) {
      if (img(i,j,0) == 0)
        continue;
      if (i>0 && img(i-1,j,0)==0)
        perim(i,j,0) = true;
      if (i<img.ni()-1 && img(i+1,j,0)==0)
        perim(i,j,0) = true;
      if (j>0 && img(i,j-1,0)==0)
        perim(i,j,0) = true;
      if (j<img.nj()-1 && img(i,j+1,0)==0)
        perim(i,j,0) = true;
    }
}

bool non_zero (vil_image_view<bool>& img)
{
  for (unsigned int j=0; j<img.nj(); j++)
    for (unsigned int i=0; i<img.ni(); i++)
      if (img(i,j,0) != 0)
        return true;
  return false;
}

//push the current reduced polygon contour into the vector of polys[]
void reduce_verts (vcl_vector<vgl_point_2d<double> >& poly_big,
                   vcl_vector<vgl_point_2d<double> >& poly)
{
  poly.clear();

  //duplicate for now!
  for (unsigned int i=0; i<poly_big.size(); i++) {
    vgl_point_2d<double> pl = poly_big[i];
    poly.push_back (pl);
  }
}










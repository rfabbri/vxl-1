// This is brcv/seg/dbdet/lvwr/dbdet_lvwr.cxx
//:
//  \file

//#include <vcl_cassert.h>

#include <dbdet/lvwr/dbdet_lvwr.h>

#include <vcl_cmath.h>
#include <vcl_algorithm.h>

#include <vil1/vil1_image_as.h>
#include <vil1/vil1_copy.h>
#include <vil1/vil1_memory_image_of.h>
#include <brip/brip_vil1_float_ops.h>
#include <vepl1/vepl1_gaussian_convolution.h>

#include <osl/osl_convolve.h>
#include <osl/internals/droid.h> //name

void dbdet_lvwr::prepare_buffers(int w, int h, int seed_x_, int seed_y_)
{
  image_h = h;
  image_w = w;
  seed_x = seed_x_;
  seed_y = seed_y_;

  image_buf.resize(image_w, image_h);
  image_gradx_buf.resize(image_w, image_h);
  image_grady_buf.resize(image_w, image_h);
  image_gradmag_buf.resize(image_w, image_h);
  image_laplacian_buf.resize(image_w, image_h);
  image_fxx_buf.resize(image_w, image_h);
  image_fxy_buf.resize(image_w, image_h);
  image_fyy_buf.resize(image_w, image_h);

  image_edgecosts_buf.resize(image_w, image_h);

  // set up window
  window_str.row_start_index = 0;
  window_str.col_start_index = 0;
  window_str.row_end_index = image_h-1;
  window_str.col_end_index = image_w-1;

  // set up 2d arrays
  expanded = new bool*[image_h];
  pointed_neighbours = new vcl_pair<float, float> *[image_h];
  path_length = new int *[image_h];
  global_costs = new float *[image_h];

  for (int loop = 0; loop<image_h; loop++) {
    expanded[loop] = new bool[image_w];
    pointed_neighbours[loop] = new vcl_pair<float, float> [image_w];
    path_length[loop] = new int[image_w];
    global_costs[loop] = new float[image_w];
  }

}

void dbdet_lvwr::free_buffers()
{

  for (int loop = 0; loop<image_h; loop++) {
    delete [] expanded[loop];
    delete [] pointed_neighbours[loop];
    delete [] path_length[loop];
    delete [] global_costs[loop];
  }

  image_h = 0;
  image_w = 0;
  seed_x = 0;
  seed_y = 0;

  image_buf.resize(image_w, image_h);
  image_gradx_buf.resize(image_w, image_h);
  image_grady_buf.resize(image_w, image_h);
  image_gradmag_buf.resize(image_w, image_h);
  image_laplacian_buf.resize(image_w, image_h);
  image_fxx_buf.resize(image_w, image_h);
  image_fxy_buf.resize(image_w, image_h);
  image_fyy_buf.resize(image_w, image_h);

  delete [] expanded;
  //free(pointed_neighbours);
  delete [] pointed_neighbours;
  //free(path_length);
  delete [] path_length;
  //free(global_costs);
  delete [] global_costs;
}
//: just assign costs from the given image
void dbdet_lvwr::set_costs_from_image(vil1_image const &image)
{
  //: assume the image has intensity range [0, 1] 
  vil1_memory_image_of<float> float_image = brip_vil1_float_ops::convert_to_float(image);
  //int h = float_image.height();
  //int w = float_image.width();
  //int h2 = image_edgecosts_buf.height();
  //int w2 = image_edgecosts_buf.width();
  for (int row = 0; row < image_h; row++)
   for (int col = 0; col < image_w; col++) {
     float val = float_image(col,row);
     image_edgecosts_buf[row][col] = 1.0-val;
    //image_edgecosts_buf[row][col] = 1.0-float_image(row,col);
   }

  return;
}

//: just give zero cost to edge pixels and 1 cost to the remaining pixels
void dbdet_lvwr::compute_costs_from_edges(vcl_list<osl_edge*> canny_edges) {

  vcl_cout << image_edgecosts_buf.width() << vcl_endl;
  vcl_cout << image_edgecosts_buf.height() << vcl_endl;

  for (int row = 0; row < image_h; row++)
   for (int col = 0; col < image_w; col++) {
    image_edgecosts_buf[row][col] = 1;
    }

  vcl_list<osl_edge*>::const_iterator i;
  for (i = canny_edges.begin(); i != canny_edges.end(); ++i)
  {
    osl_edge const* e = *i;
    float *x = e->GetY(),*y = e->GetX(); // note x-y confusion.

    for (unsigned int j = 0;j<e->size();j++)
    {
      image_edgecosts_buf[(int)(y[j])][(int)(x[j])] = 0;
      //easy_tab->add_point(x[j], y[j]);
    }
  }

}

void dbdet_lvwr::compute_gradients(vil1_image const &image)
{
  vil1_image smoothed = vepl1_gaussian_convolution(image, double(params_.gauss_sigma));

  // copy input image to byte buffer
  //vil1_image_as_byte(image).get_section(image_buf.get_buffer(), 0, 0, image_w, image_h);
  vil1_image_as_byte(smoothed).get_section(image_buf.get_buffer(), 0, 0, image_w, image_h);

  // compute gradients
#if 0
    for (int i = 0; i < 10; ++i)
      vcl_cout << "image pixel " << i << ": " << (int)image_buf(i,i+4) << '\n';
#endif

  // trim the window
  window_str.row_start_index  += 2;
  window_str.col_start_index  += 2;
  window_str.row_end_index  -= 2;
  window_str.col_end_index  -= 2;
  droid::compute_gradx_grady (&window_str,
                &image_buf,
                &image_gradx_buf,
                &image_grady_buf);

  compute_gradmag (&window_str,
                     &image_gradmag_buf,
                     &image_gradx_buf,
                     &image_grady_buf);


  //window_str.row_start_index += 2;
  //window_str.col_start_index += 2;
  //window_str.row_end_index   -= 2;
  //window_str.col_end_index   -= 2;
  droid::compute_fxx_fxy_fyy (&window_str,
                &image_gradx_buf,
                &image_grady_buf,
                &image_fxx_buf,
                &image_fxy_buf,
                &image_fyy_buf);

  compute_laplacian (&window_str,
             &image_laplacian_buf,
             &image_fxx_buf,
             &image_fyy_buf);

}

void dbdet_lvwr::compute_gradmag (osl_roi_window      *window_str,
                      vil1_memory_image_of<float>    *image_gradmag_ptr,
                      vil1_memory_image_of<int>   *image_gradx_ptr,
                      vil1_memory_image_of<int>   *image_grady_ptr)

{
  int row_start = window_str->row_start_index;
  int col_start = window_str->col_start_index;
  int row_end   = window_str->row_end_index;
  int col_end   = window_str->col_end_index;

  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {

    (*image_gradmag_ptr)[row][col] =
      (float)vcl_sqrt(vcl_pow((double)(*image_gradx_ptr)[row][col], 2.0) +
                      vcl_pow((double)(*image_grady_ptr)[row][col], 2.0));

    }
  }

  float max = 0.0;

  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {
    if ((*image_gradmag_ptr)[row][col] > max)
      max = (*image_gradmag_ptr)[row][col];
  }
  }
  vcl_cout << "max in mag: " << max << vcl_endl;


  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {
    (*image_gradmag_ptr)[row][col] = (*image_gradmag_ptr)[row][col]/max;
    (*image_gradmag_ptr)[row][col] = 1 - (*image_gradmag_ptr)[row][col];
  }
  }

}


vil1_image dbdet_lvwr::getgradmag(void)
{
  return vil1_image_as_byte(image_edgecosts_buf);
  //return vil1_image_as_byte(image_laplacian_buf);
  //return vil1_image_as_byte(image_gradmag_buf);
}

void dbdet_lvwr::compute_laplacian(osl_roi_window      *window_str,
                      vil1_memory_image_of<float>    *image_laplacian_ptr,
                      vil1_memory_image_of<float>   *image_fxx_ptr,
                      vil1_memory_image_of<float>   *image_fyy_ptr)
{
  int row_start = window_str->row_start_index;
  int col_start = window_str->col_start_index;
  int row_end   = window_str->row_end_index;
  int col_end   = window_str->col_end_index;

  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {
    (*image_laplacian_ptr)[row][col] =
      (*image_fxx_ptr)[row][col] +
      (*image_fyy_ptr)[row][col];
    }
  }

  vcl_pair<int, int> *temp = new vcl_pair<int, int>[3];
  temp[0].first = 0;
  temp[0].second = 1;
  temp[1].first = 1;
  temp[1].second = 0;
  temp[2].first = 1;
  temp[2].second = 1;

  for (int row = 0; row < image_h; row++)
   for (int col = 0; col < image_w; col++) {
    (*image_fxx_ptr)[row][col] = 1;
    }

  // find zero crossings of the laplacian and save to image_fxx buffer
  for (int row = row_start; row < row_end; row++) {
    for (int col = col_start; col < col_end; col++) {
    for (int i = 0; i<3; i++)
      if ( ((*image_laplacian_ptr)[row][col] *
          (*image_laplacian_ptr)[row+temp[i].first][col+temp[i].second]) < 0 )
      {
        //vcl_cout << " !!!!!!!!!!!! found sign change!!!!!\n";
        if (vcl_abs((*image_laplacian_ptr)[row][col]) <
          vcl_abs((*image_laplacian_ptr)[row+temp[i].first][col+temp[i].second]))
          (*image_fxx_ptr)[row][col] = 0;
        else
          (*image_fxx_ptr)[row+temp[i].first][col+temp[i].second] = 0;
      }
    }
  }

  for (int row = 0; row < image_h; row++)
    for (int col = 0; col < image_w; col++)
    (*image_laplacian_ptr)[row][col] = (*image_fxx_ptr)[row][col];

  for (int row = 0; row < image_h; row++) {
    for (int col = 0; col < image_w; col++) {
    if ((*image_laplacian_ptr)[row][col] != 0 )
      (*image_laplacian_ptr)[row][col] = 1;
    }
  }
}

vcl_ostream& operator<< (vcl_ostream& Out, const my_pixel &p1)
{
  Out << "my pixel object row: " << p1.row << " col: " << p1.col;
  Out << " total_cost: " << p1.total_cost /*<< " points to: << p1.point_to */<< "\n";
  return Out;
}
bool operator< (const my_pixel &p1, const my_pixel &p2)
  {
    if (p1.total_cost < p2.total_cost)
      return true;
    else return false;

  }
bool operator== (const my_pixel &p1, const my_pixel &p2)
{
  return (p1.row == p2.row && p1.col == p2.col);
}

bool dbdet_lvwr::contains(vcl_multiset<my_pixel> *active_pixels, my_pixel p1) {

  vcl_multiset<my_pixel>::iterator pos;
  for (pos = active_pixels->begin(); pos != active_pixels->end(); ++pos) {
    if (*pos == p1)
      return true;
  }
  return false;
}

float dbdet_lvwr::my_get_factor(int i, int j) {
  if (i == 0 || j == 0)
    return 1.0/vcl_sqrt(2.0);
  else
    return 1.0;
}

float dot(float x1, float y1, float x2, float y2) {
  return (x1*x2+y1*y2);
}

float dbdet_lvwr::edge_direction_cost(my_pixel p, my_pixel q, float *dp) {

  // d_prime is the edge direction that is perpendicular to the gradient at that poitn

  float d_prime_p_x = image_grady_buf[p.row][p.col];
  float d_prime_p_y = -image_gradx_buf[p.row][p.col];
  float d_prime_mag = vcl_sqrt( (d_prime_p_x*d_prime_p_x) + (d_prime_p_y*d_prime_p_y));

  // make it unit vector
  d_prime_p_x /= d_prime_mag;
  d_prime_p_y /= d_prime_mag;

  float d_prime_q_x = image_grady_buf[q.row][q.col];
  float d_prime_q_y = -image_gradx_buf[q.row][q.col];
  d_prime_mag = vcl_sqrt( (d_prime_q_x*d_prime_q_x) + (d_prime_q_y*d_prime_q_y));

  // make it unit vector
  d_prime_q_x /= d_prime_mag;
  d_prime_q_y /= d_prime_mag;

  // find (q-p)
  float q_minus_p_x = q.row-p.row;
  float q_minus_p_y = q.col-p.col;

  float q_minus_p_mag = vcl_sqrt( (q_minus_p_x*q_minus_p_x) + (q_minus_p_y*q_minus_p_y));

  // find l_pq
  float l_p_q_x = 0, l_p_q_y = 0;

  if (dot(d_prime_p_x, d_prime_p_y, q_minus_p_x, q_minus_p_y) >= 0) {
    l_p_q_x = q_minus_p_x / q_minus_p_mag;
    l_p_q_y = q_minus_p_y / q_minus_p_mag;
  } else {
    l_p_q_x = (p.row-q.row) / q_minus_p_mag;
    l_p_q_y = (p.col-q.col) / q_minus_p_mag;
  }

  float d_p = dot(d_prime_p_x, d_prime_p_y, l_p_q_x, l_p_q_y);
  float d_q = dot(d_prime_q_x, d_prime_q_y, l_p_q_x, l_p_q_y);

  //*dp = l_x;
  *dp = d_q;
  float acos_p, acos_q;
  if (d_p == 0) acos_p = (3.14f)/2;
  else acos_p = vcl_acos(d_p);
  if (d_q == 0) acos_q = (3.14f)/2;
  else acos_q = vcl_acos(d_q);
  return (2/(3*3.14f))*(acos_p+acos_q);
  //return vcl_acos(d_q);

}

void dbdet_lvwr::compute_directions(int seed_x_, int seed_y_)
{
  seed_x = seed_x_;
  seed_y = seed_y_;
  my_pixel p0 = my_pixel(seed_y, seed_x);

  vcl_multiset<my_pixel> active_pixels;
  vcl_multiset<my_pixel>::iterator pos;

  for (int i = 0; i<image_h; i++)
    for (int j = 0; j<image_w; j++) {
      expanded[i][j] = false;
      pointed_neighbours[i][j].first = -1;
      pointed_neighbours[i][j].second = -1;
      path_length[i][j] = 0;
      global_costs[i][j] = 0.0;
    }

  // do not expand the pixels that lie outside of the specified window

  if (seed_x-params_.window_w/2>= 0)
    for (int i = seed_y-params_.window_h/2; i<=seed_y+params_.window_h/2; i++)
      if (i >= 0 && i < image_h)
        expanded[i][seed_x-params_.window_w/2] = true;

  if (seed_x+params_.window_w/2< image_w)
    for (int i = seed_y-params_.window_h/2; i<=seed_y+params_.window_h/2; i++)
      if (i >= 0 && i < image_h)
        expanded[i][seed_x+params_.window_w/2] = true;

  if (seed_y-params_.window_h/2>= 0)
    for (int j = seed_x-params_.window_w/2; j<=seed_x+params_.window_w/2; j++)
      if (j >= 0 && j < image_w)
        expanded[seed_y-params_.window_h/2][j] = true;

  if (seed_y+params_.window_h/2< image_h)
    for (int j = seed_x-params_.window_w/2; j<=seed_x+params_.window_w/2; j++)
      if (j >= 0 && j < image_w)
        expanded[seed_y+params_.window_h/2][j] = true;

  float local_cost, glb_cost, temp_cost, scale_factor;
  my_pixel p1;

  //: dynamic algorithm that computes global costs
  // initialize active list with zero cost seed pixel
  //vcl_assert (p0.row > -10000);
  active_pixels.insert(p0);

  int counter = 0;
  while (active_pixels.size() != 0) {
    counter++;

    if (counter % 500 == 0)
      vcl_cout << "size: " << active_pixels.size() << "\n";

    //vcl_assert (active_pixels.size() > 0);
    pos = active_pixels.begin();   // begin holds the pixel with min cost
    my_pixel p = *pos;
    active_pixels.erase(pos);
    expanded[p.row][p.col] = true;

    glb_cost = p.total_cost;

    // for each neighbour
    for (int i = -1; i<2; i++) {
      if ((p.row + i > 4) && (p.row + i < image_h-5))
        for (int j = -1; j<2; j++) {
          if ((p.col + j > 4) && (p.col + j < image_w-5)) {
            if (!expanded[p.row + i][p.col + j]) {

              scale_factor = my_get_factor(i,j);
              p1 = my_pixel(p.row + i, p.col + j);

              float dp, cost;

              if (params_.canny || params_.use_given_image) {
                local_cost = (params_.weight_canny)*scale_factor*image_edgecosts_buf[p.row + i][p.col + j]
                     +
                         (params_.weight_canny_l)*(p.path_length+1*scale_factor)/params_.path_norm;
              }

              else {

              cost = edge_direction_cost(p, p1, &dp);
              local_cost = params_.weight_g*scale_factor*
                     image_gradmag_buf[p.row + i][p.col + j]
                     +
                     params_.weight_z*
                     image_laplacian_buf[p.row + i][p.col + j]
                     +params_.weight_d*cost
                     +params_.weight_l*(p.path_length+1)/params_.path_norm;
                     ;
              }

              temp_cost = glb_cost + local_cost;

              if (!contains(&active_pixels, p1)) {
                p1.total_cost = temp_cost;
                p1.path_length = p.path_length + int(scale_factor);
                pointed_neighbours[p.row + i][p.col + j].first = p.row;
                pointed_neighbours[p.row + i][p.col + j].second = p.col;
                active_pixels.insert(p1);
                path_length[p.row + i][p.col + j] = p1.path_length;
                global_costs[p.row + i][p.col + j] = p1.total_cost;
              }
              else {
                // remove p1 if it has high cost
                for (pos = active_pixels.begin();
                     pos != active_pixels.end();
                   ++pos) {
                  if (*pos == p1) {
                    if ((*pos).total_cost > temp_cost)
                      active_pixels.erase(pos);
                    break;
                  }
                }
              }
            }
          }
        }
    }


  }

}

float dbdet_lvwr::get_global_cost(int x, int y) {
  return global_costs[y][x];
}

// first  --> row
// second --> col
// x      --> col
// y      --> row

bool dbdet_lvwr::get_path(int free_x, int free_y,
                 vcl_vector<vcl_pair<int, int> > &cor) {

  bool out = false;
  // store the optimum path to this free point
  cor.clear();
  int current_x, current_y, temp_y;

  // seed just points to itself
  if (seed_x == free_x && seed_y == free_y)
    out=false;

  if (free_x <= seed_x-params_.window_w/2) {          //original condition
  //if (free_x < seed_x-params_.window_w/2){
    out = true;
    free_x = seed_x-params_.window_w/2+1;
  //free_x = seed_x-params_.window_w/2;
  }

  //if (free_x > seed_x+params_.window_w/2-1) {      //original condition
  if (free_x >= seed_x+params_.window_w/2){
    out = true;
    //free_x = seed_x+params_.window_w/2-2;
  free_x = seed_x+params_.window_w/2-1;
  }

  if (free_y <= seed_y-params_.window_h/2) {        //original condition
  //if (free_y < seed_y-params_.window_h/2){
    out = true;
    free_y = seed_y-params_.window_h/2+1;
  //free_y = seed_y-params_.window_h/2;
  }

  //if (free_y >= seed_y+params_.window_h/2-1) {           //original condition
  if (free_y >= seed_y+params_.window_h/2) {               
    out = true;
    //free_y = seed_y+params_.window_h/2-2;
  free_y = seed_y+params_.window_h/2-1;
  }

  /*
  if (free_x <= seed_x-params_.window_w/2 ||
    free_x >= seed_x+params_.window_w/2-1 ||
    free_y <= seed_y-params_.window_h/2 ||
    free_y >= seed_y+params_.window_h/2-1) {
      vcl_cout << "Free point is outside the window!\n";
      return;
  }*/

  if (free_y < 5 ||
    free_y > image_h-6 ||
    free_x < 5 ||
    free_x > image_w-6) {
      vcl_cout << "Point is out of exceptable image area!\n";
      return false;
  }

  //current_y = int(pointed_neighbours[free_y][free_x].first);
  //current_x = int(pointed_neighbours[free_y][free_x].second);
  current_x = free_x;
  current_y = free_y;

  while (!(current_x == seed_x && current_y == seed_y)) {
    cor.push_back(vcl_pair<int, int>(int(current_y),
                     int(current_x)));
    temp_y     = int(pointed_neighbours[current_y][current_x].first);
    current_x  = int(pointed_neighbours[current_y][current_x].second);
    current_y  = temp_y;
  }

  // push seed
  cor.push_back(vcl_pair<int, int>(int(current_y),
                   int(current_x)));
  return out;

}

bool dbdet_lvwr::get_processed_path(int free_x, int free_y, vcl_vector<vcl_pair<int, int> > &cor)
{
  bool out = false;
  // store the optimum path to this free point
  cor.clear();
  int current_x, current_y, temp_y;

  // seed just points to itself
  if (seed_x == free_x && seed_y == free_y)
    return false;

  if (free_x <= seed_x-params_.window_w/2) {          //original condition
  //if (free_x < seed_x-params_.window_w/2) {             
    out = true;
    free_x = seed_x-params_.window_w/2+1;
  //free_x = seed_x-params_.window_w/2;
  }

  //if (free_x >= seed_x+params_.window_w/2-1) {       //original condition
  if (free_x >= seed_x+params_.window_w/2) {
    out = true;
    //free_x = seed_x+params_.window_w/2-2;
  free_x = seed_x+params_.window_w/2-1;
  }

  if (free_y <= seed_y-params_.window_h/2) {         //original condition
   //if (free_y < seed_y-params_.window_h/2) {
    out = true;
    free_y = seed_y-params_.window_h/2+1;
  //free_y = seed_y-params_.window_h/2;
  }

  //if (free_y >= seed_y+params_.window_h/2-1) {      //original condition
  if (free_y >= seed_y+params_.window_h/2) {
    out = true;
    //free_y = seed_y+params_.window_h/2-2;
  free_y = seed_y+params_.window_h/2-1;
  }

  if (free_y < 5 ||
    free_y > image_h-6 ||
    free_x < 5 ||
    free_x > image_w-6) {
      vcl_cout << "Point is out of exceptable image area!\n";
      return false;
  }

  current_y = int(pointed_neighbours[free_y][free_x].first);
  current_x = int(pointed_neighbours[free_y][free_x].second);

  while (!(current_x == seed_x && current_y == seed_y)) {
    cor.push_back(vcl_pair<int, int>(int(current_y),
                     int(current_x)));
    temp_y     = int(pointed_neighbours[current_y][current_x].first);
    current_x  = int(pointed_neighbours[current_y][current_x].second);
    current_y  = temp_y;
  }

  // push seed
  cor.push_back(vcl_pair<int, int>(int(current_y),
                   int(current_x)));

  return out;
}


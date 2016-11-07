// This is brcv/dbdet/compass/dbdet_compass_color_edge_detector.cxx

//:
// \file

#include "dbdet_EMD.h"
#include "dbdet_compass_edge_detector.h"
#include "dbdet_compass_color_edge_detector.h"
#include "dbdet_savitzky_golay_filter_2d.h"

#include <vbl/vbl_ref_count.h>
#include <vcl_utility.h>
#include <vcl_vector.h>
#include <vcl_deque.h>
#include <vcl_list.h>

#include <vgl/vgl_point_2d.h>

#include <dbdet/edge/dbdet_gaussian_kernel.h>
#include <dbdet/edge/dbdet_interp_kernel.h>
#include <dbdet/algo/dbdet_nms.h>
#include <dbdet/algo/dbdet_subpix_convolution.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/edge/dbdet_edgemap.h>

#define MAX_VAL 255.0

//this has to be a global variable
double dbdet_EMD_cost[MAXCLUSTERS][MAXCLUSTERS];

static int CreateMask(double sigma, int n_wedges, int masksz, int weight_type, double **mask, double **sum);


//: This routine creates the mask that is one-quarter of the size of the circle
// and n_wedges deep.  
// It also creates a mask sum over all the wedges and computes the number of 
// pixels with non-zero weights.
//
int CreateMask(double sigma, int n_wedges, int masksz, int weight_type, double **mask, double **sum)
{
  //Create a normalized Gaussian mask in Quadrant I
  //Note: this mask is in column order
  double* gauss = new double[masksz*masksz];
  for (int i = 0; i < masksz; i++){
    for (int j = 0; j < masksz; j++) 
    {
      double r = vcl_sqrt((masksz-i-0.5) * (masksz-i-0.5) + (j+0.5) * (j+0.5));

      // Various pixel weighting masks
      if (weight_type==1)
        gauss[j*masksz+i] = r * vcl_exp(-(r*r)/(2*sigma*sigma));  //RAYLEIGH
      else if (weight_type==2)
        gauss[j*masksz+i] = vcl_exp(-(r*r)/(2*sigma*sigma)); //GAUSSIAN

      // gauss[j*masksz+i] = 1 - r / masksz;  // LINEAR
    }
  }

  //compute a quarter mask for a compass operator with the given parameters
  *mask = MakeQtrMask(sigma * 3.0, n_wedges);

  //also compute the mask sum over all the wedges
  *sum = new double[masksz*masksz];
  for (int i=0; i<masksz*masksz; i++)//initialize it
    (*sum)[i] = 0.0;

  for (int i = 0; i < masksz*masksz*n_wedges; i++) {
    if (weight_type>0)//weight the wedge masks by the weighting function 
      (*mask)[i] *= gauss[i % (masksz*masksz)];

    // compute the sum
    (*sum)[i % (masksz*masksz)] += (*mask)[i];
  }

  // Count up non-zero pixels
  int npoints = 0;
  for (int i = 0; i < masksz*masksz; i++)
    npoints += ((*sum)[i] > 0);

  return (4*npoints);
}

// This routine abstracts a little of the complexity of placing pixels and
// their weights into the unclustered array 
void PlacePoint(vil_image_view<double>& L, 
                vil_image_view<double>& a,
                vil_image_view<double>& b,
                int r, int c,  
                            double *point_array, 
                double *weight_array, double weight)
{
  *point_array = L(c,r);
  *(point_array+1) = a(c,r);
  *(point_array+2) = b(c,r);
  *weight_array = weight;
}

// Index should be returned for quicker building of the histograms.
int ClusterPoints(vil_image_view<double>& L, 
                  vil_image_view<double>& a,
                  vil_image_view<double>& b,
                  double *masksum, 
                  int radius, int totalpoints, 
                  int r, int c, 
                              double totalweight, 
                  double maxweight, int maxclusters, 
                              double cluster[][CDIM])
{
  double* points = new double[CDIM*totalpoints];
  double* temp = points; /* temp will walk through the data array */

  double* weights = new double[totalpoints];
  double* temp2 = weights; /* same here */

   //To get the proper sampling percentage, we have two options.  The simple
   //one is to multiply all the mask weights (which we treat as probabilities)
   //by a constant.  However, if this constant is too high, the maximum weight
   //will go above 1.0.  Therefore, we must use a more complex option.
  int simple;
  double ratio;
  if (maxweight * totalpoints * SAMPLE_PERCENTAGE / totalweight > 1.0) {
    simple = 0;
    ratio = (1.0 - SAMPLE_PERCENTAGE) / (1.0 - totalweight / totalpoints);
  } 
  else {
    simple = 1;
    ratio = (double)totalpoints * SAMPLE_PERCENTAGE / totalweight;
  }
  
  /* Determine if the window is large enough for sampling, and if so,
   * sample each pixel in the mask with a Bernoulli trial.  Otherwise,
   * use all the pixels for clustering.
   */
  double /*cutoff = 0.0,*/ mweight;
  int npoints = 0;
  for (int i = 0; i < radius; i++){
    for (int j = 0; j < radius; j++){
      if (masksum[i*radius+j] > 0) 
      {
              mweight = masksum[i*radius+j];
              PlacePoint(L,a,b,r-radius+j,c+i,temp,temp2,mweight);
              temp += CDIM;
              temp2++;
              npoints++;
              PlacePoint(L,a,b,r-i-1,c-radius+j,temp,temp2,mweight);
              temp += CDIM;
              temp2++;
              npoints++;
              PlacePoint(L,a,b,r+radius-j-1,c-i-1,temp,temp2,mweight);
              temp += CDIM;
              temp2++;
              npoints++;
              PlacePoint(L,a,b,r+i,c+radius-j-1,temp,temp2,mweight);
              temp += CDIM;
              temp2++;
              npoints++;
      }
    }
  }

  /* Cluster using binary split algorithm */
  /* Changed 18 May 99 to include indexing */
  int nclusters, *index = NULL;
  dbdet_BS(points, npoints, maxclusters, &nclusters, &temp, index);

  /* Copy output from temp to cluster */
  for (int i = 0; i < nclusters; i++)
    for (int j = 0; j < CDIM; j++)
      cluster[i][j] = temp[i * CDIM + j];

  delete points;
  delete weights;

  return nclusters;
}

// This routine abstracts a little of the complexity of placing pixels and
// their weights into the histograms
void AddWeight(vil_image_view<double>& L, 
               vil_image_view<double>& a,
               vil_image_view<double>& b, 
               double weight, 
               int r, int c, 
               double data[][CDIM], double *hist,
                     double com[][CDIM], double sum[], int nclusters)
{
  int min_index;
  double min_dist = 100 * 240 * 240;

  for (int i = 0; i < nclusters; i++) {
    double Ldist = L(c,r) - data[i][0];
    double adist = a(c,r) - data[i][1];
    double bdist = b(c,r) - data[i][2];
    double dist = Ldist * Ldist + adist * adist + bdist * bdist;
    if (dist < min_dist) {
      min_dist = dist;
      min_index = i;
    }
  }

  //accumulate it at the correct bin
  hist[min_index] += weight;
  sum[min_index]  += weight;
  com[min_index][0] += weight * L(c,r);
  com[min_index][1] += weight * a(c,r);
  com[min_index][2] += weight * b(c,r);

}

//create the histograms for each wedge from the image colors
void CreateColorWedgeHistograms(vil_image_view<double>& L, 
                                vil_image_view<double>& a,
                                vil_image_view<double>& b, 
                                double *mask, 
                                int r, int c, 
                                int masksz, int n_wedges, 
                                double cluster[][CDIM], 
                                int nclusters, double* hist)
{
  double com[MAXCLUSTERS][CDIM], sum[MAXCLUSTERS];

  // initialize the histogram to zero
  for (int i=0; i<4*n_wedges*MAXCLUSTERS; i++) hist[i] = 0.0;

  for (int k = 0; k < n_wedges; k++){
    for (int i = 0; i < masksz; i++){
      for (int j = 0; j < masksz; j++)
      {
        int index = k*masksz*masksz+j*masksz+i;
        if (mask[index] > 0) {
          AddWeight(L, a, b, mask[index], r-masksz+i,   c+j,          cluster, hist + k*NBINS,              com, sum, nclusters);
          AddWeight(L, a, b, mask[index], r-j-1,        c-masksz+i,   cluster, hist + (n_wedges+k)*NBINS,   com, sum, nclusters);
          AddWeight(L, a, b, mask[index], r+masksz-i-1, c-j-1,        cluster, hist + (2*n_wedges+k)*NBINS, com, sum, nclusters);
          AddWeight(L, a, b, mask[index], r+j,          c+masksz-i-1, cluster, hist + (3*n_wedges+k)*NBINS, com, sum, nclusters);
        }
      }
    }
  }

}

void ComputeCostMatrix(double cluster[][CDIM], int nclusters)
{
  float dist, temp_dist;

  /* cost has to be a global variable because of the way the EMD code is */
  for (int i=0; i<MAXCLUSTERS; i++)
    for (int j=0; j<MAXCLUSTERS; j++)
      dbdet_EMD_cost[i][j] = 0.0;

  for (int i = 0; i < nclusters; i++) {
    for (int j = 0; j < nclusters; j++) {
      if (i > j)
        dbdet_EMD_cost[i][j] = dbdet_EMD_cost[j][i];
      else if (i == j || cluster[i][0] == BLACK && cluster[j][0] == BLACK)
        dbdet_EMD_cost[i][j] = 0.0;
      else {
        dist = 0;
        temp_dist = (cluster[i][0] - cluster[j][0]);
        dist += temp_dist * temp_dist; 
        temp_dist = (cluster[i][1] - cluster[j][1]);
        dist += temp_dist * temp_dist;
        temp_dist = (cluster[i][2] - cluster[j][2]);
        dist += temp_dist * temp_dist;

        /* dist = sqrt(dist) / PERCEPTUAL_THRESH; 
        dbdet_EMD_cost[i][j] = (dist >= 1) ? 1 : dist;*/

        dbdet_EMD_cost[i][j] = 1 - vcl_exp(-vcl_sqrt(dist) / GAMMA);
        /* dbdet_EMD_cost[i][j] = sqrt(dist) / 100; */
      }
    }
  }

}


/* Distance function for the EMD */
double dist(int *F1, int *F2) 
{ 
  return (dbdet_EMD_cost[*F1][*F2]);
}

//: Compute color histogram gradient using the compass operator
// Note: # of orientations = 2*n_wedges
void dbdet_compute_compass_color_gradient( vil_image_view<vxl_byte>& /*image*/, int spacing, int n_wedges, 
                                           int weight_type, double sigma, int dist_op, bool SG_filter,
                                           vil_image_view<double>& hist_grad,
                                           vil_image_view<double>& hist_ori)
{
  //make sure this is a RGB color image

  //convert it to Lab color space
  vil_image_view<double> L, a, b;

  // call the other method
  dbdet_compute_compass_color_gradient(L, a, b, spacing, n_wedges, weight_type, sigma, dist_op, SG_filter, hist_grad, hist_ori);
}

//: Compute color histogram gradient using the compass operator
// Note: # of orientations = 2*n_wedges
void dbdet_compute_compass_color_gradient( vil_image_view<double>& L, 
                                           vil_image_view<double>& a,
                                           vil_image_view<double>& b,
                                           int spacing, int n_wedges, 
                                           int weight_type, double sigma, int dist_op, bool SG_filter,
                                           vil_image_view<double>& hist_grad,
                                           vil_image_view<double>& hist_ori)
{
  // determine some relevant parameters
  int n_orient = 2*n_wedges; //number of orientations
  int masksz = (int) vcl_ceil(3 * sigma); //mask size

  // allocate space for histogram gradients at various orientations
  vcl_vector<vil_image_view<double> > hist_dist(n_orient);
  for (int i=0; i<n_orient; i++){
    hist_dist[i].set_size(L.ni(), L.nj());
    hist_dist[i].fill(0.0);
  }

  //create the wedge masks
  double *mask, *masksum;
  int npoints = CreateMask(sigma, n_wedges, masksz, weight_type, &mask, &masksum);

  // Compute max weight of entire circle and sum of weights over one wedge of the circle.
  double wedge_wt=0.0, max_wt=0.0;
  for (int i = 0; i < masksz*masksz; i++) { /* sums over 1/4 of circle */
    wedge_wt += masksum[i];
    if (masksum[i] > max_wt) max_wt = masksum[i];
  }
  wedge_wt /= n_wedges;

  //allocate space for the histograms
  double cluster[MAXCLUSTERS][CDIM]; //not sure yet

  double* wHist = new double[4*n_wedges*MAXCLUSTERS];  // All wedge histograms
  double hist1[MAXCLUSTERS], hist2[MAXCLUSTERS];       // Semi-circle histograms
  dbdet_color_sig sig1, sig2;                    // Normalized semi-circle signatures
  
  int f[MAXCLUSTERS];

  for (int i=0; i<MAXCLUSTERS; i++) f[i]=i;

  double* hist1norm = sig1.Weights; //pointers to the histograms in the signatures
  double* hist2norm = sig2.Weights;

  //loop over all the pixels in the image
  for (unsigned x = masksz; x < L.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < L.nj()-masksz; y+=spacing)
    {
      //compute the clusters from the color values in the mask
      int nclusters = ClusterPoints(L, a, b, masksum, masksz, npoints, 
                                                            y, x, wedge_wt*n_wedges*4, max_wt, 
                                                            DEFAULTCLUSTERS, cluster);

      //Create wedge histograms (output in wHist)
      CreateColorWedgeHistograms(L, a, b, mask, y, x, masksz, n_wedges, cluster, nclusters, wHist);

      ComputeCostMatrix(cluster, nclusters);

      //Compute initial histogram sums (i.e., or the first orientation)
      //Note: half of the wedges contribute towards one histogram 
      //      and the rest contribute towards the other
      for (int i = 0; i < nclusters; i++) {
        hist1[i] = 0.0;
        for (int j = 0; j < 2*n_wedges; j++)
          hist1[i] += wHist [j*MAXCLUSTERS + i];

        hist2[i] = 0.0;
        for (int j = 2*n_wedges; j < 4 * n_wedges; j++)
          hist2[i] += wHist [j*MAXCLUSTERS + i];
      }

      //Loop over every orientation
      for (int i = 0; i < n_orient; i++) {

        // Normalize the histograms
        for (int j = 0; j < MAXCLUSTERS; j++) {
          // Normalize by the number of wedges 
          hist1norm[j]  = hist1[j] / (wedge_wt * n_orient);
          hist2norm[j]  = hist2[j] / (wedge_wt * n_orient);
        }

        //compute distance between the normalized signatures
        sig1.n = nclusters;
        sig2.n = nclusters;

        double d = 0.0;
        if (dist_op==0)
          d = dbdet_color_chi_sq_dist(sig1, sig2); //compute chi^2 dist
        else if (dist_op==1)
          d = dbdet_color_bhat_dist(sig1, sig2);   //compute Bhattacharya dist
        else if (dist_op==2)
          d = dbdet_color_EMD(&sig1, &sig2, NULL, NULL);     //compute EMD

        //truncate the EMD distance to [0,1]
        //if (d > 1)       d = 1.0;
        //else if (d < 0)  d = 0.0;

        //record this distance as the contrast between the two halves of the compass
        hist_dist[i](x,y) = d;

        //Update the histograms except for the last iteration
        //
        //Note: This is an optimized process for computing histograms at each of the orientations
        //      Instead of accumulating the values for the entire semicircles at each orientation,
        //      just add one wedge and remove another wedge to get the histogram for the next orientation
        if (i < n_orient - 1){
          for (int j = 0; j < MAXCLUSTERS; j++) 
          {
            hist1[j] += - wHist[i                             * MAXCLUSTERS + j]    //remove this wedge
                        + wHist[((i+n_orient) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge

            hist2[j] += - wHist[((i+n_orient) % (4*n_wedges)) * MAXCLUSTERS + j]    //remove this wedge
                        + wHist[i                             * MAXCLUSTERS + j];   //add this wedge

         }
        }
      }

    }
  }

  //garbage collection
  delete mask;
  delete masksum;
  delete wHist;

  //allocate space for computing the final contrast magnitude and orientation
  hist_ori.set_size(L.ni(), L.nj());
  hist_ori.fill(0.0);
  hist_grad.set_size(L.ni(), L.nj());
  hist_grad.fill(0.0);

  //Filter the responses at each orientation using Savistzky-Golay filtering
  //  Allocate space for the filtered responses
  vcl_vector<vil_image_view<double> > filt_hist_dist(n_orient);

  if (SG_filter){
    for (int i=0; i<n_orient; i++){
      filt_hist_dist[i].set_size(L.ni(), L.nj());
      filt_hist_dist[i].fill(0.0);

      double theta = vnl_math::pi/double(n_orient) * double(i);

      Savitzky_Golay_filter_2d(hist_dist[i], filt_hist_dist[i], 2, masksz, theta);
    }
  }

  //loop over all the pixels in the image to compute NMS over orientations
  double* dist = new double[n_orient];

  for (unsigned x = masksz; x < L.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < L.nj()-masksz; y+=spacing)
    {
      if (SG_filter){
        for (int i=0; i<n_orient; i++)
          dist[i] = filt_hist_dist[i](x,y);
      }
      else {
        for (int i=0; i<n_orient; i++)
          dist[i] = hist_dist[i](x,y);
      }

      // compute edge strength and orientation at the current pixel
      compute_strength_and_orient(dist, n_orient, hist_grad(x,y), hist_ori(x,y));
    }
  }

  delete dist;

}

//: Detect edges using the compass operator
// Note: # of orientations = 2*n_wedges
dbdet_edgemap_sptr 
dbdet_detect_compass_color_edges(vil_image_view<vxl_byte>& image, int spacing, int n_wedges, 
                                 int weight_type, double sigma, int dist_op, bool SG_filter, double threshold,
                                 vil_image_view<double>& hist_grad,  bool output_orientation_map)
{
  //make sure this is a RGB color image
  if(image.nplanes() != 3)
    return 0;

  //convert it to Lab color space
  vil_image_view<double> L, a, b;

  // call the other method
  return dbdet_detect_compass_color_edges(L, a, b, spacing, n_wedges, weight_type, sigma, dist_op, SG_filter, threshold, hist_grad, output_orientation_map);
}

//: Detect edges using the compass operator
// Note: # of orientations = 2*n_wedges
dbdet_edgemap_sptr 
dbdet_detect_compass_color_edges(vil_image_view<double>& L, 
                                 vil_image_view<double>& a,
                                 vil_image_view<double>& b, 
                                 int spacing, int n_wedges, 
                                 int weight_type, double sigma, int dist_op, bool SG_filter, double threshold,
                                 vil_image_view<double>& hist_grad,  bool output_orientation_map)
{
  // determine some relevant parameters
  int n_orient = 2*n_wedges; //number of orientations
  int masksz = (int) vcl_ceil(3 * sigma); //mask size

  // allocate space for the histograms at various orientations
  vcl_vector<vbl_array_2d<vcl_pair<dbdet_color_sig, dbdet_color_sig> > > compass_sigs(n_orient); 
  //for (int i=0; i<n_orient; i++){
  //  compass_sigs[i].resize(L.ni(), L.nj());
  //}

  // allocate space for histogram gradients at various orientations
  vcl_vector<vil_image_view<double> > hist_dist(n_orient);
  for (int i=0; i<n_orient; i++){
    hist_dist[i].set_size(L.ni(), L.nj());
    hist_dist[i].fill(0.0);
  }

  //create the wedge masks
  double *mask, *masksum;
  int npoints = CreateMask(sigma, n_wedges, masksz, weight_type, &mask, &masksum);

  // Compute max weight of entire circle and sum of weights over one wedge of the circle.
  double wedge_wt=0.0, max_wt=0.0;
  for (int i = 0; i < masksz*masksz; i++) { /* sums over 1/4 of circle */
    wedge_wt += masksum[i];
    if (masksum[i] > max_wt) max_wt = masksum[i];
  }
  wedge_wt /= n_wedges;

  //allocate space for the histograms
  double cluster[MAXCLUSTERS][CDIM]; //not sure yet

  double* wHist = new double[4*n_wedges*MAXCLUSTERS];  // All wedge histograms
  double hist1[MAXCLUSTERS], hist2[MAXCLUSTERS];       // Semi-circle histograms
  dbdet_color_sig sig1, sig2;                    // Normalized semi-circle signatures
  
  double* hist1norm = sig1.Weights; //pointers to the histograms in the signatures
  double* hist2norm = sig2.Weights;

  double /*qhist1[MAXCLUSTERS],*/ qhist2[MAXCLUSTERS], qhist3[MAXCLUSTERS];
  /*qhist4[MAXCLUSTERS];  */     // quarter-circle histograms
  dbdet_color_sig qsig1, qsig2, qsig3, qsig4;
//  double* qhist1norm = qsig1.Weights;
  double* qhist2norm = qsig2.Weights;
  double* qhist3norm = qsig3.Weights;
//  double* qhist4norm = qsig4.Weights;


  //loop over all the pixels in the image
  for (unsigned x = masksz; x < L.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < L.nj()-masksz; y+=spacing)
    {
      //compute the clusters from the color values in the mask
      int nclusters = ClusterPoints(L, a, b, masksum, masksz, npoints, 
                                                            y, x, wedge_wt*n_wedges*4, max_wt, 
                                                            DEFAULTCLUSTERS, cluster);

      //Create wedge histograms (output in wHist)
      CreateColorWedgeHistograms(L, a, b, mask, y, x, masksz, n_wedges, cluster, nclusters, wHist);

      ComputeCostMatrix(cluster, nclusters);

      //Compute initial histogram sums (i.e., or the first orientation)
      //Note: half of the wedges contribute towards one histogram 
      //      and the rest contribute towards the other
      for (int i = 0; i < nclusters; i++) {
        hist1[i] = 0.0;
        for (int j = 0; j < 2*n_wedges; j++)
          hist1[i] += wHist [j*MAXCLUSTERS + i];

        hist2[i] = 0.0;
        for (int j = 2*n_wedges; j < 4 * n_wedges; j++)
          hist2[i] += wHist [j*MAXCLUSTERS + i];
      }

      //compile quarter histograms
      for (int i = 0; i < nclusters; i++) {
        //qhist1[i] = 0.0;
        //for (int j = 0; j < n_wedges; j++)
        //  qhist1[i] += wHist [j*MAXCLUSTERS + i];

        qhist2[i] = 0.0;
        for (int j = n_wedges; j < 2 * n_wedges; j++)
          qhist2[i] += wHist [j*MAXCLUSTERS + i];

        qhist3[i] = 0.0;
        for (int j = 2 * n_wedges; j < 3*n_wedges; j++)
          qhist3[i] += wHist [j*MAXCLUSTERS + i];

        //qhist4[i] = 0.0;
        //for (int j = 3*n_wedges; j < 4 * n_wedges; j++)
        //  qhist4[i] += wHist [j*MAXCLUSTERS + i];
      }

      //Loop over every orientation
      for (int i = 0; i < n_orient; i++) {

        // Normalize the histograms
        for (int j = 0; j < MAXCLUSTERS; j++) {

          // Normalize by the number of wedges 
          hist1norm[j]  = hist1[j] / (wedge_wt * n_orient);
          hist2norm[j]  = hist2[j] / (wedge_wt * n_orient);

          //normalize quarter histograms
          //qhist1norm[j]  = qhist1[j] / (wedge_wt * n_orient);
          qhist2norm[j]  = qhist2[j] / (wedge_wt * n_orient);
          qhist3norm[j]  = qhist3[j] / (wedge_wt * n_orient);
          //qhist4norm[j]  = qhist4[j] / (wedge_wt * n_orient);

        }

        sig1.n = nclusters;
        sig2.n = nclusters;

        qsig1.n = nclusters; qsig2.n = nclusters; qsig3.n = nclusters; qsig4.n = nclusters;

        //record the normalized histograms at the current pixel
        //compass_sigs[i](x,y) = vcl_pair<dbdet_signature, dbdet_signature>(hist1norm, hist2norm);

        //record the normalized quarter histograms at the current pixel
        //compass_sigs[i](x,y) = vcl_pair<dbdet_color_sig, dbdet_color_sig>(qsig2, qsig3);

        //compute distance between the normalized signatures
        double d = 0.0;
        if (dist_op==0)
          d = dbdet_color_chi_sq_dist(sig1, sig2); //compute chi^2 dist
        else if (dist_op==1)
          d = dbdet_color_bhat_dist(sig1, sig2);   //compute Bhattacharya dist
        else if (dist_op==2)
          d = dbdet_color_EMD(&sig1, &sig2, NULL, NULL);     //compute EMD

        //truncate the EMD distance to [0,1]
        //if (d > 1)       d = 1.0;
        //else if (d < 0)  d = 0.0;

        //record this distance as the contrast between the two halves of the compass
        hist_dist[i](x,y) = d;

        //Update the histograms except for the last iteration
        //
        //Note: This is an optimized process for computing histograms at each of the orientations
        //      Instead of accumulating the values for the entire semicircles at each orientation,
        //      just add one wedge and remove another wedge to get the histogram for the next orientation
        if (i < n_orient - 1){
          for (int j = 0; j < MAXCLUSTERS; j++) 
          {
            hist1[j] += - wHist[i                             * MAXCLUSTERS + j]    //remove this wedge
                        + wHist[((i+n_orient) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge

            hist2[j] += - wHist[((i+n_orient) % (4*n_wedges)) * MAXCLUSTERS + j]    //remove this wedge
                        + wHist[i                             * MAXCLUSTERS + j];   //add this wedge

            //update the quarter histograms
            //qhist1[j] += - wHist[i                               * MAXCLUSTERS + j]    //remove this wedge
            //             + wHist[((i+  n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge

            qhist2[j] += - wHist[((i+  n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j]    //remove this wedge
                         + wHist[((i+2*n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge

            qhist3[j] += - wHist[((i+2*n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j]    //remove this wedge
                         + wHist[((i+3*n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge

            //qhist4[j] += - wHist[((i+3*n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j]    //remove this wedge
            //             + wHist[((i+4*n_wedges) % (4*n_wedges)) * MAXCLUSTERS + j];   //add this wedge
                        
          }
        }
      }

    }
  }

  //garbage collection
  delete mask;
  delete masksum;
  delete wHist;

  //allocate space for computing the final contrast magnitude and orientation
  vil_image_view<double> hist_ori(L.ni(), L.nj());
  hist_grad.set_size(L.ni(), L.nj());
  hist_grad.fill(0.0);

  //Filter the responses at each orientation using Savistzky-Golay filtering
  //  Allocate space for the filtered responses
  vcl_vector<vil_image_view<double> > filt_hist_dist(n_orient);

  if (SG_filter){
    for (int i=0; i<n_orient; i++){
      filt_hist_dist[i].set_size(L.ni(), L.nj());
      filt_hist_dist[i].fill(0.0);

      double theta = vnl_math::pi/double(n_orient) * double(i);

      Savitzky_Golay_filter_2d(hist_dist[i], filt_hist_dist[i], 2, masksz, theta);
    }
  }

  //loop over all the pixels in the image to compute NMS over orientations
  double* dist = new double[n_orient];

  for (unsigned x = masksz; x < L.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < L.nj()-masksz; y+=spacing)
    {
      if (SG_filter){
        for (int i=0; i<n_orient; i++)
          dist[i] = filt_hist_dist[i](x,y);
      }
      else {
        for (int i=0; i<n_orient; i++)
          dist[i] = hist_dist[i](x,y);
      }

      // compute edge strength and orientation at the current pixel
      compute_strength_and_orient(dist, n_orient, hist_grad(x,y), hist_ori(x,y));
    }
  }

  delete dist;

  //temp
  vil_image_view<double> dx(L.ni(), L.nj());
  vil_image_view<double> dy(L.ni(), L.nj());

  //get pointers to the data
  double* Ori = hist_ori.top_left_ptr();
  double* Gx = dx.top_left_ptr();
  double* Gy = dy.top_left_ptr();

  for(unsigned long i=0; i<hist_ori.size(); i++){
    Gx[i] = vcl_sin(Ori[i]);
    Gy[i] = vcl_cos(Ori[i]);
  }

  //the edgemap
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(L.ni(), L.nj());

  if (output_orientation_map){
    //create the edgemap from the orientation map 
    for (unsigned x = 0; x < hist_ori.ni(); x++){
      for (unsigned y = 0; y < hist_ori.nj(); y++){
        edge_map->insert(new dbdet_edgel(vgl_point_2d<double>(x, y), hist_ori(x,y), hist_grad(x,y)));
      }
    }
  }
  else {
    //Now call the NMS code to get the subpixel edge tokens
    vcl_vector<vgl_point_2d<double> > loc;
    vcl_vector<double> orientation, mag, d2f;

    dbdet_nms NMS(dbdet_nms_params(threshold, dbdet_nms_params::PFIT_3_POINTS), dx, dy, hist_grad);
    NMS.apply(true, loc, orientation, mag, d2f);
    
    //create the edgemap from the tokens collected from NMS
    for (unsigned i=0; i<loc.size(); i++)
    {
      //store the signatures of the two halves of the closest compass operator as the right and left signatures
      /*int xx = dbdet_round(loc[i].x());*/
      /*int yy = dbdet_round(loc[i].y());*/
      //int oo = (dbdet_round(n_orient*orientation[i]/vnl_math::pi) % n_orient); //modulo the # of orientations

      //dbdet_appearance* lapp = new dbdet_color_signature(compass_sigs[oo](xx,yy).first);
      //dbdet_appearance* rapp = new dbdet_color_signature(compass_sigs[oo](xx,yy).second);

      //create an edgel with the appearance information
      edge_map->insert(new dbdet_edgel(loc[i], orientation[i], mag[i], d2f[i], -1));//, lapp, rapp), xx, yy);
    }
  }

  return edge_map;

}


//: Function to find edges using independent compass operators for L, a and b
dbdet_edgemap_sptr
dbdet_detect_compass_color_edges_independent(vil_image_view<double>& L, 
                                             vil_image_view<double>& a,
                                             vil_image_view<double>& b, 
                                             int spacing, int n_wedges, int weight_type, double sigma, 
                                             int dist_op, bool SG_filter, double threshold, vil_image_view<double>& hist_grad,
                                             bool output_orientation_map)
{
  // determine some relevant parameters
  //  int n_orient = 2*n_wedges; //number of orientations

  //convert the Lab images to a 0-255 range
  vil_image_view<vxl_byte> LL(L.ni(), L.nj());
  vil_image_view<vxl_byte> AA(L.ni(), L.nj());
  vil_image_view<vxl_byte> BB(L.ni(), L.nj());

  //get pointers to the data
  double* lll = L.top_left_ptr();
  double* aaa = a.top_left_ptr();
  double* bbb = b.top_left_ptr();
  vxl_byte* ll = LL.top_left_ptr();
  vxl_byte* aa = AA.top_left_ptr();
  vxl_byte* bb = BB.top_left_ptr();

  for(unsigned long i=0; i<L.size(); i++){
    ll[i] = (vxl_byte) lll[i]; //temp
    aa[i] = (vxl_byte) aaa[i]; //temp
    bb[i] = (vxl_byte) bbb[i]; //temp
  }

  //compute the gradients and histograms independently
  vil_image_view<double> L_grad, L_ori, a_grad, a_ori, b_grad, b_ori;

  dbdet_compute_compass_gradient(LL, spacing, n_wedges, weight_type, sigma, dist_op, SG_filter,
                                 L_grad, L_ori);
  dbdet_compute_compass_gradient(AA, spacing, n_wedges, weight_type, sigma, dist_op, SG_filter,
                                 a_grad, a_ori);
  dbdet_compute_compass_gradient(BB, spacing, n_wedges, weight_type, sigma, dist_op, SG_filter,
                                 b_grad, b_ori);

  //combine the gradients into one by summing them up
  hist_grad.set_size(L.ni(), L.nj());

  double* HG = hist_grad.top_left_ptr();
  double* LG = L_grad.top_left_ptr();
  double* AG = a_grad.top_left_ptr();
  double* BG = b_grad.top_left_ptr();

  for(unsigned long i=0; i<hist_grad.size(); i++){
    HG[i] = (LG[i] + AG[i] + BG[i])/3;
  }

  //compute subpixel edges based on combined gradient and L_ori
  vil_image_view<double> dx(L.ni(), L.nj());
  vil_image_view<double> dy(L.ni(), L.nj());

  //get pointers to the data
  double* Ori = L_ori.top_left_ptr();
  double* Gx = dx.top_left_ptr();
  double* Gy = dy.top_left_ptr();

  for(unsigned long i=0; i<L_ori.size(); i++){
    Gx[i] = vcl_sin(Ori[i]);
    Gy[i] = vcl_cos(Ori[i]);
  }

  //the edgemap
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(L.ni(), L.nj());

  if (output_orientation_map){
    //create the edgemap from the orientation map 
    for (unsigned x = 0; x < hist_grad.ni(); x++){
      for (unsigned y = 0; y < hist_grad.nj(); y++){
        edge_map->insert(new dbdet_edgel(vgl_point_2d<double>(x, y), vcl_fmod(L_ori(x,y)+vnl_math::pi_over_2, vnl_math::pi), hist_grad(x,y)));
      }
    }
  }
  else {
    //Now call the NMS code to get the subpixel edge tokens
    vcl_vector<vgl_point_2d<double> > loc;
    vcl_vector<double> orientation, mag, d2f;

    dbdet_nms NMS(dbdet_nms_params(threshold, dbdet_nms_params::PFIT_3_POINTS), dx, dy, hist_grad);
    NMS.apply(true, loc, orientation, mag, d2f);
    
    //create the edgemap from the tokens collected from NMS
    for (unsigned i=0; i<loc.size(); i++)
    {
      //store the signatures of the two halves of the closest compass operator as the right and left signatures
//      int xx = dbdet_round(loc[i].x());
//      int yy = dbdet_round(loc[i].y());
//      int oo = (dbdet_round(n_orient*orientation[i]/vnl_math::pi) % n_orient); //modulo the # of orientations

      //dbdet_appearance* lapp = new dbdet_color_signature(compass_sigs[oo](xx,yy).first);
      //dbdet_appearance* rapp = new dbdet_color_signature(compass_sigs[oo](xx,yy).second);

      //create an edgel with the appearance information
      edge_map->insert(new dbdet_edgel(loc[i], orientation[i], mag[i], d2f[i], -1));//, lapp, rapp), xx, yy);
    }
  }

  return edge_map;
}



// This is brcv/dbdet/compass/dbdet_compass_edge_detector.cxx

//:
// \file

#include "dbdet_EMD.h"
#include "dbdet_compass_edge_detector.h"
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

// Integrate a circle of radius r centered at the origin from 
// Xlow to Xhigh in Quadrant I.  
// Since this function is only interested in square pixels, the Y value 
// is included so that the area below Y is ignored.
double CArea(double Xhigh, double Xlow, double Y, double r)
{
  return (0.5 * ((Xhigh * vcl_sqrt(r*r - Xhigh * Xhigh) + r*r * vcl_asin(Xhigh / r)) -
                 (Xlow  * vcl_sqrt(r*r - Xlow * Xlow)   + r*r * vcl_asin(Xlow  / r))   ) 
          - Y * (Xhigh - Xlow));
}

//: Compute the weighted masks for each wedge.
// Note: r is the radius of the circle, and n is the number of wedges.  
// The result is an array of length ceil(r) * ceil(r) * n where the 
// entries state how much area of each wedge of radius r (1/n of the quarter-circle) 
// is inside each pixel.  
//
// The pixels are in Quadrant I, listed in column-major order 
//(just as we would get by calling a MATLAB routine).  
// This is admittedly a mess, as there are at least 27 different cases for a 
// circle and a radial line and a square to interact.
//
// This function has been reproduced exactly from Ruzon's code. 
// I didn't wanna waste my time attempting to replicate it.
double* MakeQtrMask(double r, int n_wedges)
{
  //size of the quadrant
  int R = (int) vcl_ceil(r);

  //allocate the array to hold the mask
  double* mask = new double[R*R*n_wedges];

  //iniitialize
  for (int i=0; i<R*R*n_wedges; i++)
    mask[i] = 0.0;

  int ULC, URC, URL, URH, LRC, LLL, LLH;
  int InCircle, NoLine, LowLine, HighLine, LowIntersect, HighIntersect;
  double mlow, mhigh, lowangle, highangle;
  double CA, BA, AA, LA, BXC, BXL, BXH, TXC, TXL, TXH, LYC, LYL, LYH;
  double RYC, RYL, RYH, AAC, BAC, AAN, BAN, XLC, YLC, XHC, YHC;

  //Iterate over the lower left hand corner of each pixel
  for (int x = 0; x <= R - 1; x++){
    for (int y = R - 1; y >= 0; y--) {
      /* Start by computing the pixel's area in the circle */
      if (x * x + y * y >= r * r) /* Pixel entirely outside circle */
        continue;

      if ((x+1) * (x+1) + (y+1) * (y+1) <= r * r) { /* Pixel entirely inside */
        CA = 1.0;
        InCircle = 1;
        URC = 1;
      } 
      else { /* Tricky part; circle intersects pixel */
        URC = 0;
        ULC = x * x + (y+1) * (y+1) <= r * r;
        LRC = (x+1) * (x+1) + y * y <= r * r;
        BXC = sqrt(r * r - y * y);
        TXC = sqrt(r * r - (y+1) * (y+1));
        if (!ULC && !LRC)
          CA = CArea(BXC, x, y, r);
        else if (ULC && !LRC)
          CA = CArea(BXC, TXC, y, r) + TXC - x;
        else if (!ULC && LRC)
          CA = CArea(x + 1, x, y, r);
        else /* if (ULC && LRC) */
          CA = CArea(x + 1, TXC, y, r) + TXC - x;
        InCircle = 0;  /* Therefore, it must be on the border */
      }

      // Check through each wedge
      for (int i = 0; i < n_wedges; i++) {
        /* Compute area above lower radial line of wedge */
        lowangle = i * vnl_math::pi / (2 * n_wedges);
        mlow = tan(lowangle);
        TXL = (y+1)/mlow;
        BXL = y/mlow;
        if (TXL <= x)
          AA = 0.0;
        else if (i == 0 || BXL >= x+1)
          AA = 1.0;
        else {
          LLL = BXL > x;
          URL = TXL > x+1;
          LYL = mlow * x;
          RYL = mlow * (x + 1);
          if (LLL && URL)
            AA = 1 - 0.5 * (RYL-y) * (x+1-BXL);
          else if (!LLL && URL)
            AA = 0.5 * (2*(y+1)-LYL-RYL);
          else if (LLL && !URL)
            AA = 0.5 * (TXL+BXL-2*x);
          else
            AA = 0.5 * (y+1-LYL) * (TXL-x);
        }
        LowLine = AA < 1.0 && AA > 0.0;

        /* Compute area below upper radial line of wedge */
        /* The cases are reversed from the lower line cases */
        highangle = (i+1) * vnl_math::pi / (2 * n_wedges);
        mhigh = tan(highangle);
        TXH = (y+1)/mhigh;
        BXH = y/mhigh;
        RYH = mhigh*(x+1);
        LYH = mhigh*x;
        if (i == n_wedges-1 || TXH <= x)
          BA = 1.0;
        else if (BXH >= x+1)
          BA = 0.0;
        else {
          LLH = BXH < x;
          URH = TXH < x+1;
          if (LLH && URH)
            BA = 1 - 0.5 * (y+1-LYH) * (TXH-x);
          else if (!LLH && URH)
            BA = 1 - 0.5 * (BXH+TXH-2*x);
          else if (LLH && !URH)
            BA = 0.5 * (LYH+RYH-2*y);
          else /* if (!LLH && !URH) */
            BA = 0.5 * (RYH-y) * (x+1-BXH);
        }
        HighLine = BA < 1.0 && BA > 0.0;
        LA = BA + AA - 1.0;
        if (LA == 0.0) /* Pixel not in wedge */
          continue;
        NoLine = LA == 1.0;

        /* Finish the cases we know about so far */
        if (InCircle) {
          mask[i * R*R + x * R + R - 1 - y] = LA;
          continue;
        } 
        else if (NoLine) {
          mask[i * R*R + x * R + R - 1 - y] = CA;
          continue;
        }

        /* We can now assert (~InCircle && (HighLine || LowLine)) */
        /* But this does not ensure the circular arc intersects the line */
        LYC = sqrt(r * r - x * x);
        RYC = sqrt(r * r - (x+1) * (x+1));
        LowIntersect = LowLine &&
          ((!ULC && !LRC && ((LLL && BXL < BXC) || (!LLL && LYL < LYC))) ||
          (!ULC && LRC) || (ULC && !LRC) ||
          (ULC && LRC && ((!URL && TXL >= TXC) || (URL && RYL >= RYC))));

        HighIntersect = HighLine &&
          ((!ULC && !LRC && ((!LLH && BXH < BXC) || (LLH && LYH < LYC))) ||
          (!ULC && LRC) || (ULC && !LRC) ||
          (ULC && LRC && ((URH && TXH >= TXC) || (!URH && RYH >= RYC))));

        /* Recompute BA and AA (now BAC and AAC) given the intersection */
        if (LowIntersect) {
          XLC = cos(lowangle) * r;
          YLC = sin(lowangle) * r;
          if (!LRC && LLL)
            AAC = CA - 0.5 * (XLC - BXL) * (YLC - y) - CArea(BXC, XLC, y, r);
          else if (!LRC && !LLL)
            AAC = CA - 0.5 * (XLC - x) * (YLC + LYL - 2 * y) -
            CArea(BXC, XLC, y, r);
          else if (LRC && LLL)
            AAC = CArea(XLC, x, y, r) - 0.5 * (YLC - y) * (XLC - BXL);
          else /* if (LRC && !LLL) */
            AAC = CA - CArea(x+1, XLC, y, r) - 
            0.5 * (YLC + LYL - 2 * y) * (XLC - x);
        }

        if (HighIntersect) {
          XHC = cos(highangle) * r;
          YHC = sin(highangle) * r;
          if (!LRC && !LLH)
            BAC = 0.5 * (XHC - BXH) * (YHC - y) + CArea(BXC, XHC, y, r);
          else if (!LRC && LLH)
            BAC = 0.5 * (XHC - x) * (YHC + LYH - 2 * y) + 
            CArea(BXC, XHC, y, r);
          else if (LRC && LLH)
            BAC = CArea(x+1, XHC, y, r) + 
            0.5 * (YHC + LYH - 2 * y) * (XHC - x);
          else /* if (LRC && !LLH) */
            BAC = CArea(x+1, XHC, y, r) + 0.5 * (YHC - y) * (XHC - BXH);
        }

        /* Compute area for a few more cases */
        if (LowIntersect && !HighLine) {
          mask[i * R*R + x * R + R - 1 - y] = AAC;
          continue;
        } 
        else if (HighIntersect && !LowLine) {
          mask[i * R*R + x * R + R - 1 - y] = BAC;
          continue;
        } 
        else if (HighIntersect && LowIntersect) {  
          mask[i * R*R + x * R + R - 1 - y] = AAC + BAC - CA;
          continue;
        }

        /* Here we can assert (~InCircle && (HighLine || LowLine) &&
        * !LowIntersect && !HighIntersect).  There are still many 
        * possible answers.  Start by computing BAN and AAN (N for No
        * Intersection)
        */
        if (LowLine && !LowIntersect) {
          if (!ULC && !LLL)
            AAN = 0;
          else if (!LRC && LLL)
            AAN = CA;
          else if (LRC && URL && LLL)
            AAN = CA - 0.5 * (RYL - y) * (x+1 - BXL);
          else if (ULC && URL && !LLL)
            AAN = CA - 0.5 * (RYL + LYL - 2 * y);
          else /* if (ULC && !URL) */
            AAN = AA;
        }

        if (HighLine && !HighIntersect) {
          if (!ULC && LLH)
            BAN = CA;
          else if (!LRC && !LLH)
            BAN = 0;
          else if (LRC && !URH && !LLH)
            BAN = BA;
          else if (ULC && !URH && LLH)
            BAN = 0.5 * (RYL + LYL - 2 * y);
          else if (ULC && URH)
            BAN = CA + BA - 1;
        }

        if (LowLine && !LowIntersect && HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN + BAN - CA;
          continue;
        } 
        else if (LowIntersect && HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAC + BAN - CA;
          continue;
        } 
        else if (LowLine && !LowIntersect && HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN + BAC - CA;
          continue;
        } 
        else if (LowLine && !LowIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = AAN;
          continue;
        } 
        else if (HighLine && !HighIntersect) {
          mask[i * R*R + x * R + R - 1 - y] = BAN;
          continue;
        } 
        else {
          //vcl_cout << "Big nasty horrible bug just happened\n";
          mask[i * R*R + x * R + R - 1 - y] = 0.0;
        }
      }
    }
  }

  //finally the masks are ready
  return mask;      
}

//: This routine creates the mask that is one-quarter of the size of the circle
// and n_wedges deep.  
// It also creates a mask sum over all the wedges and computes the number of 
// pixels with non-zero weights.
//
double CreateMask(double sigma, int n_wedges, int masksz, int weight_type, double **mask, double **sum)
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

  // Also compute sum of weights over one wedge of the circle.
  double wedge_wt = 0.0;
  for (int i = 0; i < masksz*masksz; i++) 
    wedge_wt += (*sum)[i];
  wedge_wt /= n_wedges;

  return wedge_wt;
}

// This routine abstracts a little of the complexity of placing pixels and
// their weights into the histograms
void AddWeight(vil_image_view<vxl_byte>& image, double weight, 
               int row, int col, dbdet_bin hist[])
{
  //read image intensity at this point
  vxl_byte intensity = image(col, row);

  //quantize this intensity value to the specified bins
  int min_index = (int) vcl_floor(intensity*(NBINS - 1)/MAX_VAL);

  //accumulate it at the correct bin
  hist[min_index].weight += weight;
  hist[min_index].wsum += weight*intensity;
  // hist[min_index].value is not computed
}

//create the histograms for each wedge from the image intensities
void CreateWedgeHistograms(vil_image_view<vxl_byte>& image, double *mask, 
                           int r, int c, 
                           int masksz, int n_wedges, dbdet_bin* hist)
{
  // initialize the histograms to zero
  for (int i=0; i<4*n_wedges*NBINS; i++){
    hist[i].value = 0.0;
    hist[i].weight = 0.0;
    hist[i].wsum = 0.0;
  }

  for (int k = 0; k < n_wedges; k++){
    for (int i = 0; i < masksz; i++){
      for (int j = 0; j < masksz; j++)
      {
        int index = k*masksz*masksz+j*masksz+i;
        if (mask[index] > 0) {
          AddWeight(image, mask[index], r-masksz+i,   c+j,          hist + k*NBINS);
          AddWeight(image, mask[index], r-j-1,        c-masksz+i,   hist + (n_wedges+k)*NBINS);
          AddWeight(image, mask[index], r+masksz-i-1, c-j-1,        hist + (2*n_wedges+k)*NBINS);
          AddWeight(image, mask[index], r+j,          c+masksz-i-1, hist + (3*n_wedges+k)*NBINS);
        }
      }
    }
  }

}


//: compute the optimal orientation and edge strength from the vector of edge strengths
//  at computed at discrete orientations
void compute_strength_and_orient(double* dist, int n_orient, double& strength, double& orientation)
{
  double wedgesize = vnl_math::pi/n_orient;

  // Compute Minimum and Maximum EMD values
  double maxEMD = 0.0, minEMD = 1.0;
  int strindex = 0, abindex = 0;
  for (int i = 0; i < n_orient; i++) {
    if (dist[i] > maxEMD) {
      maxEMD = dist[i];
      strindex = i;
    }
    if (dist[i] < minEMD) {
      minEMD = dist[i];
      abindex = i;
    }
  }

  // Fit a parabola to the edge strengths at different orientations to 
  // determine the subpixel orientation and max edge strength.
  
  // The strength and orientation of an edge lie not at the
  // maximum EMD value but rather at the vertex of the parabola that runs
  // through the maximum and the two points on either side.  
  
  // The first
  // computation of orientation assumes the maximum is the y-intercept.  
  // After computing the strength, we adjust the orientation.  
  
  double maxEMDori = strindex * wedgesize;

  double a = dist[strindex];
  double b = dist[(strindex+n_orient-1) % n_orient];
  double c = dist[(strindex+n_orient+1) % n_orient];

  double d = (b + c - 2 * a);
  if (vcl_fabs(d) > 1e-3) { //not degenerate
    double x = (wedgesize/2)*(b - c)/d;
    strength = a + x*(c - b)/(2*wedgesize) + x*x*d/(2*wedgesize*wedgesize);
    orientation = vcl_fmod(maxEMDori + x + 2*vnl_math::pi, vnl_math::pi);
  } 
  else { // Uncertainty abounds
    strength = a;
    orientation = maxEMDori;
  }
}

//: Compute intensity histogram gradient using the compass operator
// Note: # of orientations = 2*n_wedges
void dbdet_compute_compass_gradient( vil_image_view<vxl_byte>& image, int spacing, int n_wedges, 
                                     int weight_type, double sigma, int dist_op, bool SG_filter,
                                     vil_image_view<double>& hist_grad,
                                     vil_image_view<double>& hist_ori)
{
  //convert to grayscale
  vil_image_view<vxl_byte> img;
  if(image.nplanes() == 3){
    vil_convert_planes_to_grey(image, img );
  }
  else if (image.nplanes() == 1){
    img = image;
  }

  // determine some relevant parameters
  int n_orient = 2*n_wedges; //number of orientations
  int masksz = (int) vcl_ceil(3 * sigma); //mask size

  // allocate space for histogram gradients at various orientations
  vcl_vector<vil_image_view<double> > hist_dist(n_orient);
  for (int i=0; i<n_orient; i++){
    hist_dist[i].set_size(img.ni(), img.nj());
    hist_dist[i].fill(0.0);
  }

  //create the wedge masks
  double *mask, *masksum;
  double wedge_wt = CreateMask(sigma, n_wedges, masksz, weight_type, &mask, &masksum);

  //allocate space for the histograms
  dbdet_bin* wHist = new dbdet_bin[4*n_wedges*NBINS];       // All wedge histograms
  dbdet_signature hist1, hist2;         // Semi-circle histograms
  dbdet_signature hist1norm, hist2norm; // normalized histograms

  //loop over all the pixels in the image
  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
    {
      //Create wedge histograms (ouput in hist)
      CreateWedgeHistograms(img, mask, y, x, masksz, n_wedges, wHist);

      //Compute initial histogram sums (i.e., or the first orientation)
      //Note: half of the wedges contribute towards one histogram 
      //      and the rest contribute towards the other
      for (int i = 0; i < NBINS; i++) 
      {
        hist1.bins[i].weight = 0.0;
        hist1.bins[i].wsum = 0.0;
        for (int j = 0; j < 2*n_wedges; j++){
          hist1.bins[i].weight += wHist[j * NBINS + i].weight;
          hist1.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        hist2.bins[i].weight = 0.0;
        hist2.bins[i].wsum = 0.0;
        for (int j = 2*n_wedges; j < 4*n_wedges; j++){
          hist2.bins[i].weight += wHist[j * NBINS + i].weight;
          hist2.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }
      }

      //Loop over every orientation
      for (int i = 0; i < n_orient; i++) {

        // Normalize the histograms
        for (int j = 0; j < NBINS; j++) {

          hist1norm.bins[j].value  = hist1.bins[j].wsum / hist1.bins[j].weight;
          hist2norm.bins[j].value  = hist2.bins[j].wsum / hist2.bins[j].weight;

          // Normalize by the number of wedges 
          hist1norm.bins[j].weight = hist1.bins[j].weight / (wedge_wt * n_orient);
          hist2norm.bins[j].weight = hist2.bins[j].weight / (wedge_wt * n_orient);
        }

        //compute distance between the normalized histograms
        double d = 0.0;
        if (dist_op==0)
          d = dbdet_chi_sq_dist(hist1norm.bins, hist2norm.bins); //compute chi^2 dist
        else if (dist_op==1)
          d = dbdet_bhat_dist(hist1norm.bins, hist2norm.bins);   //compute Bhattacharya dist
        else if (dist_op==2)
          d = dbdet_gray_EMD(hist1norm.bins, hist2norm.bins);     //compute EMD

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
          for (int j = 0; j < NBINS; j++) 
          {
            hist1.bins[j].weight += - wHist[i * NBINS + j].weight                                //remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight;   //add this wedge

            hist1.bins[j].wsum   += - wHist[i * NBINS + j].wsum                                  //remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum;     //add this wedge

            hist2.bins[j].weight += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight    //remove this wedge
                                    + wHist[i * NBINS + j].weight;                               //add this wedge

            hist2.bins[j].wsum   += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum      //remove this wedge
                                    + wHist[i * NBINS + j].wsum;                                 //add this wedge
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
  hist_ori.set_size(img.ni(), img.nj());
  hist_ori.fill(0.0);
  hist_grad.set_size(img.ni(), img.nj());
  hist_grad.fill(0.0);

  //Filter the responses at each orientation using Savistzky-Golay filtering
  //  Allocate space for the filtered responses
  vcl_vector<vil_image_view<double> > filt_hist_dist(n_orient);

  if (SG_filter){
    for (int i=0; i<n_orient; i++){
      filt_hist_dist[i].set_size(img.ni(), img.nj());
      filt_hist_dist[i].fill(0.0);

      double theta = vnl_math::pi/double(n_orient) * double(i);

      Savitzky_Golay_filter_2d(hist_dist[i], filt_hist_dist[i], 2, masksz, theta);
    }
  }

  //loop over all the pixels in the image to compute NMS over orientations
  double* dist = new double[n_orient];

  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
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
dbdet_edgemap_sptr dbdet_detect_compass_edges(vil_image_view<vxl_byte>& image, int spacing, int n_wedges, 
                                              int weight_type, double sigma, int dist_op, bool SG_filter, double threshold,
                                              bool third_order, vil_image_view<double>& hist_grad,  bool output_orientation_map)
{
  //convert to grayscale
  vil_image_view<vxl_byte> img;
  if(image.nplanes() == 3){
    vil_convert_planes_to_grey(image, img );
  }
  else if (image.nplanes() == 1){
    img = image;
  }

  // determine some relevant parameters
  int n_orient = 2*n_wedges; //number of orientations
  int masksz = (int) vcl_ceil(3 * sigma); //mask size

  // allocate space for the histograms at various orientations
  vcl_vector<vbl_array_2d<vcl_pair<dbdet_signature, dbdet_signature> > > compass_sigs(n_orient); 
  for (int i=0; i<n_orient; i++){
    compass_sigs[i].resize(img.ni(), img.nj());
  }

  // allocate space for histogram gradients at various orientations
  vcl_vector<vil_image_view<double> > hist_dist(n_orient);
  for (int i=0; i<n_orient; i++){
    hist_dist[i].set_size(img.ni(), img.nj());
    hist_dist[i].fill(0.0);
  }

  //create the wedge masks
  double *mask, *masksum;
  double wedge_wt = CreateMask(sigma, n_wedges, masksz, weight_type, &mask, &masksum);

  //allocate space for the histograms
  dbdet_bin* wHist = new dbdet_bin[4*n_wedges*NBINS];       // All wedge histograms
  dbdet_signature hist1, hist2;         // Semi-circle histograms
  dbdet_signature hist1norm, hist2norm; // normalized histograms

  dbdet_signature qhist1, qhist2, qhist3, qhist4;
  dbdet_signature qhist1_norm, qhist2_norm, qhist3_norm, qhist4_norm;

  //loop over all the pixels in the image
  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
    {
      //Create wedge histograms (ouput in hist)
      CreateWedgeHistograms(img, mask, y, x, masksz, n_wedges, wHist);

      //Compute initial histogram sums (i.e., or the first orientation)
      //Note: half of the wedges contribute towards one histogram 
      //      and the rest contribute towards the other
      for (int i = 0; i < NBINS; i++) 
      {
        hist1.bins[i].weight = 0.0;
        hist1.bins[i].wsum = 0.0;
        for (int j = 0; j < 2*n_wedges; j++){
          hist1.bins[i].weight += wHist[j * NBINS + i].weight;
          hist1.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        hist2.bins[i].weight = 0.0;
        hist2.bins[i].wsum = 0.0;
        for (int j = 2*n_wedges; j < 4*n_wedges; j++){
          hist2.bins[i].weight += wHist[j * NBINS + i].weight;
          hist2.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }
      }

      //compile quarter histograms
      for (int i = 0; i < NBINS; i++) 
      {
        /*qhist1.bins[i].weight = 0.0;
        qhist1.bins[i].wsum = 0.0;
        for (int j = 0; j < n_wedges; j++){
          qhist1.bins[i].weight += wHist[j * NBINS + i].weight;
          qhist1.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }*/

        qhist2.bins[i].weight = 0.0;
        qhist2.bins[i].wsum = 0.0;
        for (int j = n_wedges; j < 2*n_wedges; j++){
          qhist2.bins[i].weight += wHist[j * NBINS + i].weight;
          qhist2.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        qhist3.bins[i].weight = 0.0;
        qhist3.bins[i].wsum = 0.0;
        for (int j = 2*n_wedges; j < 3*n_wedges; j++){
          qhist3.bins[i].weight += wHist[j * NBINS + i].weight;
          qhist3.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }

        /*qhist4.bins[i].weight = 0.0;
        qhist4.bins[i].wsum = 0.0;
        for (int j = 3*n_wedges; j < 4*n_wedges; j++){
          qhist4.bins[i].weight += wHist[j * NBINS + i].weight;
          qhist4.bins[i].wsum   += wHist[j * NBINS + i].wsum;
        }*/
      }

      //Loop over every orientation
      for (int i = 0; i < n_orient; i++) {

        // Normalize the histograms
        for (int j = 0; j < NBINS; j++) {
          hist1norm.bins[j].wsum  = hist1.bins[j].wsum;
          hist2norm.bins[j].wsum  = hist2.bins[j].wsum;

          hist1norm.bins[j].value  = hist1.bins[j].wsum / hist1.bins[j].weight;
          hist2norm.bins[j].value  = hist2.bins[j].wsum / hist2.bins[j].weight;

          // Normalize by the number of wedges 
          hist1norm.bins[j].weight = hist1.bins[j].weight / (wedge_wt * n_orient);
          hist2norm.bins[j].weight = hist2.bins[j].weight / (wedge_wt * n_orient);

          //normalize quarter histograms
          qhist1_norm.bins[j].wsum  = qhist1.bins[j].wsum;
          qhist2_norm.bins[j].wsum  = qhist2.bins[j].wsum;
          qhist3_norm.bins[j].wsum  = qhist3.bins[j].wsum;
          qhist4_norm.bins[j].wsum  = qhist4.bins[j].wsum;

          //qhist1_norm.bins[j].value  = qhist1.bins[j].wsum / qhist1.bins[j].weight;
          qhist2_norm.bins[j].value  = qhist2.bins[j].wsum / qhist2.bins[j].weight;
          qhist3_norm.bins[j].value  = qhist3.bins[j].wsum / qhist3.bins[j].weight;
          //qhist4_norm.bins[j].value  = qhist4.bins[j].wsum / qhist4.bins[j].weight;

          //qhist1_norm.bins[j].weight = qhist1.bins[j].weight / (wedge_wt * n_orient);
          qhist2_norm.bins[j].weight = qhist2.bins[j].weight / (wedge_wt * n_orient);
          qhist3_norm.bins[j].weight = qhist3.bins[j].weight / (wedge_wt * n_orient);
          //qhist4_norm.bins[j].weight = qhist4.bins[j].weight / (wedge_wt * n_orient);
        }

        //record the normalized histograms at the current pixel
        //compass_sigs[i](x,y) = vcl_pair<dbdet_signature, dbdet_signature>(hist1norm, hist2norm);

        //record the normalized quarter histograms at the current pixel
        compass_sigs[i](x,y) = vcl_pair<dbdet_signature, dbdet_signature>(qhist2_norm, qhist3_norm);

        //compute distance between the normalized histograms
        double d = 0.0;
        if (dist_op==0)
          d = dbdet_chi_sq_dist(hist1norm.bins, hist2norm.bins); //compute chi^2 dist
        else if (dist_op==1)
          d = dbdet_bhat_dist(hist1norm.bins, hist2norm.bins);   //compute Bhattacharya dist
        else if (dist_op==2)
          d = dbdet_gray_EMD(hist1norm.bins, hist2norm.bins);     //compute EMD

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
          for (int j = 0; j < NBINS; j++) 
          {
            hist1.bins[j].weight += - wHist[i * NBINS + j].weight                                //remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight;   //add this wedge

            hist1.bins[j].wsum   += - wHist[i * NBINS + j].wsum                                  //remove this wedge
                                    + wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum;     //add this wedge

            hist2.bins[j].weight += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].weight    //remove this wedge
                                    + wHist[i * NBINS + j].weight;                               //add this wedge

            hist2.bins[j].wsum   += - wHist[((i+n_orient) % (4*n_wedges)) * NBINS + j].wsum      //remove this wedge
                                    + wHist[i * NBINS + j].wsum;                                 //add this wedge

            //update the quarter histograms
            //qhist1.bins[j].weight += - wHist[i * NBINS + j].weight                                //remove this wedge
            //                         + wHist[((i+n_wedges) % (4*n_wedges)) * NBINS + j].weight;   //add this wedge

            qhist2.bins[j].weight += - wHist[((i+  n_wedges) % (4*n_wedges)) * NBINS + j].weight  //remove this wedge
                                     + wHist[((i+2*n_wedges) % (4*n_wedges)) * NBINS + j].weight; //add this wedge

            qhist3.bins[j].weight += - wHist[((i+2*n_wedges) % (4*n_wedges)) * NBINS + j].weight  //remove this wedge
                                     + wHist[((i+3*n_wedges) % (4*n_wedges)) * NBINS + j].weight; //add this wedge

            //qhist4.bins[j].weight += - wHist[((i+3*n_wedges) % (4*n_wedges)) * NBINS + j].weight  //remove this wedge
            //                         + wHist[i * NBINS + j].weight;                               //add this wedge

            //qhist1.bins[j].wsum   += - wHist[i * NBINS + j].wsum                                  //remove this wedge
            //                         + wHist[((i+n_wedges) % (4*n_wedges)) * NBINS + j].wsum;     //add this wedge

            qhist2.bins[j].wsum   += - wHist[((i+  n_wedges) % (4*n_wedges)) * NBINS + j].wsum    //remove this wedge
                                     + wHist[((i+2*n_wedges) % (4*n_wedges)) * NBINS + j].wsum;   //add this wedge

            qhist3.bins[j].wsum   += - wHist[((i+2*n_wedges) % (4*n_wedges)) * NBINS + j].wsum    //remove this wedge
                                     + wHist[((i+3*n_wedges) % (4*n_wedges)) * NBINS + j].wsum;   //add this wedge

            //qhist4.bins[j].wsum   += - wHist[((i+3*n_wedges) % (4*n_wedges)) * NBINS + j].wsum    //remove this wedge
            //                         + wHist[i * NBINS + j].wsum;                                 //add this wedge
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
  vil_image_view<double> hist_ori(img.ni(), img.nj());
  hist_grad.set_size(img.ni(), img.nj());
  hist_grad.fill(0.0);

  //Filter the responses at each orientation using Savistzky-Golay filtering
  //  Allocate space for the filtered responses
  vcl_vector<vil_image_view<double> > filt_hist_dist(n_orient);

  if (SG_filter){
    for (int i=0; i<n_orient; i++){
      filt_hist_dist[i].set_size(img.ni(), img.nj());
      filt_hist_dist[i].fill(0.0);

      double theta = vnl_math::pi/double(n_orient) * double(i);

      Savitzky_Golay_filter_2d(hist_dist[i], filt_hist_dist[i], 2, masksz, theta);
    }
  }

  //loop over all the pixels in the image to compute NMS over orientations
  double* dist = new double[n_orient];

  for (unsigned x = masksz; x < img.ni()-masksz; x+= spacing){
    for (unsigned y = masksz; y < img.nj()-masksz; y+=spacing)
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
  vil_image_view<double> dx(img.ni(), img.nj());
  vil_image_view<double> dy(img.ni(), img.nj());

  //get pointers to the data
  double* Ori = hist_ori.top_left_ptr();
  double* Gx = dx.top_left_ptr();
  double* Gy = dy.top_left_ptr();

  for(unsigned long i=0; i<hist_ori.size(); i++){
    Gx[i] = vcl_sin(Ori[i]);
    Gy[i] = vcl_cos(Ori[i]);
  }

  //the edgemap
  dbdet_edgemap_sptr edge_map = new dbdet_edgemap(img.ni(), img.nj());

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

    //TODO: when spacing>1, the NMS has to be done on the coarser grid

    dbdet_nms NMS(dbdet_nms_params(threshold, dbdet_nms_params::PFIT_3_POINTS), dx, dy, hist_grad);
    NMS.apply(true, loc, orientation, mag, d2f);
    
    //compute third-order orientation
    vcl_vector<double> TO_orientation(loc.size());

    if (third_order)
    {
      // The components of the gradient vector of the compass (with an ambiguity of pi)
      vil_image_view<double> Dx(img.ni(), img.nj());
      vil_image_view<double> Dy(img.ni(), img.nj());

      //get pointers to the data
      double* Ori = hist_ori.top_left_ptr();
      double* H = hist_grad.top_left_ptr();
      double* ddx = Dx.top_left_ptr();
      double* ddy = Dy.top_left_ptr();

      for(unsigned long i=0; i<hist_ori.size(); i++){
        ddx[i] = H[i]*vcl_sin(vcl_fmod(Ori[i]+vnl_math::pi_over_2, vnl_math::pi));
        ddy[i] = H[i]*vcl_cos(vcl_fmod(Ori[i]+vnl_math::pi_over_2, vnl_math::pi));
      }

      //for each edge, compute all the gradients to compute the new orientation
      vcl_vector<double> Hx, Hy, Hxx, Hxy, Hyy, Ix, Iy, Ixx, Ixy, Iyy;

      dbdet_subpix_convolve_2d(hist_grad, loc, Hx,  dbdet_Gx_kernel(0.7),  double(), 0);
      dbdet_subpix_convolve_2d(hist_grad, loc, Hy,  dbdet_Gy_kernel(0.7),  double(), 0);
      dbdet_subpix_convolve_2d(hist_grad, loc, Hxx, dbdet_Gxx_kernel(0.7), double(), 0);
      dbdet_subpix_convolve_2d(hist_grad, loc, Hxy, dbdet_Gxy_kernel(0.7), double(), 0);
      dbdet_subpix_convolve_2d(hist_grad, loc, Hyy, dbdet_Gyy_kernel(0.7), double(), 0);

      dbdet_subpix_convolve_2d(Dx, loc, Ix,  dbdet_G_kernel(0.7),  double(), 0);
      dbdet_subpix_convolve_2d(Dy, loc, Iy,  dbdet_G_kernel(0.7),  double(), 0);
      dbdet_subpix_convolve_2d(Dx, loc, Ixx, dbdet_Gx_kernel(0.7), double(), 0);
      dbdet_subpix_convolve_2d(Dy, loc, Ixy, dbdet_Gx_kernel(0.7), double(), 0);
      dbdet_subpix_convolve_2d(Dy, loc, Iyy, dbdet_Gy_kernel(0.7), double(), 0);

      //Now, compute and update each edge with its new orientation
      for (unsigned i=0; i<loc.size();i++)
      {
        // compute [Fx, Fy] at zero crossings (second order equations on P = |hist-grad I|)
        double Fx = Hx[i]*Ixx[i] + Hxx[i]*Ix[i] + Hy[i]*Ixy[i] + Hxy[i]*Iy[i];
        double Fy = Hx[i]*Ixy[i] + Hxy[i]*Ix[i] + Hy[i]*Iyy[i] + Hyy[i]*Iy[i];

        double F_mag = vcl_sqrt(Fx*Fx + Fy*Fy);

        //save new orientation (tangent to the level set) is orthogonal to the gradient
        TO_orientation[i] = dbdet_angle0To2Pi(vcl_atan2(Fx/F_mag, -Fy/F_mag));
      }
    }

    //create the edgemap from the tokens collected from NMS
    for (unsigned i=0; i<loc.size(); i++)
    {
      //store the signatures of the two halves of the closest compass operator as the right and left signatures
      int xx = dbdet_round(loc[i].x());
      int yy = dbdet_round(loc[i].y());
      int oo = (dbdet_round(n_orient*orientation[i]/vnl_math::pi) % n_orient); //modulo the # of orientations

      dbdet_appearance* lapp = new dbdet_gray_signature(compass_sigs[oo](xx,yy).first);
      dbdet_appearance* rapp = new dbdet_gray_signature(compass_sigs[oo](xx,yy).second);

      //create an edgel with the appearance information
      if (third_order)
        edge_map->insert(new dbdet_edgel(loc[i], TO_orientation[i], mag[i], d2f[i], -1, lapp, rapp), xx, yy);
      else
        edge_map->insert(new dbdet_edgel(loc[i], orientation[i], mag[i], d2f[i], -1, lapp, rapp), xx, yy);
    }
  }

  return edge_map;

}


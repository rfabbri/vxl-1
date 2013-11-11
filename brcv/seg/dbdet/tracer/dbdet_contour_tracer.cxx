#include "dbdet_contour_tracer.h"
#define dbdet_contour_tracer_internal_code 1
#include "dbdet_contour_tracer_core.h"
#include <vil/vil_convert.h>
#include <vil/vil_print.h>
#include <bil/algo/bil_edt.h>
#include <dbnl/algo/dbnl_eno_image.h>
#include <dbnl/algo/dbnl_eno_zerox_image.h>
#include <bdgl/bdgl_curve_algs.h>

//#include <vcl_stdio.h>
//#include <vcl_stdlib.h>


static int 
trace_eno_zero_xings(const vil_image_view<double> &img /*surface*/, Tracer *Tr);

static void free_data_tracer(Tracer *Tr);

static void decode_old_tracer(
    const Tracer &trac, 
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > &contours,
    unsigned &idx_of_max_contour
    );

//: Main tracer routine. 
//
bool dbdet_contour_tracer::
trace(const vil_image_view<bool> &img) 
{
  bool stat;

  if (!img.is_contiguous() || img.istep() != 1) {
    vcl_cerr << "dbnl_eno_image: only contigous row-wise (col,row) images supported\n";
    return false;
  }

  vil_image_view<unsigned> img_uint;
  vil_convert_cast<bool, unsigned> (img,img_uint);

  vil_image_view<float> s_edt;

  // 1 - signed DT
  stat = bil_edt_signed(img_uint,s_edt);

  vil_image_view<double> s_edt_double;
  vil_convert_cast<float, double> (s_edt,s_edt_double);

  // 2 - curvature smoothing
  curvature_smoothing(s_edt_double.top_left_ptr(), 
      curvature_smooth_beta_ , curvature_smooth_nsteps_, 0, img.nj(),img.ni());
  
//  vil_print_all(vcl_cout, s_edt_double);


  // 3 - trace_eno_zero_xings
  Tracer trac;
  unsigned num_of_contours = trace_eno_zero_xings(s_edt_double, &trac);

  // 4 - Translate old Tracer structure into our contours_ vector

  contours_.resize(num_of_contours);
  decode_old_tracer(trac,contours_,index_of_max_contour_);
  
  // 5 - Smooth the largest contour using Gaussian smoothing 
  if (!contours_.empty() && sigma_ > 0.0) 
  {
    vcl_vector<vsol_point_2d_sptr>& contour = contours_[index_of_max_contour_];
    vcl_vector<vgl_point_2d<double> > curve;
    for (unsigned i = 0; i<contour.size(); i++) 
    {
      curve.push_back(vgl_point_2d<double>(contour[i]->x(), contour[i]->y()));
    }
    bdgl_curve_algs::smooth_curve(curve, sigma_);
    curve.erase(curve.begin());
    curve.erase(curve.begin());
    curve.erase(curve.begin());
    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);
    curve.erase(curve.end()-1);
 
    assert(curve.size() == contour.size());
    for (unsigned i = 0; i<curve.size(); i++) {
      contour[i]->set_x(curve[i].x());
      contour[i]->set_y(curve[i].y());
    }
  }
 
  free_data_tracer(&trac);

  return true;
}





// ------------------------------------------------------------------
//: Contour tracing on a signed euclidean distance transform image (or a
// embedding surface image in which the zero-levelset defines the contours to
// be traced.
bool dbdet_contour_tracer::
trace_sedt_image(const vil_image_view<float >& s_edt)
{
  unsigned nj = s_edt.nj();
  unsigned ni = s_edt.ni();

  // convert to type "double"
  vil_image_view<double> s_edt_double;
  vil_convert_cast<float, double> (s_edt,s_edt_double);

  // curvature smoothing
  curvature_smoothing(s_edt_double.top_left_ptr(), 
      curvature_smooth_beta_ , curvature_smooth_nsteps_, 0, nj, ni);

  // trace_eno_zero_xings
  Tracer trac;
  unsigned num_of_contours = trace_eno_zero_xings(s_edt_double, &trac);

  // Translate old Tracer structure into our contours_ vector
  contours_.resize(num_of_contours);
  decode_old_tracer(trac,contours_,index_of_max_contour_);
  free_data_tracer(&trac);

  return true;
}








//: Writes to a file of points with ".con" extension. 
// 
// \param[in] filename  : the name of the image file. This is used to construct
// the output name by removing the extension and appending additional info and a
// '.con' extension.
// \param[in] i : index of contour to be written
//
bool dbdet_contour_tracer::
output_con_file(const vcl_string &filename, unsigned idx)
{
    char strtemp1[100];
    char strtemp2[100];

    unsigned j;
    for ( j=0; j<filename.size() && filename[j] != '.'; j++ )
      ;
    if (j==filename.size())
      j--;
      
    strcpy(strtemp1, filename.c_str());
    strtemp1[j]= '\0';
    sprintf(strtemp2, "%s-points-%d.con", strtemp1,idx+1);
    vcl_cout << "Created " << strtemp2 << vcl_endl;


    vcl_ofstream confile;
    confile.open(strtemp2);

    confile << "CONTOUR\nCLOSE\n" << contours_[idx].size() << vcl_endl;
    for (unsigned k=0; k < contours_[idx].size(); ++k)
      confile << contours_[idx][k]->x() << " " << contours_[idx][k]->y() << " ";

    confile.close();
    return true;
}

int 
trace_eno_zero_xings(const vil_image_view<double> &img /*surface*/, Tracer *ptrac)
{
  void allocate_data_tracer(Tracer *ptrac, int size);
  void subpixel_contour_tracer_all(Tracer *ptrac, Xings *xings, int height, int width);

  unsigned y,x,pos,i;
  int no_of_contours,pts_ina_contour;
  Xings xings;
  unsigned height=img.nj(), width=img.ni();



  { // begin block

    vil_image_view<unsigned> label;
    label.set_size(width,height);
    label.fill(15);

    dbnl_eno_image *eno_image = new dbnl_eno_image();
    eno_image->interpolate(&img);

    dbnl_eno_zerox_image eno_zeros(*eno_image);
    eno_zeros.assign_labels(img,label);

    label.clear();
    delete eno_image;

    xings.vert  = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
    xings.horiz = (XingsLoc *) calloc(height*width,sizeof(XingsLoc));
    

    // Copy zero crossing info from dbnl_eno_zerox_image to Xings structure
    i=0; 
    for (x=0; x<width; ++x) {
      for (y=0; y<height; ++y) {
        pos = y*width+x;

        xings.horiz[pos].loc[1] = NOXING;
        xings.vert[pos].loc[1]  = NOXING;
        xings.horiz[pos].loc[0] = NOXING;
        xings.vert[pos].loc[0]  = NOXING;

#define label_assign(xinglabel,enolabel) xinglabel = \
        (enolabel==dbnl_eno_zerox_label::unlabeled)? (int)UNLABELLED:(int)enolabel;

        if ( (x < width-1) && eno_zeros.horiz(x,y).number()) {
          xings.horiz[pos].loc[0]   =  eno_zeros.horiz(x,y).location(0);
          label_assign(xings.horiz[pos].label[0] ,  eno_zeros.horiz_labels(x,y)[0]);

          if (eno_zeros.horiz(x,y).number() == 2) {
            xings.horiz[pos].loc[1]   =  eno_zeros.horiz(x,y).location(1);
            label_assign(xings.horiz[pos].label[1] ,  eno_zeros.horiz_labels(x,y)[1]);
          } else
            xings.horiz[pos].cnt = 1;
        }

        if ((y < height-1) && eno_zeros.vert(x,y).number()) {
          xings.vert[pos].loc[0]   =  eno_zeros.vert(x,y).location(0);
          label_assign(xings.vert[pos].label[0],  eno_zeros.vert_labels(x,y)[0]);

          if (eno_zeros.vert(x,y).number() == 2) {
            xings.vert[pos].loc[1]   =  eno_zeros.vert(x,y).location(1);
            label_assign(xings.vert[pos].label[1],  eno_zeros.vert_labels(x,y)[1]);
          } else
            xings.vert[pos].cnt = 1;
        }
      }
    }
#undef label_assign

  } // end block

  /*printf("Done Copying to Tek's Structure\n");
    fflush(stdout);*/


  /* allocate memory for tracer */ 
  allocate_data_tracer(ptrac,img.size());

  //printf("Going to Trace Points\n");
  subpixel_contour_tracer_all(ptrac, &xings, height, width); 
  //printf("Done Trace Points\n");

  i=0;
  no_of_contours=0; pts_ina_contour=0;
  while(ptrac->vert[i] != ENDOFLIST && ptrac->horiz[i] != ENDOFLIST) {
    pts_ina_contour=0;
    while(ptrac->vert[i] != ENDOFCONTOUR && ptrac->horiz[i] != ENDOFCONTOUR) {
      pts_ina_contour++; i++;
    }
    ptrac->length[no_of_contours] = pts_ina_contour;
    i++; no_of_contours++;
  }
  //printf("Found %d contour(s)\n",no_of_contours);
 
  free(xings.vert); free(xings.horiz); 

  return no_of_contours;
}

/**********************************************************************
  Description: Free data allocated for tracer.
  Input : Tr - Tracer structure.
**********************************************************************/
void free_data_tracer(Tracer *Tr)
{
  if (Tr->vert != NULL)
    free(Tr->vert);
  if (Tr->horiz != NULL)
    free(Tr->horiz);
//  if (Tr->label != NULL)
//    free(Tr->label);
//  if (Tr->id != NULL)
//    free(Tr->id);
  if (Tr->length != NULL)
    free(Tr->length);
}

//: translates old tracer structure into our class's vector of contours
// note: 
//  index_of_max_contour has to be initialized to 0
//  contours vector has to be in proper size (#of contours), although not with
//  the contours themselves 
void decode_old_tracer(
    const Tracer &trac, 
    vcl_vector<vcl_vector<vsol_point_2d_sptr> > &contours,
    unsigned &index_of_max_contour
    )
{
  unsigned i=0, ncontour=0, k, contour_ini, contour_size;

  while (trac.vert[i] != ENDOFLIST) {
    assert(trac.horiz[i] != ENDOFLIST);
    contour_ini = i;
    while (trac.vert[i] != ENDOFCONTOUR) {
      assert(trac.horiz[i] != ENDOFCONTOUR);
      ++i;
    }
    assert(trac.horiz[i] == ENDOFCONTOUR);

    contour_size = i - contour_ini;
    contours[ncontour].resize(contour_size);

    for (k=0; k < contour_size; ++k) {
        double x = trac.horiz[contour_ini+k];
        double y = trac.vert[contour_ini+k];
        contours[ncontour][k] = new vsol_point_2d (x, y);
    }
    ++i; ++ncontour;
  }
  assert(trac.horiz[i] == ENDOFLIST);


  // finds largest contour using our new structure

  unsigned maxlen=0; 
  for (i=0; i < contours.size(); ++i) {
    if (contours[i].size() > maxlen) {
      maxlen = contours[i].size();
      index_of_max_contour = i;
    }
  }
}

// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vbl/vbl_array_1d.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_new.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_rectangle_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_vil_float_ops.h>
#include <dbinfo/dbinfo_feature_format.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <dbinfo/dbinfo_feature_base.h>
#include <dbinfo/dbinfo_intensity_feature.h>
#include <dbinfo/dbinfo_gradient_feature.h>
#include <dbinfo/dbinfo_observation_matcher.h>
static void test_features(int argc, char* argv[])
{
  //Construct a test image and polygonal boundary
  unsigned cols = 10, rows = 10;
  vsol_point_2d_sptr p0 = new vsol_point_2d(2,2);
  vsol_point_2d_sptr p1 = new vsol_point_2d(5,2);
  vsol_point_2d_sptr p2 = new vsol_point_2d(3.0,4.0);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);  verts.push_back(p2); 
  vsol_polygon_2d_sptr poly = new vsol_polygon_2d(verts);
  //region geometry test
  dbinfo_region_geometry_sptr rg = new dbinfo_region_geometry(cols, rows, poly);
  vcl_cout << "Number of points " << rg->size() << '\n';
  brip_roi_sptr roi = rg->roi();
  vcl_cout << "feature test image bounds [(" << roi->cmin(0) 
           << ' ' << roi->cmax(0) << ' ' << ")(" 
           << roi->rmin(0) << ' ' << roi->rmax(0) << ")]\n";

  TEST("region_geometry, roi ", rg&&rg->size()==6&&roi&&roi->cmax(0)==5, true);

  //Get the region points
  vcl_vector<vgl_point_2d<unsigned> > points;
  vcl_vector<bool> valid;
  for(unsigned i = 0; i<rg->size(); ++i)
    {
      vgl_point_2d<float> fp = rg->point(i);
      points.push_back(vgl_point_2d<unsigned>((unsigned)fp.x(), (unsigned)fp.y()));
      valid.push_back(true);
    }

  //Test image for data scan
  vil_image_view<unsigned char> image(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      image(c,r) = r*rows + c;
  vil_image_resource_sptr imgr = vil_new_image_resource_of_view(image);
  //
  //====================  Intensity Feature ================================
  //
  dbinfo_intensity_feature_sptr ifp = new dbinfo_intensity_feature();
  bool good_scan = ifp->scan(0, points, valid, imgr);
  TEST("dbinfo_intensity_feature::scan ", good_scan, true);

  //transposed image
  dbinfo_intensity_feature_sptr tifp = new dbinfo_intensity_feature();
  vil_image_view<unsigned char> t_image(rows, cols, 1);  
  vil_image_resource_sptr t_imgr = vil_new_image_resource_of_view(t_image);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      t_image(c,r) = image(r,c);
  good_scan = tifp->scan(1, points, valid, t_imgr);
  float info = dbinfo_observation_matcher::minfo(ifp->data(), tifp->data());
  vcl_cout << "Intensity mutual information " << info << '\n';
  TEST_NEAR("Test intensity mutual information ",
            info, 0.143156, 1e-7);


  //
  //====================  Gradient Feature ================================
  //
  //Test image for gradient scan
  cols = 31;  rows = 31;
  vil_image_view<unsigned char> gimage(cols, rows, 1);
  for(unsigned r = 0; r<rows; ++r)
    for(unsigned c = 0; c<cols; ++c)
      gimage(c,r) = r + c;
  vil_image_resource_sptr gimgr = vil_new_image_resource_of_view(gimage);
  //define a 5x5 window
  vsol_rectangle_2d* rect = new vsol_rectangle_2d(new vsol_point_2d(15, 15),
                                                  5, 5, 0);
  vsol_polygon_2d_sptr gpoly = rect;
  //construct a region geometry
  dbinfo_region_geometry_sptr geom = new dbinfo_region_geometry(cols, rows, gpoly);
  //get the roi
  brip_roi_sptr groi = geom->roi();
  //expand the roi by a margin
  brip_roi_sptr ex_roi = new brip_roi(*groi, 4.0f);

  //Get the image roi
  vil_image_resource_sptr roi_resource;
  bool good_image = brip_vil_float_ops::chip(gimgr, ex_roi, roi_resource);
  vil_image_view<unsigned char> cimage_roi = roi_resource->get_view();
  for(int r = 0; r<3; ++r)
    for(int c = 0; c<3; ++c)
      vcl_cout << "cimage( "<< c << ' ' << r << ")= " << (unsigned)cimage_roi(c,r) << '\n';
  

  //reference the scan points to the origin of the expanded roi
  unsigned n_gpts = geom->size();
  vcl_vector<vgl_point_2d<unsigned> > scan_points;
  valid.clear();
  for(unsigned i = 0; i<n_gpts; ++i)
    {
      vgl_point_2d<float> p = geom->point(i);
      vgl_point_2d<unsigned> gp((unsigned)p.x(), (unsigned)p.y()) , egp;
      egp.set(ex_roi->lc(gp.x()), ex_roi->lr(gp.y()));
      scan_points.push_back(egp);
      valid.push_back(true);
    }
  dbinfo_gradient_feature_sptr gfp = new dbinfo_gradient_feature();
  good_scan = gfp->scan(0, scan_points, valid, roi_resource);
  //gfp->store();
#if 0
  dbinfo_feature_data_base_sptr gfdb = gfp->data();
  dbinfo_feature_data<vbl_array_2d<float> >* gfd =
    dbinfo_feature_data<vbl_array_2d<float> >::ptr(gfdb);
  vbl_array_2d<float>& gv = gfd->data(0);

  dbinfo_feature_data_base_sptr cgfdb = gfp->current_data();
  dbinfo_feature_data<vbl_array_2d<float> >* cgfd =
    dbinfo_feature_data<vbl_array_2d<float> >::ptr(cgfdb);
  vbl_array_2d<float>& cgv = cgfd->data(0);
 
  good_data = true;
  for(unsigned i = 0; i<scan_points.size(); ++i)
    {
      vcl_cout << "P[" << i << "]=("<< scan_points[i].x() << ' ' 
               << scan_points[i].y() << ")   D[" << i << "]=( " << gv[i][0] << ' ' << gv[i][1]
               << ")   C[" << i << "]=( " << cgv[i][0] << ' ' 
               << cgv[i][1] <<  ")\n";
      good_data = good_data && gv[i][0]==cgv[i][0]&&gv[i][1]==cgv[i][1];
      good_data = good_data && vcl_fabs(gv[i][0]-1.0)<0.001 && vcl_fabs(gv[i][1]-1.0)<0.001;
    }
#endif
  TEST("dbinfo_gradient_feature::scan ", good_image&&good_scan, true);

  vcl_vector<vbl_array_2d<float> > idata;
  vbl_array_2d<float> vv(8,2,0.0f);
  vv[0][0]=1.0f;     vv[0][1]=0.0f;
  vv[1][0]=0.5f;   vv[1][1]=0.5f;
  vv[2][0]=0.0f;     vv[2][1]=1.0f;
  vv[3][0]=-0.5f;  vv[3][1]=0.5f;
  vv[4][0]=-1.0f;    vv[4][1]=0.0f;
  vv[5][0]=-0.5f;  vv[5][1]=-0.5f;
  vv[6][0]=0.0f;     vv[6][1]=-1.0f;
  vv[7][0]=0.5f;   vv[7][1]=-0.5f;
  idata.push_back(vv);
  vil_pixel_format pix_format = VIL_PIXEL_FORMAT_BYTE;
  unsigned nplanes = 1;
  dbinfo_feature_format f_format = DBINFO_GRADIENT_FEATURE;
  dbinfo_feature_data_base_sptr fd0 = 
    new dbinfo_feature_data<vbl_array_2d<float> >(pix_format, nplanes, f_format, idata);
  //create the same data
  dbinfo_feature_data_base_sptr fd1 = 
    new dbinfo_feature_data<vbl_array_2d<float> >(pix_format, nplanes, f_format, idata);
  float minf = dbinfo_observation_matcher::minfo(fd0, fd1);
  vcl_cout << "Gradient mutual information " << minf << '\n';
  TEST_NEAR("Test gradient mutual information ", minf, 3.0, 1e-6);

  //  Test Binary I/O
  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_feature_base_io.tmp");
  // Just to be sure we get the same value back
  ifp->set_margin(10);
  dbinfo_feature_base* fb = (dbinfo_feature_base*)(ifp.ptr());
  vsl_b_write(bp_out2, fb);
  bp_out2.close();

  vsl_b_ifstream bp_in2("test_feature_base_io.tmp");
  dbinfo_feature_base* fb_in = (dbinfo_feature_base*)0;
  vsl_b_read(bp_in2, fb_in);
  bp_in2.close();
  if(fb_in)
    vcl_cout << "recovered feature " << *fb_in << '\n';
  vpl_unlink ("test_feature_base_io.tmp");
  TEST("recover from generic feature base pointer ",
       fb_in->margin() , 10);
}
TESTMAIN_ARGS(test_features);

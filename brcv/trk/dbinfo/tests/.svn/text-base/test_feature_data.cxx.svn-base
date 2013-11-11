// This is brl/bseg/dbinfo/tests/test_tracking_face_2d.cxx
#include <testlib/testlib_test.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vbl/vbl_array_1d.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_pixel_format.h>
#include <dbinfo/dbinfo_feature_data_base.h>
#include <dbinfo/dbinfo_feature_data.h>
#include <vbl/vbl_array_2d.h>
static void test_feature_data(int argc, char* argv[])
{
  //Test construction and pointer casting
  dbinfo_feature_data<vbl_array_1d<float> >* fp=
    new dbinfo_feature_data<vbl_array_1d<float> >(VIL_PIXEL_FORMAT_UNKNOWN, 1, DBINFO_INTENSITY_FEATURE, 3);
  vcl_cout << "size of new feature " << fp->size() 
           << "  address " << fp << '\n';
  dbinfo_feature_data_base_sptr fbp = fp;
  dbinfo_feature_data<vbl_array_1d<float> >* nfp = dbinfo_feature_data<vbl_array_1d<float> >::ptr(fbp);
  vcl_cout << "size of re-casted feature data " << nfp->size()  
           << " address " << nfp << '\n';
  TEST("Cast preserves pointer equality",fp, nfp);

  vcl_cout << "Stream operator dbinfo_feature_data<vbl_array_1d<float> >: " 
           << *fp << '\n';

  //Test frame/index conversion and array_1d data retrieval
  vbl_array_1d<float> s10(1,10.1f), s15(1, 50.5f), s30(1, 100.3f);
  nfp->set_sample(10, s10, 0.1f);
  nfp->set_sample(15, s15, 0.2f);
  nfp->set_sample(30, s30, 0.4f);
  for(unsigned index = 0; index<3; ++index)
    vcl_cout << "frame[" << index << "]= " << nfp->frame(index) 
             << "  index = " << nfp->index(nfp->frame(index)) 
             << " w = " << nfp->w(nfp->frame(index)) << '\n';
  bool good = nfp->index(nfp->frame(2))==2&& (nfp->data(15))[0]==50.5;
  TEST("float data storage and retrieval", good , true);

  //Test vbl_array data retrieval
  dbinfo_feature_data<vbl_array_2d<float> >* fap=
    new dbinfo_feature_data<vbl_array_2d<float> >(VIL_PIXEL_FORMAT_UNKNOWN, 1,DBINFO_GRADIENT_FEATURE, 3);
  for(unsigned frame = 10; frame<40; frame+=10)
    {
      vbl_array_2d<float> a(2,5);
      for(unsigned c = 0; c<5; c++)
        {a[0][c]=float(c+frame); a[1][c]=float(c+5+frame);}
      fap->set_sample(frame, a);
    }
  for(unsigned frame = 10; frame<40; frame+=10)
    vcl_cout << fap->data(frame) << '\n';

  TEST("vbl_array data storage and retrieval", (fap->data(30))[1][4] , 39);

  vcl_cout << "Stream operator dbinfo_feature_data<vbl_array_2d<float> >: " 
           << *fap << '\n';

  //====== Test binary I/0=========
  // Test writing and reading a gradient feature
  vsl_b_ofstream bp_out("test_feature_data_io.tmp");
  TEST("Created test_feature_data_io.tmp for writing",(!bp_out), false);
  
  vsl_b_write(bp_out, fap);
  bp_out.close();


  vsl_b_ifstream bp_in("test_feature_data_io.tmp");
  TEST("Opened test_feature_data_io.tmp for reading",(!bp_in), false);

  dbinfo_feature_data<vbl_array_2d<float> > fdin;
  vsl_b_read(bp_in, fdin);
  bp_in.close();
  vcl_cout << "recovered feature data " << fdin << '\n';

  vpl_unlink ("test_feature_data_io.tmp");
  vcl_cout << "Recovered Map \n";
  for(unsigned frame = 10; frame<40; frame+=10)
    vcl_cout << fdin.data(frame) << '\n';

  vcl_cout << "fdin.data(30))[1][4](orig) = " << (fap->data(30))[1][4] << '\n';
  vcl_cout << "fdin.data(30))[1][4](rec) = " <<  (fdin.data(30))[1][4] << '\n';
  TEST("recovered feature data valid ", (fdin.data(30))[1][4] , 39);

  //Test writing the generic feature data pointer
  vsl_b_ofstream bp_out2("test_feature_data_io.tmp");
  dbinfo_feature_data_base* dfb = (dbinfo_feature_data_base*)fap;
  vsl_b_write(bp_out2, dfb);
  bp_out2.close();

  vsl_b_ifstream bp_in2("test_feature_data_io.tmp");
  dbinfo_feature_data_base* fdb_in = (dbinfo_feature_data_base*)0;
  vsl_b_read(bp_in2, fdb_in);
  bp_in2.close();
  if(fdb_in)
    vcl_cout << "recovered feature type " << fdb_in->is_a()<< ' ' << fdb_in->format() << '\n';
  vpl_unlink ("test_feature_data_io.tmp");
  TEST("recover from generic pointer ",
       fdb_in->format() , DBINFO_GRADIENT_FEATURE);
}
TESTMAIN_ARGS(test_feature_data);

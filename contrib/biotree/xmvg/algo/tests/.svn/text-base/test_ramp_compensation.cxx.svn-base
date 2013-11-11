//: 
// \file  test_ramp_compensation.cxx
// \brief  testing suite for ramp compensation
// \author    J.L. Mundy
// \date      May 26, 2006
// 
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vnl/vnl_int_2.h>
#include <vbl/vbl_array_2d.h>
#include <xmvg/xmvg_atomic_filter_2d.h>
#include <xmvg/xmvg_composite_filter_2d.h>
#include <xmvg/algo/xmvg_ramp_compensation.h>

static void test_ramp_compensation()
{

  vcl_cout << "-----------------testing ramp compensation ------------------\n";
  //Test a filter with even number of columns
  vbl_array_2d<float> edata(18, 15);
  for(unsigned j = 0; j<15; ++j)
    {
      for(unsigned i = 0; i<18; ++i)
        edata[i][j] = 0.0f;
      edata[1][j] = 0.00033f;
      edata[2][j] = 0.0022f;
      edata[3][j] = 0.011f;
      edata[4][j] = 0.0439f;
      edata[5][j] = 0.135f;
      edata[6][j] = 0.324f;
      edata[7][j] = 0.606f;
      edata[8][j] = 0.882f;
      edata[9][j] = 0.882f;
      edata[10][j] = 0.606f;
      edata[11][j] = 0.324f;
      edata[12][j] =0.135f;
      edata[13][j] =0.0439f;
      edata[14][j] =0.011f;
      edata[15][j] =0.0022f;
      edata[16][j] =0.00033f;
    }
  xmvg_atomic_filter_2d<float> filt_even_out, filt_even_in;
  vnl_int_2 even_size, loc;
  even_size[0]=18;   even_size[1]=15;  
  loc[0]=0;   loc[1]=0; 
  filt_even_in.set_size(even_size);
  filt_even_in.set_location(loc);
  filt_even_in.set_weights(edata);
  xmvg_ramp_compensation<float>(filt_even_in, filt_even_out);

  vcl_cout << "Compensated Even Gaussian Splat\n";
  for(unsigned i = 0; i<18; ++i)
    vcl_cout << filt_even_out[i][0] << ' ';
  vcl_cout << '\n';
  TEST_NEAR("Compensated Even Gaussian Splat", filt_even_out[9][0], 2.45672f, 0.001);
  //Test a filter with odd number of columns
  vbl_array_2d<float> odata(19, 15);
  for(unsigned j = 0; j<15; ++j)
    {
      for(unsigned i = 0; i<19; ++i)
        odata[i][j] = 0.0f;
      odata[2][j] = 0.0022f;
      odata[3][j] = 0.0111f;
      odata[4][j] = 0.0439f;
      odata[5][j] = 0.135f;
      odata[6][j] =0.324f;
      odata[7][j] =0.606f;
      odata[8][j] =0.882f;
      odata[9][j] =1.0f;
      odata[10][j] =0.882f;
      odata[11][j] =0.606f;
      odata[12][j] =0.324f;
      odata[13][j] =0.135f;
      odata[14][j] =0.0439f;
      odata[15][j] =0.0111f;
      odata[16][j] =0.0022f;
    }
  xmvg_atomic_filter_2d<float> filt_odd_out, filt_odd_in;
  vnl_int_2 odd_size;
  odd_size[0]=19;   odd_size[1]=15;  
  filt_odd_in.set_size(odd_size);

  filt_odd_in.set_location(loc);
  filt_odd_in.set_weights(odata);
  xmvg_ramp_compensation<float>(filt_odd_in, filt_odd_out);
  vcl_cout << "Compensated 0dd Gaussian Splat\n";
  for(unsigned i = 0; i<19; ++i)
    vcl_cout << filt_odd_out[i][0] << ' ';
  vcl_cout << '\n';
  TEST_NEAR("Compensated Odd Gaussian Splat", filt_odd_out[10][0], 1.87378f , 0.001);
  //Test a filter with odd number of columns but larger no of elements
  // to check out normalization
  vbl_array_2d<float> lodata(45, 15);
  for(unsigned j = 0; j<15; ++j)
    {
      for(unsigned i = 0; i<45; ++i)
        lodata[i][j] = 0.0f;
      lodata[15][j] = 0.0022f;
      lodata[16][j] = 0.0111f;
      lodata[17][j] = 0.0439f;
      lodata[18][j] = 0.135f;
      lodata[19][j] =0.324f;
      lodata[20][j] =0.606f;
      lodata[21][j] =0.882f;
      lodata[22][j] =1.0f;
      lodata[23][j] =0.882f;
      lodata[24][j] =0.606f;
      lodata[25][j] =0.324f;
      lodata[26][j] =0.135f;
      lodata[27][j] =0.0439f;
      lodata[28][j] =0.0111f;
      lodata[29][j] =0.0022f;
    }
  xmvg_atomic_filter_2d<float> filt_lodd_out, filt_lodd_in;
  vnl_int_2 lodd_size;
  lodd_size[0]=45;   lodd_size[1]=15;  
  filt_lodd_in.set_size(lodd_size);
  filt_lodd_in.set_location(loc);
  filt_lodd_in.set_weights(lodata);
  xmvg_ramp_compensation<float>(filt_lodd_in, filt_lodd_out);
  vcl_cout << "Compensated Large Odd Gaussian Splat\n";
  for(unsigned i = 15; i<29; ++i)
    vcl_cout << filt_lodd_out[i][0] << ' ';
  vcl_cout << '\n';

  //Test composite filter compensation
  vcl_vector<xmvg_atomic_filter_2d<float> > filts;
  filts.push_back(filt_odd_in);   filts.push_back(filt_odd_in);
  xmvg_composite_filter_2d<float> cmpfilts(filts), outfilts;
  xmvg_ramp_compensation<float>(cmpfilts, outfilts);
  xmvg_atomic_filter_2d<float> temp = outfilts.atomic_filter(1);
  vcl_cout << "Composite Compensated Result\n";
  for(unsigned i = 0; i<19; ++i)
    vcl_cout << temp[i][0] << ' ';
  vcl_cout << '\n';
  TEST_NEAR("test ramp compensated composite", temp[9][0], 2.45778f, 0.001);
  //Test full reconstruction 
  //The variation along z for the Gaussian 2nd derivative filter
  vcl_vector<float> S(15);
  S[0]=0;
  S[1]=0;
  S[2]=0;
  S[3]=0.000167733f;
  S[4]=0.00555455f;
  S[5]=0.0676683f;
  S[6]=0.303268f;
  S[7]=0.500005f;
  S[8]=0.303268f;
  S[9]=0.0676683f;
  S[10]=0.00555455f;
  S[11]=0.000167733f;
  S[12]=0;
  S[13]=0;
  S[14]=0;  
  //Define the spat for the Gaussian 2nd derivative filter
  vbl_array_2d<float> M(15,15);
  for(unsigned j = 0; j<15; ++j)
    {
      M[0][j]=S[j]*0;
      M[1][j]=S[j]*0;
      M[2][j]=S[j]*-0.0000447199f;
      M[3][j]=S[j]*-0.00251597f;
      M[4][j]=S[j]*-0.044436f;
      M[5][j]=S[j]*-0.203003f;
      M[6][j]=S[j]*0;
      M[7][j]=S[j]*0.500001f;
      M[8][j]=S[j]*0;
      M[9][j]=S[j]*-0.203003f;
      M[10][j]=S[j]*-0.044436f;
      M[11][j]=S[j]*-0.00251597f;
      M[12][j]=S[j]*-0.0000447199f;
      M[13][j]=S[j]*0;
      M[14][j]=S[j]*0;
    }
  xmvg_atomic_filter_2d<float> gf_out, gf_in;
   vnl_int_2 size;
  size[0]=15;   size[1]=15; 
 gf_in.set_size(size);
  gf_in.set_location(loc);
  gf_in.set_weights(M);
  xmvg_ramp_compensation<float>(gf_in, gf_out);
  vcl_cout <<"Compensated splat values vs x and z\n";
  vcl_cout << "z = 0\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gf_out[i][7] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 1\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gf_out[i][8] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 2\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gf_out[i][9] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 3\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gf_out[i][10] << ' ';
  vcl_cout << '\n';

  //Now simulate adding up the spats over the orbit
  //Divide 360 degrees into 0.1 degree increments
  vcl_cout << "Reconstructed Gaussian 2nd derivative response (positive half)\n";
  float xsum = 0.0f;
  for(float x = 0; x<=7.0f; x+=1.0f)
    {
      float sum = 0.0f;
      for(float theta = 0.0f; theta<360.0f; theta+=0.1f)
        {
          float theta_rad = 3.14159f*theta/180.0f;
          float fpos = x*vcl_cos(theta_rad)+7.0f;
          unsigned upos = static_cast<unsigned>(fpos);
          sum += gf_out[upos][7];
        }
      xsum += sum/3600;
      vcl_cout << "f(" << x << ")=" << sum/3600 << '\n';
    }
  vcl_cout << "Impulse at (7,7) " << gf_out[7][7] << '\n';
  //The cylinder impulse along the x axis
  vcl_cout << "Xsum = " << xsum << '\n';
  //The cylinder impulse along the z axis
  float zsum = 0.0f;
  for(unsigned j = 7; j<15; ++j)
    zsum += gf_out[7][j];
  vcl_cout << "Zsum = " << zsum << '\n';
  // the ratio zsum/xsum should be 2.0 but this is the highest resolution
  // filter possible, 1 sigma per pixel.
  TEST_NEAR("Xsum " , xsum, 3.10264, 0.001);
  TEST_NEAR("Zsum " , zsum, 4.35929, 0.001);
  //Test a size that is not factorable
  vnl_int_2 sizef, locf;
  sizef[0]=31;  sizef[1]=31;
  locf[0]=0;   locf[1]=0; 
  vbl_array_2d<float> Mf(31,31,0.0f);
  for(int j = 0; j<15; ++j)
    for(int i = 0; i<15; ++i)
      Mf[j+8][i+8] = M[j][i];

  xmvg_atomic_filter_2d<float> gfp_out, gfp_in;
  gfp_in.set_size(sizef);
  gfp_in.set_location(locf);
  gfp_in.set_weights(Mf);
  xmvg_ramp_compensation<float>(gfp_in, gfp_out);
  vcl_cout <<"Prime factor compensated splat values vs x and z\n";
  vcl_cout << "z = 0\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gfp_out[i+8][15] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 1\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gfp_out[i+8][16] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 2\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gfp_out[i+8][17] << ' ';
  vcl_cout << '\n';
  vcl_cout << "z = 3\n";
  for(unsigned i=0; i<15; ++i)
    vcl_cout << gfp_out[i+8][18] << ' ';
  vcl_cout << '\n';
  TEST_NEAR("primefactor size test", gfp_out[15][15], 2.48704, 0.001);
}

TESTMAIN(test_ramp_compensation);

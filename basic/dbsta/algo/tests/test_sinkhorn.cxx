#include <testlib/testlib_test.h>
#include <dbsta/algo/dbsta_sinkhorn.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


template <class T>
void test_sinkhorn_type(T tol, const vcl_string& type_name)
{

  vnl_matrix<T> M(4,3,T(0));
  vnl_vector<T> m_cols(4,T(0));
  vnl_vector<T> m_rows(3,T(0));



  M(0,0)=(T)1;M(0,1)=(T)2;M(0,2)=(T)3;
  M(1,0)=(T)4;M(1,1)=(T)5;M(1,2)=(T)6;
  M(2,0)=(T)7;M(2,1)=(T)8;M(2,2)=(T)9;
  M(3,0)=(T)10;M(3,1)=(T)11;M(3,2)=(T)12;

  m_cols[0]=(T)2;m_cols[1]=(T)5;m_cols[2]=(T)8;m_cols[3]=(T)11;

  m_rows[0]=(T)4;m_rows[1]=(T)5;m_rows[2]=(T)6;

  dbsta_sinkhorn<T> tester(M,m_cols,m_rows);
  tester.normalize(T(0.05));


  M=tester.M();
  m_cols=tester.mcols();
  m_rows=tester.mrows();

  //: computed using rangarajan's code in matlab
  vnl_matrix<T> m1(4,3);
  m1(0,0)=(T)0.0588;m1(0,1)=(T)0.0928;m1(0,2)=(T)0.1149;
  m1(1,0)=(T)0.0929;m1(1,1)=(T)0.0916;m1(1,2)=(T)0.0908;
  m1(2,0)=(T)0.1013;m1(2,1)=(T)0.0914;m1(2,2)=(T)0.0849;
  m1(3,0)=(T)0.1051;m1(3,1)=(T)0.0912;m1(3,2)=(T)0.0822;

  vnl_vector<T> mcols1(4);
  mcols1[0]=(T)0.7328;mcols1[1]=(T)0.7239;mcols1[2]=(T)0.7217;mcols1[3]=(T)0.7207;

  vnl_vector<T> mrows1(3);
  mrows1[0]=(T)0.6418;mrows1[1]=(T)0.6330;mrows1[2]=(T)0.6272;

 
   TEST(("M <"+type_name+">").c_str(), 
       (M - m1).array_one_norm() < tol, true);
   TEST(("mrows <"+type_name+">").c_str(), 
       (m_rows-mrows1).two_norm() < tol, true);
   TEST(("mcols <"+type_name+">").c_str(), 
       (m_cols-mcols1).two_norm() < tol, true);

}




MAIN( test_sinkhorn )
{
  START ("sinkhorn");
  test_sinkhorn_type(double(1e-3),"double");
  SUMMARY();
}



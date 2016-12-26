#include <stdio.h>
#include <stdlib.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <time.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

#define MPI   (3.14159265)
#define SMALL  (1.0E-4)

#include <bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <bsol/bsol_intrinsic_curve_2d.h>

#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <dbsol/algo/dbsol_geno.h>


int main(int argc, char *argv[]) { 
  double *x, *y;
  int N, NO;
  char strtemp1[100];
  char strtemp2[100];

  if (argc != 6) {
    printf("USAGE: subsampler <file.con> <1 if even sampling 0 if random sampling> <N> <id> <interp type: 0 if linear 1 if geno>\n");
    printf("N: number of points\n");
    printf("id: id of output contour, will be appended to the name\n");
    return 0;
  }

  vcl_string con_filename(argv[1]);
  int e = atoi(argv[2]);
  bool even = (e == 1 ? true : false);
  N = atoi(argv[3]);
  int id = 0;
  if (!even)
    id = atoi(argv[4]);
  int interp = atoi(argv[5]);
  bool linear_interp = (interp == 0 ? true : false);

  x = (double *)malloc(sizeof(double)*N);
  y = (double *)malloc(sizeof(double)*N);

  bsol_intrinsic_curve_2d_sptr curve_2d = new bsol_intrinsic_curve_2d;
  curve_2d->readCONFromFile( con_filename.c_str() );
  
  NO = curve_2d->size();
  vcl_cout << "Number of points in original curve: " << NO << vcl_endl;

  if (!even) {
    vcl_vector<int> values;

    srand(time(NULL));
    // pick N random distinct numbers in (0,..,NO-1)
    for (int i = 0; i<N; i++) {
      double r = ((double)rand()/(double)(RAND_MAX));
      int a = int(vcl_floor(r*(NO-1)+0.5f)); // round the value to nearest integer
      int tag = 0;
      //vcl_cout << "a: " << a << "\n";
      for (int j = 0; j<i; j++) {
        //vcl_cout << "values[" << j << "]: " << values[j] << "\n";
        if (a == values[j]) {
          vcl_cout << "HIT!!\n";
          i--;
          tag = 1;
          break;
        }
      }
      if (tag) continue;
      values.push_back(a);
    }

    vcl_cout << " values size: " << values.size() << " N: " << N << vcl_endl;

    //: Sort by the values.
    vcl_sort(values.begin(), values.end());
    
    //for (int i = 0; i<values.size(); i++) 
    //  vcl_cout << "values[" << i << "]: " << values[i] << vcl_endl;

    strcpy(strtemp1, con_filename.c_str());
    int i;
    for (i=0; strtemp1[i] != '.'; i++ ){}
    strtemp1[i]= 0;
    sprintf(strtemp2, "%s-random-%d-%02d", strtemp1, N, id);
    sprintf(strtemp2, "%s.con", strtemp2);

    vcl_cout << "output name: " << strtemp2 << vcl_endl;
    FILE *out = fopen(strtemp2, "w");
    fprintf(out, "CONTOUR\nOPEN\n%d\n", N);
    for (int i = 0; i<N; i++)
      fprintf(out, "%lf %lf\n", curve_2d->x(values[i]), curve_2d->y(values[i]));
  
    fclose(out);
  } else {

    vcl_vector<vsol_point_2d_sptr> inp1;
    for (int k = 0; k<curve_2d->size(); k++)
      inp1.push_back(curve_2d->vertex(k));

    dbsol_interp_curve_2d_sptr curve;
    
    if (linear_interp) {
      curve = new dbsol_interp_curve_2d();
      dbsol_curve_algs::interpolate_linear(curve.ptr(), inp1, true);  // true since always closed curve  
    } else {
      curve = new dbsol_geno_curve_2d();
      dbsol_geno::interpolate((dbsol_geno_curve_2d *)curve.ptr(),inp1, true);
    }

    double len = curve->length();
    vcl_cout << " len: " << len << vcl_endl;

    double delta_s = len/N;


    strcpy(strtemp1, con_filename.c_str());
    int i;
    for (i=0; strtemp1[i] != '.'; i++ ){}
    strtemp1[i]= 0;
    
    if (linear_interp)
      sprintf(strtemp2, "%s-even-linear-%d", strtemp1, N);
    else 
      sprintf(strtemp2, "%s-even-geno-%d", strtemp1, N);
    sprintf(strtemp2, "%s.con", strtemp2);

    vcl_cout << "output name: " << strtemp2 << vcl_endl;
    FILE *out = fopen(strtemp2, "w");
    fprintf(out, "CONTOUR\nOPEN\n%d\n", N);
    
    double s = 0;
    int cnt = 0;
    while (s < len) {
      vsol_point_2d_sptr pt = curve->point_at(s);
      fprintf(out, "%lf %lf\n", pt->x(), pt->y());
      s += delta_s;
      cnt++;
      if (cnt == N-1) break;
    }
    vsol_point_2d_sptr pt = curve->point_at(len);
    fprintf(out, "%lf %lf\n", pt->x(), pt->y());
    cnt++;
    vcl_cout << "printed " << cnt << " points N was: " << N << " \n";

    fclose(out);

  }

  return 0;


}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vcl_vector.h>
#include <time.h>
#include <vcl_iostream.h>
#include <vcl_algorithm.h>

#define MPI   (3.14159265)
#define SMALL  (1.0E-4)

void generate_curve(float *x, float *y, int N, float rad, float min_angle, float max_angle) {
float r;

vcl_vector<double> angles;

angles.push_back(min_angle);
angles.push_back(max_angle);
srand(time(NULL));
// generate N-2 random distinct angles between min_angle and max_angle
for (int m = 2; m<N; m++) {
  r = ((double)rand()/(double)(RAND_MAX));
  double a = r*(max_angle-min_angle)+min_angle;
  int tag = 0;
  //vcl_cout << "a: " << a << "\n";
  for (int i = 0; i<m; i++) {
    //vcl_cout << "angles[" << i << "]: " << angles[i] << "\n";
    if (fabs(a-angles[i]) <= SMALL) {
      vcl_cout << "HIT too close\n";
      m--;
      tag = 1;
      break;
    }
  }
  if (tag) continue;
  angles.push_back(a);
}
//for (m = 0; m<N; m++)
//  printf("angles[%d]: %f\n", m, angles[m]);

vcl_sort(angles.begin(), angles.end());

//for (m = 0; m<N; m++)
//  printf("angles[%d]: %f\n", m, angles[m]);


for (int m = 0; m<N; m++) {
  x[m] = rad*cos(angles[m]);
  y[m] = rad*sin(angles[m]);
}

return;
}


int main(int argc, char *argv[]) { 
float min_angle, max_angle;
float *x1, *y1;
float *x2, *y2;
float rad;
int N;
FILE *out;

if (argc != 10) {
 printf("USAGE: sampler <radius> <min_a> <max_a> <N> <1> <out_file.con> <rad2 if biarc> <N2 if biarc> <max_a2 if biarc>\n");
 printf("min_a: starting angle on circle in degrees\n");
 printf("max_a: ending angle on circle in degrees\n");
 printf("N: number of points on the curve\n");
 printf("1: if single arc, 0 if biarc.\n");
 return 0;
}

rad = atof(argv[1]);
printf("rad: %f\n", rad);
min_angle = MPI*(atof(argv[2])/180);
printf("min_a: %f\n", min_angle);
max_angle = MPI*(atof(argv[3])/180);
printf("max_a: %f\n", max_angle);
N = atoi(argv[4]);
printf("N: %d\n", N);
x1 = (float *)malloc(sizeof(float)*N);
y1 = (float *)malloc(sizeof(float)*N);

out = fopen(argv[6], "w");
if (!out) {
  printf("output file could not be opened!\n");
  return 0;
}

int bi = atoi(argv[5]);
bool biarc = (bi == 0 ? true : false);

if (biarc) {

  generate_curve(x1, y1, N, rad, min_angle, max_angle);

  float rad2 = atof(argv[7]);
  vcl_cout << "rad2: " << rad2 << vcl_endl;
  int N2 = atoi(argv[8]);
  vcl_cout << "N2: " << N2 << vcl_endl;
  float max2 = MPI*(atof(argv[9]))/180;
  vcl_cout << "max2: " << max2 << vcl_endl;
  x2 = (float *)malloc(sizeof(float)*N2);
  y2 = (float *)malloc(sizeof(float)*N2);

  generate_curve(x2, y2, N2, rad2, 0, max2);

  fprintf(out, "CONTOUR\nOPEN\n%d\n", N+N2);
  for (int i = N-1; i>=0; i--)
    fprintf(out, "%f %f\n", x1[i], y1[i]);  
  for (int i = 0; i<N2; i++)
    fprintf(out, "%f %f\n", (-x2[i])+x2[0]+x1[0], (-y2[i]) + y2[0]+y1[0]);  

} else {

  generate_curve(x1, y1, N, rad, min_angle, max_angle);
  
  fprintf(out, "CONTOUR\nOPEN\n%d\n", N);
  for (int i = 0; i<N; i++)
    fprintf(out, "%f %f\n", x1[i], y1[i]);  
}

fclose(out);
//for (int i = 0; i<=100000000; i++) vcl_cout << "counting: " << i << "\n";
return 0;
}

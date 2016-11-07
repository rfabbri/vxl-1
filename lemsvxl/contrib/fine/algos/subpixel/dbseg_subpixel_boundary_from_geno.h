#ifdef __cplusplus
extern "C" {
#endif

#include "dbseg_subpixel_utils.h"
#include "dbseg_subpixel_graphics.h"
#include "dbseg_subpixel_geno-types.h"
#include "dbseg_subpixel_geno.h" 
#include <assert.h>

#include "dbseg_subpixel_shocks_from_cedt.h"

#define NOLABEL -15
#define VERY_SMALL_VALUE 1E-3


int almost_equal(double a, double b);

int almost_equal_with_tolerance(double a, double b, double tolerance);

GENO_Curve_List* make_geno_curve(double *contour_array,  /*Array of points
                               xyxy...*/
                                 int no_of_points, /*No of points*/
                                 int closed);       /*1 -> closed
                              0-> open */
int cedt_boundary_with_geno(Tracer *Tr, Boundary *boundary,int height,
                             int width);

void initialize_cedt_boundary_geno_curve(double *contour_array,
                                         Boundary *boundary,
                                         GENO_Interpolated_Curve*icurves,
                                         int *boundary_index,
                                         int contour_index,
                                         int no_of_points,
                                         int closed);

void initialize_cedt_boundary_point(double *contour_array,
                                    Boundary *boundary,
                                    int *boundary_index,
                                    int contour_index);
void initialize_cedt_boundary_23points(double *contour_array,
                       Boundary *boundary,
                       int *boundary_index, 
                       int contour_index, int no_points);

void copy_geno_intv_to_boundary(GENO_Interval * intv, Boundary *boundary, 
                int index,int contour_id);

#ifdef __cplusplus
}
#endif


// This is /lemsvxl/contrib/firat/dbsksp_object_warehouse/driver.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Apr 8, 2011

#include "dbsksp_object_warehouse.h"

int main()
{
	dbsksp_object_warehouse warehouse("/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles",
	                "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-gt-xgraph.txt");
	warehouse.perturb_exemplar(0, 5);
	warehouse.save("/home/firat/Desktop/db/bottle_warehouse");
	return 0;
}

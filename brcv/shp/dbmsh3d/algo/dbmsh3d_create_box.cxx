
///#include <vsol/vsol_point_3d.h>

#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/dbmsh3d_pt_set.h>

//: Constants for the box generator

/*void dbmsh3d_point_gene_set::GenerateBox ()
{
  //: Variables for the box generator
  vsol_point_3d* BoxPoint[8];
  BoxPoint[0] = new vsol_point_3d (0,0,0);
  BoxPoint[1] = new vsol_point_3d (1,0,0);
  BoxPoint[2] = new vsol_point_3d (1,1,0);
  BoxPoint[3] = new vsol_point_3d (0,1,0);
  BoxPoint[4] = new vsol_point_3d (0,0,1);
  BoxPoint[5] = new vsol_point_3d (1,0,1);
  BoxPoint[6] = new vsol_point_3d (1,1,1);
  BoxPoint[7] = new vsol_point_3d (0,1,1);

//: Steps.  
#define STEP_X 4
#define STEP_Y 3
#define STEP_Z 2

  //: local variables
  int x, y, z=0;


  //:1)bottom face: z=0
  for (x=0; x<STEP_X; x++) {
    for (y=0; y<STEP_Y; y++) {
      //vsol_point_3d x1 (
    }
  }

  addPointGene (x, y, z);

  for (int i=0; i<8; i++)
    delete BoxPoint[i];
}*/

#define SIZE_X 100
#define SIZE_Y 100
#define SIZE_Z 100

/*
void dbmsh3d_point_gene_set::GenerateRandPts (int nPoints)
{
  int seed = 10000;
  srand(seed);

  for (int i=0; i<nPoints; i++) {
    float x = (float)rand() / RAND_MAX * SIZE_X;
    float y = (float)rand() / RAND_MAX * SIZE_Y;
    float z = (float)rand() / RAND_MAX * SIZE_Z;

    addPointGene (x, y, z);
  }
}
*/


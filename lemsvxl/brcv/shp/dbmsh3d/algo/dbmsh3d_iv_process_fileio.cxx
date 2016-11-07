//: 040312 MingChing Chang
//: My own IV file pharser to read in IV and output G3D
//: Do interpolation if needed.

#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vcl_string.h>
#include <dbmsh3d/dbmsh3d_utils.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>
#include <vcl_cstring.h>

/* --------------------- Structures ------------------------*/

#define MAX_VERTICES_SIZE 5  //5
typedef struct {
  int iSize;
  int vertex[MAX_VERTICES_SIZE];
} Face3dVertices;


class Pt3dCoord
{
public:
   float    fPosX, fPosY, fPosZ;
   
   Pt3dCoord () {}
   Pt3dCoord (float fx, float fy, float fz) {
      fPosX = fx;
      fPosY = fy;
      fPosZ = fz;
   }
};

//Ming: version 2: read everything into an array,
// process, and output the g3d file!
int iNumPoints    = 0;
Pt3dCoord* pPoints = NULL;

int iNumFaces   = 0;
Face3dVertices* pFaces = NULL;

int iSizeNumMorePoints = 0;
int iNumMorePoints = 0;
Pt3dCoord* pMorePoints = NULL;

double dMinEdgeLength = 100000;
double dAvgEdgeLength = 0;
double dMinEdgeThreshold = 10000;

double distPointPoint (Pt3dCoord p1, Pt3dCoord p2) {
  double sq = (p1.fPosX-p2.fPosX)*(p1.fPosX-p2.fPosX) +
          (p1.fPosY-p2.fPosY)*(p1.fPosY-p2.fPosY) +
          (p1.fPosZ-p2.fPosZ)*(p1.fPosZ-p2.fPosZ);
  return sqrt (sq);
}

///extern float fScale;
///extern float fMaxPerturb;
///extern float fIVRecursiveThreshold;

void addPoint (Pt3dCoord p1, Pt3dCoord p2, float fMaxPerturb)
{
  int seed;
  float pert;
  pMorePoints[iNumMorePoints].fPosX = (p1.fPosX + p2.fPosX)/2;
  pMorePoints[iNumMorePoints].fPosY = (p1.fPosY + p2.fPosY)/2;
  pMorePoints[iNumMorePoints].fPosZ = (p1.fPosZ + p2.fPosZ)/2;

  //Add some perturbation!!!! 
  seed = 10000;
  srand(seed);
  pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f; //now pert is in (-0.5 to 0.5)
  pert *= fMaxPerturb;                      //now pert is in the specified range (say, -0.005 to 0.005)
  pMorePoints[iNumMorePoints].fPosX += (float) pert*(p2.fPosX-p1.fPosX);
  pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f;
  pert *= fMaxPerturb;
  pMorePoints[iNumMorePoints].fPosY += (float) pert*(p2.fPosY-p1.fPosY);
  pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f;
  pert *= fMaxPerturb;  
  pMorePoints[iNumMorePoints].fPosZ += (float) pert*(p2.fPosZ-p1.fPosZ);

  iNumMorePoints++;

  assert (iNumMorePoints < iSizeNumMorePoints);
}

void addPointsForFaces3 (Pt3dCoord p1, Pt3dCoord p2, Pt3dCoord p3, float fMaxPerturb)
{
  Pt3dCoord m12, m23, m31;
  double  d12, d23, d31;
  m12.fPosX = (p1.fPosX+p2.fPosX)/2;
  m12.fPosY = (p1.fPosY+p2.fPosY)/2;
  m12.fPosZ = (p1.fPosZ+p2.fPosZ)/2;
  m23.fPosX = (p2.fPosX+p3.fPosX)/2;
  m23.fPosY = (p2.fPosY+p3.fPosY)/2;
  m23.fPosZ = (p2.fPosZ+p3.fPosZ)/2;
  m31.fPosX = (p3.fPosX+p1.fPosX)/2;
  m31.fPosY = (p3.fPosY+p1.fPosY)/2;
  m31.fPosZ = (p3.fPosZ+p1.fPosZ)/2;

  d12 = distPointPoint (p1, p2);
  d23 = distPointPoint (p2, p3);
  d31 = distPointPoint (p3, p1);

  //If any of the edges need split, split into 4
  if (d12>dMinEdgeThreshold && d23>dMinEdgeThreshold && d31>dMinEdgeThreshold) {

    addPoint (p1, p2, fMaxPerturb);
    addPoint (p2, p3, fMaxPerturb);
    addPoint (p3, p1, fMaxPerturb);

    //Recursively split into 4
    addPointsForFaces3 (p1, m12, m31, fMaxPerturb);
    addPointsForFaces3 (p2, m12, m23, fMaxPerturb);
    addPointsForFaces3 (p3, m23, m31, fMaxPerturb);
    addPointsForFaces3 (m12, m23, m31, fMaxPerturb);

    return;
  }

  //If any 2 of the 3 edges need split
  if (d12>dMinEdgeThreshold && d23>dMinEdgeThreshold) {
    //Connect m12-m23
    addPoint (p1, p2, fMaxPerturb);   
    addPoint (p2, p3, fMaxPerturb);
    addPointsForFaces3 (p2, m12, m23, fMaxPerturb);

    if (distPointPoint(p1, m23) < distPointPoint (p3, m12)) {
      addPointsForFaces3 (m12, p1, m23, fMaxPerturb);
      addPointsForFaces3 (m23, p1, p3, fMaxPerturb);
    }
    else {
      addPointsForFaces3 (m12, p1, p3, fMaxPerturb);
      addPointsForFaces3 (m12, m23, p3, fMaxPerturb);
    }

    return;
  }
  if (d12>dMinEdgeThreshold && d31>dMinEdgeThreshold) {
    //Connect m12-m31
    addPoint (p1, p2, fMaxPerturb);   
    addPoint (p3, p1, fMaxPerturb);
    addPointsForFaces3 (p1, m12, m31, fMaxPerturb);

    if (distPointPoint(p2, m31) < distPointPoint (p3, m12)) {
      addPointsForFaces3 (p3, m31, p2, fMaxPerturb);
      addPointsForFaces3 (m12, m31, p2, fMaxPerturb);
    }
    else {
      addPointsForFaces3 (p3, m31, m12, fMaxPerturb);
      addPointsForFaces3 (m12, p2, p3, fMaxPerturb);
    }

    return;
  }
  if (d23>dMinEdgeThreshold && d31>dMinEdgeThreshold) {
    //Connect m23-m31
    addPoint (p2, p1, fMaxPerturb);   
    addPoint (p1, p2, fMaxPerturb);
    addPointsForFaces3 (p3, m23, m31, fMaxPerturb);

    if (distPointPoint(p1, m23) < distPointPoint (p2,m31)) {
      addPointsForFaces3 (p1, m31, m23, fMaxPerturb);
      addPointsForFaces3 (p1, m23, p2, fMaxPerturb);
    }
    else {
      addPointsForFaces3 (p1, m31, p2, fMaxPerturb);
      addPointsForFaces3 (m31, m23, p2, fMaxPerturb);
    }

    return;
  }

  //If any one of the 3 edges needs split
  if (d12>dMinEdgeThreshold) {
    addPoint (p1, p2, fMaxPerturb);
    addPointsForFaces3 (m12, p3, p1, fMaxPerturb);
    addPointsForFaces3 (m12, p3, p2, fMaxPerturb);

    return;
  }
  if (d23>dMinEdgeThreshold) {
    addPoint (p2, p3, fMaxPerturb);
    addPointsForFaces3 (m23, p1, p2, fMaxPerturb);
    addPointsForFaces3 (m23, p1, p3, fMaxPerturb);

    return;
  }
  if (d31>dMinEdgeThreshold) {
    addPoint (p3, p1, fMaxPerturb);
    addPointsForFaces3 (m31, p2, p1, fMaxPerturb);
    addPointsForFaces3 (m31, p2, p3, fMaxPerturb);
    return;
  }

  //Else, Terminate recursion.
}

//Temporarily, just do one layer
void addPointsForFaces4 (Pt3dCoord p1, Pt3dCoord p2, Pt3dCoord p3, Pt3dCoord p4, float fMaxPerturb)
{
  addPoint (p1, p3, fMaxPerturb);
  addPoint (p1, p2, fMaxPerturb);
  addPoint (p2, p3, fMaxPerturb);
  addPoint (p3, p4, fMaxPerturb);
  addPoint (p4, p1, fMaxPerturb);
}

//Temporarily, just do one layer
void addPointsForFaces5 (Pt3dCoord p1, Pt3dCoord p2, Pt3dCoord p3, Pt3dCoord p4, Pt3dCoord p5, float fMaxPerturb)
{
  addPoint (p1, p3, fMaxPerturb);
  addPoint (p1, p4, fMaxPerturb);
  addPoint (p1, p2, fMaxPerturb);
  addPoint (p2, p3, fMaxPerturb);
  addPoint (p3, p4, fMaxPerturb);
  addPoint (p4, p5, fMaxPerturb);
  addPoint (p5, p1, fMaxPerturb);
}

bool IVToG3DPoints (const char* pcFileNameInIV, const char* pcFileName, bool iFlagMoreSamples, 
                    float fMaxPerturb, float fIVRecursiveThreshold)
{
  const char    *pcFName = "IVToG3DPoints_vertex";
  char    buffer[100] = ""; //200
  FILE    *fp1, *fp2;
  float   fPosX, fPosY, fPosZ;
  int   iVerticesPerFace, iTmp, i;//, j, k;
  int   iFace1, iFace2, iNum3VerticesFace, iNum4VerticesFace, iNum5VerticesFace;
  char    inputFName[200], outputFName[200];
  int   iNumPoints;
  double  dist, dMaxPerturb;
  float   minX, minY, minZ, maxX, maxY, maxZ;
  int   end_of_file;
  bool    this_round_done;
  int   seed;
  float   pert;

//  float maxWidth;
  float R;
  int num;

  vcl_strcpy (inputFName, pcFileNameInIV);
  //strcat (inputFName, ".iv");
  if ((fp1 = fopen(inputFName, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): Can't open input IV file %s\n",
          pcFName, inputFName);
    return false; 
  }

  //######################################################
  //1)Go through the .iv once to get the total number of points,
  //  min max coordinates

  minX = minY = minZ = FLT_MAX;
  maxX = maxY = maxZ = -FLT_MAX;

  iNumPoints = 0;
  iNumFaces = 0;
  end_of_file = 0;
  iFace1 = 0;
  while (1) {
    //1-1)search for first "vertex" or "point"
    while (vcl_strcmp (buffer, "vertex")!=0 && vcl_strcmp (buffer, "point")!=0) {
      end_of_file = vcl_fscanf (fp1, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE;
    }

    //search for "["
    vcl_fscanf (fp1, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in (x, y, z) coordinates of vertices
    this_round_done = false;
    while (!this_round_done) {
      vcl_fscanf (fp1, "%lf %lf %lf%s", &fPosX, &fPosY, &fPosZ, buffer);
      if (fPosX<minX) minX = fPosX;
      if (fPosY<minY) minY = fPosY;
      if (fPosZ<minZ) minZ = fPosZ;
      if (fPosX>maxX) maxX = fPosX;
      if (fPosY>maxY) maxY = fPosY;
      if (fPosZ>maxZ) maxZ = fPosZ;
    
      ////vul_printf (vcl_cout, "x: %lf, y: %lf, z: %lf, \n", fPosX, fPosY, fPosZ);
      iNumPoints++;

      if (vcl_strcmp (buffer, "]")==0)
        this_round_done = true;
    }

    //1-2)search for first "coordIndex"
    while (vcl_strcmp (buffer, "coordIndex")!=0) {
      end_of_file = vcl_fscanf (fp1, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE;
    }
    //search for "["
    vcl_fscanf (fp1, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in [?, ?, ?, -1, ?, ?, ?, ?, -1] indices of faces
    //note that a face can have either 3, 4, 5, or more vertices.
    this_round_done = false;
    iVerticesPerFace = 0;
    while (!this_round_done) {
      vcl_fscanf (fp1, "%s", buffer);
      iTmp = atoi (buffer);
      if (iTmp == -1) {
        iFace1++;
        //assert (iVerticesPerFace<=MAX_VERTICES_SIZE);
        if (iVerticesPerFace>MAX_VERTICES_SIZE)
          vul_printf (vcl_cout, "ERROR(%s): iVerticesPerFace (%d) > MAX_VERTICES_SIZE (%d).\n",
                pcFName, iVerticesPerFace, MAX_VERTICES_SIZE);
        iVerticesPerFace = 0;
      }
      else {
        iVerticesPerFace++;
      }

      if (vcl_strcmp (buffer, "]")==0)
        this_round_done = true;
    }

  }//end while(1)

READ_IV_DONE:
  fclose (fp1);

  //2) Now we know iNumPoints and iNumFaces.
  //   Alloc memory for pPoints[] and pFaces[]

  iNumFaces = iFace1;
  iSizeNumMorePoints = iNumFaces*100; //*10000    *7

  if ((pPoints = (Pt3dCoord*) calloc(iNumPoints, sizeof(Pt3dCoord))) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): CALLOC fails on pPoints[%d].\n",
          pcFName, iNumPoints);
    exit(-4); 
  }
  if ((pFaces = (Face3dVertices*) calloc(iNumFaces, sizeof(Face3dVertices))) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): CALLOC fails on pFaces[%d].\n",
          pcFName, iNumFaces);
    exit(-4); 
  }

  //   Alloc memory for pMorePoints
  if ((pMorePoints = (Pt3dCoord*) calloc(iSizeNumMorePoints, sizeof(Pt3dCoord))) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): CALLOC fails on pMorePoints[%d].\n",
          pcFName, iNumMorePoints);
    exit(-4); 
  }


  //######################################################
  //3) Go through the .iv second time to Write the points into pPoints[]
  //  and the face vertices info into pFaces[]
  //  Normalize the coordinates if needed.

  //3-1)Normalization factor
  ///#define NORMALIZED_WIDTH 100.0
  ///maxWidth = max ( max(maxX-minX, maxY-minY), maxZ-minZ);
  ///if (fScale==1)
    R = 1;
  ///else
  /// R = (float) NORMALIZED_WIDTH/maxWidth;

  //===== Open *.iv again to output (xyz)s =====
  num = 0;
  dAvgEdgeLength = 0;
  if ((fp1 = fopen(inputFName, "r")) == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): Can't open input IV file %s\n",
          pcFName, inputFName);
    return false; 
  }

  end_of_file = 0;
  iFace2 = 0;
  while (1) {
    //1-1) search for first "vertex"
    while (vcl_strcmp (buffer, "vertex")!=0 && vcl_strcmp (buffer, "point")!=0) {
      end_of_file = vcl_fscanf (fp1, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE2;
    }

    //search for "["
    vcl_fscanf (fp1, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in (x, y, z) coordinates of vertices
    this_round_done = false;
    while (!this_round_done) {
      vcl_fscanf (fp1, "%f %f %f%s", &fPosX, &fPosY, &fPosZ, buffer);
    
      pPoints[num].fPosX = fPosX*R;
      pPoints[num].fPosY = fPosY*R;
      pPoints[num].fPosZ = fPosZ*R;
      //vcl_fprintf (fp2, "%f %f %f %f %f %f\n", fPosX*R, fPosY*R, fPosZ*R, 1.0, 1.0, 1.0);
      num++;
      if (vcl_strcmp (buffer, "]")==0)
        this_round_done = true;
    }

    //1-2)search for first "coordIndex"
    while (vcl_strcmp (buffer, "coordIndex")!=0) {
      end_of_file = vcl_fscanf (fp1, "%s", buffer);
      if (end_of_file==EOF)
        goto READ_IV_DONE2; //READ_IV_DONE
    }
    //search for "["
    vcl_fscanf (fp1, "%s", buffer);
    assert (vcl_strcmp (buffer, "[")==0);

    //read in [?, ?, ?, -1, ?, ?, ?, ?, -1] indices of faces
    //note that a face can have either 3, 4, 5, or more vertices.
    this_round_done = false;
    iVerticesPerFace = 0;
    while (!this_round_done) {
      vcl_fscanf (fp1, "%s", buffer);
      iTmp = atoi (buffer);
      if (iTmp == -1) {
        iFace2++;
        ///assert (iVerticesPerFace<=MAX_VERTICES_SIZE);
        iVerticesPerFace = 0;
      }
      //avoid overflow!
      else if (iVerticesPerFace<MAX_VERTICES_SIZE) {
        //Put the value into the pFaces[] array
        pFaces[iFace2].vertex[iVerticesPerFace] = iTmp;

        //detect the fMinEdgeLength...
        if (iVerticesPerFace>0) {
          //compute the length of pFaces[iFace2].vertex[iVerticesPerFace-1] and 
          dist = distPointPoint (pPoints[pFaces[iFace2].vertex[iVerticesPerFace-1]],
                          pPoints[pFaces[iFace2].vertex[iVerticesPerFace]]);
          if (dist >0.01 && dist<dMinEdgeLength)
            dMinEdgeLength = dist;
          dAvgEdgeLength += dist;
        } 

        iVerticesPerFace++;
        pFaces[iFace2].iSize = iVerticesPerFace;
      }

      if (vcl_strcmp (buffer, "]")==0)
        this_round_done = true;
    }

  }//end while(1)

READ_IV_DONE2:
  fclose (fp1);
  assert (num == iNumPoints);
  assert (iFace1 == iFace2);
  //===== End Open *.iv again to output (xyz)s =====

  dAvgEdgeLength /= iNumPoints;
  vul_printf (vcl_cout, "\nMinimum dist between vertices: %lf.\n",
        dMinEdgeLength);
  vul_printf (vcl_cout, "\nAverage dist between vertices: %lf.\n",
        dAvgEdgeLength);

  dMinEdgeThreshold = fIVRecursiveThreshold * dAvgEdgeLength;
  //dMinEdgeThreshold = fIVRecursiveThreshold * dMinEdgeLength; //fth=10 by default.

  //######################################################
  //3.5) Perturb the input points by dMaxPerturb*rand
  if (fMaxPerturb!=0) {
    //dMaxPerturb = (float) dMinEdgeLength * 13; //iIVPerturb=13 by default.
    dMaxPerturb = dAvgEdgeLength * fMaxPerturb;

    vul_printf (vcl_cout, "\nMaximum perturbation for each point: %lf.\n",
          dMaxPerturb);

    seed = 67890123;
    srand(seed);

    for (i=0; i<iNumPoints; i++) {
      pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f; //now pert is in (-0.5 to 0.5)
      pert *= (float)dMaxPerturb;
      pPoints[i].fPosX += pert;
      pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f; //now pert is in (-0.5 to 0.5)
      pert *= (float)dMaxPerturb;
      pPoints[i].fPosY += pert;
      pert = (float)rand() / (float)(RAND_MAX+1) - 0.5f; //now pert is in (-0.5 to 0.5)
      pert *= (float)dMaxPerturb;
      pPoints[i].fPosZ += pert;
    }
  }


  //######################################################
  //4) For each Faces, put more samples if needed!

  if (iFlagMoreSamples) {

    iNumMorePoints = 0;
    iNum3VerticesFace = 0;
    iNum4VerticesFace = 0;
    iNum5VerticesFace = 0;
    for (i=0; i<iNumFaces; i++) {
      assert (pFaces[i].iSize <= MAX_VERTICES_SIZE);
      if (pFaces[i].iSize==3) {

        addPointsForFaces3 (pPoints[pFaces[i].vertex[0]],
                      pPoints[pFaces[i].vertex[1]],
                    pPoints[pFaces[i].vertex[2]], fMaxPerturb);

        iNum3VerticesFace++;
      }
      else if (pFaces[i].iSize==4) {

        addPointsForFaces4 (pPoints[pFaces[i].vertex[0]],
                      pPoints[pFaces[i].vertex[1]],
                      pPoints[pFaces[i].vertex[2]],
                      pPoints[pFaces[i].vertex[3]], fMaxPerturb);
          
        iNum4VerticesFace++;
      }
      else if (pFaces[i].iSize==5) {

        addPointsForFaces5 (pPoints[pFaces[i].vertex[0]],
                      pPoints[pFaces[i].vertex[1]],
                      pPoints[pFaces[i].vertex[2]],
                      pPoints[pFaces[i].vertex[3]],
                      pPoints[pFaces[i].vertex[4]], fMaxPerturb);

        iNum5VerticesFace++;
      }
      else {
        ////assert (0);
      }
    }
    vul_printf (vcl_cout, "\n# of Faces of 3 Vertices: %d.\n",
          iNum3VerticesFace);
    vul_printf (vcl_cout, "\n# of Faces of 4 Vertices: %d.\n",
          iNum4VerticesFace);
    vul_printf (vcl_cout, "\n# of Faces of 5 Vertices: %d.\n",
          iNum5VerticesFace);
    assert (iNumMorePoints <= iSizeNumMorePoints);
  }

  //######################################################
  //5) Output .g3d file
  vcl_strcpy (outputFName, pcFileName);
  strcat (outputFName, ".g3d");
  fp2 = fopen (outputFName, "w");
  if (fp2 == NULL) {
    vul_printf (vcl_cout, "ERROR(%s): Can't open file %s.\n", pcFName, outputFName);
    exit (-6); 
  }

  vul_printf (vcl_cout, "MESG(%s):\n\tSaving data in %s.\n", pcFName, outputFName);
  /* -- First: Save Initial Source Data -- */
  vcl_fprintf (fp2, "%d %d\n", 2, iNumPoints+iNumMorePoints);

  for (i=0; i<iNumPoints; i++) {
    vcl_fprintf (fp2, "%f %f %f %f %f %f\n", pPoints[i].fPosX, pPoints[i].fPosY, pPoints[i].fPosZ, 1.0, 1.0, 1.0);
  }

  for (i=0; i<iNumMorePoints; i++) {
    vcl_fprintf (fp2, "%f %f %f %f %f %f\n", pMorePoints[i].fPosX, pMorePoints[i].fPosY, pMorePoints[i].fPosZ, 1.0, 1.0, 1.0);
  }

  vul_printf (vcl_cout, "\n# of Original Vertices: %d.\n",
          iNumPoints);

  vul_printf (vcl_cout, "\n# of Vertices added: %d.\n",
          iNumMorePoints);

  fclose (fp2);

  //######################################################
  //6) Release memories.
  free (pPoints);
  //////free (pFaces);   //BAD!!!
  free (pMorePoints);   //BAD!!!

  return true;
}

// #################################################################
//    MESH PROCESSING
// #################################################################

bool dbmsh3d_load_iv_ifs_intp (const char *pcIVFile, const char *pcG3DFile, int option,
                               float fMaxPerturb, float fIVRecursiveThreshold)
{
  if (option==1)
    IVToG3DPoints (pcIVFile, pcG3DFile, 0, fMaxPerturb, fIVRecursiveThreshold);
  else if (option==2)
    IVToG3DPoints (pcIVFile, pcG3DFile, 1, fMaxPerturb, fIVRecursiveThreshold);

  return true;
}







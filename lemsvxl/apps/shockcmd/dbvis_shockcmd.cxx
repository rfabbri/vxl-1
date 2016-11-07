#include "shock/ishock.h"
#include "shock/ishock_sptr.h"
#include <shock/IShockXShock.h>
#include <shock/xshock.h>

#include "opt.h"

//////////////////////////////////////////////////////////
// Process the arugments
char  *pcFileIn = NULL;
char  *pcFileOut = NULL;
float fPruningThreshold = 1.0f;
float fSamplingArcLength = 1.0f;
int   nNumElmsPerBucket = 15;

static OPTTABLE optab [] = {
  {"i", _STRING, 0, &pcFileIn, "Input Contour File (*.con) or Boundary File (*.bnd)"},
  {"o", _STRING, 0, &pcFileOut, "Output Extrinsic Shock File (*.esf)"},
  {"t", _FLOAT, 0, &fPruningThreshold, "Pruning Threshold (Def.=1.0)"},
  {"s", _FLOAT, 0, &fSamplingArcLength, "Sampling Arc Length (Def.=1.0)"},
  {"n", _INT, 0, &nNumElmsPerBucket, "Number of Elements per Bucket (Def.=15)"},
  {(char*)0, (OPT_TYPE)0, (int*)0, (void*)0, (char*)0} 
};

int main(int argc, char** argv)
{

  //1)Read in data file (*.bnd) and set parameters.
  opt_parse_args(argc, argv, optab);        
  if (pcFileIn == NULL) {
    vcl_fprintf (stderr, "ERROR: Missing input (prefix) filename: %s\n", pcFileIn);
    vcl_fprintf (stderr, "\n\t use -h for more help.\n");
    exit(-1);
  }
  if (fPruningThreshold < 0.0f || fPruningThreshold > 100000) {
    vcl_fprintf (stderr, "ERROR(%s): Incorrect Pruning Threshold = %f .\n", fPruningThreshold);
    exit(-1);
  }
  if (fSamplingArcLength < 0.0f || fSamplingArcLength > 100000) {
    vcl_fprintf (stderr, "ERROR(%s): Incorrect Sampling Arc Length = %f .\n", fSamplingArcLength);
    exit(-1);
  }
  vcl_printf ("Input boundary file= %s\n", pcFileIn);
  vcl_printf ("Output ESF shock file= %s\n", pcFileOut);
  vcl_printf ("Pruning Threshold= %f\n", fPruningThreshold);
  vcl_printf ("Sampling Arc Length= %f\n", fSamplingArcLength);

  Boundary* _boundary = new Boundary_Bucketing ();
  _boundary->LoadFile (pcFileIn);

  IShock_sptr _shock = new IShock_Bucketing (_boundary);

  _shock->DetectShocks( BUCKETING_INIT , LAGRANGIAN );
  _shock->Prune (fPruningThreshold);


  IShockXShock* xshock = new IShockXShock(_shock.ptr());
  xshock->clear();
  xshock->sample_intrinsic_shocks(INSIDE);

  xshock->write_esf_file(pcFileOut);

  return 0;
}


//: This is 3DShock_Scaffold_Graph_SA_Smoothing.cxx
//  MingChing Chang 040227
//  3D ShockGraph Smoothing

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_3d.h>
#include <vul/vul_printf.h>

#include <dbgl/algo/dbgl_curve_smoothing.h>

#include <dbsk3d/dbsk3d_ms_curve.h>
#include <dbsk3d/dbsk3d_sg_sa.h>


//: Do Gaussian smoothing on the specifed dbsk3d_ms_curve on curve segment (from sample start to sample end)
//  Note that the position of sample start and sample end should be fixed.
void gaussian_sm_SC (dbsk3d_ms_curve* MC, int start, int end, int iters)
{
   vcl_vector<vgl_point_3d<double> > curve;
   curve.clear();

   assert (start < end);
   assert (end <= (int)MC->V_vec().size());
   //For each rib curve, put each red_vertex into a vector<point3d>...
   for (int j=start; j<=end; j++) {
      const dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
      curve.push_back (vgl_point_3d<double> (FV->pt().x(), FV->pt().y(), FV->pt().z()));
   }

   dbgl_gaussian_smooth_curve_3d_fixedendpt (curve, 1, iters);

   //Put the smooth curve back to the structure...
   for (int j=start; j<=end; j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) MC->V_vec(j);
      int jj = j-start;
      FV->get_pt().set (curve[jj].x(), curve[jj].y(), curve[jj].z());
   }
}

#define N_SAMPLE_SMOOTH_SEGMENT  50

//: For long curves, especially loop like structure, the smoothed curve will shift a lot,
//  because only the end points are fixed.
//  This function smooth the curve segment by segment to prevent the shifting effect.
void gaussian_sm_sg_sa (dbsk3d_sg_sa* sg_sa, const unsigned int iters)
{
  vul_printf (vcl_cout, "\ngaussian_sm_sg_sa(): %d iterations on %u ms_curves.\n", 
              iters, sg_sa->edgemap().size());

  //Smooth all scaffold_curves...
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = sg_sa->edgemap().begin();
  for (; SC_it != sg_sa->edgemap().end(); SC_it++) {
      dbsk3d_ms_curve* MC = (dbsk3d_ms_curve*) (*SC_it).second;

      int nSegmentSmoothTimes = MC->V_vec().size() / N_SAMPLE_SMOOTH_SEGMENT;

      //: The SEGMENT part: 0-20, 20-40, 40-60, ...
      for (int j=0; j<nSegmentSmoothTimes; j++) {
         int seg_start = j*N_SAMPLE_SMOOTH_SEGMENT - 1;
         if (seg_start < 0)
            seg_start = 0;
         int seg_end = (j+1)*N_SAMPLE_SMOOTH_SEGMENT - 1;
         gaussian_sm_SC (MC, seg_start, seg_end, iters);
      }

      //: The last part: 60-75.
      int final_start = nSegmentSmoothTimes*N_SAMPLE_SMOOTH_SEGMENT - 1;
      if (final_start < 0)
         final_start = 0;
      int final_end = MC->V_vec().size() - 1;
      if (final_start < final_end)
         gaussian_sm_SC (MC, final_start, final_end, iters);
  }
}




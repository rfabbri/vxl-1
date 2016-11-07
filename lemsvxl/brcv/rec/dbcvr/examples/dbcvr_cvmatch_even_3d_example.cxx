//: 
// \file    dbcvr_cvmatch_even_3d_example.cxx
// \brief   
// \author  H. Can Aras
// \date    2006-12-12
// 
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vsol/vsol_point_3d.h>
#include <dbsol/dbsol_interp_curve_3d.h>
#include <dbsol/algo/dbsol_curve_3d_algs.h>
#include <dbcvr/dbcvr_cvmatch_even_3d.h>

void loadCON(vcl_string fileName, vcl_vector<vsol_point_3d_sptr> &points)
{
  vcl_ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (strncmp(magicNum,"CONTOUR",7))
  {
    vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cerr << "Should be CONTOUR " << magicNum << vcl_endl;
    exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!strncmp(openFlag,"OPEN",4))
    vcl_cout << "Open Curve\n" << vcl_endl;
  else if (!strncmp(openFlag,"CLOSE",5))
    vcl_cout << "Closed Curve\n" << vcl_endl;
  else
  {
    vcl_cerr << "Invalid File " << fileName.c_str() << vcl_endl;
    vcl_cerr << "Should be OPEN/CLOSE " << openFlag << vcl_endl;
    exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y,z;
  for (i=0;i<numOfPoints;i++)
  {
    infp >> x >> y >> z ;
    points.push_back(new vsol_point_3d(x, y, z));
  }
  infp.close();
}

void writeCON(vcl_string fileName, dbsol_interp_curve_3d &c, int numpoints)
{
  vcl_ofstream outfp(fileName.c_str());
  assert(outfp != NULL);
  outfp << "CONTOUR" << vcl_endl;
  outfp << "OPEN" << vcl_endl;
  outfp << numpoints << vcl_endl;
  double ds = c.length()/(numpoints-1);
  for(int i=0; i<numpoints; i++)
  {
    vsol_point_3d_sptr p = c.point_at(i*ds);
    outfp << p->x() << " " << p->y() << " " << p->z() << vcl_endl;
  }
  outfp.close();
}

#define R1 0.7
#define R2 0.07
#define FORMULA_TYPE EXTRINSIC
#define ANG_DER_COMP USE_ENO //This has no significance when the cost formula type is extrinsic.

//--------------------------------------------------------MAIN-1--------------------------------------------------------//
// The user should pass a text file as command line argument. The format of the file is as follows:
//
// "con file name 1"
// "con file name 2"
// "output folder"
//
// "con file name 1"
// "con file name 2"
// "output folder"
// ...
//
// There may be as many matches as the user likes. For example, the file looks like this:
// 
// C:/Temp/curve1.con
// C:/Temp/curve2.con
// C:/Temp/curve1_vs_curve2/
//
// C:/Temp/curve3.con
// C:/Temp/curve4.con
// C:/Temp/curve3_vs_curve4/
//
// For this case, 
// curve1 is matched against curve2 and the results are written under curve1_vs_curve2 folder
// curve3 is matched against curve4 and the results are written under curve3_vs_curve4 folder
// All four possible matches (CW-CW, CCW-CW, CW-CCW, CCW-CCW) are not tried, 
// curves are matched only in the direction given by the contour file

#if 0
int main(int argc, char** argv)
{
  vcl_cout << "3D CURVE MATCHING EXAMPLE" << vcl_endl;
  vcl_string batch_fname = argv[1];
  vcl_ifstream infp(batch_fname.c_str());

  while(1)
  {
    vcl_string inp1, inp2, out;
    infp >> inp1;
    infp >> inp2;
    infp >> out;
    if(inp1.size() == 0)
      break;

    vcl_cout << inp1 << vcl_endl;
    vcl_cout << inp2 << vcl_endl;
    vcl_cout << out << vcl_endl;

    // construct the first curve
    vcl_vector<vsol_point_3d_sptr> points1;
    loadCON(inp1, points1);
    dbsol_interp_curve_3d curve1;
    vcl_vector<double> samples1;
    dbsol_curve_3d_algs::interpolate_eno_3d(&curve1,points1,samples1);

    // construct the second curve
    vcl_vector<vsol_point_3d_sptr> points2;
    loadCON(inp2, points2);
    dbsol_interp_curve_3d curve2;
    vcl_vector<double> samples2;
    dbsol_curve_3d_algs::interpolate_eno_3d(&curve2,points2,samples2);

    // match curve1 and curve2
    dbcvr_cvmatch_even_3d curvematch(&curve1, &curve2, curve1.size()+1, curve2.size()+1, R1, R2, 3, FORMULA_TYPE, ANG_DER_COMP);
    curvematch.Match();

    vcl_string curve_out_1 = out;
    vcl_string curve_out_2 = out;
    vcl_string match_out = out;
    curve_out_1 += "curve1.con";
    curve_out_2 += "curve2.con";
    match_out += "match.txt";

    // write interpolated curves
    writeCON(curve_out_1, curve1, curve1.size()+1);
    writeCON(curve_out_2, curve2, curve2.size()+1);
    // write the matches
    FinalMapType* fmap = curvematch.finalMap();
    vcl_ofstream outfp(match_out.c_str());
    // cost normalized by the length
    outfp << "Final Cost = " << curvematch.finalCost()/*/(curve1.length() + curve2.length())*20*/ << vcl_endl;
    
    outfp << fmap->size() << vcl_endl;

    for(unsigned i = 0; i < fmap->size(); i++)
      outfp << (*fmap)[i].first << " " << (*fmap)[i].second << vcl_endl;

    vcl_cout << curvematch.finalCost()/*/(curve1.length() + curve2.length())*100*/ << vcl_endl;
  }
  return 0;
}
#endif

//--------------------------------------------------------MAIN-2--------------------------------------------------------//
// The user should pass a text file as command line argument. The format of the file is as follows:
//
// "input files folder"
// "output folder"
// "number of curves"
// "curve names, each at a new line"
//
// For example, the file looks like this:
// 
// C:/Temp/curves/
// C:/Temp/results/
// 5
// curve1
// curve2
// curve3
// curve4
// curve5
//
// For this case, 
// "curve1, curve2, curve3, curve4, curve5" are contour files in "C:/Temp/curves/" folder
// These are matched one-by-one creating a 5x5 costs matrix (written to "C:/Temp/results/" folder)
// All four possible matches are tried, and the one with the minimum cost is picked as the best match
// All matches are written to the output folder, too.
// writeCON lines below can be uncommented below to write the re-sampled curves as contour files 
// to the output folder for each match
#if 1
int main(int argc, char** argv)
{
  vcl_cout << "3D CURVE MATCHING EXAMPLE" << vcl_endl;
  vcl_string batch_fname = argv[1];
  vcl_ifstream infp(batch_fname.c_str());

  vcl_vector<vcl_string> curve_names;
  vcl_vector<dbsol_interp_curve_3d> curves;
  vcl_vector<dbsol_interp_curve_3d> curves_inv;

  vcl_string in_folder;
  infp >> in_folder;
  vcl_string out_folder;
  infp >> out_folder;
  int num_curves;
  infp >> num_curves;

  vcl_string out_fname = out_folder + "out.txt";
  FILE *fp = vcl_fopen(out_fname.c_str(), "w");

  curves.resize(num_curves);
  curves_inv.resize(num_curves);

  // read the curves and store them
  for(int j=0; j<num_curves; j++)
  {
    vcl_string inp;
    infp >> inp;
    curve_names.push_back(inp);

    // construct the curve and its inverse
    inp = in_folder + inp + ".con";
    vcl_vector<vsol_point_3d_sptr> points;
    vcl_vector<vsol_point_3d_sptr> points_inv;

    loadCON(inp, points);
    for(int i=points.size()-1; i>=0; i--)
      points_inv.push_back(points[i]);
    
    vcl_vector<double> samples;
    dbsol_curve_3d_algs::interpolate_eno_3d(&(curves[j]),points,samples);
    dbsol_curve_3d_algs::interpolate_eno_3d(&(curves_inv[j]),points_inv,samples);
  }

  vcl_fprintf(fp, "        ");
  for(unsigned i=0; i<curves.size(); i++)
    vcl_fprintf(fp, "%s      ", curve_names[i].c_str());
  vcl_fprintf(fp, "\n");
  
  for(unsigned i=0; i<curves.size(); i++)
  {
    vcl_fprintf(fp, "%s  ", curve_names[i].c_str());
    double cost;
    for(unsigned j=0; j<curves.size(); j++)
    {
      if(1)
      {
        // first curve clockwise, second curve clockwise
        dbcvr_cvmatch_even_3d curvematch1(&(curves[i]), &(curves[j]), curves[i].size()+1, curves[j].size()+1, 
          R1, R2, 3, FORMULA_TYPE, ANG_DER_COMP);
        dbcvr_cvmatch_even_3d curvematch2(&(curves[i]), &(curves_inv[j]), curves[i].size()+1, curves_inv[j].size()+1, 
          R1, R2, 3, FORMULA_TYPE, ANG_DER_COMP);
        dbcvr_cvmatch_even_3d curvematch3(&(curves_inv[i]), &(curves_inv[j]), curves_inv[i].size()+1, curves_inv[j].size()+1, 
          R1, R2, 3, FORMULA_TYPE, ANG_DER_COMP);
        dbcvr_cvmatch_even_3d curvematch4(&(curves_inv[i]), &(curves[j]), curves_inv[i].size()+1, curves[j].size()+1, 
          R1, R2, 3, FORMULA_TYPE, ANG_DER_COMP);
        curvematch1.Match();
        curvematch2.Match();
        curvematch3.Match();
        curvematch4.Match();
        double cost1 = curvematch1.finalCost()/*/(curves[i].length() + curves[j].length())*20*/;
        double cost2 = curvematch2.finalCost()/*/(curves[i].length() + curves_inv[j].length())*20*/;
        double cost3 = curvematch3.finalCost()/*/(curves_inv[i].length() + curves_inv[j].length())*20*/;
        double cost4 = curvematch4.finalCost()/*/(curves_inv[i].length() + curves[j].length())*20*/;

        dbsol_interp_curve_3d *write_curve_1;
        dbsol_interp_curve_3d *write_curve_2;
        FinalMapType* fmap;
        int which_pair;

        if(cost1 < cost2)
        {
          cost = cost1;
          which_pair = 1;
        }
        else
        {
          cost = cost2;
          which_pair = 2;
        }
        if(cost3 < cost)
        {
          cost = cost3;
          which_pair = 3;
        }
        if(cost4 < cost)
        {
          cost = cost4;
          which_pair = 4;
        }
        vcl_fprintf(fp, "%4.6f | ", cost);

        switch(which_pair)
        { 
        case 1:
          fmap = curvematch1.finalMap();
          write_curve_1 = &curves[i];
          write_curve_2 = &curves[j];
          break;
        case 2:
          fmap = curvematch2.finalMap();
          write_curve_1 = &curves[i];
          write_curve_2 = &curves_inv[j];
          break;
        case 3:
          fmap = curvematch3.finalMap();
          write_curve_1 = &curves_inv[i];
          write_curve_2 = &curves_inv[j];
          break;
        case 4:
          fmap = curvematch4.finalMap();
          write_curve_1 = &curves_inv[i];
          write_curve_2 = &curves[j];
          break;
        default:
          break;
        }

        vcl_string curve_out_1 = out_folder + curve_names[i] + "_" + curve_names[j] + "_curve1.con";
        vcl_string curve_out_2 = out_folder + curve_names[i] + "_" + curve_names[j] + "_curve2.con";
        vcl_string match_out = out_folder + curve_names[i] + "_" + curve_names[j] + "_match.txt";
        // write interpolated curves
//        writeCON(curve_out_1, *write_curve_1, (*write_curve_1).size()+1);
//        writeCON(curve_out_2, *write_curve_2, (*write_curve_2).size()+1);
        // write the match
        vcl_ofstream outfp(match_out.c_str());
        // cost normalized by the length
        outfp << "Final Cost = " << cost << vcl_endl;

        outfp << fmap->size() << vcl_endl;

        for(unsigned k = 0; k < fmap->size(); k++)
          outfp << (*fmap)[k].first << " " << (*fmap)[k].second << vcl_endl;

        vcl_printf("Matching %s and %s, Cost: %4.6f, Matched Config: %d\n", 
          curve_names[i].c_str(), curve_names[j].c_str(), cost, which_pair);
      }
      else
        vcl_fprintf(fp, "xxxxxxxx | ");
    }
    vcl_fprintf(fp, "\n");
  }

  vcl_fclose(fp);
  return 0;
}
#endif


//: 
// \file    dbcvr_open_2d_cvmatch_even_grid_example.cxx
// \brief   
// \author  H. Can Aras
// \date    2007-01-31
// 
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstring>
#include <vsol/vsol_point_2d.h>
#include <bsold/bsold_interp_curve_2d.h>
#include <bsold/algo/bsold_curve_algs.h>
#include <dbcvr/dbcvr_open_2d_cvmatch_even_grid.h>
#include <bgld/bgld_eno_curve.h>

// Can says: In the future, it would be better to write an I/O class 
// for bsold curve class. I am re-using these functions for the time-being.

void loadCON(std::string fileName, std::vector<vsol_point_2d_sptr> &points)
{
  std::ifstream infp(fileName.c_str());
  char magicNum[200];

  infp.getline(magicNum,200);
  if (std::strncmp(magicNum,"CONTOUR",7))
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be CONTOUR " << magicNum << std::endl;
    std::exit(1);
  }

  char openFlag[200];
  infp.getline(openFlag,200);
  if (!std::strncmp(openFlag,"OPEN",4))
    std::cout << "Open Curve\n" << std::endl;
  else if (!std::strncmp(openFlag,"CLOSE",5))
    std::cout << "Closed Curve\n" << std::endl;
  else
  {
    std::cerr << "Invalid File " << fileName.c_str() << std::endl;
    std::cerr << "Should be OPEN/CLOSE " << openFlag << std::endl;
    std::exit(1);
  }

  int i,numOfPoints;
  infp >> numOfPoints;

  double x,y;
  for (i=0;i<numOfPoints;i++)
  {
    infp >> x >> y;
    points.push_back(new vsol_point_2d(x, y));
  }
  infp.close();
}

void writeCON(std::string fileName, bsold_interp_curve_2d &c, int numpoints)
{
  std::ofstream outfp(fileName.c_str());
  assert(outfp.fail());
  outfp << "CONTOUR" << std::endl;
  outfp << "OPEN" << std::endl;
  outfp << numpoints << std::endl;
  double ds = c.length()/(numpoints-1);
  for(int i=0; i<numpoints; i++)
  {
    vsol_point_2d_sptr p = c.point_at(i*ds);
    outfp << p->x() << " " << p->y() << " " << std::endl;
  }
  outfp.close();
}

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

#if 1
int main(int argc, char** argv)
{
  std::cout << "3D CURVE MATCHING EXAMPLE" << std::endl;
  std::string batch_fname = argv[1];
  std::ifstream infp(batch_fname.c_str());

  while(1)
  {
    std::string inp1, inp2, out;
    infp >> inp1;
    infp >> inp2;
    infp >> out;
    if(inp1.size() == 0)
      break;

    std::cout << inp1 << std::endl;
    std::cout << inp2 << std::endl;
    std::cout << out << std::endl;

    // construct the first curve
    std::vector<vsol_point_2d_sptr> points1;
    loadCON(inp1, points1);
    bsold_interp_curve_2d curve1;
    vnl_vector<double> samples1;
    bsold_curve_algs::interpolate_eno(&curve1,points1,samples1);

    // construct the second curve
    std::vector<vsol_point_2d_sptr> points2;
    loadCON(inp2, points2);
    bsold_interp_curve_2d curve2;
    vnl_vector<double> samples2;
    bsold_curve_algs::interpolate_eno(&curve2,points2,samples2);

    double R1 = 0.7;
    int template_size = 3;
/*
    FILE *fp = std::fopen("D:/MyDocs/projects/PuzzleSolving/Temp/2d_match_tests/info.txt", "w");
//    std::fprintf(fp, "intervals: %d\n", curve1.size());
    for(unsigned i=0; i < curve1.size(); i++)
    {
      bgld_eno_curve *p = (bgld_eno_curve *) curve1.interval(i);
//      std::fprintf(fp, "tstart: %f pstart: %f %f\n", p->start_t(), p->point_at(p->start_t()).x(), p->point_at(p->start_t()).y());
//      std::fprintf(fp, "%.10f %.10f\n", p->point_at(p->start_t()).x(), p->point_at(p->start_t()).y());
//      std::fprintf(fp, "tend: %f pend: %f %f\n", p->end_t(), p->point_at(p->end_t()).x(), p->point_at(p->end_t()).y());
//      std::fprintf(fp, "tangentatstart: %f %f\n", p->tangent_at(p->start_t()).x(), p->tangent_at(p->start_t()).y());
      std::fprintf(fp, "%f\n", p->tangent_angle_at(p->start_t()));
//      std::fprintf(fp, "curvatureatstart: %f\n", p->curvature_at(p->start_t()));
//      std::fprintf(fp, "%.10f\n", p->curvature_at(p->start_t()));
//      std::fprintf(fp, "xcoeffs: %f %f %f\n", p->coef_x(0), p->coef_x(1), p->coef_x(2));
//      std::fprintf(fp, "ycoeffs: %f %f %f\n", p->coef_y(0), p->coef_y(1), p->coef_y(2));
    }
    std::fclose(fp);
*/
    dbcvr_open_2d_cvmatch_even_grid matching(&curve1, &curve2,
                                             curve1.size()+1, curve2.size()+1,
                                             R1, template_size);

    std::pair<double, FinalMapType*> result;
    result = matching.match();
    
    std::string curve_out_1 = out;
    std::string curve_out_2 = out;
    std::string match_out = out;
    curve_out_1 += "curve1.con";
    curve_out_2 += "curve2.con";
    match_out += "match.txt";

    // write interpolated curves
    writeCON(curve_out_1, curve1, curve1.size()+1);
    writeCON(curve_out_2, curve2, curve2.size()+1);
    // write the matches
    FinalMapType* fmap = result.second;
    std::ofstream outfp(match_out.c_str());
    outfp << "Final Cost = " << result.first << std::endl;
        outfp << fmap->size() << std::endl;
    for(unsigned i = 0; i < fmap->size(); i++)
      outfp << (*fmap)[i].first << " " << (*fmap)[i].second << std::endl;
    ///////

    std::cout << result.first << std::endl;
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
#if 0
int main(int argc, char** argv)
{
  double R1 = 0.7;
  int template_size = 3;

  std::cout << "OPEN SPACE CURVE MATCHING USING EVEN GRID EXAMPLE" << std::endl;
  std::string batch_fname = argv[1];
  std::ifstream infp(batch_fname.c_str());

  std::vector<std::string> curve_names;
  std::vector<bsold_interp_curve_2d> curves;
  std::vector<bsold_interp_curve_2d> curves_inv;

  std::string in_folder;
  infp >> in_folder;
  std::string out_folder;
  infp >> out_folder;
  int num_curves;
  infp >> num_curves;

  std::string out_fname = out_folder + "out.txt";
  FILE *fp = std::fopen(out_fname.c_str(), "w");

  curves.resize(num_curves);
  curves_inv.resize(num_curves);

  // read the curves and store them
  for(int j=0; j<num_curves; j++)
  {
    std::string inp;
    infp >> inp;
    curve_names.push_back(inp);

    // construct the curve and its inverse
    inp = in_folder + inp + ".con";
    std::vector<vsol_point_2d_sptr> points;
    std::vector<vsol_point_2d_sptr> points_inv;

    loadCON(inp, points);
    for(int i=points.size()-1; i>=0; i--)
      points_inv.push_back(points[i]);
    
    vnl_vector<double> samples1;
    vnl_vector<double> samples2;
    bsold_curve_algs::interpolate_eno(&(curves[j]),points,samples1);
    bsold_curve_algs::interpolate_eno(&(curves_inv[j]),points_inv,samples2);
  }

  std::fprintf(fp, "        ");
  for(unsigned i=0; i<curves.size(); i++)
    std::fprintf(fp, "%s      ", curve_names[i].c_str());
  std::fprintf(fp, "\n");
  
  for(unsigned i=0; i<curves.size(); i++)
  {
    std::fprintf(fp, "%s  ", curve_names[i].c_str());
    double cost;
    for(unsigned j=0; j<curves.size(); j++)
    {
      if(1)
      {
        // first curve clockwise, second curve clockwise

        dbcvr_open_2d_cvmatch_even_grid matching1(&(curves[i]), &(curves[j]), curves[i].size()+1, curves[j].size()+1,
                                                    R1, template_size);

//        dbcvr_open_2d_cvmatch_even_grid matching2(&(curves[i]), &(curves_inv[j]), curves[i].size()+1, curves_inv[j].size()+1,
//                                                    R1, template_size);

//        dbcvr_open_2d_cvmatch_even_grid matching3(&(curves_inv[i]), &(curves_inv[j]), curves_inv[i].size()+1, curves_inv[j].size()+1,
//                                                    R1, template_size);

//        dbcvr_open_2d_cvmatch_even_grid matching4(&(curves_inv[i]), &(curves[j]), curves_inv[i].size()+1, curves[j].size()+1,
//                                                    R1, template_size);

        std::pair<double, FinalMapType*> result1, result2, result3, result4;
        result1 = matching1.match();
//        result2 = matching2.match();
//        result3 = matching3.match();
//        result4 = matching4.match();

        double cost1 = result1.first;
//        double cost2 = result2.first;
//        double cost3 = result3.first;
//        double cost4 = result4.first;

        bsold_interp_curve_2d *write_curve_1;
        bsold_interp_curve_2d *write_curve_2;
        FinalMapType* fmap;
        /*int which_pair;

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
        }*/
        cost = cost1;
        fmap = result1.second;
        write_curve_1 = &curves[i];
        write_curve_2 = &curves[j];
        std::fprintf(fp, "%4.6f | ", cost);

        /*switch(which_pair)
        { 
        case 1:
          fmap = result1.second;
          write_curve_1 = &curves[i];
          write_curve_2 = &curves[j];
          break;
        case 2:
          fmap = result2.second;
          write_curve_1 = &curves[i];
          write_curve_2 = &curves_inv[j];
          break;
        case 3:
          fmap = result3.second;
          write_curve_1 = &curves_inv[i];
          write_curve_2 = &curves_inv[j];
          break;
        case 4:
          fmap = result4.second;
          write_curve_1 = &curves_inv[i];
          write_curve_2 = &curves[j];
          break;
        default:
          break;
        }*/

        std::string curve_out_1 = out_folder + curve_names[i] + "_" + curve_names[j] + "_curve1.con";
        std::string curve_out_2 = out_folder + curve_names[i] + "_" + curve_names[j] + "_curve2.con";
        std::string match_out = out_folder + curve_names[i] + "_" + curve_names[j] + "_match.txt";
        // write interpolated curves

      writeCON(curve_out_1, *write_curve_1, (*write_curve_1).size()+1);
      writeCON(curve_out_2, *write_curve_2, (*write_curve_2).size()+1);

        // write the match
        std::ofstream outfp(match_out.c_str());
        // cost normalized by the length
        outfp << "Final Cost = " << cost << std::endl;

        outfp << fmap->size() << std::endl;

        for(unsigned k = 0; k < fmap->size(); k++)
          outfp << (*fmap)[k].first << " " << (*fmap)[k].second << std::endl;

        std::printf("Matching %s and %s, Cost: %4.6f\n", 
          curve_names[i].c_str(), curve_names[j].c_str(), cost);

//        std::printf("Matching %s and %s, Cost: %4.6f, Matched Config: %d\n", 
//          curve_names[i].c_str(), curve_names[j].c_str(), cost, which_pair);
      }
      else
        std::fprintf(fp, "xxxxxxxx | ");
    }
    std::fprintf(fp, "\n");
  }

  std::fclose(fp);
  return 0;
}
#endif

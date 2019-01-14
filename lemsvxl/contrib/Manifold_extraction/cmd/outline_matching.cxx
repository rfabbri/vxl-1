//: 
// \file    outline_matching.cxx
// \brief   an example for matching the outlines of two shapes based on Lie distance notion
// 
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include "Lie_contour_match.h"



void loadCON(std::string fileName, std::vector<vsol_point_2d_sptr> &points)
    {
    std::ifstream infp(fileName.c_str());
    char magicNum[200];

    infp.getline(magicNum,200);
    if (strncmp(magicNum,"CONTOUR",7))
        {
        std::cerr << "Invalid File " << fileName.c_str() << std::endl;
        std::cerr << "Should be CONTOUR " << magicNum << std::endl;
        exit(1);
        }

    char openFlag[200];
    infp.getline(openFlag,200);
    if (!strncmp(openFlag,"OPEN",4))
        std::cout << "Open Curve\n" << std::endl;
    else if (!strncmp(openFlag,"CLOSE",5))
        std::cout << "Closed Curve\n" << std::endl;
    else
        {
        std::cerr << "Invalid File " << fileName.c_str() << std::endl;
        std::cerr << "Should be OPEN/CLOSE " << openFlag << std::endl;
        exit(1);
        }

    int i,numOfPoints;
    infp >> numOfPoints;

    double x,y;
    for (i=0;i<numOfPoints;i++)
        {
        infp >> x >> y;
        std::cout << "x: " << x << "y: " << y << std::endl;
        points.push_back(new vsol_point_2d(x, y));
        }
    infp.close();
    }

void writeCON(std::string fileName, dbsol_interp_curve_2d_sptr c, int numpoints)
    {
    std::ofstream outfp(fileName.c_str());
    assert(outfp != NULL);
    outfp << "CONTOUR" << std::endl;
    outfp << "CLOSE" << std::endl;
    outfp << numpoints << std::endl;
    double ds = c->length()/(numpoints-1);
    for(int i=0; i<numpoints; i++)
        {
        vsol_point_2d_sptr p = c->point_at(i*ds);
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


int main(int argc, char** argv)
    {
    std::cout << "3D CURVE MATCHING EXAMPLE" << std::endl;
    std::string batch_fname = argv[1];
    std::ifstream infp(batch_fname.c_str());
 
        std::string inp1, inp2, out;
        infp >> inp1;
        infp >> inp2;
        infp >> out;
        
        std::cout << inp1 << std::endl;
        std::cout << inp2 << std::endl;
        std::cout << out << std::endl;

        // construct the first curve
        std::vector<vsol_point_2d_sptr> points1;
        loadCON(inp1, points1);
        dbsol_interp_curve_2d curve1;
        vnl_vector<double> samples1;
        
        // construct the second curve
        std::vector<vsol_point_2d_sptr> points2;
        loadCON(inp2, points2);
        dbsol_interp_curve_2d curve2;
        vnl_vector<double> samples2;
       

        double R1 = 0.7;
        int template_size = 3;

        
  dbsol_interp_curve_2d_sptr curve1_sptr = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curve2_sptr = new dbsol_interp_curve_2d();

  dbsol_curve_algs::interpolate_eno(curve1_sptr,points1,samples1);
  dbsol_curve_algs::interpolate_eno(curve2_sptr,points2,samples2);

   /*curve1_sptr->make(curve1.intervals());
   curve2_sptr->make(curve2.intervals());*/

//curve1.make(curve1_sptr->intervals());
//curve2.make(curve2_sptr->intervals());


        Lie_contour_match matching(curve1_sptr,curve2_sptr,curve1_sptr->size(), curve2_sptr->size(),
            R1, template_size);

        /*dbcvr_clsd_cvmatch_even matching(&curve1,&curve2,curve1.size()+1, curve2.size()+1,
            R1, template_size);*/

        matching.Match();

        std::vector<double> cost_vec = matching.finalCost();

        double min_cost = 1e20;
        int min_idx = -1; // infinity

        for (unsigned int i = 0;i<cost_vec.size();i++)
            {
            std::cout << "cost vector: " << i << " " << cost_vec[i] << std::endl;
            if (min_cost > cost_vec[i])
                {
                min_idx = i;
                min_cost = cost_vec[i];
                }
            }

        std::cout << "minimum index: " << min_idx << std::endl;

        FinalMapType fmap = matching.finalMap(min_idx);

        std::string curve_out_1 = out;
        std::string curve_out_2 = out;
        std::string match_out = out;
        curve_out_1 += "curve1.con";
        curve_out_2 += "curve2.con";
        match_out += "match.txt";

        // write interpolated curves
        /*writeCON(curve_out_1, curve1_sptr, curve1.size()+1);
        writeCON(curve_out_2, curve2_sptr, curve2.size()+1);*/

        std::ofstream outfp(match_out.c_str());
        outfp << "Final Cost = " << matching.finalCost(min_idx) << std::endl;
        outfp << fmap.size() << std::endl;
        for(unsigned i = 0; i < fmap.size(); i++)
            outfp << (fmap)[i].first << " " << (fmap)[i].second << std::endl;

    return 0;
    }

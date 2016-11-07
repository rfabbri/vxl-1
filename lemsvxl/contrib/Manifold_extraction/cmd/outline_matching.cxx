//: 
// \file    outline_matching.cxx
// \brief   an example for matching the outlines of two shapes based on Lie distance notion
// 
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include "Lie_contour_match.h"



void loadCON(vcl_string fileName, vcl_vector<vsol_point_2d_sptr> &points)
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

    double x,y;
    for (i=0;i<numOfPoints;i++)
        {
        infp >> x >> y;
        vcl_cout << "x: " << x << "y: " << y << vcl_endl;
        points.push_back(new vsol_point_2d(x, y));
        }
    infp.close();
    }

void writeCON(vcl_string fileName, dbsol_interp_curve_2d_sptr c, int numpoints)
    {
    vcl_ofstream outfp(fileName.c_str());
    assert(outfp != NULL);
    outfp << "CONTOUR" << vcl_endl;
    outfp << "CLOSE" << vcl_endl;
    outfp << numpoints << vcl_endl;
    double ds = c->length()/(numpoints-1);
    for(int i=0; i<numpoints; i++)
        {
        vsol_point_2d_sptr p = c->point_at(i*ds);
        outfp << p->x() << " " << p->y() << " " << vcl_endl;
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
    vcl_cout << "3D CURVE MATCHING EXAMPLE" << vcl_endl;
    vcl_string batch_fname = argv[1];
    vcl_ifstream infp(batch_fname.c_str());
 
        vcl_string inp1, inp2, out;
        infp >> inp1;
        infp >> inp2;
        infp >> out;
        
        vcl_cout << inp1 << vcl_endl;
        vcl_cout << inp2 << vcl_endl;
        vcl_cout << out << vcl_endl;

        // construct the first curve
        vcl_vector<vsol_point_2d_sptr> points1;
        loadCON(inp1, points1);
        dbsol_interp_curve_2d curve1;
        vnl_vector<double> samples1;
        
        // construct the second curve
        vcl_vector<vsol_point_2d_sptr> points2;
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

        vcl_vector<double> cost_vec = matching.finalCost();

        double min_cost = 1e20;
        int min_idx = -1; // infinity

        for (unsigned int i = 0;i<cost_vec.size();i++)
            {
            vcl_cout << "cost vector: " << i << " " << cost_vec[i] << vcl_endl;
            if (min_cost > cost_vec[i])
                {
                min_idx = i;
                min_cost = cost_vec[i];
                }
            }

        vcl_cout << "minimum index: " << min_idx << vcl_endl;

        FinalMapType fmap = matching.finalMap(min_idx);

        vcl_string curve_out_1 = out;
        vcl_string curve_out_2 = out;
        vcl_string match_out = out;
        curve_out_1 += "curve1.con";
        curve_out_2 += "curve2.con";
        match_out += "match.txt";

        // write interpolated curves
        /*writeCON(curve_out_1, curve1_sptr, curve1.size()+1);
        writeCON(curve_out_2, curve2_sptr, curve2.size()+1);*/

        vcl_ofstream outfp(match_out.c_str());
        outfp << "Final Cost = " << matching.finalCost(min_idx) << vcl_endl;
        outfp << fmap.size() << vcl_endl;
        for(unsigned i = 0; i < fmap.size(); i++)
            outfp << (fmap)[i].first << " " << (fmap)[i].second << vcl_endl;

    return 0;
    }

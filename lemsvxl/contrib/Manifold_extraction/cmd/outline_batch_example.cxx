// \file    outline_batch_example.cxx
// \brief   an example for matching the outlines of a database of shapes

// 
#include <vcl_cstdio.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>


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

double compute_lie_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples )
    {
    double lie_cost = 0,length_1,length_2,angle_1,angle_2,scale_comp,angle_comp;

  for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
      {
      length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
      length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

      angle_1 = vcl_atan2(curve1_samples[i+1]->y()-curve1_samples[i]->y(),curve1_samples[i+1]->x()-curve1_samples[i]->x());
      angle_2 = vcl_atan2(curve2_samples[i+1]->y()-curve2_samples[i]->y(),curve2_samples[i+1]->x()-curve2_samples[i]->x());

      scale_comp = vcl_log(length_2/length_1);
      angle_comp = angle_2 - angle_1;
      lie_cost = lie_cost +  scale_comp* scale_comp + vcl_fabs(angle_comp);
      }

   return lie_cost;
    }

double compute_kimia_cost(vcl_vector<vsol_point_2d_sptr> curve1_samples,vcl_vector<vsol_point_2d_sptr> curve2_samples,
                           vcl_vector<double> tangent_angle_c1,vcl_vector<double> tangent_angle_c2)
    {
double kimia_cost = 0,length_1,length_2;

      for (unsigned int i = 0;i<curve1_samples.size()-1;i++)
          {
    length_1 = curve1_samples[i]->distance(curve1_samples[i+1]);
      length_2 = curve2_samples[i]->distance(curve2_samples[i+1]);

       kimia_cost = kimia_cost + ((length_1 - length_2)*(length_1 - length_2))/(length_1 + length_2) + 
           (vcl_fabs(tangent_angle_c1[i] - tangent_angle_c2[i]));
          }

      return kimia_cost;
    }


int main(int argc, char** argv)
    {
    vcl_string batch_fname = argv[1];
    vcl_ifstream infp(batch_fname.c_str());
 
        vcl_string inp1, out;
        infp >> out;
        infp >> inp1;

        vcl_ofstream outfp(out.c_str());

        // construct the first curve
        vcl_vector<vsol_point_2d_sptr> points1;
        loadCON(inp1, points1);
        dbsol_interp_curve_2d curve1;
        vnl_vector<double> samples1;

        while (1)
            {
            vcl_string inp2;
        infp >> inp2;

        if(inp2.size() == 0)
      break;

        vcl_cout << inp1 << vcl_endl;
        vcl_cout << inp2 << vcl_endl;
        vcl_cout << out << vcl_endl;

        // construct the second curve
        vcl_vector<vsol_point_2d_sptr> points2;
        loadCON(inp2, points2);
        dbsol_interp_curve_2d curve2;
        vnl_vector<double> samples2;

  dbsol_curve_algs::interpolate_eno(&curve1,points1,samples1);
  dbsol_curve_algs::interpolate_eno(&curve2,points2,samples2);

     vcl_vector<double> tangent_angle_c1,tangent_angle_c2;
     unsigned int num_samples_c1 = 500,num_samples_c2 = 500;
     double s,Lie_cost,kimia_cost;
     vcl_vector<vsol_point_2d_sptr> curve1_samples,curve2_samples;

   for (unsigned int i = 0;i<num_samples_c1;i++)
       {
    s = (double(i)/double(num_samples_c1))*curve1.length();

   vsol_point_2d_sptr sample = curve1.point_at(s);

   curve1_samples.push_back(sample);

   tangent_angle_c1.push_back(curve1.tangent_angle_at(s));

   // vcl_cout << "curve 1: " << " " << s << " " << sample->x() << " " << sample->y() << vcl_endl;
       }

   for (unsigned int i = 0;i<num_samples_c2;i++)
       {
    s = (double(i)/double(num_samples_c2))*curve2.length();

   vsol_point_2d_sptr sample = curve2.point_at(s);

   curve2_samples.push_back(sample);

    tangent_angle_c2.push_back(curve2.tangent_angle_at(s));

   // vcl_cout << "curve 2: " << " " << s << " " << sample->x() << " " << sample->y() << vcl_endl;
       }

   Lie_cost = compute_lie_cost(curve1_samples,curve2_samples );
   kimia_cost = compute_kimia_cost(curve1_samples,curve2_samples,tangent_angle_c1,tangent_angle_c2);

   outfp << Lie_cost << "  " << kimia_cost << vcl_endl;

            }

    outfp.close();
    return 0;
    }

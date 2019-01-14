//  This is under contrib\Manifold_extraction\vis
// Brief: An example for visualizing stereographic projections of geodesics from S(3) onto S(2)


#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_complexify.h>
#include <vnl/vnl_vector_fixed.h>
#include <vector>
#include <vbl/vbl_array_3d.h>
#include <vgl/vgl_point_3d.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>
#include <vgui/vgui_dialog.h>

#include <bgui3d/bgui3d.h>


#include <Inventor/nodes/SoGroup.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeData.h>
#include <VolumeViz/nodes/SoVolumeRender.h>
#include <VolumeViz/nodes/SoVolumeRendering.h>
#include <VolumeViz/nodes/SoTransferFunction.h>
#include <Inventor/nodes/SoCylinder.h>
#include <biov/biov_examiner_tableau.h>

vnl_matrix<std::complex<double> > matrix_exp_SU2 (double t,vnl_matrix<std::complex<double> > g)
    {
vnl_matrix<std::complex<double> >I(2,2,0.0);
I.put(0,0,1);
I.put(1,1,1);

vnl_matrix<std::complex<double> >resultant(2,2,0.0);

if (t == 0)
    {
resultant.put(0,0,1);
resultant.put(1,1,1);
    }
else
    {
double x1 = t*g.get(0,0).imag();
double x2 = t*g.get(0,1).imag();
double x3 = t*(-g.get(0,1).real());

double phi = std::sqrt(std::pow(x1,2) + std::pow(x2,2) + std::pow(x3,2));

resultant = I*std::cos(phi) + g*std::sin(phi)*t/phi;
    }
return resultant;
    }


//biov_examiner_tableau_new setup_biov_examiner_tableau(SbVec3s dim,uint8_t * voxels)
//    {
//
//
//    SoGroup *root = new SoGroup;
//  
//root->ref();
//SoVolumeRendering::init();
//
//  // Add SoVolumeData to scene graph
//  SoVolumeData * volumedata = new SoVolumeData();
//  volumedata->setVolumeData(dim, voxels, SoVolumeData::UNSIGNED_BYTE);
//  root->addChild(volumedata);
//  
//  // Add TransferFunction (color map) to scene graph
//
// SoTransferFunction * transfunc = new SoTransferFunction();
//  transfunc->predefColorMap = SoTransferFunction::PHYSICS;
//  root->addChild(transfunc);
//  
//  // Add VolumeRender to scene graph
//  SoVolumeRender * volrend = new SoVolumeRender();
//  root->addChild(volrend);
//
//  biov_examiner_tableau_new tab3d(root, transfunc);
//  // root->unref();
//  return tab3d;
//    }


int main(int argc,char **argv)
    {
std::string projected_points = argv[1];
std::string vol3d = argv[2];


int my_argc = 1;
    char ** my_argv = new char*[argc+1];
    for (int i = 0;i<argc;i++)
        my_argv[i] = argv[i];
    my_argv[argc] = "--mfc-use-gl";
    vgui::init(my_argc,my_argv);
    delete []my_argv;

    std::ofstream ofstr(projected_points.c_str());
    std::ofstream fstream(vol3d.c_str());

    // generate a set of elements along a geodesic on the manifold by finding 
    // exp(A*t) where A is a Lie algebra element and t is a continuous valued
    // variable 


        vnl_matrix<double> r1(2,2,0.0);
        vnl_matrix<double> i1(2,2,0.0);
        vnl_matrix<double> r2(2,2,0.0);
        vnl_matrix<double> i2(2,2,0.0);
         vnl_matrix<double> r3(2,2,0.0);
        vnl_matrix<double> i3(2,2,0.0);

        vnl_vector_fixed<double,2> i1_row1(0.0,1);
        vnl_vector_fixed<double,2> i1_row2(1,0.0);

        vnl_vector_fixed<double,2> r2_row1(0.0,1);
        vnl_vector_fixed<double,2> r2_row2(-1,0.0);

         vnl_vector_fixed<double,2> i3_row1(1,0.0);
        vnl_vector_fixed<double,2> i3_row2(0.0,-1);

        i1.set_row(0,i1_row1);
        i1.set_row(1,i1_row2);

        r2.set_row(0,r2_row1);
        r2.set_row(1,r2_row2);

        i3.set_row(0,i3_row1);
        i3.set_row(1,i3_row2);

       
// g1,g2 and g3 represent the generators of the su(2) Lie algebra
        vnl_matrix<std::complex<double> > g1 = 
  vnl_complexify(r1,i1);

 //       std::cout << g1 << std::endl;

        vnl_matrix<std::complex<double> > g2 = 
  vnl_complexify(r2,i2);

 //       std::cout << g2 << std::endl;

        vnl_matrix<std::complex<double> > g3 = 
  vnl_complexify(r3,i3);

 //       std::cout << g3 << std::endl;


// vnl_matrix<std::complex<double> > g = g1*1 + g2*(0) + g3*(0);

         double q0,q1,q2,q3,x,y,z,min_x = 1e10,max_x = 1e-10,min_y = 1e10,max_y = 1e-10,min_z = 1e10,max_z = 1e-10;
  std::vector<vgl_point_3d<double> >coordinates;

        for (unsigned int k = 0;k<=10;k = k + 1)
            {
        vnl_matrix<std::complex<double> > g = g1  + g2*k ;
        double step_size = 0.01,count = 0;

 for (double t = 0;t<=4*vnl_math::pi ;t += step_size)
        {
  count++;

vnl_matrix<std::complex<double> >G = matrix_exp_SU2(t,g);

//ofstr << count << std::endl;
//ofstr << G << std::endl;

q0 = G.get(0,0).real();
q1 = G.get(0,0).imag();
q2 = G.get(0,1).real();
q3 = G.get(0,1).imag();
//q0 = std::cos(t/2);
//q1 = 0;
//q2 = 0;
//q3 = std::sin(t/2);

//ofstr << q0 << std::endl;
//ofstr << q1 << std::endl;
//ofstr << q2 << std::endl;
//ofstr << q3 << std::endl;

#if 0
if (q0 != 0)
    {
x = q1/(1-q0);
y = q2/(1-q0);
z = q3/(1-q0);
        }
#endif

if (q0 + q1 + q2 + q3 != 2)
    {
x = 1+((2*q1-1)/(2-q0-q1-q2-q3));
y = 1+((2*q2-1)/(2-q0-q1-q2-q3));
z = 1+((2*q3-1)/(2-q0-q1-q2-q3));
   /* x = cos(t);
    y = 0;
    z = sin(t);*/
        
x = 100*x;
y = 100*y;
z = 100*z;

vgl_point_3d<double> P(x,y,z);

coordinates.push_back(P);

if (x >= max_x)
max_x = x;
if(x <= min_x)
min_x = x;
if (y >= max_y)
max_y = y;
if(y <= min_y)
min_y = y;
if (z >= max_z)
max_z = z;
if(z <= min_z)
min_z = z;

// ofstr << "(x=" <<x <<"," <<"y=" << y <<","<<"z="<<z<<")" << std::endl;
ofstr << x << std::endl;
ofstr << y << std::endl;
ofstr << z << std::endl;
    }
        }
            }

int x_dim = (unsigned int)(max_x - min_x);
int y_dim = (unsigned int)(max_y - min_y);
int z_dim = (unsigned int)(max_z - min_z);
 
  vbl_array_3d<unsigned char>vol(2*x_dim,2*y_dim,2*z_dim,0.0);
// vbl_array_3d<unsigned char>vol(100,100,100,0.0);
    
std::cout << "co-ordinates size" << coordinates.size() << std::endl;

for (std::vector<vgl_point_3d<double> >::iterator iter = coordinates.begin();
     iter != coordinates.end();iter++)
    {
    int x1 = iter->x() - min_x;
    int y1 = iter->y() - min_y;
    int z1 = iter->z() - min_z;
    vol[x1][y1][z1] = (unsigned char)255;
   // fstream << "loc : " << "(" << x1 << "," << y1 << "," << z1 << ")" << "value: " << vol[x1][y1][z1] << std::endl;
    fstream << x1 << std::endl;
    fstream << y1 << std::endl;
    fstream << z1 << std::endl; 
    }

std::vector<unsigned char>vol_vec;

for (unsigned int k = 0;k<vol.get_row3_count();k++)
    {
    for (unsigned int j = 0;j<vol.get_row2_count();j++)
        {
        for (unsigned int i = 0;i<vol.get_row1_count();i++)
            {
vol_vec.push_back(vol[i][j][k]);

            }
        }
    }

 bgui3d_init();

const size_t blocksize = vol_vec.size();// vol_vector.size();
  uint8_t * voxels = new uint8_t[blocksize];

  (void)memset(voxels, 0, blocksize);

   for(unsigned long i = 0; i < blocksize; i++)
 voxels[i] =  vol.data_block()[i];

   SbVec3s dim = SbVec3s(vol.get_row1_count(),vol.get_row2_count(),vol.get_row3_count());

 // biov_examiner_tableau_new tab3d = setup_biov_examiner_tableau(dim,voxels);

   SoGroup *root = new SoGroup;
  
root->ref();
SoVolumeRendering::init();

  // Add SoVolumeData to scene graph
  SoVolumeData * volumedata = new SoVolumeData();
  volumedata->setVolumeData(dim, voxels, SoVolumeData::UNSIGNED_BYTE);
  root->addChild(volumedata);
  
  // Add TransferFunction (color map) to scene graph

 SoTransferFunction * transfunc = new SoTransferFunction();
  transfunc->predefColorMap = SoTransferFunction::PHYSICS;
  root->addChild(transfunc);
  
  // Add VolumeRender to scene graph
  SoVolumeRender * volrend = new SoVolumeRender();
  root->addChild(volrend);

  biov_examiner_tableau_new tab3d(root, transfunc);
  // root->unref();

  vgui_shell_tableau_new shell(tab3d);
  int return_value = vgui::run(shell, 400, 400);
  delete [] voxels;
  ofstr.close();
  fstream.close();
  return return_value;
    }



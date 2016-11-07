//// This files finds the shutter speeds corresponiding to a set of stacked png images, based on the
//// intensity ratio among images. It uses SVD to solve for absolute shutter sppeds
//
//// if Si/Sj = Rij, the given the set of Rij, find Si, Sj
//
//// 
//#include <vcl_iostream.h>
//#include <vcl_fstream.h>
//#include <vcl_string.h>
//#include <vcl_vector.h>
//#include <vcl_cstdio.h>
//#include <vcl_vector.h>
//#include <vnl/vnl_matrix.h>
//#include <vnl/vnl_vector.h>
//#include <vnl/algo/vnl_svd.h>
//#include <vgl/vgl_point_2d.h>
//#include <vgl/vgl_distance.h>
//#include <vgl/vgl_polygon.h>
//#include <vil/vil_image_view.h>
//#include <vil/vil_load.h>
//#include <vil/vil_save.h>
//#include <vil/vil_crop.h>
//#include <vil/vil_convert.h>
//#include <vul/vul_file.h>
//#include <vul/vul_file_iterator.h>
//
//
//// Get all intensities in a region.
//bool get_shutter(vcl_string ofile,
//                 vcl_vector<vcl_string> ifile)
//{
//
//    vcl_ofstream ofs( ofile.c_str() );
//    bool is_missing = true;
//    vil_image_view<vxl_byte> this_image;
//    vil_image_view<vxl_byte> next_image;
//    long float shutter = 0.0;
//    float shutter_2 = 0.0;
//    float variance = 0.0;
//    float stdv = 0.0;
//    float count = 0.0;
//    float diff = 0.0;
//    unsigned this_j0; 
//    unsigned next_j0; 
//    float temp1;
//    float temp2;
//    float coeff = 0.0;
//
//    // Set up the coefficient matrix.
//    vnl_matrix<float> coeff_matrix( 672, 8 , 0.0);
//    vnl_matrix<float> percent_matrix(168, 8 , 0.0);
//    unsigned matrix_index = 0;
//    unsigned percent_index = 0;
//
//    vcl_vector<unsigned> image_pos;
//    image_pos.push_back(2);
//    image_pos.push_back(4);
//    image_pos.push_back(5);
//    image_pos.push_back(6);
//    image_pos.push_back(7);
//    image_pos.push_back(8);
//    image_pos.push_back(9);
//    image_pos.push_back(10);
//
//    for (vcl_vector<vcl_string>::iterator it = ifile.begin(); it<ifile.end(); it++)
//    {
//        vcl_string file = (*it);
//        vil_image_view<vxl_byte> src(vil_load(file.c_str()) );
//
//        for( unsigned n =0; n<image_pos.size()-1; n++){ 
//
//            unsigned s = image_pos[n];
//            this_j0 = (s*480);
//            this_image = vil_crop(src, 0,640,this_j0,480);
//
//            for(unsigned m = n+1; m < image_pos.size(); m++)
//            {
//                unsigned next_s = image_pos[m];
//                next_j0 = (next_s)*480;
//                next_image = vil_crop(src, 0,640,next_j0,480);
//
//                shutter = 0.0;
//                diff = 0.0;
//                count = 0.0;
//                variance = 0.0;
//
//                //check if the image was missing (all white)
//                for( int i = 0; i < 640; i++ ){
//                    for( int j = 0; j < 480; j++ ){
//                        if ((this_image(i,j)<=30)||( this_image(i,j)>=230) ||
//                            (next_image(i,j)<=30) ||(next_image(i,j)>=230))
//                            continue;
//                        else {
//                            temp1 = float(next_image(i,j));
//                            temp2 = float(this_image(i,j));
//                            shutter = shutter + temp2/temp1;
//                            shutter_2 = shutter_2 + (temp1/temp2*temp1/temp2);
//                            diff = diff + abs(temp1 - temp2);
//                            count ++;
//                        }
//                    }
//
//                }
//
//                //check for if there are less than 3% of samples, if so, ignore
//                if (count*100/(648*480)< 70){ 
//                    coeff = 0.0;
//                    shutter = 0.0;
//                }
//                else{
//                    shutter = shutter/count;
//                    coeff = 1.0;
//                }
//                //      vcl_cout<< shutter<<"\n";
//                //variance = (shutter_2/count) - (shutter* shutter);
//                //diff = diff/count;
//                //ofs << s << ' ' << next_s << ' ' <<  shutter<< ' ' << variance << ' ' << count*100/(648*480) <<"\n";
//
//                //fill-in coefficient matrix
//                coeff_matrix(matrix_index, n) = coeff;
//                coeff_matrix(matrix_index, m)= - shutter;
//                percent_matrix(percent_index+n,m)= count*100/(648*480);
//                matrix_index ++;
//            }
//        }
//        percent_index = percent_index + 7;  
//    }
//    vnl_svd<float> svd(coeff_matrix);
//    // null vector gives the solution to the linear equation where b=[0]
//    //  vcl_cout<<svd.rank()<<"\n";
//    vcl_cout<<coeff_matrix<<"\n";
//    vcl_cout<<percent_matrix<<"\n";
//    vnl_vector<float> s = svd.nullvector();
//
//    ofs << 0.0 << "\n";
//    ofs << 0.0 << "\n";
//    ofs << s(0) << "\n";
//    ofs << 0.0 << "\n";
//
//    for(unsigned i =1; i< s.size(); i++)
//    {
//        ofs << s(i) << "\n";
//    }
//
//    ofs << 0.0 << "\n";
//
//    return true;
//
//}
//
//
//int main( int argc, char* argv[] )
//{  
//
//  vcl_vector<vcl_string> ifile;
//  ifile.push_back("E:\\dome_images\\00_calibration\\20832_1_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11006_1_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11213_1_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11108_1_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11709_2_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11808_1_01.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\20832_1_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11006_1_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11213_1_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11400_1_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11709_2_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11808_1_02.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\20832_1_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11006_1_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11213_1_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11400_1_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11709_2_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11808_1_13.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\20832_1_15.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11006_1_15.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11213_1_15.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11400_1_15.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11709_2_15.png");
//  ifile.push_back("E:\\dome_images\\00_calibration\\11808_1_15.png");
//  vcl_string ofile = "E:\\dome_images\\shutter_speed\\shutter_speeds.txt";
//    //vcl_string ifile = "E:\\dome_images\\00_calibration\\11709_1_16.png";
//
//    get_shutter(ofile, ifile);
//
//}

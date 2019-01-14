#include <cali/cali_param.h>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <cstddef>
#include <cstdlib>
#include <stdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>



cali_param::cali_param(std::string pathname):pathname_(pathname)
{
        std::vector<std::string>filepath;
        std::vector<double>parameters;
        // std::ifstream fstream(pathname_.c_str(),std::ios::in);
        std::ifstream qfstream(pathname_.c_str());

        char * val_string = new char[500];
        char* valstr;
        char* valstr1=NULL;

        double value;
        int i=1;

        // reading out all the paths specified in the parameters text file until '#end_of_paths' is encountered


        while (i) {

                qfstream.getline(val_string, 500);
                valstr = strtok(val_string,"=");
                std::cout<<" val_string "<<val_string<<std::endl;
                if ( strncmp(val_string,"#end_of_paths",13))

                {
                        valstr1 = strtok(NULL,";");

                        filepath.push_back(valstr1);

                }
                else
                        i = 0;
        }

        // reading out the numerical values specified in the paramters text file 
        while ((!i)&(!qfstream.eof()))
        {
                qfstream.getline(val_string, 300);
                valstr = strtok(val_string,"=");


                if (valstr != NULL)
                {
                        valstr1 = strtok(NULL,";");
                        value = atof(valstr1);
                        parameters.push_back(value);
                }

        }
 int j = 0;
 CENTERS = filepath[j++];
 DIFF = filepath[j++];
 CONVERGEDVALUES = filepath[j++];
 CONVERGEDVALUES_STAGE1 = filepath[j++];
 CONVERGEDVALUES_STAGE2 = filepath[j++];
 CMMINFO = filepath[j++];
 LOGFILE = filepath[j++];
 CONICS_BIN_FILE_BASE = filepath[j++];
 CORRESPONDENCEINFO = filepath[j++];
 OUTPUT_SCANFILE = filepath[j++];

 std::FILE *fp = std::fopen(LOGFILE.c_str(),"r");
 imgr_skyscan_log_header header(fp);


 j = 0;
 BALL_NUMBER = static_cast<int>(parameters[j++]);
 BALL_RADIUS_BIG = parameters[j++];
 BALL_DENSITY = parameters[j++];
 HEIGHT = parameters[j++];
 RADIUS = parameters[j++];
 THRESHOLD =parameters[j++] ;
 GAUSSIAN_SIGMA = parameters[j++];
 MIN_FIT_LENGTH = static_cast<int>(parameters[j++]);
 RMS_DISTANCE = parameters[j++];
 ASPECT_RATIO = parameters[j++];
 WIDTH_HEIGHT_DIFF_THRESHOLD = parameters[j++];
 WIDTH_THRESHOLD = parameters[j++];
 X_COORD_DIFF_RANGE = parameters[j++];
 Y_COORD_DIFF_RANGE = parameters[j++];
 SIZE_OF_X = static_cast<int>(parameters[j++]);
 NUM_OF_CORRESP = static_cast<unsigned>(parameters[j++]);
 NUM_OF_CONICS = static_cast<unsigned>(parameters[j++]);
 NUM_OF_PARAMETERS = static_cast<unsigned>(parameters[j++]);
 START = static_cast<int>(parameters[j++]);
 END = static_cast<int>(parameters[j++]);
 INTERVAL = static_cast<int>(parameters[j++]);
 ERROR_MARGIN_FACTOR = static_cast<unsigned>(parameters[j++]);

 std::cout<<" no. of parameters " << parameters.size() <<std::endl;
 std::cout<<" centers " << CENTERS <<std::endl;
 std::cout<<" diff " << DIFF <<std::endl;
 std::cout<<" convergedvalues " << CONVERGEDVALUES <<std::endl;
 std::cout << "converged values stage 1 "<<CONVERGEDVALUES_STAGE1<<std::endl;
 std::cout << "converged values stage 2 "<<CONVERGEDVALUES_STAGE2<<std::endl;
 std::cout<<"  cmminfo " << CMMINFO <<std::endl;
 std::cout<<" logfile " << LOGFILE <<std::endl;
 std::cout<<" conics_bin_file_base " <<  CONICS_BIN_FILE_BASE <<std::endl;
 std::cout<<" correspondenceinfo " <<  CORRESPONDENCEINFO <<std::endl;
 std::cout<<" output_scanfile " <<  OUTPUT_SCANFILE <<std::endl;

 std::cout<<" ball number " <<  BALL_NUMBER <<std::endl;
 std::cout<<" ball_radius_big " << BALL_RADIUS_BIG <<std::endl;
 std::cout<<" ball_density " << BALL_DENSITY  <<std::endl;
 std::cout<<" height" << HEIGHT <<std::endl;
 std::cout<<" radius" << RADIUS  <<std::endl;
 std::cout<<" threshold" << THRESHOLD  <<std::endl;
 std::cout<<" gaussian sigma" << GAUSSIAN_SIGMA  <<std::endl;
 std::cout<<" min fit length" << MIN_FIT_LENGTH <<std::endl;
 std::cout<<" rms distance" << RMS_DISTANCE  <<std::endl;
 std::cout<<" aspect ratio" << ASPECT_RATIO  <<std::endl;
 std::cout<<" width height diff threshold " << WIDTH_HEIGHT_DIFF_THRESHOLD <<std::endl;
 std::cout<<" width threshold" << WIDTH_THRESHOLD  <<std::endl;
 std::cout<<" x coord diff range " << X_COORD_DIFF_RANGE <<std::endl;
 std::cout<<" y coord diff range " << Y_COORD_DIFF_RANGE <<std::endl;
 std::cout<<" size_of_x " << SIZE_OF_X   <<std::endl;
 std::cout<<" num_of_corresp " << NUM_OF_CORRESP  <<std::endl;
 std::cout<<" num_of_conics " << NUM_OF_CONICS  <<std::endl;
 std::cout<<" num_of_parameters " << NUM_OF_PARAMETERS  <<std::endl;
 std::cout<<" starting at image " <<START <<std::endl;
 std::cout<<" ending at image " <<END <<std::endl;
 std::cout<<" interval " << INTERVAL  <<std::endl;
 std::cout<<" error margin factor " << ERROR_MARGIN_FACTOR <<std::endl;

SETPARAM.set_size(SIZE_OF_X);
PARAM.set_size(SIZE_OF_X);
SETMASK.set_size(BALL_NUMBER);
for (int i =0;i<SIZE_OF_X;i++)
{
    SETPARAM[i] = static_cast<int>(parameters[j++]);
}
for (int i =0;i<SIZE_OF_X;i++)
{
    PARAM[i] = parameters[j++];
}
for (int i =0;i<BALL_NUMBER;i++)
{
    SETMASK[i] = static_cast<int>(parameters[j++]);
  
}

 int k = 7;

 // setting the initial values of some of the parameters from the log file  

// getting the x-scale & the y-scale
PARAM[k++] =  header.camera_to_source_dist_ * 1000/header.cam_pixel_size_;
PARAM[k++] =  (header.camera_to_source_dist_ * 1000)/(header.cam_pixel_size_ *header.cam_xy_ratio_);

// getting the principal point
PARAM[k++] =  (header.number_of_columns_ - 1) / 2.0;
PARAM[k++] = header.optical_axis_;

// the next 4 parameters correspond to the turn table rotation quaternion which is assumed to be 90 degree
// rotation about x-axis according to the biotree coordinate system i.e to say turntable coordinate system should
// be rotated 90 degrees about x axis to align with the world coordinate system

PARAM[k++] = -0.707106781186547;
PARAM[k++] = 0;
PARAM[k++] = 0;
PARAM[k++] = 0.707106781186547;

// turn table translation
PARAM[k++] = 0;
PARAM[k++] = 0;
PARAM[k++] = header.object_to_source_dist_;



for (int i =0;i<SIZE_OF_X;i++)
{
    std::cout<<" parameter: "<<i<<"  " <<PARAM[i] <<std::endl;
}

delete valstr;
delete val_string;
qfstream.close();

std::string txt_file = CORRESPONDENCEINFO;

// reading out the information  from CORRESPONDENCEINFO file & storing in COORDS & HEIGHT_RADII
std::ifstream fstream(txt_file.c_str(),std::ios::in);
std::cout << "opening " << txt_file << "\n";
 
  
char val_string1[300];
double x = 0;
double y = 0; 
double w = 0; 
double h = 0;

if(fstream.good()){

    while (!fstream.eof()) {

           fstream >> x;
           fstream >> y;
           fstream >> w;
           fstream >> h;

           std::cout << x << " " << y << " " << w << " " << h << "\n";

           COORDS.push_back(std::pair<double,double> (x,y));
           HEIGHT_RADII.push_back(std::pair<double,double> (w,h));
   }  
}
else{
            std::cerr << " bad CORRESPONDENCEINFO filename: " << txt_file << "\n";
            std::cerr << "required for  reading out the information"
                    << " from  file & storing in COORDS & HEIGHT_RADII\n";

            exit(1);
}

/*
   while (!fstream.eof()) {


           fstream.getline(val_string1, 300);
           std::cout << val_string1;
           x = std::atof(val_string1);


           fstream.getline(val_string1, 300);

           y = std::atof(val_string1);



           COORDS.push_back(std::pair<double,double> (x,y));

           fstream.getline(val_string1, 300);
           std::cout << val_string1;
           w = std::atof(val_string1);


           fstream.getline(val_string1, 300);

           h = std::atof(val_string1);

           HEIGHT_RADII.push_back(std::pair<double,double> (w,h));
   }
   */
  fstream.close();
  
  std::fclose(fp);
}

cali_param::cali_param(void)
{
}

// copy constructor

cali_param::
cali_param(cali_param const& par) :
CENTERS(par.CENTERS),DIFF(par.DIFF), CONVERGEDVALUES(par.CONVERGEDVALUES),
CONVERGEDVALUES_STAGE1(par.CONVERGEDVALUES_STAGE1),CONVERGEDVALUES_STAGE2(par.CONVERGEDVALUES_STAGE2),
CMMINFO(par.CMMINFO),LOGFILE(par.LOGFILE),CONICS_BIN_FILE_BASE(par.CONICS_BIN_FILE_BASE), CORRESPONDENCEINFO(par.CORRESPONDENCEINFO),
OUTPUT_SCANFILE(par.OUTPUT_SCANFILE),BALL_NUMBER(par.BALL_NUMBER),BALL_RADIUS_BIG(par.BALL_RADIUS_BIG),
 BALL_DENSITY(par. BALL_DENSITY),HEIGHT(par.HEIGHT),RADIUS(par.RADIUS),THRESHOLD(par.THRESHOLD),
 GAUSSIAN_SIGMA(par.GAUSSIAN_SIGMA),MIN_FIT_LENGTH(par.MIN_FIT_LENGTH), 
 RMS_DISTANCE(par.RMS_DISTANCE), ASPECT_RATIO(par.ASPECT_RATIO),WIDTH_HEIGHT_DIFF_THRESHOLD(par.WIDTH_HEIGHT_DIFF_THRESHOLD),
 WIDTH_THRESHOLD(par.WIDTH_THRESHOLD),X_COORD_DIFF_RANGE(par.X_COORD_DIFF_RANGE),Y_COORD_DIFF_RANGE(par.Y_COORD_DIFF_RANGE),
 SIZE_OF_X(par.SIZE_OF_X),NUM_OF_CORRESP(par.NUM_OF_CORRESP),
 NUM_OF_CONICS(par.NUM_OF_CONICS),NUM_OF_PARAMETERS(par.NUM_OF_PARAMETERS),START(par.START),END(par.END),
 INTERVAL(par.INTERVAL),ERROR_MARGIN_FACTOR(par.ERROR_MARGIN_FACTOR),SETPARAM(par.SETPARAM),PARAM(par.PARAM),
 SETMASK(par.SETMASK),COORDS(par.COORDS),HEIGHT_RADII(par.HEIGHT_RADII),pathname_(par.pathname_)
{

}


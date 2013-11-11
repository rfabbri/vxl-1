#include <cali/cali_param.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstddef.h>
#include <vcl_cstdlib.h>
#include <stdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>



cali_param::cali_param(vcl_string pathname):pathname_(pathname)
{
        vcl_vector<vcl_string>filepath;
        vcl_vector<double>parameters;
        // vcl_ifstream fstream(pathname_.c_str(),vcl_ios::in);
        vcl_ifstream qfstream(pathname_.c_str());

        char * val_string = new char[500];
        char* valstr;
        char* valstr1=NULL;

        double value;
        int i=1;

        // reading out all the paths specified in the parameters text file until '#end_of_paths' is encountered


        while (i) {

                qfstream.getline(val_string, 500);
                valstr = strtok(val_string,"=");
                vcl_cout<<" val_string "<<val_string<<vcl_endl;
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

 vcl_FILE *fp = vcl_fopen(LOGFILE.c_str(),"r");
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

 vcl_cout<<" no. of parameters " << parameters.size() <<vcl_endl;
 vcl_cout<<" centers " << CENTERS <<vcl_endl;
 vcl_cout<<" diff " << DIFF <<vcl_endl;
 vcl_cout<<" convergedvalues " << CONVERGEDVALUES <<vcl_endl;
 vcl_cout << "converged values stage 1 "<<CONVERGEDVALUES_STAGE1<<vcl_endl;
 vcl_cout << "converged values stage 2 "<<CONVERGEDVALUES_STAGE2<<vcl_endl;
 vcl_cout<<"  cmminfo " << CMMINFO <<vcl_endl;
 vcl_cout<<" logfile " << LOGFILE <<vcl_endl;
 vcl_cout<<" conics_bin_file_base " <<  CONICS_BIN_FILE_BASE <<vcl_endl;
 vcl_cout<<" correspondenceinfo " <<  CORRESPONDENCEINFO <<vcl_endl;
 vcl_cout<<" output_scanfile " <<  OUTPUT_SCANFILE <<vcl_endl;

 vcl_cout<<" ball number " <<  BALL_NUMBER <<vcl_endl;
 vcl_cout<<" ball_radius_big " << BALL_RADIUS_BIG <<vcl_endl;
 vcl_cout<<" ball_density " << BALL_DENSITY  <<vcl_endl;
 vcl_cout<<" height" << HEIGHT <<vcl_endl;
 vcl_cout<<" radius" << RADIUS  <<vcl_endl;
 vcl_cout<<" threshold" << THRESHOLD  <<vcl_endl;
 vcl_cout<<" gaussian sigma" << GAUSSIAN_SIGMA  <<vcl_endl;
 vcl_cout<<" min fit length" << MIN_FIT_LENGTH <<vcl_endl;
 vcl_cout<<" rms distance" << RMS_DISTANCE  <<vcl_endl;
 vcl_cout<<" aspect ratio" << ASPECT_RATIO  <<vcl_endl;
 vcl_cout<<" width height diff threshold " << WIDTH_HEIGHT_DIFF_THRESHOLD <<vcl_endl;
 vcl_cout<<" width threshold" << WIDTH_THRESHOLD  <<vcl_endl;
 vcl_cout<<" x coord diff range " << X_COORD_DIFF_RANGE <<vcl_endl;
 vcl_cout<<" y coord diff range " << Y_COORD_DIFF_RANGE <<vcl_endl;
 vcl_cout<<" size_of_x " << SIZE_OF_X   <<vcl_endl;
 vcl_cout<<" num_of_corresp " << NUM_OF_CORRESP  <<vcl_endl;
 vcl_cout<<" num_of_conics " << NUM_OF_CONICS  <<vcl_endl;
 vcl_cout<<" num_of_parameters " << NUM_OF_PARAMETERS  <<vcl_endl;
 vcl_cout<<" starting at image " <<START <<vcl_endl;
 vcl_cout<<" ending at image " <<END <<vcl_endl;
 vcl_cout<<" interval " << INTERVAL  <<vcl_endl;
 vcl_cout<<" error margin factor " << ERROR_MARGIN_FACTOR <<vcl_endl;

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
    vcl_cout<<" parameter: "<<i<<"  " <<PARAM[i] <<vcl_endl;
}

delete valstr;
delete val_string;
qfstream.close();

vcl_string txt_file = CORRESPONDENCEINFO;

// reading out the information  from CORRESPONDENCEINFO file & storing in COORDS & HEIGHT_RADII
vcl_ifstream fstream(txt_file.c_str(),vcl_ios::in);
vcl_cout << "opening " << txt_file << "\n";
 
  
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

           vcl_cout << x << " " << y << " " << w << " " << h << "\n";

           COORDS.push_back(vcl_pair<double,double> (x,y));
           HEIGHT_RADII.push_back(vcl_pair<double,double> (w,h));
   }  
}
else{
            vcl_cerr << " bad CORRESPONDENCEINFO filename: " << txt_file << "\n";
            vcl_cerr << "required for  reading out the information"
                    << " from  file & storing in COORDS & HEIGHT_RADII\n";

            exit(1);
}

/*
   while (!fstream.eof()) {


           fstream.getline(val_string1, 300);
           vcl_cout << val_string1;
           x = vcl_atof(val_string1);


           fstream.getline(val_string1, 300);

           y = vcl_atof(val_string1);



           COORDS.push_back(vcl_pair<double,double> (x,y));

           fstream.getline(val_string1, 300);
           vcl_cout << val_string1;
           w = vcl_atof(val_string1);


           fstream.getline(val_string1, 300);

           h = vcl_atof(val_string1);

           HEIGHT_RADII.push_back(vcl_pair<double,double> (w,h));
   }
   */
  fstream.close();
  
  vcl_fclose(fp);
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




// dbgl headers
#include <shape_align/dbskr_align_shapes.h>

#include <unistd.h>
#include <vcl_sstream.h>

void print_usage()
{

    vcl_cout<<"./ed_match_exec -m <file model esfs> -q <file query esfs> -s <sampling> -r <R matching> -h <help>"<<vcl_endl;
    vcl_cout<<" -m: List of esf files that represent models/training"<<vcl_endl;
    vcl_cout<<" -q: List of esf files that represent query/test"<<vcl_endl;
    vcl_cout<<" -s: Sampling rate, unit of pixels, space between samples, 5 pixels default"<<vcl_endl;
    vcl_cout<<" -r: R weights the bending and stretching costs"<<vcl_endl;
    vcl_cout<<" -l: Perform lambda scaling (1,0)"<<vcl_endl;
    vcl_cout<<" -a: Area in units of pixesl to scale all shapes to will do if -l equals 1"<<vcl_endl;
    vcl_cout<<" -h: Help,prints out usage"<<vcl_endl;

}
int main( int argc, char *argv[] )
{

    // Check make sure have enough arguments
    if ( argc == 1 )
    {
        print_usage();
    }

    int c;

    vcl_string mfile,qfile;

    vcl_stringstream ds_stream,r_stream,l_stream,a_stream;

    bool elastic_splice_cost    = false; 
    float scurve_sample_ds      = 5.0f; 
    float scurve_interpolate_ds = 1.0f; 
    bool localized_edit         = false;
    double scurve_matching_R    = 6.0f; 
    bool circular_ends          = true; 
    bool combined_edit          = false; 
    bool use_approx             = true;
    bool lambda_scaling         = false;
    double lambda_area          = 10000;

    while ((c = getopt(argc, argv, "m:q:s:r:l:a:h")) != -1)
    {
        switch (c) 
        {
        case 'm':
            mfile = optarg;
            break;
        case 'q':
            qfile = optarg;
            break;
        case 's':
            ds_stream<<optarg;
            ds_stream>>scurve_sample_ds;
            break;
        case 'r':
            r_stream<<optarg;
            r_stream>>scurve_matching_R;
            break;
        case 'l':
            l_stream<<optarg;
            l_stream>>lambda_scaling;
            break;
        case 'a':
            a_stream<<optarg;
            a_stream>>lambda_area;
            break;
        case 'h':
            print_usage();
            exit(0);
            break;
        default:
            print_usage();
            exit(0);
            break;
        }
    }

    vcl_cout<<"Matching esfs from "<<mfile<<" to esfs from "<<qfile<<vcl_endl;
    vcl_cout<<"Matching with a dS of "
            <<scurve_sample_ds<<" and a R of "<<scurve_matching_R<<vcl_endl;
    if ( lambda_scaling )
    {
        vcl_cout<<"Performing lambda scaling at an area of "<<lambda_area
                <<" pixels"<<vcl_endl;
    }

    vcl_cout<<vcl_endl;

    //: Constructor
    dbskr_align_shapes matcher(mfile,
                               qfile,
                               elastic_splice_cost    ,
                               scurve_sample_ds      ,
                               scurve_interpolate_ds ,
                               localized_edit         ,
                               scurve_matching_R    ,
                               circular_ends          ,
                               combined_edit          ,
                               use_approx             ,
                               lambda_scaling         ,
                               lambda_area);


    matcher.match();

    return 0;
}

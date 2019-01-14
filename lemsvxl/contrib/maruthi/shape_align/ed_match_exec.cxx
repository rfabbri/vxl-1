

// dbgl headers
#include <shape_align/dbskr_align_shapes.h>

#include <vul/vul_timer.h>

#include <unistd.h>
#include <sstream>

void print_usage()
{

    std::cout<<"./ed_match_exec -m <file model esfs> -q <file query esfs> -s <sampling> -r <R matching> -h <help>"<<std::endl;
    std::cout<<" -m: List of esf files that represent models/training"<<std::endl;
    std::cout<<" -q: List of esf files that represent query/test"<<std::endl;
    std::cout<<" -s: Sampling rate, unit of pixels, space between samples, 5 pixels default"<<std::endl;
    std::cout<<" -r: R weights the bending and stretching costs"<<std::endl;
    std::cout<<" -l: Perform lambda scaling (1,0)"<<std::endl;
    std::cout<<" -a: Area in units of pixesl to scale all shapes to will do if -l equals 1"<<std::endl;
    std::cout<<" -d: Whether to save out dense correspondence , default is true"<<std::endl;
    std::cout<<" -h: Help,prints out usage"<<std::endl;

}
int main( int argc, char *argv[] )
{

    // Check make sure have enough arguments
    if ( argc == 1 )
    {
        print_usage();
    }

    int c;

    std::string mfile,qfile;

    std::stringstream ds_stream,r_stream,l_stream,a_stream,d_stream;

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
    bool save_dc                = true;

    while ((c = getopt(argc, argv, "m:q:s:r:l:a:d:h")) != -1)
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
        case 'd':
            d_stream<<optarg;
            d_stream>>save_dc;
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

    std::cout<<"Matching esfs from "<<mfile<<" to esfs from "<<qfile<<std::endl;
    std::cout<<"Matching with a dS of "
            <<scurve_sample_ds<<" and a R of "<<scurve_matching_R<<std::endl;
    if ( lambda_scaling )
    {
        std::cout<<"Performing lambda scaling at an area of "<<lambda_area
                <<" pixels"<<std::endl;
    }

    if ( save_dc )
    {
        std::cout<<"Also saving out Dense Correspondence"<<std::endl;
    }
    
    std::cout<<std::endl;

    vul_timer t;

    //: Constructor
    dbskr_align_shapes matcher(mfile,
                               qfile,
                               elastic_splice_cost    ,
                               scurve_sample_ds       ,
                               scurve_interpolate_ds  ,
                               localized_edit         ,
                               scurve_matching_R      ,
                               circular_ends          ,
                               combined_edit          ,
                               use_approx             ,
                               save_dc                ,
                               lambda_scaling         ,
                               lambda_area);


    matcher.match();


    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;

    return 0;
}

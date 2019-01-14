

// dbgl headers
#include <shape_align/dbskr_test_routines.h>

#include <vul/vul_timer.h>
#include <sstream>

#include <vul/vul_arg.h>

int main( int argc, char *argv[] )
{


    vul_arg<char* > query_file("-q",
                               "Query set of ESF files",
                               vul_arg<char *>::is_required);
    vul_arg<char* > dc_file("-e",
                            "Dense Correspondence File",
                            vul_arg<char *>::is_required);
    vul_arg<char* > model_file("-m",
                               "Model set of ESF files",
                               vul_arg<char *>::is_required);
    
    vul_arg<char* > gmm("-g",
                             "Gaussian Mixture Model file",
                             vul_arg<char *>::is_required);
    vul_arg<char* > pca_modes("-v",
                              "EigenVector Matrix",
                              vul_arg<char *>::is_required);
    vul_arg<char* > pca_mean("-d",
                             "PCA Mean file",
                             vul_arg<char *>::is_required);

    vul_arg<int> descr_type("-t","Descriptor Type: Gradient=0, Color=1",0);
    vul_arg<int> color_space("-c","Color Space: RGB=0,OPP=1,NOPP=2,LAB=3",3);
    vul_arg<int> stride("-s","Stride for dense sampling",8);
    vul_arg<double> alpha("-a","Normalization of FVS",0.5);
    vul_arg<bool> write_out("-w","Write out DC mapping to image",false);

    vul_arg_parse(argc, argv);

    dbskr_test_routines::DescriptorType descriptor=(descr_type()==0)?
        dbskr_test_routines::GRADIENT:dbskr_test_routines::COLOR;

    dbskr_test_routines::ColorSpace color;
    if ( color_space()==0 )
    {
        color=dbskr_test_routines::RGB;
    }
    else if ( color_space() == 1 )
    {
        color=dbskr_test_routines::OPP;
    }
    else if ( color_space() == 2 )
    {
        color=dbskr_test_routines::NOPP;
    }
    else
    {
        color=dbskr_test_routines::LAB;
    }

    std::string query_esf_file(query_file());
    std::string query_dc_file(dc_file());
    std::string model_filename(model_file());
    std::string gmm_file(gmm());
    std::string pca_M_file(pca_modes());
    std::string pca_mean_file(pca_mean());

    vul_timer t;

    //: Constructor
    dbskr_test_routines tester(query_esf_file,
                               query_dc_file,
                               model_filename,
                               gmm_file,
                               pca_M_file,
                               pca_mean_file,
                               descriptor,
                               color,
                               stride(),
                               alpha(),
                               write_out());
    
    
    double vox_time = t.real()/1000.0;
    t.mark();
    std::cout<<std::endl;
    std::cout<<"************ Time taken: "<<vox_time<<" sec"<<std::endl;

    return 0;
}

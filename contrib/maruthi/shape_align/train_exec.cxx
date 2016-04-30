

// dbgl headers
#include <shape_align/dbskr_train_routines.h>

#include <vul/vul_timer.h>
#include <vcl_sstream.h>

#include <vul/vul_arg.h>

int main( int argc, char *argv[] )
{


    vul_arg<char* > filename("-m",
                             "Model File to Open",vul_arg<char *>::is_required);
    vul_arg<int> descr_type("-t","Descriptor Type: Gradient=0, Color=1",0);
    vul_arg<int> color_space("-c","Color Space: RGB=0,OPP=1,NOPP=2,LAB=3",3);
    vul_arg<int> keywords("-k","Number of components for GMM",1024);
    vul_arg<int> pca("-d","PCA Dimension Reduction", 128);

    vul_arg_parse(argc, argv);

    dbskr_train_routines::DescriptorType descriptor=(descr_type()==0)?
        dbskr_train_routines::GRADIENT:dbskr_train_routines::COLOR;

    dbskr_train_routines::ColorSpace color;
    if ( color_space()==0 )
    {
        color=dbskr_train_routines::RGB;
    }
    else if ( color_space() == 1 )
    {
        color=dbskr_train_routines::OPP;
    }
    else if ( color_space() == 2 )
    {
        color=dbskr_train_routines::NOPP;
    }
    else
    {
        color=dbskr_train_routines::LAB;
    }

    vcl_string mfile(filename());

    vul_timer t;

    //: Constructor
    dbskr_train_routines trainer(mfile,
                                 descriptor,
                                 color,
                                 keywords(),
                                 pca());


    double vox_time = t.real()/1000.0;
    t.mark();
    vcl_cout<<vcl_endl;
    vcl_cout<<"************ Time taken: "<<vox_time<<" sec"<<vcl_endl;

    return 0;
}

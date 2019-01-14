#include <iostream>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <vil/vil_crop.h>
#include <cstddef>
#include <cali/cali_artf_corresponder.h>
#include <cali/cali_param.h>

int main(int argc, char* argv[]) {

        if(argc < 2 ) {
                std::cerr << "Usage: " << argv[0] << " <parameter file>\n";
                exit(1);
        }
        std::string path = argv[1];
        cali_param par(path);
        std::string fname = par.LOGFILE;
        cali_artf_corresponder corresp(par);
        int start=par.START, end=par.END;

        std::string txt_file =  corresp.gen_write_fname(fname, 0);
        std::string dir = vul_file::dirname(txt_file);
        std::string ext = vul_file::extension(txt_file);
        std::string base = vul_file::basename(txt_file,ext.c_str());

        #if defined(VCL_WIN32)
        txt_file = dir + "\\" +  base + ".log";
        #else
        txt_file = dir + "/" + base + ".log";
        #endif
        std::cout << "txt_file is " << txt_file << "\n";

        std::ofstream fstream(txt_file.c_str());



        for (int i=start; i <=end; i+=par.INTERVAL) {
                std::string outname = corresp.gen_write_fname(fname, i);
                dir = vul_file::dirname(outname);
                ext = vul_file::extension(outname);
                base = vul_file::basename(outname,ext.c_str());

                #if defined(VCL_WIN32)
                fname = dir + "\\" +  base + ".tif";
                #else
                fname = dir + "/" + base + ".tif";
                #endif

                std::cout << "loading " << fname << "\n";
                vil_image_resource_sptr img = vil_load_image_resource(fname.c_str());

                conic_vector_set conics = corresp.fit_conics(img);
                // generate file name with .bin extension

                //std::size_t slash_pos = fname.find_last_of("\\");
                //outname.assign(fname, 0, dot_pos+1);
                //outname.append("bin");
                //outname.insert(slash_pos+1, "bins\\");
                std::cout << "writing " << outname << "\n";
                corresp.save_conics_bin(conics, outname);

                // also save into a txt file
                fstream << "Conic File Number:" << i << "\n";
                corresp.print(fstream, conics);
        }
        return 0;

}

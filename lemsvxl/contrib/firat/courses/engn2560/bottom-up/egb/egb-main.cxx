// Modified by Firat Kalaycilar
/*
Copyright (C) 2006 Pedro Felzenszwalb

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#include <cstdio>
#include <cstdlib>
#include "image.h"
#include "misc.h"
#include "pnmfile.h"
#include "segment-image.h"
#include <dbul/dbul_random.h>
#include <string>
#include <vul/vul_file.h>
#include <vpl/vpl.h>
#include <dbul/dbul_octave.h>
#include <cmath>

int main(int argc, char **argv) {
    if (argc != 12) {
        fprintf(stderr, "usage: %s sigma0 sigma_end sigma_inc k0 k_end k_inc min0 min_end min_inc input output\n", argv[0]);
        return 1;
    }

    std::printf("loading input image.\n");
    char* input_image_file = argv[10];
    std::string extension = vul_file::extension(input_image_file);
    bool isconv = false;
    std::string temp_file;
    if(extension != ".ppm")
    {
        temp_file = dbul_get_random_alphanumeric_string(10) + ".ppm";
        std::string command = "convert ";
        command = command + input_image_file + " " + temp_file;
        std::system(command.c_str());
        isconv = true;
    }
    else
    {
        temp_file = input_image_file;
    }

    image<rgb> *input = loadPPM(temp_file.c_str());

    std::printf("processing\n");
    int num_ccs;
    int paramid = 1;
    int total_digits = 4;
    for(float sigma = atof(argv[1]); sigma <= atof(argv[2]); sigma += atof(argv[3]))
    {
        for(float k = atof(argv[4]); k <= atof(argv[5]); k += atof(argv[6]))
        {
            for(int min_size = atoi(argv[7]); min_size <= atoi(argv[8]); min_size += atoi(argv[9]))
            {
                std::printf("sigma = %f  k = %f  min_size = %d\n", sigma, k, min_size);
                char buffer[32];
                int number_of_digits = total_digits - std::floor(std::log10(paramid));
                std::string zeros = "";
                for(int iii = 0; iii < number_of_digits; iii++)
                {
                    zeros += "0";
                }
                std::sprintf(buffer, "%d", paramid);
                image<rgb> *seg = segment_image(input, sigma, k, min_size, &num_ccs);
                std::string temp_file2 = std::string(argv[11]) + std::string("_") + zeros + std::string(buffer) + ".ppm";
                savePPM(seg, temp_file2.c_str());

                dbul_octave_argument_list inargs, outargs;
                inargs(0) = temp_file2;

                inargs(1) = std::string(argv[11]) + std::string("_") + zeros + std::string(buffer) + ".txt";
                dbul_octave.run("/home/firat/lemsvxl/src/contrib/firat/courses/engn2560/bottom-up/egb","convert2labelmap", inargs, outargs);

                std::printf("got %d components\n", num_ccs);
                std::printf("done! uff...thats hard work.\n");
                //vpl_unlink(temp_file2.c_str());
                paramid++;
            }
        }
    }

    if(isconv)
    {
        vpl_unlink(temp_file.c_str());
    }

    return 0;
}


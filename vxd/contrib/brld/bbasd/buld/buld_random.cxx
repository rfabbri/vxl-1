// This is /lemsvxl/bbasd/buld/buld_random.cxx

//:
// \file
// \author Based on original code by  Firat Kalaycilar (@lems.brown.edu)
// \date   Mar 8, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#include "buld_random.h"

#include<cstdlib>
#include<ctime>

void buld_random_init()
{
    static bool is_init = false;
    if(!is_init)
    {
        std::srand((unsigned)std::time(NULL));
        is_init = true;
    }
}

std::string buld_get_random_alphanumeric_string(int len)
{
    buld_random_init();
    std::string ret;
    for (int i = 0; i < len; ++i) {
        char randomChar = rand()%(26+26+10);
        if (randomChar < 26)
            ret += std::string(1, 'a' + randomChar);
        else if (randomChar < 26+26)
            ret += std::string(1,'A' + randomChar - 26);
        else
            ret += std::string(1,'0' + randomChar - 26 - 26);
    }
    return ret;
}

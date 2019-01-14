//:
// \file
#include <testlib/testlib_test.h>

#include "../ncn_mutual_information.h"

#include <vnl/vnl_math.h>

#include <iostream>
#include <set>
#include <vector>



static void test_mutual_information_ranking()
{

    std::vector< ncn_mutual_information > mi;
    mi.push_back(ncn_mutual_information(.2,1));
    mi.push_back(ncn_mutual_information(.1,2));
    mi.push_back(ncn_mutual_information(.7,3));
    mi.push_back(ncn_mutual_information(10.3,4));
    mi.push_back(ncn_mutual_information(2,5));

    //insert in a set
    std::set<ncn_mutual_information> sm(mi.begin(),mi.end());

    unsigned i = 0;
    for(std::set<ncn_mutual_information>::iterator sit = sm.begin(); sit!=sm.end();++sit,++i)
        std::cout << "rank[" << i << "] = " << sit->mi() << "\t index[" << i << "] = " << sit->index() << std::endl;
}

TESTMAIN(test_mutual_information_ranking);


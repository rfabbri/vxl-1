//:
// \file
#include <testlib/testlib_test.h>

#include "../ncn_mutual_information.h"

#include <vnl/vnl_math.h>

#include <vcl_iostream.h>
#include <vcl_set.h>
#include <vcl_vector.h>



static void test_mutual_information_ranking()
{

    vcl_vector< ncn_mutual_information > mi;
    mi.push_back(ncn_mutual_information(.2,1));
    mi.push_back(ncn_mutual_information(.1,2));
    mi.push_back(ncn_mutual_information(.7,3));
    mi.push_back(ncn_mutual_information(10.3,4));
    mi.push_back(ncn_mutual_information(2,5));

    //insert in a set
    vcl_set<ncn_mutual_information> sm(mi.begin(),mi.end());

    unsigned i = 0;
    for(vcl_set<ncn_mutual_information>::iterator sit = sm.begin(); sit!=sm.end();++sit,++i)
        vcl_cout << "rank[" << i << "] = " << sit->mi() << "\t index[" << i << "] = " << sit->index() << vcl_endl;
}

TESTMAIN(test_mutual_information_ranking);


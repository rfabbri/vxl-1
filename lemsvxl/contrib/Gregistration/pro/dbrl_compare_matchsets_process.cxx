
#include "dbrl_compare_matchsets_process.h"
#include<bpro1/bpro1_parameters.h>
#include<georegister/dbrl_feature_sptr.h>
#include<georegister/dbrl_feature_point.h>
#include "dbrl_match_set_storage.h"
#include "dbrl_match_set_storage_sptr.h"
#include<georegister/dbrl_id_point_2d.h>
#include<georegister/dbrl_match_set_sptr.h>

//: Constructor
dbrl_compare_matchsets_process::dbrl_compare_matchsets_process(void): bpro1_process()
    {
    }

//: Destructor
dbrl_compare_matchsets_process::~dbrl_compare_matchsets_process()
    {
    }


//: Return the name of this process
vcl_string
dbrl_compare_matchsets_process::name()
    {
        return "Compare Match Sets";
    }


//: Return the number of input frame for this process
int
dbrl_compare_matchsets_process::input_frames()
    {
    return 1;
    }


//: Return the number of output frames for this process
int
dbrl_compare_matchsets_process::output_frames()
    {
    return 0;
    }


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_compare_matchsets_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbrl_match_set" );
    to_return.push_back( "dbrl_match_set" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_compare_matchsets_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    return to_return;
    }


//: Execute the process
bool
dbrl_compare_matchsets_process::execute()
    {
    if ( input_data_[0].size() != 2 )
        {
        vcl_cout << "In dbrl_compare_matchsets_process::execute() - "<< "not exactly two match sets \n";
        return false;
        }
    clear_output();

    dbrl_match_set_storage_sptr match_set1, match_set2;

    match_set1.vertical_cast(input_data_[0][1]);
    match_set2.vertical_cast(input_data_[0][0]);

    dbrl_match_set_sptr ms1= match_set1->matchset();
    dbrl_match_set_sptr ms2= match_set2->matchset();

    //: compare the number of features in each set
    //: if they have different number of features return false
    if(ms1->size_set1()!=ms2->size_set1() && ms1->size_set2()!=ms2->size_set2())
        {
        vcl_cout<<"\nMatch Sets are different";
        return false;
        }
    //: comparing the matching sets
    double cnt=0;
    for(int i=0;i<ms1->size_set1();i++)
        {
        if(ms1->correspondence1to2(i)==  ms2->correspondence1to2(i))
            cnt++;
        }

    vcl_cout<<"\nNo of correct matches are "<<cnt/(double)ms1->size_set1();

    return true;  
    }
//: Clone the process
bpro1_process*
dbrl_compare_matchsets_process::clone() const
    {
    return new dbrl_compare_matchsets_process(*this);
    }

bool
dbrl_compare_matchsets_process::finish()
    {
    return true;
    }





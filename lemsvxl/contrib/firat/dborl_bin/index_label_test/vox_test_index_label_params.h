// This is /lemsvxl/contrib/firat/dborl_bin/index_label_test/vox_test_index_label_params.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 5, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef VOX_TEST_INDEX_LABEL_PARAMS_H_
#define VOX_TEST_INDEX_LABEL_PARAMS_H_

#include <dborl/algo/dborl_algo_params.h>

//: put all the additional params into this class, and add them
//  to the parameter list in the constructor so that
//  all the files related to the parameter set of the algorithm
//  are generated automatically via the methods of the base class
//  no need to overwrite any of the methods in the base class thanks
//  to the parameter list
class vox_test_index_label_params : public dborl_algo_params
{
public:

    //: Constructor
    vox_test_index_label_params(vcl_string algo_name);

    // MEMBER VARIABLES

    //: Name of input object
    dborl_parameter<vcl_string> input_object_name_;
    //parameter for the index file
    //Is a flat image database
    dborl_parameter<vcl_string> index_filename_;

    dborl_parameter<vcl_string> assoc_label_;

};

#endif /* VOX_TEST_INDEX_LABEL_PARAMS_H_ */

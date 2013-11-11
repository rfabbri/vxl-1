// This is /lemsvxl/brcv/seg/dbdet/pro/dbdet_kovesi_edge_linker_process.h

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date   Mar 2, 2010
// \brief 
//
// \verbatim
//
// \endverbatim
//

#ifndef DBDET_KOVESI_EDGE_LINKER_PROCESS_H_
#define DBDET_KOVESI_EDGE_LINKER_PROCESS_H_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

class dbdet_kovesi_edge_linker_process : public bpro1_process
{
public:

    dbdet_kovesi_edge_linker_process();
    virtual ~dbdet_kovesi_edge_linker_process();

    //: Clone the process
    virtual bpro1_process* clone() const;

    vcl_string name();

    int input_frames();
    int output_frames();

    vcl_vector< vcl_string > get_input_type();
    vcl_vector< vcl_string > get_output_type();

    bool execute();
    bool finish();
};

#endif /* DBDET_KOVESI_EDGE_LINKER_PROCESS_H_ */

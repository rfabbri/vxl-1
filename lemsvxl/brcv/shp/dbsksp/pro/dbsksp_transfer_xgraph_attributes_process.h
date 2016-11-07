// This is /lemsvxl/brcv/shp/dbsksp/pro/dbsksp_transfer_xgraph_attributes_process.h.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Sep 13, 2010
//

#ifndef DBSKSP_TRANSFER_XGRAPH_ATTRIBUTES_PROCESS_H_
#define DBSKSP_TRANSFER_XGRAPH_ATTRIBUTES_PROCESS_H_

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline
class dbsksp_transfer_xgraph_attributes_process : public bpro1_process
{

    public:
        //: Constructor
        dbsksp_transfer_xgraph_attributes_process();

        //: Destructor
        virtual ~dbsksp_transfer_xgraph_attributes_process();

        //: Clone the process
        virtual bpro1_process* clone() const;

        //: Returns the name of this process
        vcl_string name();

        vcl_vector< vcl_string > get_input_type();
        vcl_vector< vcl_string > get_output_type();

        int input_frames();
        int output_frames();

        bool execute();
        bool finish();

};

#endif /* DBSKSP_TRANSFER_XGRAPH_ATTRIBUTES_PROCESS_H_ */

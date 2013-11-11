// This is brcv/shp/dbskfg/algo/dbskfg_detect_fragments.h
#ifndef dbskfg_detect_fragments_h_
#define dbskfg_detect_fragments_h_
//:
// \file
// \brief Algorithm to detect fragments from a match matrix xml file
// \author Maruthi Narayanan
// \date 10/18/10
// 

// \verbatim
//  Modifications
//   Maruthi Narayanan 10/18/2010    Initial version.
//
// \endverbatim 

#include <bxml/bxml_read.h>
#include <vsol/vsol_box_2d.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_utility.h>


//: Form Composite Graph algorithm
class dbskfg_detect_fragments
{

public:

    //: Constructor
    dbskfg_detect_fragments
        ( vcl_string input_match_file);

    //: Destructor
    ~dbskfg_detect_fragments();
  
    //: Detect the fragments
    void detect(double threshold,
                double N_query_frags,
                double K_model_frags,
                vsol_box_2d_sptr& bounding_box);

private:

    //: Load match file
    void load_match_file(vcl_string input_sim_matrix);

    //: Load bounding boxes
    void load_bbox(const bxml_data_sptr& root_xml);

    //: Load matrix
    void load_matrix(const bxml_data_sptr& root_xml);

    //: Keep a map of each row
    vcl_map<unsigned int,vcl_vector<vcl_pair<double,unsigned int> > > 
        sim_matrix_;

    //: Keep all bounding boxes in another file
    vcl_map<unsigned int,vsol_box_2d> bbox_;

    // Make copy ctor private
    dbskfg_detect_fragments(const dbskfg_detect_fragments&);

    // Make assign operator private
    dbskfg_detect_fragments& operator
        =(const dbskfg_detect_fragments& );
   
};

#endif //dbsk2d_ishock_prune_h_

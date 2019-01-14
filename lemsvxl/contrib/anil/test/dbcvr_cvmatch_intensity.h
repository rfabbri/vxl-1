#ifndef dbcvr_cvmatch_intensity_
#define dbcvr_cvmatch_intensity_

//---------------------------------------------------------------------
// This is contrib/anil/mw/mw_cvmatch_appearance.h
//:
// \file
// \brief open curve matching with intensity value cost
//        this is only for testing purposes
//        the two member vectors are supposed to simulate pixel values
//        read off of curve samples
// \author
//  Anil Usumezbas 
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <dbcvr/dbcvr_cvmatch.h>

class dbcvr_cvmatch_intensity : public dbcvr_cvmatch
{
public:

    dbcvr_cvmatch_intensity() {}
    dbcvr_cvmatch_intensity(std::vector<double> pixel1, std::vector<double> pixel2);

    double computeIntervalCost(int i, int ip, int j, int jp);
    void setPixelVector1(std::vector<double> pixel1) { pixel_vector1_=pixel1; }
    void setPixelVector2(std::vector<double> pixel2) { pixel_vector2_=pixel2; }
    void initializeDPCosts();
    void computeDPCosts();
    void findDPCorrespondence();
    void Match();


protected:

    std::vector<double> pixel_vector1_;
    std::vector<double> pixel_vector2_;

};

#endif

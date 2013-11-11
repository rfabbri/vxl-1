/**************************************************************
 *    Name: Daniel Spinosa
 *    File: bfrag2D_assembly.h
 *    Asgn: 
 *    Date: 
 *
 *      A bfrag2D_assembly is one assembly of [0-N] bfrag2D's.
 *    The assembly holds all information about the relative locations
 *    and neighborhood connectivity among the bfrags within the assembly.
 *
 ***************************************************************/

#ifndef __BFRAG2D_ASSEMBLY_H__
#define __BFRAG2D_ASSEMBLY_H__

#include "bfrag2D.h"

class bfrag2D_assembly {
public:
    
    bfrag2D_assembly(){};
    ~bfrag2D_assembly(){};

    void addPiece( bfrag2D* newFrag, double* matrix ){ theFrags.push_back( newFrag );
                                                       theMatrices.push_back( matrix ); 
                                                      };

    vcl_vector< bfrag2D* > getFrags(){ return theFrags; };
    vcl_vector< double* > getMatrices(){ return theMatrices; };


private:

    vcl_vector< bfrag2D* > theFrags;
    vcl_vector< double* > theMatrices;


};


#endif

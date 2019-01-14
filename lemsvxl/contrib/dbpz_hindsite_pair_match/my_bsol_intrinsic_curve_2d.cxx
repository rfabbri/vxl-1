#include "my_bsol_intrinsic_curve_2d.h"


void my_bsol_intrinsic_curve_2d::changeStart( int newStart ){

    int size = storage_->size();

    std::cout << "CHANGE START TO: " << newStart << std::endl;
    std::cout << "size b4: " << storage_->size() << std::endl << "first point before: (" << (*storage_)[1]->x() << "," << (*storage_)[1]->y() << ")" <<std::endl;

    std::vector<vsol_point_2d_sptr> tempPoints( *storage_ );

    storage_->clear();
    delete storage_;
    storage_ = new std::vector<vsol_point_2d_sptr>();
    
    for( int i = newStart+1; i != newStart; i++){
        if( i == size ) i = 0;
        //if( i == size ) break;
        storage_->push_back( tempPoints[i] );        
    }
    storage_->push_back( tempPoints[newStart] );

    std::cout << "size after: " << storage_->size() << std::endl  << "first point after: (" << (*storage_)[1]->x() << "," << (*storage_)[1]->y() << ")" <<std::endl;

}

void my_bsol_intrinsic_curve_2d::reverse(){
    
    int size = storage_->size();
    std::vector< vsol_point_2d_sptr > temppoints;

    for( int i = size-1; i >= 0; i-- ){
        temppoints.push_back( (*storage_)[i] );
    }
    storage_->clear();
    for( int i = 0; i < size; i++ ){
        storage_->push_back( temppoints[i] );
    }

}

void my_bsol_intrinsic_curve_2d::coarseResample( int skipRate ){
    std::vector< vsol_point_2d_sptr > temppoints;

    for( int i = 0; i < storage_->size(); i += skipRate ){
        temppoints.push_back( (*storage_)[i] );
    }
    storage_->clear();

    storage_ = new std::vector< vsol_point_2d_sptr >( temppoints );
    
}

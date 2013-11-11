#include "my_bsol_intrinsic_curve_2d.h"


void my_bsol_intrinsic_curve_2d::changeStart( int newStart ){

    int size = storage_->size();

    vcl_cout << "CHANGE START TO: " << newStart << vcl_endl;
    vcl_cout << "size b4: " << storage_->size() << vcl_endl << "first point before: (" << (*storage_)[1]->x() << "," << (*storage_)[1]->y() << ")" <<vcl_endl;

    vcl_vector<vsol_point_2d_sptr> tempPoints( *storage_ );

    storage_->clear();
    delete storage_;
    storage_ = new vcl_vector<vsol_point_2d_sptr>();
    
    for( int i = newStart+1; i != newStart; i++){
        if( i == size ) i = 0;
        //if( i == size ) break;
        storage_->push_back( tempPoints[i] );        
    }
    storage_->push_back( tempPoints[newStart] );

    vcl_cout << "size after: " << storage_->size() << vcl_endl  << "first point after: (" << (*storage_)[1]->x() << "," << (*storage_)[1]->y() << ")" <<vcl_endl;

}

void my_bsol_intrinsic_curve_2d::reverse(){
    
    int size = storage_->size();
    vcl_vector< vsol_point_2d_sptr > temppoints;

    for( int i = size-1; i >= 0; i-- ){
        temppoints.push_back( (*storage_)[i] );
    }
    storage_->clear();
    for( int i = 0; i < size; i++ ){
        storage_->push_back( temppoints[i] );
    }

}

void my_bsol_intrinsic_curve_2d::coarseResample( int skipRate ){
    vcl_vector< vsol_point_2d_sptr > temppoints;

    for( int i = 0; i < storage_->size(); i += skipRate ){
        temppoints.push_back( (*storage_)[i] );
    }
    storage_->clear();

    storage_ = new vcl_vector< vsol_point_2d_sptr >( temppoints );
    
}

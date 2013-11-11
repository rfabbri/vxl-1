/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_soview2D_bfrag2D_assembly.cpp
 *    Asgn: 
 *    Date: 
 *
 ***************************************************************************/

 #include "vgui_soview2D_bfrag2D_assembly.h"


vgui_soview2D_bfrag2D_assembly::vgui_soview2D_bfrag2D_assembly( vgui_soview2D_bfrag2D* in, 
                                                                double * inMatrix ){

    vcl_cout << "soview_assmebly -> init w/ piece w/ posisiton information" << vcl_endl;

    theViews.push_back( in );
    //XXX also need to push back in parallel an identity matrix

}


void vgui_soview2D_bfrag2D_assembly::addPositionedPiece( vgui_soview2D_bfrag2D* in,
                                                         double *inMatrix ){

    vcl_cout << "soview_assmebly -> addiding piece w/ posisiton information" << vcl_endl;

    theViews.push_back( in );
    //XXX also need to push back in parallel an identity matrix

}

void vgui_soview2D_bfrag2D_assembly::draw_select() const {
    //draw in a unique way such that we draw the entire are within the top curve
    //an nothing outside of it...

    for( unsigned i = 0; i < theViews.size(); i++ ){

        //XXX DONT FORGET TO DO SAME TRANSFORMATIONS AS WE DO IN DRAW
        //push transform using theMatrices[i];
        glPushMatrix();

        //i can't freakin get ROTATE to work
        //glRotated( 90, 0, 0, -1 );

        //THE FOLLOWING TWO CALLS WORK FINE
        glTranslatef( i*200.0, 0.0, 0.0 );
        //glScalef( 2.0, 3.0, 2.0 );

        //theViews[i]->draw_select();
        
        //pop transform
        glPopMatrix();

    }

}

void vgui_soview2D_bfrag2D_assembly::draw() const {

    //it seems since an iterator needs the .begin() and .end() of a vector, methods that aren't 
    //const, i cannot use an iterator in draw() and as such i am using the array style
    //indexing into the vector

    for( unsigned i = 0; i < theViews.size(); i++ ){

        //push transform using theMatrices[i];
        glPushMatrix();

        //i can't freakin get ROTATE to work
        //glRotated( 90, 0, 0, -1 );

        //THE FOLLOWING TWO CALLS WORK FINE
        glTranslatef( i*200.0, 0.0, 0.0 );
        //glScalef( 2.0, 3.0, 2.0 );

        //theViews[i]->draw();
        
        //pop transform
        glPopMatrix();
  
    }
        

}



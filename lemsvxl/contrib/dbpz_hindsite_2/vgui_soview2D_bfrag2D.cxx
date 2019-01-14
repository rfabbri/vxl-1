/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_soview2D_bfrag2D.cpp
 *    Asgn: 
 *    Date: 
 *
 ***************************************************************************/

 #include "vgui_soview2D_bfrag2D.h"

vgui_soview2D_bfrag2D::vgui_soview2D_bfrag2D(bgui_vsol_soview2D_new_polygon * tc, 
                                             bgui_vsol_soview2D_new_polygon * bc,
                                             vgui_soview2D_image * ti,
                                             vgui_soview2D_image * bi,
                                             bfrag2D *f) {


    theFrag = f;
    topCurve = tc;
    botCurve = bc;
    topImage = ti;
    botImage = bi;
    
    if(topCurve){
        topCurve->set_selectable(true);
        drawTopCurve = true;
    }
    else drawTopCurve = false;
    if(topImage){
        topImage->set_selectable(false);
        drawTopImage = true;
    }
    else drawTopImage = false;
    if(botCurve){
        botCurve->set_selectable(false);
        drawBotCurve = true;
    }
    else drawBotCurve = false;
    if(botImage){
        botImage->set_selectable(false);
        drawBotImage = true;
    }
    else drawBotImage = false;
}

void vgui_soview2D_bfrag2D::draw_select() const{

    
    //just using top curve for selection!
    //bgui_vsol_soview2D_new_polygon *topCurve
    //gunna draw that on my own here...
    if (topCurve) topCurve->draw_select();
   
}

void vgui_soview2D_bfrag2D::draw() const{
    
    if(botImage && drawBotImage) botImage->draw();    
    if(botCurve && drawBotCurve) botCurve->draw();
    if(topImage && drawTopImage) topImage->draw();
    if(topCurve && drawTopCurve) topCurve->draw();
    
}

void vgui_soview2D_bfrag2D::popupInfo(){

    /*
    std::cout << "------------BFRAG [" << theFrag->getIndex() << "] " << theFrag->getName() << " INFO----------------------" << std::endl;
    std::cout << "             topCurve: " << theFrag->getTopCurveFn() << std::endl;
    std::cout << "             topImage: " << theFrag->getTopImageFn() << std::endl;
    std::cout << "             botCurve: " << theFrag->getBotCurveFn() << std::endl;
    std::cout << "             botImage: " << theFrag->getBotImageFn() << std::endl;
    */

    theFrag->popupInfo(drawTopImage, drawBotCurve, drawBotImage);


}

float vgui_soview2D_bfrag2D::distance_squared(float x, float y) const{
    
    //TODO : implement this
    //std::cout << "SOVIEW BFRAG2D: [distance_squared] not yet implemented!" << std::endl;

    return 0.0;

}


void vgui_soview2D_bfrag2D::get_centroid(float* x, float* y) const{

    //TODO : implement this
    std::cout << "SOVIEW BFRAG2D: [get_centroid] not yet implemented!" << std::endl;

}


void vgui_soview2D_bfrag2D::translate(float x, float y){

    //TODO : implement this
    std::cout << "SOVIEW BFRAG2D: [translate] not yet implemented!" << std::endl;

}

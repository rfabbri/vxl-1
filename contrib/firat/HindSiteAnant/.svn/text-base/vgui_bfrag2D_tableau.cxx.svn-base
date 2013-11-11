/**************************************************************************
 *    Name: Daniel Spinosa
 *    File: vgui_bfrag2D_tableau.cpp
 *    Asgn: 
 *    Date: 
 *
 ***************************************************************************/

#include "vgui_bfrag2D_tableau.h"
#include "bfrag_Manager.h"
//rotation
#include <vxl_config.h> // for vxl_byte
#include <vil/vil_rotate.txx>
#include <vil/vil_resample_bilin.txx>
//rotation

vgui_bfrag2D_tableau::vgui_bfrag2D_tableau(const char* n) : bgui_vsol2D_tableau(n) { 
    generalInit();
}

vgui_bfrag2D_tableau::vgui_bfrag2D_tableau( vgui_image_tableau_sptr const& it,
                                            const char* n) : bgui_vsol2D_tableau(it, n) { 
    generalInit();
}
 
vgui_bfrag2D_tableau::vgui_bfrag2D_tableau(vgui_tableau_sptr const& t,
                                           const char* n) : bgui_vsol2D_tableau(t, n) { 
    generalInit();
}

vgui_bfrag2D_tableau::~vgui_bfrag2D_tableau(){}

void vgui_bfrag2D_tableau::generalInit(){
    modal = false;
    myIndex = -1; numPairs = 0;
};

void vgui_bfrag2D_tableau::add_popup(vgui_menu& menu)
{
  if( myIndex != -1 && numPairs > 1)
  {
    //i think the junk in the popup menu is DEBUG stuff that will be removed 
    //whenth is complied in releae mode...
    menu.clear();
    //modify the right click popup menu
    vgui_menu popup;
    char str[8];
    for( int i = 0; i < numPairs; i++ )
    {
      if( i != myIndex )
      {
        sprintf( str, "%i", i+1 );
        popup.add( str, new vgui_command_bound_method<vgui_bfrag2D_tableau,int>(this,&vgui_bfrag2D_tableau::reorderPair,i) );
      } 
      else 
        popup.separator();
    }
    menu.add( "Move To Position", popup );
    //deletion choices
    vgui_menu deleteMenu;
    deleteMenu.add( "This One", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::deleteMePair) );
    deleteMenu.add( "All Selected", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::deleteSelectedPairs) );
    menu.add( "Delete...", deleteMenu );
  } 
  else if( myIndex != -1 && numPairs == -9)
  {
    menu.clear();
    //deletion choices
    vgui_menu deleteMenu;
    deleteMenu.add( "This One", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::deleteMeFrag) );
    deleteMenu.add( "All Selected", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::deleteSelectedFrags) );
    menu.add( "Delete...", deleteMenu );
  } 
  else if( myIndex != -1 && numPuzzles > 0 )
  {
    menu.clear();
    vgui_menu popup;
    char str[8];
    for( int i = 0; i < numPuzzles; i++ )
    {
      if( i != myIndex )
      {
        sprintf( str, "%i", i+1 );
        popup.add( str, new vgui_command_bound_method<vgui_bfrag2D_tableau,int>(this,&vgui_bfrag2D_tableau::reorderPuzzle,i) );
      } 
      else 
        popup.separator();
    }

    vgui_menu show_cost;
    show_cost.add("This One", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::showPuzzleCost));
    show_cost.add("All", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::showPuzzleCosts));

    vgui_menu update_cost;
    update_cost.add("This One", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::updatePuzzleCost));
    update_cost.add("All", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::updatePuzzleCosts));

    menu.add("Show Cost(s)...", show_cost);
    menu.add("Move to Position", popup);
    menu.add("Delete", new vgui_command_simple<vgui_bfrag2D_tableau>(this,&vgui_bfrag2D_tableau::deleteMePuzzle));
    menu.add("Update Cost(s)...", update_cost);
  }
}

//PUZZLE
void vgui_bfrag2D_tableau::reorderPuzzle( int toWhere )
{
    bfrag_Manager::instance()->reorderPuzzle( myIndex, toWhere );
}

void vgui_bfrag2D_tableau::deleteSelectedPuzzles(){
    bfrag_Manager::instance()->removeSelectedPuzzles();
    
}

void vgui_bfrag2D_tableau::deleteMePuzzle()
{
    bfrag_Manager::instance()->removePuzzle( myIndex );
}

void vgui_bfrag2D_tableau::showPuzzleCost()
{
    bfrag_Manager::instance()->showPuzzleCost(myIndex);
}

void vgui_bfrag2D_tableau::showPuzzleCosts()
{
    bfrag_Manager::instance()->showPuzzleCosts();
}

void vgui_bfrag2D_tableau::updatePuzzleCost()
{
    bfrag_Manager::instance()->updatePuzzleCost(myIndex);
}

void vgui_bfrag2D_tableau::updatePuzzleCosts()
{
    bfrag_Manager::instance()->updatePuzzleCosts();
}

//FRAG
void vgui_bfrag2D_tableau::deleteSelectedFrags(){
    bfrag_Manager::instance()->removeSelectedFragments();
    
}

void vgui_bfrag2D_tableau::deleteMeFrag(){
    bfrag_Manager::instance()->removeFragment( myIndex );
}

//PAIR
void vgui_bfrag2D_tableau::deleteSelectedPairs(){
    bfrag_Manager::instance()->removeSelectedPairs();
}

void vgui_bfrag2D_tableau::deleteMePair(){
    bfrag_Manager::instance()->removePair( myIndex );
}

void vgui_bfrag2D_tableau::reorderPair( int toWhere ){
    bfrag_Manager::instance()->reorderPair( myIndex, toWhere );
}


//override handle function
bool vgui_bfrag2D_tableau::handle(vgui_event const& e) {
    if (image_slot && (e.type != vgui_DRAW || gl_mode != GL_SELECT))
        image_slot.handle(e);

    if( e.type == vgui_MOUSE_UP && e.button == vgui_LEFT && e.modifier == vgui_MODIFIER_NULL ){
        //vcl_cout << "___________________________________________________" << vcl_endl;
        //vcl_cout << "highlighted is: " << highlighted << " @ (" << e.wx << "," << e.wy << ")" << vcl_endl;
        
        /*
        vgui_soview2D_bfrag2D *sel =  (vgui_soview2D_bfrag2D*)(get_highlighted_soview());
        if(sel && !modal){
            //this should keep control from entering this for loop while the
            //info popup box is up
            modal = true;

            //if they did click on a bfrag soview (hence it is currently highlighted)...
            sel->popupInfo();
            post_redraw();

            modal = false;
        } 
        */
              
    }


    return vgui_displaylist2D_tableau::handle(e);
}


// over rode this so i can force alpha blending ON
//: Add an image at the given position to the display.
vgui_soview2D_image* vgui_bfrag2D_tableau::add_image( float x, float y,
vil_image_view_base const& img )
{
    //the TRUE on this line sets alpha blending on
    vgui_soview2D_image *obj = new vgui_soview2D_image( x, y, img, true);
    add(obj);
    return obj;
}

vcl_string vgui_bfrag2D_tableau::type_name() const {
   return "vgui_bfrag2D_tableau";
}

void vgui_bfrag2D_tableau::addCurve( bfrag_curve *theCurve, int offset_x, int offset_y, const vgui_style_sptr& style)
{

  /****************************************************************
  * heres how im gunna do it
  * just get the frag from the curve
  * then add the frag like we do below, but now
  * integrate the xform.  thats where im at, getting this xform, 
  * then time to make sense of it...
  *****************************************************************/

  //bfrag2D* curvesFrag = theCurve->getFrag();
  //assert( curvesFrag );
  //bool a;
  //curvesFrag->popupInfo(a,a,a);

  vcl_vector< vsol_point_2d_sptr > points;
//  PuzPoint<double> curPoint;
  vsol_point_2d curPoint;
  for (unsigned i=0; i < theCurve->num_fine_points(); i++) 
  {
    curPoint = theCurve->point(i);
    //add_vertex(x,y);
    vsol_point_2d_sptr newPt = new vsol_point_2d (curPoint.x()+offset_x, curPoint.y()+offset_y);
    points.push_back(newPt);
  }


  vsol_polygon_2d_sptr newPolygon = new vsol_polygon_2d(points);
  //this tableau inherted the add polygon method, use it...
  add_new_vsol_polygon_2d( newPolygon, style );

  post_redraw();
}

// added this so i can add MY polygon w/ correct draw_select stuff
bgui_vsol_soview2D_new_polygon*
vgui_bfrag2D_tableau::add_new_vsol_polygon_2d(vsol_polygon_2d_sptr const& pline,
                                                const vgui_style_sptr& style)
{
  bgui_vsol_soview2D_new_polygon* obj =
      new bgui_vsol_soview2D_new_polygon(pline);
  add(obj);
  if (style)
    obj->set_style( style );
  else
    obj->set_style( polyline_style_ );
  return obj;
}


vgui_soview2D_bfrag2D *vgui_bfrag2D_tableau::addbfrag2D( bfrag2D *theFrag, XForm3x3 *theXform ){

    assert( theFrag );

    bool hasX = false;
    if ( !(*theXform == XForm3x3()) ){
//        vcl_cout << "HAS X FORM" << vcl_endl;
//        vcl_cout << "rot: " << theXform->rotAngle << " transx: " << theXform->transx << " transy: " << theXform->transy << vcl_endl;

        hasX = true;
    } else {
//        vcl_cout << "no x FORM" << vcl_endl;
    }
    


    //vcl_cout << "BFRAG2D_TABLEAU Adding bfrag index [" << theFrag->getIndex() << "]" << vcl_endl;

    /*******************************************************************************************
     * i suppose it's pretty hacky to use ADD to get the soviews and then remove them,
     * but i want to control how/when the soviews draw themselves using my vgui_soview2D_bfrag
     *******************************************************************************************/

    bgui_vsol_soview2D_new_polygon *topCurve = 0, *botCurve = 0;
    vgui_soview2D_image *topImage = 0, *botImage = 0;
    if( !hasX && theFrag->getTopPolygon() ){
         topCurve = add_new_vsol_polygon_2d( theFrag->getTopPolygon() );
         remove( topCurve );
         //vcl_cout << "ADDED TOP POLYGON" << vcl_endl;
    }
    if( !hasX && theFrag->getBotPolygon() ){
        botCurve = add_new_vsol_polygon_2d( theFrag->getBotPolygon() );
        remove( botCurve );
        //vcl_cout << "ADDED BOT POLYGON" << vcl_endl;
    }
    if( theFrag->getTopRGBImage() ){
        //DO ROTATION
        vil_image_view<vxl_byte> original = theFrag->getTopRGBImage();
        if( hasX ){

            vil_image_view<vxl_byte> rotated;
            vil_rotate_image( original, rotated, theXform->rotAngle );
            topImage = add_image( 0, 0, rotated );

        } else {
            topImage = add_image( theXform->transx, theXform->transy, original );
        }

        remove( topImage );
        //vcl_cout << "ADDED TOP IMAGE" << vcl_endl;
    }
    if( theFrag->getBotRGBImage() ){
        //DO ROTATION
        vil_image_view<vxl_byte> original = theFrag->getBotRGBImage();
        if( hasX ){

            vil_image_view<vxl_byte> rotated;
            vil_rotate_image( original, rotated, theXform->rotAngle );
            botImage = add_image( theXform->transx, theXform->transy, rotated );

        } else {
            botImage = add_image( 0, 0, original );
        }

        remove( botImage );
        //vcl_cout << "ADDED BOT IMAGE" << vcl_endl;
    }

    vgui_soview2D_bfrag2D *theView = new vgui_soview2D_bfrag2D( topCurve, botCurve,
                                                                topImage, botImage,
                                                                theFrag );
    
    add( theView );
    post_redraw();

    return theView;

}

vgui_soview2D_bfrag2D_assembly *vgui_bfrag2D_tableau::addbfrag2D_assembly( bfrag2D_assembly *theAssembly, vcl_vector<vgui_soview2D_bfrag2D*> *views ){


    vgui_soview2D_bfrag2D_assembly *theAssemblyView = new vgui_soview2D_bfrag2D_assembly();
    vgui_soview2D_bfrag2D *singleView;

    //for each bfrag2D and its corresponding matrix in theAssembly we must
    // addbfrag2D, remove( what we just added ), put the soview and matrix into the 
    // new soview_assembly
    //finally we add the assemblyview and return it

    vcl_vector< bfrag2D* > assemblyFrags = theAssembly->getFrags();
    vcl_vector< double* > assemblyMatrices = theAssembly->getMatrices();

    for( unsigned i = 0; i < assemblyFrags.size(); i++){
        
        singleView = addbfrag2D( assemblyFrags[i] );

        //push back the new view so the caller can keep track of all views if they wish
        if(views != 0){
            views->push_back(singleView);
        }

        theAssemblyView->addPositionedPiece( singleView, assemblyMatrices[i] );

    }


    add( theAssemblyView );
    post_redraw();

    return theAssemblyView;

}


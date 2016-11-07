#include "my_bfrag_Menu.h"
#include <vgui/vgui.h>

my_bfrag_Menu::my_bfrag_Menu()
{
    whereToStart_ = 1;
    
}

my_bfrag_Menu::~my_bfrag_Menu()
{
}

void my_bfrag_Menu::add_to_menu( vgui_menu & menu )
{
  vgui_menu fileItems;
  fileItems.add( "&Load...", load ); 
  fileItems.add( "&Load...", load );     
  fileItems.add( "&Save...", save );
  fileItems.add( "&Quit...", quit );
  menu.add( "&File" , fileItems );

  vgui_menu display_items;
  display_items.add( "&Display All Fragment Contours...", displayAllContours);
  display_items.add( "&Display N Fragment Contours...", displayNContours );
  display_items.add( "&Display All Pairwise Matches...", displayCurrentPairs );
  display_items.add( "&Display N Pairwise Matches...", displayNPairs );
  display_items.add( "&Display All Puzzle Solutions...", displayAllPuzzleSolutions );
  display_items.add( "&Display N Puzzle Solutions...", displayNPuzzleSolutions );
  menu.add( "&Display" , display_items );

  vgui_menu help;
  help.add( "&Contents", help_contents );
  help.separator();
  help.add( "&About", about );
  menu.add( "&Help", help );
}


void my_bfrag_Menu::removeSeletedPuzzles()
{
  bfrag_Manager::instance()->removeSelectedPuzzles();
}

void my_bfrag_Menu::removeSelectedFragment()
{
  bfrag_Manager::instance()->removeSelectedFragments();
}

void my_bfrag_Menu::displayAllContours()
{
  bfrag_Manager::instance()->displayAllContours();
}

void my_bfrag_Menu::displayNContours()
{
  vgui_dialog which_dl("Display N Contours");
  int chosen, whereToStart = 1;
  vcl_vector<vcl_string> numPairs;
  numPairs.push_back("1");
  numPairs.push_back("4");
  numPairs.push_back("9");
  numPairs.push_back("16");
  numPairs.push_back("25");
  numPairs.push_back("36");
  numPairs.push_back("49");
  which_dl.choice("Number of Contours To Display: ", numPairs, chosen );
  which_dl.field("Starting with Contour: ", whereToStart );
  which_dl.message("NOTE: first contour is 1");

  if( which_dl.ask() )
  {
    switch(chosen)
    {
        case 0:
          chosen = 1;
          break;
        case 1:
          chosen = 4;
          break;
        case 2:
          chosen = 9;
          break;
        case 3:
          chosen = 16;
          break;
        case 4:
          chosen = 25;
          break;
        case 5:
          chosen = 36;
          break;
        case 6:
          chosen = 49;
          break;
    }
    bfrag_Manager::instance()->displayNContours(chosen, whereToStart);
  }
}

void my_bfrag_Menu::displayAllPuzzleSolutions()
{
  bfrag_Manager::instance()->displayAllPuzzleSolutions();
}

void my_bfrag_Menu::displayNPuzzleSolutions()
{
  bfrag_Manager::instance()->displayNPuzzleSolutions();
}

void my_bfrag_Menu::displayNPairs()
{
  vgui_dialog which_dl("Display N Pairs");
  int chosen, whereToStart = 1;
  vcl_vector<vcl_string> numPairs;
  numPairs.push_back("1");
  numPairs.push_back("4");
  numPairs.push_back("9");
  numPairs.push_back("16");
  numPairs.push_back("25");
  numPairs.push_back("36");
  numPairs.push_back("49");
  which_dl.choice("Number of Pairs To Display: ", numPairs, chosen );
  which_dl.field("Starting with Pair: ", whereToStart );
  which_dl.message("NOTE: first pair is 1");

  if( which_dl.ask() )
  {
    switch(chosen)
    {
        case 0:
          chosen = 1;
          break;
        case 1:
          chosen = 4;
          break;
        case 2:
          chosen = 9;
          break;
        case 3:
          chosen = 16;
          break;
        case 4:
          chosen = 25;
          break;
        case 5:
          chosen = 36;
          break;
        case 6:
          chosen = 49;
          break;
    }
    bfrag_Manager::instance()->displayNPairs(chosen, whereToStart);
  }
}

void my_bfrag_Menu::resetPairs()
{
  bfrag_Manager::instance()->resetPairs();
}

void my_bfrag_Menu::removeSelectedPairs()
{
  bfrag_Manager::instance()->removeSelectedPairs();
}

void my_bfrag_Menu::displayCurrentPairs()
{
  bfrag_Manager::instance()->displayCurrentPairs();
}


void my_bfrag_Menu::displayNextPair()
{
  assert(0);
  //bfrag_Manager::instance()->displayNextPair();
}

void my_bfrag_Menu::help_contents()
{  
  vgui_dialog about_dl("HindSite Visualizer Help System");
  about_dl.message( "Ctrl + Click Left Mouse Button : Zoom in" );
  about_dl.message( "Ctrl + Click Right Mouse Button : Zoom out" );
  about_dl.message( "Ctrl + Hold Middle Mouse Button + Move Mouse : Drag" );
  about_dl.message( "Click Right Mouse Button : Menu" );
  about_dl.set_cancel_button(0);
  about_dl.ask(); 
}

void my_bfrag_Menu::about()
{
  vgui_dialog about_dl("About HindSite Visualizer -- 2007");

  about_dl.message( "HindSite Visualizer \n(C) 2007 LEMS @ Brown University" );
  about_dl.message( "Based on M.S. works by Jonah McBride and H. Can Aras and the visualizing tool by Dan Spinosa");
  about_dl.message( "Author: H. Can Aras\nEmail: can@lems.brown.com" );

  about_dl.set_cancel_button(0);
  about_dl.ask();
}

void my_bfrag_Menu::save()
{
  vgui_dialog params("Save...");
  vcl_string ext = "*.xml";
  vcl_string filename;
  params.file("Specify XML file to save files", ext, filename);
    
  if (!params.ask())
    return;

  bfrag_Manager::instance()->save_xml(filename);
}

void my_bfrag_Menu::displayContour()
{
  bfrag_Manager::instance()->displayContour();
}

void my_bfrag_Menu::displayAssembly()
{
  bfrag_Manager::instance()->displayAssembly();
}


void my_bfrag_Menu::load()
{
  vgui_dialog params("Load...");
  vcl_string ext = "*.xml";
  vcl_string filename;
  vcl_string filename_images;
  vcl_string image_files;
  int chosen;
  params.file("Specify XML file to load files", ext, filename);
  params.choice("Show", "Only contours", "Only images", "Both contours and images", chosen);
  params.file("Specify process input XML file location for getting images", ext, filename_images);
  params.field("Specify  image names",image_files );
  if (!params.ask())
    return;

  bfrag_Manager::instance()->load_xml(filename, chosen, filename_images,image_files);
  bfrag_Manager::instance()->displayAllPuzzleSolutions();
}

void my_bfrag_Menu::quit()
{
  vgui_dialog quit_dlg("Quit...");
  quit_dlg.message("Are you sure you want to quit? (Unsaved changes will be lost!)");
  bool quit_really = quit_dlg.ask();
  if(quit_really)
    bfrag_Manager::instance()->quit();
  else
  {
    vcl_cout << "Quit operation cancelled by user!" << vcl_endl;
    return;
  }
}

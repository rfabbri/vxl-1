
// \: file
// \brief This program compute the isosurface of a vil3d image
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date July 6, 2006




// This example reads a volume dataset, extracts an isosurface that
// represents the skin and displays it.

#include <vil3d/vil3d_image_view.h>
#include <dbmsh3d/algovtk/dbmsh3d_isosurface.h>

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vul/vul_file.h>


#include <vcl_iostream.h>
#include <vgui/vgui.h>
#include <vgui/vgui_shell_tableau.h>

#include <vgui/vgui_grid_tableau.h>
#include <bgui3d/bgui3d_examiner_tableau.h>
#include <bgui3d/bgui3d.h>
#include <Inventor/nodes/SoSeparator.h>

#include <dbmsh3d/dbmsh3d_mesh.h>
#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <dbmsh3d/vis/dbmsh3d_vis_backpt.h>
#include <dbmsh3d/vis/dbmsh3d_vis_mesh.h>
#include <dbmsh3d/vis/dbmsh3d_vis_face.h>


int main (int argc, char **argv)
{
  ////
  //if (argc < 2)
  //  {
  //    cout << "Usage: " << argv[0] << " DATADIR/headsq/quarter" << endl;
  //  return 1;
  //  }

  // Create the renderer, the render window, and the interactor. The renderer
  // draws into the render window, the interactor enables mouse- and 
  // keyboard-based interaction with the data within the render window.
  //



  vil3d_image_view<vxl_byte > img3d(64, 64, 64);

  if ( ! img3d.is_contiguous())
  {
    vcl_cout << "img3d not contiguous." << vcl_endl;
    return 1;
  }
  
  
  
  vcl_cout << "img3d is contiguous";
  vcl_cout << "i_step = " << img3d.istep() << vcl_endl;
  vcl_cout << "j_step = " << img3d.jstep() << vcl_endl;
  vcl_cout << "k_step = " << img3d.kstep() << vcl_endl;

  img3d.fill(0);
  for (unsigned i=30; i<50; ++i)
    for (unsigned j=30; j<50; ++j)
      for (unsigned k=0; k<64; ++k)
        img3d(i,j,k) = 255;




  dbmsh3d_mesh mesh;
  dbmsh3d_isosurface(mesh, img3d);



  int my_argc = 2;
  char** my_argv = new char*[2];
  my_argv[0] = "mesh_vis";
  my_argv[1] = "--mfc-use-gl";
  vgui::init(my_argc, my_argv);
  delete []my_argv;

  // initialize bgui_3d
  bgui3d_init();

  // create the scene graph root
  SoSeparator *root = new SoSeparator;
  root->ref();

  // #################################################
  // Load and display meshes

  dbmsh3dvis_init_vispt_OpenInventor_classes ();

  // fixed mesh
  //mesh.IFS_to_MHE();
  mesh.build_IFS_mesh();
  SoSeparator* vis = new SoSeparator;

  //SbColor color = SbColor (1.0f, 0.1f, 0.1f);
  //// Draw each individual faces/edges/vertices as separate object to allow getInfo.
  //vis = draw_mesh_faces (&mesh, true, 0.0f, color);
  //root->addChild (vis);

  // Draw the whole mesh to a single object
  vis = draw_M(&mesh, true, 0.0f);
  root->addChild(vis);


  // #################################################

  // wrap the scene graph in an examiner tableau
  bgui3d_examiner_tableau_new tab3d(root);
  root->unref();

  // Put a shell tableau at the top of our tableau tree.
  vgui_shell_tableau_new shell(tab3d);

  // Create a window, add the tableau and show it on screen.
  return vgui::run(shell, 400, 400);


















#if 0

    vtkRenderer *aRenderer = vtkRenderer::New();
  vtkRenderWindow *renWin = vtkRenderWindow::New();
    renWin->AddRenderer(aRenderer);
  vtkRenderWindowInteractor *iren = vtkRenderWindowInteractor::New();
    iren->SetRenderWindow(renWin);

  
  // create an importer to read the data back in
  vtkImageImport *importer = vtkImageImport::New();
  
  importer->SetWholeExtent(1, img3d.ni(), 1, img3d.nj(), 1, img3d.nk());
  importer->SetDataExtentToWholeExtent();
  
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetImportVoidPointer(img3d.origin_ptr());

  // An isosurface, or contour value of 500 is known to correspond to the
  // skin of the patient. Once generated, a vtkPolyDataNormals filter is
  // is used to create normals for smooth surface shading during rendering.
  vtkContourFilter *skinExtractor = vtkContourFilter::New();
  
  skinExtractor->SetInputConnection(importer->GetOutputPort());
    skinExtractor->SetValue(0, 128);

    vtkPolyData* poly = vtkPolyData::New();
    skinExtractor->Update();
    poly = skinExtractor->GetOutput();
    poly->PrintSelf(vcl_cout, 2);

  vtkPolyDataNormals *skinNormals = vtkPolyDataNormals::New();
    skinNormals->SetInputConnection(skinExtractor->GetOutputPort());
    skinNormals->SetFeatureAngle(60.0);
  
  vtkPolyDataMapper *skinMapper = vtkPolyDataMapper::New();
    skinMapper->SetInputConnection(skinNormals->GetOutputPort());
    skinMapper->ScalarVisibilityOff();
  vtkActor *skin = vtkActor::New();
    skin->SetMapper(skinMapper);

  // An outline provides context around the data.
  //
  vtkOutlineFilter *outlineData = vtkOutlineFilter::New();
     outlineData->SetInputConnection(importer->GetOutputPort());
  vtkPolyDataMapper *mapOutline = vtkPolyDataMapper::New();
    mapOutline->SetInputConnection(outlineData->GetOutputPort());
  vtkActor *outline = vtkActor::New();
    outline->SetMapper(mapOutline);
    outline->GetProperty()->SetColor(0,0,0);

  // It is convenient to create an initial view of the data. The FocalPoint
  // and Position form a vector direction. Later on (ResetCamera() method)
  // this vector is used to position the camera to look at the data in
  // this direction.
  vtkCamera *aCamera = vtkCamera::New();
    aCamera->SetViewUp (0, 0, -1);
    aCamera->SetPosition (0, 1, 0);
    aCamera->SetFocalPoint (0, 0, 0);
    aCamera->ComputeViewPlaneNormal();

  // Actors are added to the renderer. An initial camera view is created.
  // The Dolly() method moves the camera towards the FocalPoint,
  // thereby enlarging the image.
  aRenderer->AddActor(outline);
  aRenderer->AddActor(skin);
  aRenderer->SetActiveCamera(aCamera);
  aRenderer->ResetCamera ();
  aCamera->Dolly(1.5);

  // Set a background color for the renderer and set the size of the
  // render window (expressed in pixels).
  aRenderer->SetBackground(1,1,1);
  renWin->SetSize(640, 480);

  // Note that when camera movement occurs (as it does in the Dolly()
  // method), the clipping planes often need adjusting. Clipping planes
  // consist of two planes: near and far along the view direction. The 
  // near plane clips out objects in front of the plane; the far plane
  // clips out objects behind the plane. This way only what is drawn
  // between the planes is actually rendered.
  aRenderer->ResetCameraClippingRange ();

  // Initialize the event loop and then start it.
  iren->Initialize();
  iren->Start(); 

  ////
  // It is important to delete all objects created previously to prevent
  // memory leaks. In this case, since the program is on its way to
  // exiting, it is not so important. But in applications it is
  // essential.
  //v16->Delete();
  skinExtractor->Delete();
  skinNormals->Delete();
  skinMapper->Delete();
  skin->Delete();
  outlineData->Delete();
  mapOutline->Delete();
  outline->Delete();
  aCamera->Delete();
  iren->Delete();
  renWin->Delete();
  aRenderer->Delete();

  importer->Delete();

#endif

  return 0;
}

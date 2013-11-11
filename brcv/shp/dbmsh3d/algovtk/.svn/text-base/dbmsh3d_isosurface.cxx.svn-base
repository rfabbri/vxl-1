// This is basic/dbgl/algo/dbmsh3d_isosurface.cxx

#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif



#include "vtkImageImport.h"
//#include "vtkContourFilter.h"
#include "vtkMarchingCubes.h"
#include "vtkCellArray.h"
#include "dbmsh3d_isosurface.h"

//: This function takes in a 3D image and computes its isosurface at `val'
bool dbmsh3d_isosurface(dbmsh3d_mesh& mesh, 
                        const vil3d_image_view<vxl_byte >& img3d, 
                        vxl_byte val,
                        double data_spacing_dx,
                        double data_spacing_dy,
                        double data_spacing_dz
                        )
{
  // create a 3D image identical to the original image but with contiguous 
  // memory allocation
  vil3d_image_view<vxl_byte > data;
  if (img3d.is_contiguous())
    data = img3d;
  else
    data.deep_copy(img3d);

  if (! data.is_contiguous())
  {
    vcl_cerr << "In " << __FILE__ 
      << " : Could not create an contiguous image." << vcl_endl;
    return false;
  }

  // Now we're sure ``data" is contiguous, we convert it to vtk image format

  // create an importer to read the data
  vtkImageImport *importer = vtkImageImport::New();
  importer->SetWholeExtent(1, data.ni(), 1, data.nj(), 1, data.nk());
  importer->SetDataExtentToWholeExtent();
  importer->SetDataScalarTypeToUnsignedChar();
  importer->SetImportVoidPointer(data.origin_ptr());

  importer->SetDataSpacing(data_spacing_dx, data_spacing_dy, data_spacing_dz);


  // use an isocontour filter to extract isosurface of the 3d image
  // vtkContourFilter *contourExtractor = vtkContourFilter::New();  
  vtkMarchingCubes *contourExtractor = vtkMarchingCubes::New();  
  contourExtractor->SetInputConnection(importer->GetOutputPort());
  contourExtractor->SetValue(0, val);
  contourExtractor->Update();




  // extract the output mesh
  vtkPolyData* polyData = contourExtractor->GetOutput();

  // convert from vtkPolyData to dbmsh3d_mesh
  //mesh.clean_IFS_mesh();
  mesh.clear();

  // vertices
  vtkPoints* points = polyData->GetPoints();
  for (vtkIdType i=0; i < points->GetNumberOfPoints(); ++i)
  {
    double x[3];
    points->GetPoint(i, x);

    // INSERT VERTEX INTO THE MESH
    dbmsh3d_vertex* vertex = 
      mesh._new_vertex ();
    vertex->set_pt(vgl_point_3d<double >(x));
    mesh._add_vertex (vertex);
  }

  // faces
  vtkCellArray* polys = polyData->GetPolys();

  // Traverse thru all the faces of the mesh
  polys->InitTraversal();
  vtkIdType npts;
  vtkIdType* pts;
  while (polys->GetNextCell(npts, pts))
  {
    // INSERT FACE INTO THE MESH
    dbmsh3d_face* face = mesh._new_face ();
    
    // get pointers to the vertices of the new face
    for (vtkIdType k=0; k < npts; ++k ) 
    {
      // get pointer to the vertex from its index
      dbmsh3d_vertex* vertex = 
        mesh.vertexmap(pts[k]);
      assert(vertex);
      face->_ifs_add_bnd_V(vertex);
    }
    mesh._add_face (face);
  }
  contourExtractor->Delete();
  importer->Delete();
 
  return true;
}


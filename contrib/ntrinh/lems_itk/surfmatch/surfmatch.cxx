// \: file
// \brief This function matches two surfaces using ICP and similarity transform
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 29, 2006

#include <vcl_vector.h>

#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <dbmsh3d/dbmsh3d_mesh.h>

#include <dbgl/dbgl_minimal_container.h>

#include "itkTranslationTransform.h"
#include "itkSimilarity3DTransform.h"
#include "itkVersorRigid3DTransform.h"
#include "itkCenteredSimilarity2DTransform.h"
#include "itkAffineTransform.h"

#include "itkEuclideanDistancePointMetric.h"
#include "itkLevenbergMarquardtOptimizer.h"
#include "itkPointSet.h"
#include "itkPointSetToPointSetRegistrationMethod.h"
#include "itkDanielssonDistanceMapImageFilter.h"
#include "itkPointSetToImageFilter.h"
#include "itkTransformMeshFilter.h"

#include "itkLandmarkBasedTransformInitializer.h"




vgl_point_3d<double > translate_scale(const vgl_point_3d<double >& p, 
                                      const vgl_vector_3d<double >& v,
                                      double scale)
{
  vgl_point_3d<double > tp = p + v;
  tp.set(tp.x() * scale, tp.y()*scale, tp.z() * scale); 

  return tp;

}


// given two corresponding point sets, find the best rigid transform with least mean square
 bool match_landmarks(
  const vcl_vector<vgl_point_3d<double > >& fixed_points, 
  const vcl_vector<vgl_point_3d<double > >& moving_points,
  vgl_h_matrix_3d<double >& transform_matrix)
{
  if (fixed_points.size() != moving_points.size())
  {
    vcl_cerr << "Two point sets need to be of the same size." << vcl_endl;
    return false;
  }

  const unsigned int Dimension = 3;
  typedef itk::Image< double, Dimension >  ImageType;
  typedef itk::VersorRigid3DTransform<double > TransformType;
  
  typedef itk::LandmarkBasedTransformInitializer<TransformType, ImageType, ImageType> TransformInitType;

  TransformInitType::Pointer transform_init = TransformInitType::New();

  TransformInitType::LandmarkPointContainer fixed_landmarks;
  TransformInitType::LandmarkPointContainer moving_landmarks;
  TransformType::Pointer transform = TransformType::New();


  // set the landmarks

  // fixed points
  for (unsigned int i=0; i<fixed_points.size(); ++i)
  {
    vgl_point_3d<double > fixed_pt = fixed_points[i];
    TransformInitType::LandmarkPointType pt;
    pt[0] = fixed_pt.x();
    pt[1] = fixed_pt.y();
    pt[2] = fixed_pt.z();
    fixed_landmarks.push_back(pt);
  }

  // moving points
  for (unsigned int i=0; i<moving_points.size(); ++i)
  {
    vgl_point_3d<double > moving_pt = moving_points[i];
    TransformInitType::LandmarkPointType pt;
    pt[0] = moving_pt.x();
    pt[1] = moving_pt.y();
    pt[2] = moving_pt.z();
    moving_landmarks.push_back(pt);
  }


  // Need to reverse the roles of fixed and moving landmarks for correct result
  
  transform_init->SetFixedLandmarks(moving_landmarks);
  transform_init->SetMovingLandmarks(fixed_landmarks);
  transform_init->SetTransform(transform);
  transform_init->InitializeTransform();

  vcl_cout << "Init transform = " << transform << vcl_endl;

  transform_matrix = vgl_h_matrix_3d<double >(transform->GetMatrix().GetVnlMatrix(),
    transform->GetOffset().Get_vnl_vector());
 

  ///typedef itk::PointSet< double, Dimension >   PointSetType;

  //  // Registration Method
  //typedef itk::PointSetToPointSetRegistrationMethod< 
  //                                          PointSetType, 
  //                                          PointSetType >
  //                                                  RegistrationType;


  //RegistrationType::Pointer   registration  = RegistrationType::New();

  //double dist;
  //registration->GetOptimizer()->get

  
  

  return true;

  
  
};


bool surfmatch(dbmsh3d_mesh& fixed_mesh, 
               dbmsh3d_mesh& moving_mesh, 
               vgl_h_matrix_3d<double >& transform_matrix,
               const vgl_h_matrix_3d<double >& init_matrix,
               const vcl_string& report_file)
{  
  const unsigned int Dimension = 3;
  
  

  typedef itk::PointSet< double, Dimension >   PointSetType;

  PointSetType::Pointer fixedPointSet  = PointSetType::New();
  PointSetType::Pointer movingPointSet = PointSetType::New();

  typedef PointSetType::PointType     PointType;
  typedef PointSetType::PointsContainer  PointsContainer;

  PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
  PointsContainer::Pointer movingPointContainer = PointsContainer::New();

  PointType fixedPoint;
  PointType movingPoint;

  // traverse thru all vertices and write to ply file
  unsigned int pointId = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fixed_mesh.vertexmap().begin();
  for (; vit != fixed_mesh.vertexmap().end(); ++vit) 
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
    vgl_point_3d<double > pt = v->get_pt();

    fixedPoint[0] = pt.x();
    fixedPoint[1] = pt.y();
    fixedPoint[2] = pt.z();

    fixedPointContainer->InsertElement(pointId, fixedPoint);
    ++pointId;
  }
  fixedPointSet->SetPoints( fixedPointContainer );

  vcl_cout << "Npts of fixedPointSet = " << 
    fixedPointSet->GetNumberOfPoints() << vcl_endl;

  // traverse thru all vertices and write to ply file
  pointId = 0;
  vit = moving_mesh.vertexmap().begin();
  for (; vit != moving_mesh.vertexmap().end(); ++vit) 
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
    vgl_point_3d<double > pt = v->get_pt();

    //
    movingPoint[0] = pt.x();
    movingPoint[1] = pt.y();
    movingPoint[2] = pt.z();

    movingPointContainer->InsertElement(pointId, movingPoint);
    ++pointId;
  }
  movingPointSet->SetPoints( movingPointContainer );
  
  vcl_cout << "Npts of movingPointSet = " << 
    movingPointSet->GetNumberOfPoints() << vcl_endl;


  // ======


  //-----------------------------------------------------------
  // Set up  the Metric
  //-----------------------------------------------------------
  typedef itk::EuclideanDistancePointMetric<  
                                    PointSetType, 
                                    PointSetType>
                                                    MetricType;

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;
  typedef TransformBaseType::JacobianType           JacobianType;

  MetricType::Pointer  metric = MetricType::New();


  //-----------------------------------------------------------
  // Set up a Transform
  //-----------------------------------------------------------

  ////
  typedef itk::Similarity3DTransform< double >      TransformType;
  TransformType::Pointer transform = TransformType::New();
  

  // Optimizer Type
  typedef itk::LevenbergMarquardtOptimizer OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  optimizer->SetUseCostFunctionGradient(false);
  
  //optimizer->DebugOn();

  // Registration Method
  typedef itk::PointSetToPointSetRegistrationMethod< 
                                            PointSetType, 
                                            PointSetType >
                                                    RegistrationType;


  RegistrationType::Pointer   registration  = RegistrationType::New();

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );
  //scales.Fill( 0.01 );

  scales.Fill( 1 );
  double translation_scale = 0.1;
  double versor_scale = 1;
  double scaling = 10;
  scales[0] = scales[1] = scales[2] = versor_scale;
  scales[3] = scales[4] = scales[5] = translation_scale;
  scales[6] = scaling;

 // //
 //this->m_Parameters[0] = this->GetVersor().GetX();
 // this->m_Parameters[1] = this->GetVersor().GetY();
 // this->m_Parameters[2] = this->GetVersor().GetZ();

  //// Transfer the translation
  //this->m_Parameters[3] = this->GetTranslation()[0];
  //this->m_Parameters[4] = this->GetTranslation()[1];
  //this->m_Parameters[5] = this->GetTranslation()[2];


  
  unsigned long   numberOfIterations =  100;
  double          gradientTolerance  =  1e-5;    // convergence criterion
  double          valueTolerance     =  1e-5;    // convergence criterion
  double          epsilonFunction    =  1e-6;   // convergence criterion

  optimizer->SetScales( scales );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetValueTolerance( valueTolerance );
  optimizer->SetGradientTolerance( gradientTolerance );
  optimizer->SetEpsilonFunction( epsilonFunction );

  //// Start from an Identity transform (in a normal case, the user 
  //// can probably provide a better guess than the identity...
  //transform->SetIdentity();
  //
  // determine rotational center and initial translation value
  // rotational center is the centroid of the fixed point set
  typedef PointsContainer::Iterator PointIterator;
  //
  //PointType fixedCentroid;
  //for (unsigned int i=0; i<fixedCentroid.GetPointDimension(); ++i)
  //{
  //  fixedCentroid.SetElement(i, 0);
  //}

  //// Go trough the list of fixed point and find the closest distance
  //PointIterator pointItr2 = fixedPointSet->GetPoints()->Begin();
  //PointIterator pointEnd2 = fixedPointSet->GetPoints()->End();

  //while( pointItr2 != pointEnd2 )  
  //{
  //  for (unsigned int i=0; i<fixedCentroid.GetPointDimension(); ++i)
  //  {
  //    fixedCentroid.SetElement(i, fixedCentroid.GetElement(i) + pointItr2.Value().GetElement(i));
  //  }
  //  pointItr2++;
  //}

  //for (unsigned int i=0; i<fixedCentroid.GetPointDimension(); ++i)
  //{
  //  fixedCentroid.SetElement(i, fixedCentroid.GetElement(i) / fixedPointSet->GetNumberOfPoints());
  //}

  //vcl_cout << "Fixed centroid " << fixedCentroid << vcl_endl;

  //PointType movingCentroid;
  //for (unsigned int i=0; i<movingCentroid.GetPointDimension(); ++i)
  //{
  //  movingCentroid.SetElement(i, 0);
  //}

  //// Go trough the list of fixed point and find the closest distance
  //pointItr2 = movingPointSet->GetPoints()->Begin();
  //pointEnd2 = movingPointSet->GetPoints()->End();

  //while( pointItr2 != pointEnd2 )  
  //{
  //  for (unsigned int i=0; i<movingCentroid.GetPointDimension(); ++i)
  //  {
  //    movingCentroid.SetElement(i, movingCentroid.GetElement(i) + pointItr2.Value().GetElement(i));
  //  }
  //  pointItr2++;
  //}

  //for (unsigned int i=0; i<movingCentroid.GetPointDimension(); ++i)
  //{
  //  movingCentroid.SetElement(i, movingCentroid.GetElement(i) / movingPointSet->GetNumberOfPoints());
  //}
  //vcl_cout << "Moving centroid " << movingCentroid << vcl_endl;

  //transform->SetTranslation(fixedCentroid-movingCentroid);


  itk::Matrix<double, 3, 3> m;
  for (int c=0; c<3; ++c)
    for (int r=0; r<3; ++r)
      m[r][c] = init_matrix.get(r, c);

  transform->SetMatrix(m);

  itk::Vector<double, 3> offset;
  for (int i = 0; i <3; ++i)
    offset[i] = init_matrix.get(i, 3);
  
  transform->SetOffset(offset);





  vcl_cout << "Initial translation = " <<  transform->GetTranslation() << vcl_endl;



  registration->SetInitialTransformParameters( transform->GetParameters() );

  //------------------------------------------------------
  // Connect all the components required for Registration
  //------------------------------------------------------
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedPointSet( fixedPointSet );
  registration->SetMovingPointSet(   movingPointSet   );










 

  //------------------------------------------------------
  // Prepare the Distance Map in order to accelerate 
  // distance computations.
  //------------------------------------------------------
  //
  //  First map the Fixed Points into a binary image.
  //  This is needed because the DanielssonDistance 
  //  filter expects an image as input.
  //
  //-------------------------------------------------



  vcl_cout << "preparing distance map for ICP .. \n";




  typedef itk::Image< unsigned char,  Dimension >  BinaryImageType;

  typedef itk::PointSetToImageFilter< 
                            PointSetType,
                            BinaryImageType> PointsToImageFilterType;

  PointsToImageFilterType::Pointer 
                  pointsToImageFilter = PointsToImageFilterType::New();
  
  pointsToImageFilter->SetInput( fixedPointSet );

  BinaryImageType::SpacingType spacing;
  spacing.Fill( 1.0 );

  BinaryImageType::PointType origin;
  origin.Fill( 0.0 );

  pointsToImageFilter->SetSpacing( spacing );
  pointsToImageFilter->SetOrigin( origin   );
  
  pointsToImageFilter->Update();

  BinaryImageType::Pointer binaryImage = pointsToImageFilter->GetOutput();


  typedef itk::Image< unsigned short, Dimension >  DistanceImageType;

  typedef itk::DanielssonDistanceMapImageFilter< 
                                          BinaryImageType,
                                          DistanceImageType> DistanceFilterType;

  DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();
  
  distanceFilter->SetInput( binaryImage );

  distanceFilter->Update();

  metric->SetDistanceMap( distanceFilter->GetOutput() );







  vcl_cout << "start registration ... " << vcl_endl;


  // for now
  try 
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << e << std::endl;
    return false;
    }

  optimizer->GetOptimizer()->diagnose_outcome();


  vcl_cout << "Final transform " << transform << vcl_endl;
  vcl_cout << "Solution = " << transform->GetParameters() << vcl_endl;

  //vcl_cout << "Versor = " << transform->GetVersor() << vcl_endl;
  //vcl_cout << "Center = " << transform->GetCenter() << vcl_endl;
  //vcl_cout << "Translation = " << transform->GetTranslation() << vcl_endl;

  // transform the mesh
  // Go trough the list of fixed point and find the closest distance
  PointIterator pointItr2 = movingPointSet->GetPoints()->Begin();
  PointIterator pointEnd2 = movingPointSet->GetPoints()->End();

  pointId = 0;
  while( pointItr2 != pointEnd2 )  
  {
    PointType pt1 = pointItr2.Value();

    itk::Point<double, Dimension> pt2; 
    pt2[0] = pt1[0];
    pt2[1] = pt1[1];
    pt2[2] = pt1[2];
    itk::Point<double, Dimension> pt3 = transform->TransformPoint(pt2);
    
    vgl_point_3d<double > pt(pt3[0], pt3[1], pt3[2]);
        
    // get the vertex
    dbmsh3d_vertex* v = (dbmsh3d_vertex*)moving_mesh.vertexmap(pointId);
    v->set_pt(pt);
   
    ++pointId;
    pointItr2++;
  }


  // quaternion
  double qx, qy, qz, qw;

  // translation
  double tx, ty, tz;

  // scaling
  double lambda;

  // retrieve the parameters from the transformation
  qx = transform->GetVersor().GetX();
  qy = transform->GetVersor().GetY();
  qz = transform->GetVersor().GetZ();
  qw = transform->GetVersor().GetW();

  //// Transfer the translation
  tx = transform->GetTranslation()[0];
  ty = transform->GetTranslation()[1];
  tz = transform->GetTranslation()[2];

  lambda = transform->GetScale();


  transform_matrix = vgl_h_matrix_3d<double >(transform->GetMatrix().GetVnlMatrix(),
    transform->GetOffset().Get_vnl_vector());


  double end_error = optimizer->GetOptimizer()->get_end_error();
  double start_error = optimizer->GetOptimizer()->get_start_error();

  vcl_ofstream out_file(report_file.c_str());

  for (int r=0; r<4; ++r)
  {
    for (int c=0; c<4; ++c)
    {
      out_file << transform_matrix.get(r, c) << " ";
    }
    out_file << "\n";
  }

  optimizer->GetOptimizer()->diagnose_outcome(out_file);

  out_file.close();
  

  return true;

}





bool surfmatch2(dbmsh3d_mesh& fixed_mesh, 
               dbmsh3d_mesh& moving_mesh, 
               vgl_h_matrix_3d<double >& transform_matrix,
               const vgl_h_matrix_3d<double >& init_matrix,
               const vcl_string& report_file)
{
  vcl_cout << "Report file = " << report_file << vcl_endl;


  // get the point clouds from the meshes
  vcl_vector<vgl_point_3d<double > > fixed_cloud;
  fixed_cloud.reserve(fixed_mesh.vertexmap().size());
  for (vcl_map<int, dbmsh3d_vertex*>::iterator vit = fixed_mesh.vertexmap().begin();
    vit != fixed_mesh.vertexmap().end(); ++vit) 
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
    fixed_cloud.push_back(v->get_pt());
  }

  // moving cloud
  vcl_vector<vgl_point_3d<double > > moving_cloud;
  moving_cloud.reserve(moving_mesh.vertexmap().size());
  
  for (vcl_map<int, dbmsh3d_vertex*>::iterator vit = moving_mesh.vertexmap().begin(); 
    vit != moving_mesh.vertexmap().end(); ++vit) 
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
    moving_cloud.push_back(v->get_pt());
  }


  // now apply the initial transform to the moving mesh
  for (unsigned int i=0; i<moving_cloud.size(); ++i)
  {
    vgl_homg_point_3d<double > homg_pt(moving_cloud[i]);
    vgl_homg_point_3d<double > tp = init_matrix(homg_pt);
    double vx, vy, vz;
    tp.get_nonhomogeneous(vx, vy, vz);
    moving_cloud.at(i).set(vx, vy, vz);
  }


  // normalize both point clouds to the (0, 0, 0)
  vgl_point_3d<double > coord_origin(0, 0, 0);
  vgl_point_3d<double > fixed_center;
  double fixed_radius;

  vgl_point_3d<double > moving_center;
  double moving_radius;

  dbgl_minimal_enclosing_sphere(fixed_cloud, fixed_center, fixed_radius);
  vgl_vector_3d<double > fixed_v = fixed_center - coord_origin;
  vcl_cout << "Fixed Center = " << fixed_center << vcl_endl;
  vcl_cout << "Fixed radius = " << fixed_radius << vcl_endl;


  dbgl_minimal_enclosing_sphere(moving_cloud, moving_center, moving_radius);
  vgl_vector_3d<double > moving_v = moving_center - coord_origin;
  vcl_cout << "Moving Center = " << moving_center << vcl_endl;
  vcl_cout << "Moving radius = " << moving_radius << vcl_endl;


  // move fixed_cloud to origin
  for (unsigned int i=0; i<fixed_cloud.size(); ++i)
  {
    vgl_point_3d<double > pt = fixed_cloud[i];
    //fixed_cloud.at(i) = pt + vgl_vector_3d<double >(1, 1, 1); 
    fixed_cloud.at(i) = translate_scale(pt, -fixed_v, 1);
  }


  // move and scale moving_cloud to match with fixed cloud


  vnl_matrix_fixed<double, 4, 4> m1;
  m1.set_identity();

  // scaling
  m1[3][3] = moving_radius / fixed_radius;

  // translation
  m1[0][3] = -fixed_v.x();
  m1[1][3] = -fixed_v.y();
  m1[2][3] = -fixed_v.z();

  vgl_h_matrix_3d<double > hmatrix_trans_scale(m1);

  for (unsigned int i=0; i<moving_cloud.size(); ++i)
  {
    vgl_homg_point_3d<double > homg_pt(moving_cloud[i]);
    vgl_homg_point_3d<double > tp = hmatrix_trans_scale(homg_pt);
    double vx, vy, vz;
    tp.get_nonhomogeneous(vx, vy, vz);
    moving_cloud[i].set(vx, vy, vz);
  } 


  int id_count = 0;
  

  
  // Perform rigid registration between the two clouds
  const unsigned int Dimension = 3;
    
  

  typedef itk::PointSet< double, Dimension >   PointSetType;

  PointSetType::Pointer fixedPointSet  = PointSetType::New();
  PointSetType::Pointer movingPointSet = PointSetType::New();

  typedef PointSetType::PointType     PointType;
  typedef PointSetType::PointsContainer  PointsContainer;

  PointsContainer::Pointer fixedPointContainer  = PointsContainer::New();
  PointsContainer::Pointer movingPointContainer = PointsContainer::New();

  PointType fixedPoint;
  PointType movingPoint;

  //// =========================

  // convert point vector to PointSet
  unsigned int pointId = 0;
  for (unsigned int i=0; i< fixed_cloud.size(); ++i)
  {
    vgl_point_3d<double > pt = fixed_cloud.at(i);
    fixedPoint[0] = pt.x();
    fixedPoint[1] = pt.y();
    fixedPoint[2] = pt.z();

    fixedPointContainer->InsertElement(pointId, fixedPoint);
    ++pointId;
  }
  fixedPointSet->SetPoints( fixedPointContainer );

  vcl_cout << "Npts of fixedPointSet = " << 
    fixedPointSet->GetNumberOfPoints() << vcl_endl;

  // traverse thru all vertices and write to ply file
  pointId = 0;
  for (unsigned int i=0; i<moving_cloud.size(); ++i) 
  {
    vgl_point_3d<double > pt = moving_cloud.at(i);
    movingPoint[0] = pt.x();
    movingPoint[1] = pt.y();
    movingPoint[2] = pt.z();
    movingPointContainer->InsertElement(pointId, movingPoint);
    ++pointId;
  }
  movingPointSet->SetPoints( movingPointContainer );
  
  vcl_cout << "Npts of movingPointSet = " << 
    movingPointSet->GetNumberOfPoints() << vcl_endl;



// =============


  //-----------------------------------------------------------
  // Set up  the Metric
  //-----------------------------------------------------------
  typedef itk::EuclideanDistancePointMetric<  
                                    PointSetType, 
                                    PointSetType>
                                                    MetricType;

  typedef MetricType::TransformType                 TransformBaseType;
  typedef TransformBaseType::ParametersType         ParametersType;
  typedef TransformBaseType::JacobianType           JacobianType;

  MetricType::Pointer  metric = MetricType::New();


  //-----------------------------------------------------------
  // Set up a Transform
  //-----------------------------------------------------------

  ////
  //typedef itk::Similarity3DTransform< double >      TransformType;
  
  //typedef itk::VersorRigid3DTransform< double >      TransformType;
  typedef itk::AffineTransform<double, Dimension >    TransformType;

  //typedef itk::TranslationTransform< double, Dimension >      TransformType;
  TransformType::Pointer transform = TransformType::New();
  


  // Optimizer Type
  typedef itk::LevenbergMarquardtOptimizer OptimizerType;

  OptimizerType::Pointer      optimizer     = OptimizerType::New();
  optimizer->SetUseCostFunctionGradient(false);
 


  // Registration Method
  typedef itk::PointSetToPointSetRegistrationMethod< 
                                            PointSetType, 
                                            PointSetType >
                                                    RegistrationType;


  RegistrationType::Pointer   registration  = RegistrationType::New();

  // Scale the translation components of the Transform in the Optimizer
  OptimizerType::ScalesType scales( transform->GetNumberOfParameters() );
  //scales.Fill( 0.01 );

  scales.Fill( 1 );

  unsigned long   numberOfIterations =  50;
  double          gradientTolerance  =  1e-5;    // convergence criterion
  double          valueTolerance     =  1e-5;    // convergence criterion
  double          epsilonFunction    =  1e-6;   // convergence criterion

  optimizer->SetScales( scales );
  optimizer->SetNumberOfIterations( numberOfIterations );
  optimizer->SetValueTolerance( valueTolerance );
  optimizer->SetGradientTolerance( gradientTolerance );
  optimizer->SetEpsilonFunction( epsilonFunction );

  // Start from an Identity transform (in a normal case, the user 
  // can probably provide a better guess than the identity...
  transform->SetIdentity();

  //
  // determine rotational center and initial translation value
  // rotational center is the centroid of the fixed point set
  //typedef PointsContainer::Iterator PointIterator;

  registration->SetInitialTransformParameters( transform->GetParameters() );

  //------------------------------------------------------
  // Connect all the components required for Registration
  //------------------------------------------------------
  registration->SetMetric(        metric        );
  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetFixedPointSet( fixedPointSet );
  registration->SetMovingPointSet(   movingPointSet   );


  //------------------------------------------------------
  // Prepare the Distance Map in order to accelerate 
  // distance computations.
  //------------------------------------------------------
  //
  //  First map the Fixed Points into a binary image.
  //  This is needed because the DanielssonDistance 
  //  filter expects an image as input.
  //
  //-------------------------------------------------



  vcl_cout << "preparing distance map for ICP .. \n";




  typedef itk::Image< unsigned char,  Dimension >  BinaryImageType;

  typedef itk::PointSetToImageFilter< PointSetType,
                            BinaryImageType> PointsToImageFilterType;

  PointsToImageFilterType::Pointer 
                  pointsToImageFilter = PointsToImageFilterType::New();
  
  pointsToImageFilter->SetInput( fixedPointSet );

  BinaryImageType::SpacingType spacing;
  spacing.Fill( 1.0 );

  BinaryImageType::PointType origin;
  origin.Fill( 0.0 );

  pointsToImageFilter->SetSpacing( spacing );
  pointsToImageFilter->SetOrigin( origin   );
  
  pointsToImageFilter->Update();

  BinaryImageType::Pointer binaryImage = pointsToImageFilter->GetOutput();


  typedef itk::Image< unsigned short, Dimension >  DistanceImageType;

  typedef itk::DanielssonDistanceMapImageFilter< 
                                          BinaryImageType,
                                          DistanceImageType> DistanceFilterType;

  DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();
  
  distanceFilter->SetInput( binaryImage );

  distanceFilter->Update();

  metric->SetDistanceMap( distanceFilter->GetOutput() );


  vcl_cout << "start registration ... " << vcl_endl;


  // for now
  try 
    {
    registration->StartRegistration();
    }
  catch( itk::ExceptionObject & e )
    {
    std::cout << e << std::endl;
    return false;
    }

  optimizer->GetOptimizer()->diagnose_outcome();


  vcl_cout << "Final transform " << transform << vcl_endl;
  vcl_cout << "Solution = " << transform->GetParameters() << vcl_endl;

  //vcl_cout << "Versor = " << transform->GetVersor() << vcl_endl;
  //vcl_cout << "Center = " << transform->GetCenter() << vcl_endl;
  //vcl_cout << "Translation = " << transform->GetTranslation() << vcl_endl;


  vgl_h_matrix_3d<double > hmatrix_rigid(transform->GetMatrix().GetVnlMatrix(),
    transform->GetOffset().Get_vnl_vector());


  vgl_h_matrix_3d<double > hmatrix_trans_back;
  hmatrix_trans_back.set_identity();
  hmatrix_trans_back.set_translation(fixed_v.x(), fixed_v.y(), fixed_v.z()); 

  //transform_matrix = init_matrix * hmatrix_trans_scale * hmatrix_rigid * hmatrix_trans_back;
  transform_matrix =  hmatrix_trans_back * hmatrix_rigid * hmatrix_trans_scale *init_matrix ;


  double end_error = optimizer->GetOptimizer()->get_end_error();
  double start_error = optimizer->GetOptimizer()->get_start_error();

  vcl_ofstream out_file(report_file.c_str());

  for (int r=0; r<4; ++r)
  {
    for (int c=0; c<4; ++c)
    {
      out_file << transform_matrix.get(r, c) << " ";
    }
    out_file << "\n";
  }

  optimizer->GetOptimizer()->diagnose_outcome(out_file);

  out_file.close();


   // update the meshes

  // moving cloud
  id_count = 0;
  for (vcl_map<int, dbmsh3d_vertex*>::iterator vit = moving_mesh.vertexmap().begin(); 
    vit != moving_mesh.vertexmap().end(); ++vit) 
  {
    dbmsh3d_vertex* v = (dbmsh3d_vertex*) (*vit).second;
    vgl_homg_point_3d<double > homg_pt(v->get_pt());
    vgl_homg_point_3d<double > tp = transform_matrix(homg_pt);
    double vx, vy, vz;
    tp.get_nonhomogeneous(vx, vy, vz);
    v->get_pt().set(vx, vy, vz);    
  }

  return true;

}



//:
//\file
//\brief Enhanced Multiview Curve Sketch (mcs_e) main command
//\author Ricardo Fabbri, Brown & Rio de Janeiro State U. (rfabbri.github.io)
//\author Anil Usumezbas (extensions)
//
#include <vul/vul_arg.h>
#include <vul/vul_timer.h> // Gabriel: Maybe is better explicitly
#include <vul/vul_file.h>
#include <iomanip>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <sdetd/io/sdetd_load_edg.h>
#include <sdetd/algo/sdetd_convert_edgemap.h>
#include <bild/algo/bild_exact_distance_transform.h>
#include <buld/buld_arg.h>
#include <bmcsd/bmcsd_util.h>
#include <bmcsd/algo/bmcsd_algo_util.h>
#include <bmcsd/bmcsd_discrete_corresp_e.h>
#include <bmcsd/algo/bmcsd_data.h>
#include <bmcsd/pro/bmcsd_stereo_driver_e.h>
#include <bsold/bsold_file_io.cxx>


#define MY_ASSERT(msg, a, b) if ((a) != (b)) { std::cerr << (msg) << std::endl; exit(1); }

bool
write_edge_support(std::string prefix, const std::vector<bmcsd_curve_3d_attributes_e> &attr)
{
    std::string myprefix = prefix + std::string("/crvs/rec-3dcurve-");

    for (unsigned c=0; c < attr.size(); ++c) {
        std::ostringstream crv_id;
        crv_id << std::setw(std::ceil(std::log(attr.size())/std::log(10.0))+1) << std::setfill('0');
        crv_id << c;

        std::string filename = myprefix + crv_id.str() + std::string("-support.txt");

        std::ofstream attr_file;

        attr_file.open(filename.c_str());

        if (!attr_file) {
            std::cerr << "write_edge_support: error, unable to open file name" << std::endl;
            return false;
        }

        std::vector<std::set<int> > supportingEdgels = attr[c].supportingEdgelsPerConfView_;
        unsigned numConfViews = supportingEdgels.size();
            
        bmcsd_stereo_views_sptr stereoViews = attr[c].v_;
        assert(stereoViews->num_confirmation_views()==numConfViews);

        unsigned numSupportingViews = 0;
        //Count how many confirmation views have nonzero support

        for (unsigned m=0; m<numConfViews; ++m)
            if (!supportingEdgels[m].empty())
                numSupportingViews++;

        attr_file << numSupportingViews << std::endl;

        for (unsigned m=0; m<numConfViews; ++m) {
            unsigned confView = stereoViews->confirmation_view(m);
            std::set<int> edgelList = supportingEdgels[m];

            if (!edgelList.empty()) {
                attr_file << confView << " " << edgelList.size() << " ";

                std::set<int>::const_iterator list_it;

                for (list_it = edgelList.begin(); list_it != edgelList.end(); ++list_it)
                    attr_file << *list_it << " ";

                attr_file << std::endl;
            }
        }  

        attr_file.close();
    }
  return true;
}

int
main(int argc, char **argv)
{
  std::string prefix_default(".");

  vul_arg<std::string> a_prefix("-prefix", 
      "path to directory of files",prefix_default.c_str());
  vul_arg<std::string> a_cam_type("-cam_type",
      "camera type: intrinsic_extrinsic or projcamera","intrinsic_extrinsic");
  vul_arg<std::string> a_out_dir("-outdir", "output directory relative to -prefix", "out/");
  vul_arg<double> a_distance_threshold("-dist", 
      "(in pixels) threshold for an edgel to be an inlier to the reprojected curve in each view", 10.0);
  vul_arg<double> a_dtheta_threshold("-dtheta", 
      "(in degrees) threshold in orientation difference for an edgel to be an inlier to reprojected curve in each view", 10.0);
  vul_arg<unsigned> a_min_samples_per_curve_frag("-minsamples", 
      "Used to prune the curves by enforcing a minimum number of samples.", 30);
  vul_arg<double> a_min_length_per_curve_frag("-minlength", 
      "Used to prune the curves by enforcing a minimum length (this is xor minsamples).", 40);
  vul_arg<bool> a_prune_by_length("-prune_by_length", 
      "Prune using length? if not, use number of samples", true);
  vul_arg<double> a_min_epiangle("-minepiangle", 
      "(in degrees) minimum angle between an epipolar line and curve samples to consider", 30.0);

  vul_arg<unsigned> a_min_epipolar_overlap("-min_epipolar_overlap", 
      "minimum number of intersections a curve has to have with the epiband to be a candidate", 5);

  vul_arg<unsigned> a_min_inliers_per_view("-mininliers_view", 
      "If a view has less than this number of inliers, it will not vote at all towards the curve.", 20);
  vul_arg<unsigned> a_min_total_inliers("-mininliers_total", 
      "If a view has less than this number of inliers, it will not vote at all towards the curve.", 5);

  vul_arg<double> a_min_first_to_second_ratio("-first_to_second_ratio", 
      "minimum ratio of first to second best to consider as reliable unambiguous match. Set to zero if you always want to keep the best match", 1.5);

  vul_arg<unsigned> a_lonely_threshold("-lonely", 
      "minimum #votes of a lonely correspondence = -lonely*-first_to_second_ratio will be considered as reliable match (defaults to mininliers_total)", a_min_total_inliers());

  vul_arg<unsigned> a_max_concurrent_matchers("-nmatchers", 
      "The maximum number of matchers to run simultaneously", 1);
  vul_arg<bool> a_write_corresp("-write_corresp", 
      "write correspondence", false);
  vul_arg<bool> a_use_curvelets("-use_curvelets", 
      "(EXPERIMENTAL) Use curvelets to filter inlier edgels", false);

  vul_arg<unsigned> a_min_num_inliers_per_curvelet("-mininliers_cvlet", 
      "(EXPERIMENTAL) the minimum number of inlier edgels a curvelet must have in order to be inlier curvelet. Works only if -use_curvelets is set.", 3);

  vul_arg_parse(argc,argv);
  std::cout << "\n";

  bmcsd_util::camera_file_type cam_type;

  if (a_cam_type() == "intrinsic_extrinsic") {
    cam_type = bmcsd_util::BMCS_INTRINSIC_EXTRINSIC;
  } else {
    if (a_cam_type() == "projcamera")
      cam_type = bmcsd_util::BMCS_3X4;
    else  {
      std::cerr << "Error: invalid camera type " << a_cam_type() << std::endl;
      return 1;
    }
  }

  bmcsd_curve_stereo_data_path dpath;
  bool retval = 
    bmcsd_data::read_frame_data_list_txt(a_prefix(), &dpath, cam_type);
  if (!retval) return 1;
  std::cout << "Dpath:\n" << dpath << std::endl;

  bmcsd_stereo_instance_views seed_frames_to_match;

  retval = bmcsd_view_set::read_txt(
      a_prefix() + std::string("/mcs_stereo_instances.txt"), 
      &seed_frames_to_match); // Gabriel: In this context is seed!
  MY_ASSERT("frames to match from file", retval, true);
  std::cout << "Instances:\n" << seed_frames_to_match << std::endl;

  if (a_use_curvelets() && !dpath.has_curvelets()) {
    std::cerr << "Error: curvelets requested, but no file names found.\n";
  }

  //Anil: First stereo instance is used to compute the total number of 
  //confirmation views used for the elongation iterations, as well as for high-level
  //ordering of tasks
  
  //Anil: Get the view set information
  bmcsd_stereo_views_sptr seedViews = seed_frames_to_match.instance(0);
  unsigned numConf = seedViews->num_confirmation_views();

  //Anil: Create storage for all the view data
  std::vector<bmcsd_stereo_instance_views> all_frames_to_match;

  //Anil: Create storage for all the cam, edge and curve data
  std::vector<bdifd_camera> allCams(numConf+2);
  std::vector<sdet_edgemap_sptr> allEdges(numConf+2);
  std::vector<vil_image_view<vxl_uint_32> > allDTs(numConf+2);
  std::vector<vil_image_view<unsigned> > allLabels(numConf+2);
  std::vector<std::vector<vsol_polyline_2d_sptr> > allCurves(numConf+2);
  std::vector<std::vector<std::vector<double> > > allTangents(numConf+2);

  vul_timer file_io;

  //Anil: Load all the necessary edges, curves and calibration
  for (unsigned v=0; v < numConf+2; ++v) {

     // 1 Cam loader
    vpgl_perspective_camera<double> cam;
    bool retval = bmcsd_util::read_cam_anytype(dpath[v].cam_full_path(), dpath[v].cam_file_type(), &cam);
    if (!retval)
      return 1;

    allCams[v].set_p(cam);

    // 2 Edge map loader
    static const bool my_bSubPixel = true;
    static const double my_scale=1.0;
    sdet_edgemap_sptr edge_map;
    retval = sdetd_load_edg(dpath[v].edg_full_path(), my_bSubPixel, my_scale, edge_map);
    allEdges[v] = edge_map;

    // 3 Curve fragment loader
    std::string fname = dpath[v].frag_full_path();
    std::string ext = vul_file::extension(fname);
    unsigned min_samples = 0;
    unsigned min_length = 0.0;
    bool use_length = true;
    std::vector< vsol_spatial_object_2d_sptr > base;

    if (ext == ".vsl") {
      vsl_b_ifstream bp_in(fname.c_str());
      if (!bp_in) {
        std::cout << " Error opening file  " << fname << std::endl;
          return BPROD_INVALID;
        }

        std::cout << "Opened vsl file " << fname <<  " for reading" << std::endl;

        vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
        output_vsol->b_read(bp_in);
        bp_in.close();
        base = output_vsol->all_data();
      } else {
        bool retval = bsold_load_cem(base, fname);
        if (!retval) {
        return BPROD_INVALID;
      }
      std::cout << "Opened cemv file " << fname <<  " for reading" << std::endl;
    }

    std::vector< vsol_polyline_2d_sptr > curves;
    curves.reserve(base.size());

    // Cast everything to polyline

    for (unsigned i=0; i < base.size(); ++i) {
      vsol_polyline_2d_sptr
        p = dynamic_cast<vsol_polyline_2d *> (base[i].ptr());

      if (!p) {
        std::cerr << "Non-polyline found, but only POLYLINES supported!" << std::endl;
        return 1;
      }

      bool include_curve = (use_length)? (p->length() > min_length) : (p->size() > min_samples);

      if (include_curve)
        curves.push_back(p);
    }

    // The swap trick reduces the excess memory used by curves
    std::vector< vsol_polyline_2d_sptr >(curves).swap(curves);
    std::cout << "Curves: #curves =  " << curves.size() << std::endl;

    allCurves[v] = curves;

    // 4 Edge dt computation

    vil_image_view<vxl_byte > bw_image;

    //: Assumes the conversion maps edges to 255 and others to 0.
    retval = sdetd_convert_edgemap_to_image(*edge_map, bw_image);
    if (!retval) return 1;

    vil_image_view<vxl_uint_32> dt(bw_image.ni(), bw_image.nj(), 1);

    for (unsigned i=0; i<dt.ni(); i++)
      for (unsigned j=0; j<dt.nj(); j++)
        dt(i,j) = static_cast<vxl_uint_32>(bw_image(i,j)<127);

    vil_image_view<unsigned> imlabel(dt.ni(), dt.nj(), 1);

    retval = bild_exact_distance_transform_maurer_label(dt, imlabel);
    if (!retval) return 1;

    allDTs[v] = dt;
    allLabels[v] = imlabel;

    // 5 Tangent computation
    
    std::cout << "Started tgt computation.\n";
    //std::cout << "Curve size: " << curves.size()<< "\n";
    std::vector<std::vector<double> > tangents(curves.size());

    for (unsigned c=0; c < curves.size(); ++c) {
      sdet_edgel_chain ec;
      bmcsd_algo_util::extract_edgel_chain(*curves[c], &ec);

      tangents[c].resize(ec.edgels.size());
      for (unsigned i=0; i < ec.edgels.size(); ++i) {
        tangents[c][i] = ec.edgels[i]->tangent;
        delete ec.edgels[i];
      }
    }

    allTangents[v] = tangents;

  }

  //Anil: Populate the view data
  //i) Input read from file, first anchor instance
  all_frames_to_match.push_back(seed_frames_to_match);

  /*
  std::vector<unsigned> firstAnchorOrder;
  std::vector<bool> usedViews(numConf+2);
  firstAnchorOrder.push_back(seedViews->stereo0());
  usedViews[seedViews->stereo0()] = true;

  unsigned nextView = seedViews->stereo1();

  while(nextView+3 < numConf+2){
    bmcsd_stereo_instance_views frames;
    bmcsd_stereo_views_sptr instance = new bmcsd_stereo_views();
    instance->set_stereo0(nextView);

    firstAnchorOrder.push_back(nextView);
    usedViews[nextView] = true;

    instance->set_stereo1(nextView+3);
    instance->reserve_num_confirmation_views(numConf);

    for (unsigned cv=0; cv<numConf+2; ++cv)
      if ((cv != nextView) && (cv != nextView+3))
	instance->add_confirmation_view(cv);

    frames.add_instance(instance);
    all_frames_to_match.push_back(frames);
    nextView+=3;
  }

  for (unsigned vo=0; vo<numConf+2; ++vo)
    if (!usedViews[vo])
    firstAnchorOrder.push_back(vo);*/

  //ii) Second anchor instance
  bmcsd_stereo_instance_views frames;
  bmcsd_stereo_views_sptr instance = new bmcsd_stereo_views();
  instance->set_stereo0(seedViews->stereo1());
  instance->set_stereo1(seedViews->confirmation_view(3));
  instance->reserve_num_confirmation_views(numConf);

  for (unsigned cv=0; cv<numConf; ++cv)
    if (cv != 3)
      instance->add_confirmation_view(seedViews->confirmation_view(cv));

  instance->add_confirmation_view(seedViews->stereo0());
  frames.add_instance(instance);
  all_frames_to_match.push_back(frames);

  //iii) Instances of all but 2 of the confirmation views
  for (unsigned v=0; v<numConf-2; ++v) {
    unsigned curView = seedViews->confirmation_view(v);
    bmcsd_stereo_instance_views curFrames;
    bmcsd_stereo_views_sptr curInstance = new bmcsd_stereo_views();
    curInstance->set_stereo0(curView);
    curInstance->set_stereo1(seedViews->confirmation_view(v+2));
    curInstance->reserve_num_confirmation_views(numConf);

    for (unsigned cv=0; cv<numConf; ++cv)
      if ((cv != v) && (cv != (v+2)))
	curInstance->add_confirmation_view(seedViews->confirmation_view(cv));

    curInstance->add_confirmation_view(seedViews->stereo0());
    curInstance->add_confirmation_view(seedViews->stereo1());
    curFrames.add_instance(curInstance);
    all_frames_to_match.push_back(curFrames);
  }

  //iv) Instances for the last 2 confirmation views
  {
    unsigned view1 = seedViews->confirmation_view(numConf-2);
    unsigned view2 = seedViews->stereo0();
    bmcsd_stereo_instance_views curFrames;
    bmcsd_stereo_views_sptr curInstance = new bmcsd_stereo_views();
    curInstance->set_stereo0(view1);
    curInstance->set_stereo1(view2);
    curInstance->reserve_num_confirmation_views(numConf);

    for (unsigned cv=0; cv<numConf; ++cv)
      if (cv != (numConf-2))
	curInstance->add_confirmation_view(seedViews->confirmation_view(cv));

    curInstance->add_confirmation_view(seedViews->stereo1());
    curFrames.add_instance(curInstance);
    all_frames_to_match.push_back(curFrames);
  }

  {
    unsigned view1 = seedViews->confirmation_view(numConf-1);
    unsigned view2 = seedViews->stereo1();
    bmcsd_stereo_instance_views curFrames;
    bmcsd_stereo_views_sptr curInstance = new bmcsd_stereo_views();
    curInstance->set_stereo0(view1);
    curInstance->set_stereo1(view2);
    curInstance->reserve_num_confirmation_views(numConf);

    for (unsigned cv=0; cv<numConf; ++cv)
      if (cv != numConf-1)
	curInstance->add_confirmation_view(seedViews->confirmation_view(cv));

    curInstance->add_confirmation_view(seedViews->stereo0());
    curFrames.add_instance(curInstance);
    all_frames_to_match.push_back(curFrames);
  }

  bool startRun = true;
  unsigned numInstances = all_frames_to_match.size();
  std::cout << numInstances << " starting instances will be used." << std::endl;
  //std::cout << "A total of " << firstAnchorOrder.size() << " instances will be used." << std::endl;

  //Anil: Data structure to store the marked portions of each image curve in each view
  std::vector<std::vector<std::vector<bool> > > usedCurvesAll(numConf+2);

  for (unsigned v=0; v<numConf+2; ++v)
    usedCurvesAll[v].resize(200000);



  //Anil: Create buffer storage for all the cam, edge and curve data
  //The order of data in these containers will be changed to suit each stereo run
  std::vector<bdifd_camera> curCams(numConf+2);
  std::vector<sdet_edgemap_sptr> curEdges(numConf+2);
  std::vector<vil_image_view<vxl_uint_32> > curDTs(numConf+2);
  std::vector<vil_image_view<unsigned> > curLabels(numConf+2);
  std::vector<std::vector<vsol_polyline_2d_sptr> > curCurves(numConf+2);
  std::vector<std::vector<std::vector<double> > > curTangents(numConf+2);

  //Anil: The cumulative curves after the initial subsampling run need to be stored.
  std::vector<std::vector<std::vector<std::vector<bdifd_1st_order_point_3d> > > > cumulativeCurveBox(numConf+2);

  long file_io_time = file_io.real();
  std::cout << "#1 FILE IO: " << file_io_time << std::endl;

  numInstances = 5;
  std::vector<std::map<unsigned,std::pair<unsigned,unsigned> > > image_to_3d_links(numConf+2);

  for (unsigned ins=0; ins<numInstances; ++ins) {

    //Anil: Containers for the reduced 3d curves
    std::vector<bdifd_1st_order_curve_3d> reducedCurves;
    std::vector<bmcsd_curve_3d_attributes_e> reducedAttr;
    unsigned reducedCurveID = 0;

    //STEP #1: Run 2-view stereo with confirmation views

    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-------------------------------(RUNNING INSTANCE: " << ins << ")-------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;
    std::cout << "-----------------------------------------------------------------------------------------------------------------------" << std::endl;


    bmcsd_stereo_instance_views frames_to_match = all_frames_to_match[ins];
    bmcsd_stereo_views_sptr fa_views = frames_to_match.instance(0);

    std::cout << "FIRST ANCHOR: " << fa_views->stereo0() << std::endl;
    std::cout << "SECOND ANCHOR: " << fa_views->stereo1() << std::endl;
    std::cout << "CONFIRMATION VIEWS: ";

    for (unsigned pr=0; pr<numConf; ++pr)
      std::cout << fa_views->confirmation_view(pr) << " ";

    std::cout << std::endl;

    bmcsd_concurrent_stereo_driver_e s(dpath, frames_to_match);
    
    curCams[0] = allCams[fa_views->stereo0()];
    curCurves[0] = allCurves[fa_views->stereo0()];
    curTangents[0] = allTangents[fa_views->stereo0()];

    curCams[1] = allCams[fa_views->stereo1()];
    curCurves[1] = allCurves[fa_views->stereo1()];
    curTangents[1] = allTangents[fa_views->stereo1()];

    for (unsigned vv=0; vv<numConf; ++vv) {
      curCams[vv+2] = allCams[fa_views->confirmation_view(vv)];
      curEdges[vv+2] = allEdges[fa_views->confirmation_view(vv)];
      curDTs[vv+2] = allDTs[fa_views->confirmation_view(vv)];
      curLabels[vv+2] = allLabels[fa_views->confirmation_view(vv)];
    }

  s.set_dtheta_threshold(a_dtheta_threshold());
  s.set_distance_threshold(a_distance_threshold());

  if (a_prune_by_length())
    s.set_min_length_per_curve_frag(a_min_length_per_curve_frag());
  else
    s.set_min_samples_per_curve_frag(a_min_samples_per_curve_frag());
  s.set_min_inliers_per_view(a_min_inliers_per_view()) ;
  s.set_min_epiangle(a_min_epiangle()) ;
  s.set_min_epipolar_overlap(a_min_epipolar_overlap()) ;
  s.set_min_total_inliers(a_min_total_inliers()) ;
  s.set_min_first_to_second_best_ratio(a_min_first_to_second_ratio()) ;
  s.set_lonely_threshold(a_lonely_threshold()) ;
  s.set_use_curvelets(a_use_curvelets());
  s.set_min_num_inlier_edgels_per_curvelet(a_min_num_inliers_per_curvelet());
  s.set_isFirstRun(true);


  std::vector<std::vector<unsigned> > fa_usedCurveIDs(2);
  if (!startRun) {
    for (unsigned imc=0; imc<usedCurvesAll[fa_views->stereo0()].size(); ++imc) {
	    unsigned numUsed = 0;
	    for (unsigned s=0; s<usedCurvesAll[fa_views->stereo0()][imc].size(); ++s)
	      if (usedCurvesAll[fa_views->stereo0()][imc][s])
	        numUsed++;

      double ratio = double(numUsed)/double((usedCurvesAll[fa_views->stereo0()][imc].size()));
      if (ratio >= 0.75)
        fa_usedCurveIDs[0].push_back(imc);
    }

    for (unsigned imc=0; imc<usedCurvesAll[fa_views->stereo1()].size(); ++imc) {
      unsigned numUsed = 0;
      for (unsigned s=0; s<usedCurvesAll[fa_views->stereo1()][imc].size(); ++s)
        if (usedCurvesAll[fa_views->stereo1()][imc][s])
          numUsed++;

      double ratio = double(numUsed)/double((usedCurvesAll[fa_views->stereo1()][imc].size()));
      if (ratio >= 0.75)
        fa_usedCurveIDs[1].push_back(imc);
    }
  }

  s.set_usedCurves(fa_usedCurveIDs);

  //: How many stereo matchers can run simultaneously
  s.set_max_concurrent_matchers(a_max_concurrent_matchers());

  retval = s.init(curCams,
      curEdges,
      curCurves,
      curTangents,
      curDTs,
      curLabels);
  MY_ASSERT("Stereo driver init return value", retval, true);

  //: Run many pairwise stereo programs, as many as
  // frames_to_match.num_instances();
  retval = s.run();
  MY_ASSERT("Stereo driver run return value", retval, true);


  bmcsd_curve_3d_sketch_e csk;

  //Gabriel: Get the curve sketch
  //
  s.get_curve_sketch(&csk);
  //STEP #2: Process the 3D curves to remove the segments that did not gather sufficient edge support
  //A segment is removed only if its size is 3 samples or more
  //A curve segment is created only if its size is 8 samples or more
  std::vector<bdifd_1st_order_curve_3d> fullCurves = csk.curves_3d();


  std::vector<bdifd_1st_order_curve_3d> supportedSegments;
  std::vector<bmcsd_curve_3d_attributes_e> supportedAttr;

  std::ofstream curve_links("curve_links.txt");

  //Anil: Attributes contain mate curve information, get it from the curve sketch data structure 
  const std::vector<bmcsd_curve_3d_attributes_e> attrVec = csk.attributes();
  unsigned seedCurveSize = attrVec.front().origCurveSize_;


    //Anil: Data structure for stitching 3D curves together at their corresponding samples
    //1st index is the image curve ID, 2nd index is for different 3D curves and 3rd index is for sample IDs 
    std::vector<std::vector<std::vector<bdifd_1st_order_point_3d> > > cumulativeCurve(200000);
    std::vector<std::vector<std::vector<bdifd_1st_order_point_3d> > > dmy_cumulativeCurve(200000);
    std::vector<std::vector<std::vector<std::set<int> > > > cumulativeEdgeIndexChain(200000);

    //if (usedViews[fa_views->stereo0()])
    //  cumulativeCurve = cumulativeCurveBox[fa_views->stereo0()];

    unsigned numCurves = attrVec.size();
    bmcsd_curve_3d_attributes_e seedAttr;
    if (numCurves>0)
      seedAttr = attrVec.front();
    for (unsigned c=0; c<fullCurves.size(); ++c)
      {
	bdifd_1st_order_curve_3d curCurve = fullCurves[c];
	bmcsd_curve_3d_attributes_e curAttr = attrVec[c];
	std::vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	unsigned offset = curAttr.imageCurveOffset_;
	unsigned offset_v1 = curAttr.imageCurveOffset_v1_;
	unsigned origID = curAttr.orig_id_v0_;
	unsigned origID_v1 = curAttr.orig_id_v1_;
	unsigned origCurveSize = curAttr.origCurveSize_;
	unsigned origCurveSize_v1 = curAttr.origCurveSize_v1_;
	std::vector<unsigned> usedSamples_v1 = curAttr.used_samples_v1_;
	std::vector<bool> certaintyFlags = curAttr.certaintyFlags_;
	unsigned v0_seed = fa_views->stereo0();
	unsigned v1_seed = fa_views->stereo1();
	
	std::vector<std::vector<int> > edge_index_chain = curAttr.edge_index_chain_;
	
	if (cumulativeCurve[origID].empty()){
	  cumulativeCurve[origID].resize(origCurveSize);
	  cumulativeEdgeIndexChain[origID].resize(numConf+2);
	  for (unsigned ceic=0; ceic<numConf+2; ceic++)
	    cumulativeEdgeIndexChain[origID][ceic].resize(origCurveSize);
	}

	unsigned breakLength = 0;
	unsigned initPoint = 0;
	int breakPoint = -1;
	
	for (unsigned s=0; s<curCurve.size(); ++s)
	  if (!certaintyFlags[s])
	    curSupp[s] = 0;
    
	for (unsigned s=0; s<curCurve.size(); ++s)
	  {
	    if (curSupp[s]<6)
	      {
		if (breakPoint==-1)
		  breakPoint = s;
		breakLength++;
	      }
	    else
	      {
		if (breakLength>2)
		  {
		    if (breakPoint>initPoint+7)
		      {
			bdifd_1st_order_curve_3d newCurve;

			for (int p=initPoint; p<breakPoint; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

			  for (unsigned vp=0; vp<numConf; vp++)
			    if (!edge_index_chain[vp].empty())
			      if (edge_index_chain[vp][p]!=-1)
				cumulativeEdgeIndexChain[origID][fa_views->confirmation_view(vp)][p+offset].insert(edge_index_chain[vp][p]);

			  if (usedCurvesAll[v0_seed][origID].empty())
			    usedCurvesAll[v0_seed][origID].resize(origCurveSize);

			  if (usedCurvesAll[v1_seed][origID_v1].empty())
			    usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

			  usedCurvesAll[v0_seed][origID][p+offset]=true;
			  usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
			}
			supportedSegments.push_back(newCurve);
			supportedAttr.push_back(curAttr);
			curve_links << origID << " " << offset+initPoint << std::endl;
		      }
		    initPoint = s;
		  }
		breakLength = 0;
		breakPoint = -1;
	      }

	    if (s==(curCurve.size()-1))
	      {
		if (curSupp[s]<6)
		  {
		    if (breakPoint>initPoint+7)
		      {
			bdifd_1st_order_curve_3d newCurve;
			for (int p=initPoint; p<breakPoint; ++p){

			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

			  for (unsigned vp=0; vp<numConf; vp++)
			    if (!edge_index_chain[vp].empty())
			      if (edge_index_chain[vp][p]!=-1)
				cumulativeEdgeIndexChain[origID][fa_views->confirmation_view(vp)][p+offset].insert(edge_index_chain[vp][p]);

			  if (usedCurvesAll[v0_seed][origID].empty())
			    usedCurvesAll[v0_seed][origID].resize(origCurveSize);

			  if (usedCurvesAll[v1_seed][origID_v1].empty())
			    usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

			  usedCurvesAll[v0_seed][origID][p+offset]=true;
			  usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
			}
			supportedSegments.push_back(newCurve);
			supportedAttr.push_back(curAttr);
			curve_links << origID << " " << offset+initPoint << std::endl;
		      }
		  }
		else
		  {
		    if (s>initPoint+6)
		      {
			bdifd_1st_order_curve_3d newCurve;
			for (int p=initPoint; p<s+1; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

			  for (unsigned vp=0; vp<numConf; vp++)
			    if (!edge_index_chain[vp].empty())
			      if (edge_index_chain[vp][p]!=-1)
				cumulativeEdgeIndexChain[origID][fa_views->confirmation_view(vp)][p+offset].insert(edge_index_chain[vp][p]);

			  if (usedCurvesAll[v0_seed][origID].empty())
			    usedCurvesAll[v0_seed][origID].resize(origCurveSize);

			  if (usedCurvesAll[v1_seed][origID_v1].empty())
			    usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

			  usedCurvesAll[v0_seed][origID][p+offset]=true;
			  usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
			}
			supportedSegments.push_back(newCurve);
			supportedAttr.push_back(curAttr);
			curve_links << origID << " " << offset+initPoint << std::endl;
		      }
		  }
	      }
	  }
      }

    
    //Anil: Container for all the mate curves
    //First index is views and second index is the image curves in v0()
    std::vector<std::vector<std::set<int> > > cumulativeMates;
    cumulativeMates.resize(numConf);


    /*//STEP #3: Loop over all the confirmation views to gather all mate curves together
    for (unsigned v=0; v<numConf; ++v)
      {
	cumulativeMates[v].resize(2000);
	unsigned curView = fa_views->confirmation_view(v);

	for (unsigned c=0; c<numCurves; ++c){
	  bmcsd_curve_3d_attributes_e curAttr = attrVec[c];
	  unsigned origID = curAttr.orig_id_v0_;
	  std::set<int> curMates = curAttr.mate_curves_[v];

	  for (std::set<int>::iterator mit=curMates.begin(); mit!=curMates.end(); ++mit)
	    cumulativeMates[v][origID].insert(*mit);
	}
      } 

    //STEP #4: Loop over all the confirmation views to process the cumulative mate curves in each one
    for (unsigned v=0; v<numConf; ++v)
      {
	bmcsd_curve_3d_sketch csk_elong;
	unsigned curView = fa_views->confirmation_view(v);
	std::cout << "LOOKING FOR CUES IN VIEW: " << curView << std::endl;
	bmcsd_stereo_instance_views curFrames;
	bmcsd_stereo_views_sptr curInstance = new bmcsd_stereo_views();
	curInstance->set_stereo0(fa_views->stereo0());
	curInstance->set_stereo1(curView);
	curInstance->reserve_num_confirmation_views(numConf);

	for (unsigned cv=0; cv<numConf; ++cv)
	  if (cv != v)
	    curInstance->add_confirmation_view(fa_views->confirmation_view(cv));

	curInstance->add_confirmation_view(fa_views->stereo1());
	curFrames.add_instance(curInstance);

	bmcsd_concurrent_stereo_driver cur_s(dpath, curFrames);

	cur_s.set_dtheta_threshold(a_dtheta_threshold());
	cur_s.set_distance_threshold(a_distance_threshold());

	if (a_prune_by_length())
	  cur_s.set_min_length_per_curve_frag(a_min_length_per_curve_frag());
	else
	  cur_s.set_min_samples_per_curve_frag(a_min_samples_per_curve_frag());
	cur_s.set_min_inliers_per_view(a_min_inliers_per_view()) ;
	cur_s.set_min_epiangle(a_min_epiangle()) ;
	cur_s.set_min_epipolar_overlap(a_min_epipolar_overlap()) ;
	cur_s.set_min_total_inliers(a_min_total_inliers()) ;
	cur_s.set_min_first_to_second_best_ratio(a_min_first_to_second_ratio()) ;
	cur_s.set_lonely_threshold(a_lonely_threshold()) ;
	cur_s.set_use_curvelets(a_use_curvelets());
	cur_s.set_min_num_inlier_edgels_per_curvelet(a_min_num_inliers_per_curvelet());

	//: How many stereo matchers can run simultaneously
	cur_s.set_max_concurrent_matchers(a_max_concurrent_matchers());
	cur_s.set_mate_curves_v1(cumulativeMates[v]);
	cur_s.set_isFirstRun(false);    

	std::vector<std::vector<unsigned> > cur_usedCurveIDs(2);
	if (!startRun) {
      
	  for (unsigned imc=0; imc<usedCurvesAll[fa_views->stereo0()].size(); ++imc){
	    unsigned numUsed = 0;
	    for (unsigned s=0; s<usedCurvesAll[fa_views->stereo0()][imc].size(); ++s)
	      if (usedCurvesAll[fa_views->stereo0()][imc][s])
		numUsed++;

	    double ratio = double(numUsed)/double((usedCurvesAll[fa_views->stereo0()][imc].size()));
	    if (ratio >= 0.75)
	      cur_usedCurveIDs[0].push_back(imc);

	  }

	  for (unsigned imc=0; imc<usedCurvesAll[curView].size(); ++imc){
    
	    unsigned numUsed = 0;
	    for (unsigned s=0; s<usedCurvesAll[curView][imc].size(); ++s)
	      if (usedCurvesAll[curView][imc][s])
		numUsed++;

	    double ratio = double(numUsed)/double((usedCurvesAll[curView][imc].size()));
	    if (ratio >= 0.75)
	      cur_usedCurveIDs[1].push_back(imc);

	  }
	}

	cur_s.set_usedCurves(cur_usedCurveIDs);

	retval = cur_s.init();
	MW_ASSERT("Stereo driver init return value", retval, true);

	//: Run many pairwise stereo programs, as many as
	// frames_to_match.num_instances();
	retval = cur_s.run();
	MW_ASSERT("Stereo driver run return value", retval, true);

	cur_s.get_curve_sketch(&csk_elong);

	//Anil: Same as before - Process the 3D curves to remove the segments that did not gather sufficient edge support
	//A segment is removed only if its size is 3 samples or more
	//A curve segment is created only if its size is 8 samples or more
	std::vector<dbdif_1st_order_curve_3d> cur_fullCurves = csk_elong.curves_3d();
 
	std::vector<dbdif_1st_order_curve_3d> cur_supportedSegments;
	std::vector<bmcsd_curve_3d_attributes_e> cur_supportedAttr;

	const std::vector<bmcsd_curve_3d_attributes_e> cur_attrVec = csk_elong.attributes();

	for (unsigned c=0; c<cur_fullCurves.size(); ++c)
	  {
	    dbdif_1st_order_curve_3d curCurve = cur_fullCurves[c];
	    bmcsd_curve_3d_attributes_e_e curAttr = cur_attrVec[c];
	    std::vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	    unsigned cur_offset = curAttr.imageCurveOffset_;
	    unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	    unsigned cur_origID = curAttr.orig_id_v0_;
	    unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	    unsigned cur_origCurveSize = curAttr.origCurveSize_;
	    unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	    std::vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	    std::vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	    unsigned cur_v0_seed = fa_views->stereo0();
	    unsigned cur_v1_seed = curView;

	    unsigned breakLength = 0;
	    unsigned initPoint = 0;
	    int breakPoint = -1;
	
	    for (unsigned s=0; s<curCurve.size(); ++s)
	      if (!cur_certaintyFlags[s])
		curSupp[s] = 0;

	    for (unsigned s=0; s<curCurve.size(); ++s)
	      {
		if (curSupp[s]<6)
		  {
		    if (breakPoint==-1)
		      breakPoint = s;
		    breakLength++;
		  }
		else
		  {
		    if (breakLength>2)
		      {
			if (breakPoint>initPoint+7)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for (int p=initPoint; p<breakPoint; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			      usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			      usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			    }
			    supportedSegments.push_back(newCurve);
			    supportedAttr.push_back(curAttr);
			  }
			initPoint = s;
		      }
		    breakLength = 0;
		    breakPoint = -1;
		  }

		if (s==(curCurve.size()-1))
		  {
		    if (curSupp[s]<6)
		      {
			if (breakPoint>initPoint+7)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for (int p=initPoint; p<breakPoint; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			      usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			      usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;
			    }
			    supportedSegments.push_back(newCurve);
			    supportedAttr.push_back(curAttr);
			  }
		      }
		    else
		      {
			if (s>initPoint+6)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for (int p=initPoint; p<s+1; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			      usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			      usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			    }
			    supportedSegments.push_back(newCurve);
			    supportedAttr.push_back(curAttr);
			  }
		      }
		  }
	      }
	  }

      }

    /*bmcsd_curve_3d_sketch csk_supported(supportedSegments,supportedAttr);
    //: Write 3D curves and attributes to file.
    retval = csk_supported.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
    MW_ASSERT("Error while trying to write file.\n", retval, true);*/

    /*static const int arr[] = {-2, 4, -4, 1, -1, 3, -3, 5, -5};   
    std::vector<int> modifVec(arr, arr + sizeof(arr)/sizeof(arr[0]));
    std::vector<unsigned> visitationSchedule;
    std::vector<bool> cur_usedViews(numConf+2);
    cur_usedViews[fa_views->stereo0()] = true;
    cur_usedViews[fa_views->stereo1()] = true;

    for (unsigned vs=0; vs<modifVec.size(); ++vs) {
      if (usedViews[fa_views->stereo0()] && modifVec[vs]==3)
	continue;
      int curVisit = static_cast<int>(fa_views->stereo0()) + modifVec[vs];
      if (curVisit<0)
	curVisit = curVisit + numConf + 2;
      else if (curVisit >= numConf+2)
	curVisit = curVisit - (numConf + 2);
	
      visitationSchedule.push_back(static_cast<unsigned>(curVisit));
      cur_usedViews[static_cast<unsigned>(curVisit)] = true;
    }

    for (unsigned vs=0; vs<numConf+2; ++vs)
      if (!cur_usedViews[vs])
      visitationSchedule.push_back(vs);*/

    //STEP #5: Make each confirmation view the second anchor to try and reconstruct unused curves
    //for (unsigned vis=0; vis<visitationSchedule.size(); ++vis) {
    for (unsigned vi=0; vi<numConf; ++vi) {
      
      //unsigned curView = visitationSchedule[vis];
      bmcsd_curve_3d_sketch_e csk_iterate;
      unsigned curView = fa_views->confirmation_view(vi);
      std::cout << "FIRST ANCHOR: " << fa_views->stereo0() << std::endl;
      std::cout << "SWITCHING SECOND ANCHOR TO VIEW: " << curView << std::endl;
      std::cout << "CONFIRMATION VIEWS: ";

      bmcsd_stereo_instance_views curFrames;
      bmcsd_stereo_views_sptr curInstance = new bmcsd_stereo_views();
      curInstance->set_stereo0(fa_views->stereo0());
      curInstance->set_stereo1(curView);
      curInstance->reserve_num_confirmation_views(numConf);

      for (unsigned cv=0; cv<numConf; ++cv){
	if (fa_views->confirmation_view(cv) != curView){
	//if (fa_views->confirmation_view(cv) != vi)
	  curInstance->add_confirmation_view(fa_views->confirmation_view(cv));
	  std::cout << fa_views->confirmation_view(cv) << " ";
	}
      }

      curInstance->add_confirmation_view(fa_views->stereo1());
      curFrames.add_instance(curInstance);
      std::cout << fa_views->stereo1() << std::endl;

      curCams.clear();
      curEdges.clear();
      curDTs.clear();
      curLabels.clear();
      curCurves.clear();
      curTangents.clear();

      curCams.resize(numConf+2);
      curEdges.resize(numConf+2);
      curDTs.resize(numConf+2);
      curLabels.resize(numConf+2);
      curCurves.resize(numConf+2);
      curTangents.resize(numConf+2);

      curCams[0] = allCams[curInstance->stereo0()];
      curCurves[0] = allCurves[curInstance->stereo0()];
      curTangents[0] = allTangents[curInstance->stereo0()];

      curCams[1] = allCams[curInstance->stereo1()];
      curCurves[1] = allCurves[curInstance->stereo1()];
      curTangents[1] = allTangents[curInstance->stereo1()];

      for (unsigned vv=0; vv<numConf; ++vv) {
	curCams[vv+2] = allCams[curInstance->confirmation_view(vv)];
	curEdges[vv+2] = allEdges[curInstance->confirmation_view(vv)];
	curDTs[vv+2] = allDTs[curInstance->confirmation_view(vv)];
	curLabels[vv+2] = allLabels[curInstance->confirmation_view(vv)];
      }

      std::vector<std::vector<unsigned> > usedCurveIDs(2);
      /*for (unsigned imc=0; imc<usedCurvesAll[fa_views->stereo0()].size(); ++imc){
	unsigned numUsed = 0;
	for (unsigned s=0; s<usedCurvesAll[fa_views->stereo0()][imc].size(); ++s)
	  if (usedCurvesAll[fa_views->stereo0()][imc][s])
	    numUsed++;

	double ratio = double(numUsed)/double((usedCurvesAll[fa_views->stereo0()][imc].size()));
	if (ratio >= 0.75)
	  usedCurveIDs[0].push_back(imc);

      }

      for (unsigned imc=0; imc<usedCurvesAll[curView].size(); ++imc){
    
	unsigned numUsed = 0;
	for (unsigned s=0; s<usedCurvesAll[curView][imc].size(); ++s)
	  if (usedCurvesAll[curView][imc][s])
	    numUsed++;

	double ratio = double(numUsed)/double((usedCurvesAll[curView][imc].size()));
	if (ratio >= 0.75)
	  usedCurveIDs[1].push_back(imc);

      }*/

      bmcsd_concurrent_stereo_driver_e cur_s(dpath, curFrames);

      cur_s.set_dtheta_threshold(a_dtheta_threshold());
      cur_s.set_distance_threshold(a_distance_threshold());

      if (a_prune_by_length())
	cur_s.set_min_length_per_curve_frag(a_min_length_per_curve_frag());
      else
	cur_s.set_min_samples_per_curve_frag(a_min_samples_per_curve_frag());
      cur_s.set_min_inliers_per_view(a_min_inliers_per_view()) ;
      cur_s.set_min_epiangle(a_min_epiangle()) ;
      cur_s.set_min_epipolar_overlap(a_min_epipolar_overlap()) ;
      cur_s.set_min_total_inliers(a_min_total_inliers()) ;
      cur_s.set_min_first_to_second_best_ratio(a_min_first_to_second_ratio()) ;
      cur_s.set_lonely_threshold(a_lonely_threshold()) ;
      cur_s.set_use_curvelets(a_use_curvelets());
      cur_s.set_min_num_inlier_edgels_per_curvelet(a_min_num_inliers_per_curvelet());

      //: How many stereo matchers can run simultaneously
      cur_s.set_max_concurrent_matchers(a_max_concurrent_matchers());
      cur_s.set_isFirstRun(true);   
      cur_s.set_usedCurves(fa_usedCurveIDs);

      retval = cur_s.init(curCams,
		      curEdges,
		      curCurves,
		      curTangents,
		      curDTs,
		      curLabels);

      MY_ASSERT("Stereo driver init return value", retval, true);

      //: Run many pairwise stereo programs, as many as
      // frames_to_match.num_instances();
      retval = cur_s.run();
      MY_ASSERT("Stereo driver run return value", retval, true);

      cur_s.get_curve_sketch(&csk_iterate);

      //Anil: Same as before - Process the 3D curves to remove the segments that did not gather sufficient edge support
      //A segment is removed only if its size is 3 samples or more
      //A curve segment is created only if its size is 8 samples or more
      std::vector<bdifd_1st_order_curve_3d> cur_fullCurves = csk_iterate.curves_3d();
      unsigned cur_numCurves = cur_fullCurves.size();
 
      std::vector<bdifd_1st_order_curve_3d> cur_supportedSegments;
      std::vector<bmcsd_curve_3d_attributes_e> cur_supportedAttr;

      const std::vector<bmcsd_curve_3d_attributes_e> cur_attrVec = csk_iterate.attributes();

      for (unsigned c=0; c<cur_fullCurves.size(); ++c)
	{
	  bdifd_1st_order_curve_3d curCurve = cur_fullCurves[c];
	  bmcsd_curve_3d_attributes_e curAttr = cur_attrVec[c];
	  std::vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	  unsigned cur_offset = curAttr.imageCurveOffset_;
	  unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	  unsigned cur_origID = curAttr.orig_id_v0_;
	  unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	  unsigned cur_origCurveSize = curAttr.origCurveSize_;
	  unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	  std::vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	  std::vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	  unsigned cur_v0_seed = fa_views->stereo0();
	  unsigned cur_v1_seed = curView;
	  std::vector<std::vector<int> > cur_edge_index_chain = curAttr.edge_index_chain_;
    
	  if (cumulativeCurve[cur_origID].empty()){
	    cumulativeCurve[cur_origID].resize(cur_origCurveSize);
	    cumulativeEdgeIndexChain[cur_origID].resize(numConf+2);
	    for (unsigned ceic=0; ceic<numConf+2; ceic++)
	      cumulativeEdgeIndexChain[cur_origID][ceic].resize(cur_origCurveSize);
	  }

	  unsigned breakLength = 0;
	  unsigned initPoint = 0;
	  int breakPoint = -1;
	
	  for (unsigned s=0; s<curCurve.size(); ++s)
	    if (!cur_certaintyFlags[s])
	      curSupp[s] = 0;

	  for (unsigned s=0; s<curCurve.size(); ++s)
	    {
	      if (curSupp[s]<6)
		{
		  if (breakPoint==-1)
		    breakPoint = s;
		  breakLength++;
		}
	      else
		{
		  if (breakLength>2)
		    {
		      if (breakPoint>initPoint+7)
			{
			  bdifd_1st_order_curve_3d newCurve;
			  for (int p=initPoint; p<breakPoint; ++p){
			    newCurve.push_back(curCurve[p]);
			    cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			    for (unsigned vp=0; vp<numConf; vp++)
			      if (!cur_edge_index_chain[vp].empty())
				if (cur_edge_index_chain[vp][p]!=-1)
				  cumulativeEdgeIndexChain[cur_origID][curInstance->confirmation_view(vp)][p+cur_offset].insert(cur_edge_index_chain[vp][p]);

			    if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
			      usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			    if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
			      usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			    usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			    usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			  }
			  supportedSegments.push_back(newCurve);
			  supportedAttr.push_back(curAttr);
			  curve_links << cur_origID << " " << cur_offset+initPoint << std::endl;
			}
		      initPoint = s;
		    }
		  breakLength = 0;
		  breakPoint = -1;
		}

	      if (s==(curCurve.size()-1))
		{
		  if (curSupp[s]<6)
		    {
		      if (breakPoint>initPoint+7)
			{
			  bdifd_1st_order_curve_3d newCurve;
			  for (int p=initPoint; p<breakPoint; ++p){
			    newCurve.push_back(curCurve[p]);
			    cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			    for (unsigned vp=0; vp<numConf; vp++)
			      if (!cur_edge_index_chain[vp].empty())
				if (cur_edge_index_chain[vp][p]!=-1)
				  cumulativeEdgeIndexChain[cur_origID][curInstance->confirmation_view(vp)][p+cur_offset].insert(cur_edge_index_chain[vp][p]);			    

			    if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
			      usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			    if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
			      usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			    usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			    usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;
			  }
			  supportedSegments.push_back(newCurve);
			  supportedAttr.push_back(curAttr);
			  curve_links << cur_origID << " " << cur_offset+initPoint << std::endl;
			}
		    }
		  else
		    {
		      if (s>initPoint+6)
			{
			  bdifd_1st_order_curve_3d newCurve;
			  for (int p=initPoint; p<s+1; ++p){
			    newCurve.push_back(curCurve[p]);
			    cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			    for (unsigned vp=0; vp<numConf; vp++)
			      if (!cur_edge_index_chain[vp].empty())
				if (cur_edge_index_chain[vp][p]!=-1)
				  cumulativeEdgeIndexChain[cur_origID][curInstance->confirmation_view(vp)][p+cur_offset].insert(cur_edge_index_chain[vp][p]);
			    

			    if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
			      usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			    if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
			      usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

			    usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
			    usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			  }
			  supportedSegments.push_back(newCurve);
			  supportedAttr.push_back(curAttr);
			  curve_links << cur_origID << " " << cur_offset+initPoint << std::endl;
			}
		    }
		}
	    }
	}
    
      //Anil: Container for all the mate curves
      //First index is views and second index is the image curves in v0()
      std::vector<std::vector<std::set<int> > > cur_cumulativeMates;
      cur_cumulativeMates.resize(numConf);

      /*//Loop over all the confirmation views to gather all mate curves together
      for (unsigned v=0; v<numConf; ++v)
	{
	  cur_cumulativeMates[v].resize(2000);
	  //unsigned curView = curInstance->confirmation_view(v);

	  for (unsigned c=0; c<cur_numCurves; ++c){
	    bmcsd_curve_3d_attributes_e curAttr = cur_attrVec[c];
	    unsigned origID = curAttr.orig_id_v0_;
	    std::set<int> curMates = curAttr.mate_curves_[v];

	    for (std::set<int>::iterator mit=curMates.begin(); mit!=curMates.end(); ++mit)
	      cur_cumulativeMates[v][origID].insert(*mit);
	  }
	} 

      //Loop over all the confirmation views to process the cumulative mate curves in each one
      for (unsigned v=0; v<numConf; ++v)
	{
	  bmcsd_curve_3d_sketch csk_iter_elong;
	  unsigned iterView = curInstance->confirmation_view(v);
	  std::cout << "LOOKING FOR CUES IN VIEW: " << iterView << std::endl;
	  bmcsd_stereo_instance_views iterFrames;
	  bmcsd_stereo_views_sptr iterInstance = new bmcsd_stereo_views();
	  iterInstance->set_stereo0(curInstance->stereo0());
	  iterInstance->set_stereo1(iterView);
	  std::cout << curInstance->stereo0() << " " << iterView << std::endl;
	  iterInstance->reserve_num_confirmation_views(numConf);

	  for (unsigned cv=0; cv<numConf; ++cv)
	    if (cv != v){
	      iterInstance->add_confirmation_view(curInstance->confirmation_view(cv));
	      std::cout << curInstance->confirmation_view(cv) << " ";
	    }
	
	  iterInstance->add_confirmation_view(curInstance->stereo1());
	  std::cout << curInstance->stereo1() << std::endl;
	  iterFrames.add_instance(iterInstance);

	  bmcsd_concurrent_stereo_driver iter_s(dpath, iterFrames);

	  std::vector<std::vector<unsigned> > iter_usedCurveIDs(2);
	  for (unsigned imc=0; imc<usedCurvesAll[curInstance->stereo0()].size(); ++imc){
	    unsigned numUsed = 0;
	    for (unsigned s=0; s<usedCurvesAll[curInstance->stereo0()][imc].size(); ++s)
	      if (usedCurvesAll[curInstance->stereo0()][imc][s])
		numUsed++;

	    double ratio = double(numUsed)/double((usedCurvesAll[curInstance->stereo0()][imc].size()));
	    if (ratio >= 0.75)
	      iter_usedCurveIDs[0].push_back(imc);

	  }

	  for (unsigned imc=0; imc<usedCurvesAll[iterView].size(); ++imc){
    
	    unsigned numUsed = 0;
	    for (unsigned s=0; s<usedCurvesAll[iterView][imc].size(); ++s)
	      if (usedCurvesAll[iterView][imc][s])
		numUsed++;

	    double ratio = double(numUsed)/double((usedCurvesAll[iterView][imc].size()));
	    if (ratio >= 0.75)
	      iter_usedCurveIDs[1].push_back(imc);

	  }

	  iter_s.set_dtheta_threshold(a_dtheta_threshold());
	  iter_s.set_distance_threshold(a_distance_threshold());

	  if (a_prune_by_length())
	    iter_s.set_min_length_per_curve_frag(a_min_length_per_curve_frag());
	  else
	    iter_s.set_min_samples_per_curve_frag(a_min_samples_per_curve_frag());
	  iter_s.set_min_inliers_per_view(a_min_inliers_per_view()) ;
	  iter_s.set_min_epiangle(a_min_epiangle()) ;
	  iter_s.set_min_epipolar_overlap(a_min_epipolar_overlap()) ;
	  iter_s.set_min_total_inliers(a_min_total_inliers()) ;
	  iter_s.set_min_first_to_second_best_ratio(a_min_first_to_second_ratio()) ;
	  iter_s.set_lonely_threshold(a_lonely_threshold()) ;
	  iter_s.set_use_curvelets(a_use_curvelets());
	  iter_s.set_min_num_inlier_edgels_per_curvelet(a_min_num_inliers_per_curvelet());

	  //: How many stereo matchers can run simultaneously
	  iter_s.set_max_concurrent_matchers(a_max_concurrent_matchers());
	  iter_s.set_mate_curves_v1(cur_cumulativeMates[v]);
	  iter_s.set_isFirstRun(false);    
	  iter_s.set_usedCurves(iter_usedCurveIDs);

	  retval = iter_s.init();
	  MW_ASSERT("Stereo driver init return value", retval, true);

	  //: Run many pairwise stereo programs, as many as
	  // frames_to_match.num_instances();
	  retval = iter_s.run();
	  MW_ASSERT("Stereo driver run return value", retval, true);

	  iter_s.get_curve_sketch(&csk_iter_elong);

	  //Anil: Same as before - Process the 3D curves to remove the segments that did not gather sufficient edge support
	  //A segment is removed only if its size is 3 samples or more
	  //A curve segment is created only if its size is 8 samples or more
	  std::vector<dbdif_1st_order_curve_3d> iter_fullCurves = csk_iter_elong.curves_3d();
 
	  std::vector<dbdif_1st_order_curve_3d> iter_supportedSegments;
	  std::vector<bmcsd_curve_3d_attributes_e> iter_supportedAttr;

	  const std::vector<bmcsd_curve_3d_attributes_e> iter_attrVec = csk_iter_elong.attributes();

	  for (unsigned c=0; c<iter_fullCurves.size(); ++c)
	    {
	      dbdif_1st_order_curve_3d curCurve = iter_fullCurves[c];
	      bmcsd_curve_3d_attributes_e curAttr = iter_attrVec[c];
	      std::vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	      unsigned cur_offset = curAttr.imageCurveOffset_;
	      unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	      unsigned cur_origID = curAttr.orig_id_v0_;
	      unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	      unsigned cur_origCurveSize = curAttr.origCurveSize_;
	      unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	      std::vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	      std::vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	      unsigned cur_v0_seed = curInstance->stereo0();
	      unsigned cur_v1_seed = iterView;

	      unsigned breakLength = 0;
	      unsigned initPoint = 0;
	      int breakPoint = -1;
		
	      for (unsigned s=0; s<curCurve.size(); ++s)
		if (!cur_certaintyFlags[s])
		  curSupp[s] = 0;

	      for (unsigned s=0; s<curCurve.size(); ++s)
		{
		  if (curSupp[s]<6)
		    {
		      if (breakPoint==-1)
			breakPoint = s;
		      breakLength++;
		    }
		  else
		    {
		      if (breakLength>2)
			{
			  if (breakPoint>initPoint+7)
			    {
			      dbdif_1st_order_curve_3d newCurve;
			      for (int p=initPoint; p<breakPoint; ++p){
				newCurve.push_back(curCurve[p]);
				cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

				if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				  usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

				if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				  usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

				usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
				usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			      }
			      supportedSegments.push_back(newCurve);
			      supportedAttr.push_back(curAttr);
			    }
			  initPoint = s;
			}
		      breakLength = 0;
		      breakPoint = -1;
		    }

		  if (s==(curCurve.size()-1))
		    {
		      if (curSupp[s]<6)
			{
			  if (breakPoint>initPoint+7)
			    {
			      dbdif_1st_order_curve_3d newCurve;
			      for (int p=initPoint; p<breakPoint; ++p){
				newCurve.push_back(curCurve[p]);
				cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

				if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				  usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

				if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				  usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

				usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
				usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;
			      }
			      supportedSegments.push_back(newCurve);
			      supportedAttr.push_back(curAttr);
			    }
			}
		      else
			{
			  if (s>initPoint+6)
			    {
			      dbdif_1st_order_curve_3d newCurve;
			      for (int p=initPoint; p<s+1; ++p){
				newCurve.push_back(curCurve[p]);
				cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

				if (usedCurvesAll[cur_v0_seed][cur_origID].empty())
				  usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

				if (usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
				  usedCurvesAll[cur_v1_seed][cur_origID_v1].resize(cur_origCurveSize_v1);

				usedCurvesAll[cur_v0_seed][cur_origID][p+cur_offset]=true;
				usedCurvesAll[cur_v1_seed][cur_origID_v1][cur_usedSamples_v1[p]+cur_offset_v1]=true;

			      }
			      supportedSegments.push_back(newCurve);
			      supportedAttr.push_back(curAttr);
			    }
			}
		    }
		}
	    }

	}*/
      
    }

    std::cout << "FINISHED GATHERING!!!!" << std::endl;

    //STEP #6: Take the average of all the corresponding curve samples 
    //to reduce the strand to a single curve
    //process 1 image curve at a time

    std::stringstream attributes_stream;
    attributes_stream << "attributes_";
    attributes_stream << fa_views->stereo0();
    attributes_stream << ".txt";
    std::string attributes_fname = attributes_stream.str();
    std::ofstream attributes_file(attributes_fname.c_str());


    for (unsigned i=0; i<200000; ++i)
      {
	std::vector<std::vector<bdifd_1st_order_point_3d> > cur_cumulativeCurve = cumulativeCurve[i];
	std::vector<std::vector<std::vector<int> > > unionEdgeIndexChain(numConf+2);
	bdifd_1st_order_curve_3d averageCurve;
	
	
	if (!cur_cumulativeCurve.empty())
	  {
	    unsigned startPoint = 0;
	    for (unsigned s=0; s<cur_cumulativeCurve.size(); ++s)
	      {
		if (cur_cumulativeCurve[s].empty())
		  {
		    if (!averageCurve.empty())
		      {
			reducedCurves.push_back(averageCurve);
			unsigned avCurveSize = averageCurve.size();
			averageCurve.clear();

			attributes_file << i << " " << reducedCurveID << " " << startPoint << std::endl;
			attributes_file << avCurveSize << std::endl;

			

			for (unsigned sav=0; sav<avCurveSize; sav++){
			  unsigned num_non_empty=0;
			  std::vector<unsigned> writeVector;
			  for (unsigned vp=0; vp<numConf+2; vp++){
			    if (!unionEdgeIndexChain[vp][sav].empty()){
			      num_non_empty++;
			      writeVector.push_back(vp);
			      writeVector.push_back(unionEdgeIndexChain[vp][sav].size());
			      for (unsigned evp=0; evp<unionEdgeIndexChain[vp][sav].size(); evp++)
				writeVector.push_back(unionEdgeIndexChain[vp][sav][evp]);
			    }
			  }
			
			  if (!writeVector.empty()){
			    attributes_file << num_non_empty << " ";
			    for (unsigned wv=0; wv<writeVector.size(); wv++)
			      attributes_file << writeVector[wv] << " ";
			  }
			  else{
			    attributes_file << int(-1);
			  }


			  attributes_file << std::endl;
			}

			reducedCurveID++;
			unionEdgeIndexChain.clear();
			unionEdgeIndexChain.resize(numConf+2);

			//TODO: This is dummy attribute. Remove this and be able to write curve sketch without attributes
			reducedAttr.push_back(seedAttr);
		      }
		  }
		else
		  {
		    unsigned numPoints = cur_cumulativeCurve[s].size();
		    double xSum=0; 
		    double ySum=0;
		    double zSum=0;
		    double txSum=0; 
		    double tySum=0;
		    double tzSum=0;
		    for (unsigned p=0; p<numPoints; ++p)
		      {
			xSum+=cur_cumulativeCurve[s][p].Gama[0];
			ySum+=cur_cumulativeCurve[s][p].Gama[1];
			zSum+=cur_cumulativeCurve[s][p].Gama[2];

			txSum+=cur_cumulativeCurve[s][p].T[0];
			tySum+=cur_cumulativeCurve[s][p].T[1];
			tzSum+=cur_cumulativeCurve[s][p].T[2];
		      }
		    bdifd_vector_3d avGama,avT,finalAvGama,finalAvT;
		    avGama[0] = xSum/numPoints;
		    avGama[1] = ySum/numPoints;
		    avGama[2] = zSum/numPoints;

		    avT[0] = txSum/numPoints;
		    avT[1] = tySum/numPoints;
		    avT[2] = tzSum/numPoints;

		    xSum=0; 
		    ySum=0;
		    zSum=0;
		    txSum=0; 
		    tySum=0;
		    tzSum=0;
		    double numSamples=0;

		    //: Anil: Reject outlier samples - a sample is an outlier if its squared distance from average is more than 0.05
		    //: TODO: Reject entire curves when one sample is an outlier
		    for (unsigned p=0; p<numPoints; ++p)
		      {
			double x=cur_cumulativeCurve[s][p].Gama[0];
			double y=cur_cumulativeCurve[s][p].Gama[1];
			double z=cur_cumulativeCurve[s][p].Gama[2];

			double distSq = std::pow(x-avGama[0],2) + std::pow(y-avGama[1],2) + std::pow(z-avGama[2],2);
			if (distSq<0.05)
			  {
			    xSum+=x;
			    ySum+=y;
			    zSum+=z;

			    txSum+=cur_cumulativeCurve[s][p].T[0];
			    tySum+=cur_cumulativeCurve[s][p].T[1];
			    tzSum+=cur_cumulativeCurve[s][p].T[2];

			    numSamples++;
			  }
		      }

		    if (numSamples > 0){
		      finalAvGama[0] = xSum/numSamples;
		      finalAvGama[1] = ySum/numSamples;
		      finalAvGama[2] = zSum/numSamples;

		      finalAvT[0] = txSum/numSamples;
		      finalAvT[1] = tySum/numSamples;
		      finalAvT[2] = tzSum/numSamples;

		      bdifd_1st_order_point_3d avPoint;
		      avPoint.Gama = finalAvGama;
		      avPoint.T = finalAvT;

		      if (averageCurve.empty())
			startPoint = s;
			
		      averageCurve.push_back(avPoint);
		      for (unsigned vp=0; vp<numConf+2; vp++){
			std::vector<int> temp;
			std::set<int> curSet = cumulativeEdgeIndexChain[i][vp][s];
			for (std::set<int>::iterator sit=curSet.begin(); sit!=curSet.end(); sit++)
			  temp.push_back(*sit);
			unionEdgeIndexChain[vp].push_back(temp);
			
		      }

		    }
		    else if (!averageCurve.empty()){
		      std::cout << "WARNING: All measurements have been rejected for sample " << s << std::endl;
		      reducedCurves.push_back(averageCurve);
		      unsigned avCurveSize = averageCurve.size();
		      averageCurve.clear();

		      attributes_file << i << " " << reducedCurveID << " " << startPoint << std::endl;
		      attributes_file << avCurveSize << std::endl;

		     

		      for (unsigned sav=0; sav<avCurveSize; sav++){
			std::vector<unsigned> writeVector;
			unsigned num_non_empty=0;
			for (unsigned vp=0; vp<numConf+2; vp++){
			  if (!unionEdgeIndexChain[vp][sav].empty()){
			    num_non_empty++;
			    writeVector.push_back(vp);
			    writeVector.push_back(unionEdgeIndexChain[vp][sav].size());
			    for (unsigned evp=0; evp<unionEdgeIndexChain[vp][sav].size(); evp++)
			      writeVector.push_back(unionEdgeIndexChain[vp][sav][evp]);
			  }
			}
		      
			
			if (!writeVector.empty()){
			  attributes_file << num_non_empty << " ";
			  for (unsigned wv=0; wv<writeVector.size(); wv++)
			    attributes_file << writeVector[wv] << " ";
			}
			else{
			  attributes_file << int(-1);
			}

			attributes_file << std::endl;
		      }

		      reducedCurveID++;
		      unionEdgeIndexChain.clear();
		      unionEdgeIndexChain.resize(numConf+2);

		      //TODO: This is dummy attribute. Remove this and be able to write curve sketch without attributes
		      reducedAttr.push_back(seedAttr);
		    }

		  }  
	      }
	    if (!averageCurve.empty()){
	      reducedCurves.push_back(averageCurve);
	      unsigned avCurveSize = averageCurve.size();
	      averageCurve.clear();

	      attributes_file << i << " " << reducedCurveID << " " << startPoint << std::endl;
	      attributes_file << avCurveSize << std::endl;

	      for (unsigned sav=0; sav<avCurveSize; sav++){
		std::vector<unsigned> writeVector;
		unsigned num_non_empty=0;
		for (unsigned vp=0; vp<numConf+2; vp++){
		  if (!unionEdgeIndexChain[vp][sav].empty()){
		    num_non_empty++;
		    writeVector.push_back(vp);
		    writeVector.push_back(unionEdgeIndexChain[vp][sav].size());
		    for (unsigned evp=0; evp<unionEdgeIndexChain[vp][sav].size(); evp++)
		      writeVector.push_back(unionEdgeIndexChain[vp][sav][evp]);
		  }
		}
	      
			
		if (!writeVector.empty()){
		  attributes_file << num_non_empty << " ";
		  for (unsigned wv=0; wv<writeVector.size(); wv++)
		    attributes_file << writeVector[wv] << " ";
		}
		else{
		  attributes_file << int(-1);
		}

		attributes_file << std::endl;
	      }

	      reducedCurveID++;
	      unionEdgeIndexChain.clear();
	      unionEdgeIndexChain.resize(numConf+2);

	      //TODO: This is dummy attribute. Remove this and be able to write curve sketch without attributes
	      reducedAttr.push_back(seedAttr);
	    }
	  }
      }
    startRun = false;
    attributes_file.close();

    curCams.clear();
    curEdges.clear();
    curDTs.clear();
    curLabels.clear();
    curCurves.clear();
    curTangents.clear();

    curCams.resize(numConf+2);
    curEdges.resize(numConf+2);
    curDTs.resize(numConf+2);
    curLabels.resize(numConf+2);
    curCurves.resize(numConf+2);
    curTangents.resize(numConf+2);

   

    std::cout << "NUMBER OF REDUCED CURVES: " << reducedCurves.size() << std::endl;
    bmcsd_curve_3d_sketch_e csk_reduced(reducedCurves,reducedAttr);
    //: Write 3D curves and attributes to file.
    std::stringstream num_stream;
    num_stream << fa_views->stereo0();
    std::string num_str = num_stream.str();
    vul_file::make_directory(a_prefix() + std::string("/") + a_out_dir());
    vul_file::make_directory(a_prefix() + std::string("/") + a_out_dir() + std::string("/") + num_str);
    std::cout << a_prefix() + std::string("/") + a_out_dir() + std::string("/") + num_str << std::endl;
 
    retval = csk_reduced.write_dir_format(a_prefix() + std::string("/") + a_out_dir() + std::string("/") + num_str);


    curve_links.close();
    /*bmcsd_curve_3d_sketch supportedCurves(supportedSegments,supportedAttr);
    retval = supportedCurves.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
    MW_ASSERT("Error while trying to write file.\n", retval, true);//*/

  }
  

  /*//: Write 3D curves and attributes to file.
  retval = csk.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*//: Write 3D curves and attributes to file.
  retval = csk.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*//: Write 3D curves and attributes to file.
  retval = supportedCurves.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*if (a_write_corresp()) {
    for (unsigned i=0; i < s.num_corresp(); ++i) {
      std::ostringstream ns;
      ns << i;
      std::string fname
        = a_prefix() + std::string("/") + a_out_dir() + std::string("/corresp.vsl") + ns.str();
      vsl_b_ofstream corr_ofs(fname);
      vsl_b_write(corr_ofs, s.corresp(i));
    }
  }

  //Anil: Write the edge support to different txt files
  if (!write_edge_support(a_prefix() + std::string("/") + a_out_dir(), csk.attributes()))
  std::cout << "Error writing edge support files!" << std::endl;*/

  /*for (unsigned u=0; u<numConf+2; ++u)
  {
    std::stringstream used_stream;
    used_stream << "used_samples_";
    used_stream << u;
    used_stream << ".txt";
    std::string used_fname = used_stream.str();
    std::ofstream used_file(used_fname.c_str());

    unsigned numImageCurvesProcessed = 0;
    for (unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if (!usedCurvesAll[u][imc].empty())
	  numImageCurvesProcessed++;
      }

    used_file << numImageCurvesProcessed << std::endl;

    for (unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if (!usedCurvesAll[u][imc].empty())
	  {
	    used_file << imc << " " << usedCurvesAll[u][imc].size() << " ";
	    for (unsigned s=0; s<usedCurvesAll[u][imc].size(); ++s)
	      used_file << usedCurvesAll[u][imc][s] << " ";
      
	    used_file << std::endl;
	  }
      }

    used_file.close();
  }//*/

  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------

  /*std::vector<std::vector<unsigned> > usedCurveIDs(2);

  for (unsigned imc=0; imc<usedCurvesAll[12].size(); ++imc){
    unsigned numUsed = 0;
    for (unsigned s=0; s<usedCurvesAll[12][imc].size(); ++s)
      if (usedCurvesAll[12][imc][s])
	numUsed++;

    double ratio = double(numUsed)/double((usedCurvesAll[12][imc].size()));
    if (ratio >= 0.75)
      usedCurveIDs[0].push_back(imc);

  }

  for (unsigned imc=0; imc<usedCurvesAll[14].size(); ++imc){
    
    unsigned numUsed = 0;
    for (unsigned s=0; s<usedCurvesAll[14][imc].size(); ++s)
      if (usedCurvesAll[14][imc][s])
	numUsed++;

    double ratio = double(numUsed)/double((usedCurvesAll[14][imc].size()));
    if (ratio >= 0.75)
      usedCurveIDs[1].push_back(imc);

  }

  bmcsd_curve_3d_sketch csk_second_round;
  bmcsd_stereo_instance_views frames2;
  bmcsd_stereo_views_sptr instance2 = new bmcsd_stereo_views();
  instance2->set_stereo0(12);
  instance2->set_stereo1(14);
  instance2->reserve_num_confirmation_views(numConf);

  for (unsigned cv=0; cv<numConf+2; ++cv)
    if (cv!=12 && cv!=14)
      instance2->add_confirmation_view(cv);

  frames2.add_instance(instance2);

  bmcsd_concurrent_stereo_driver s2(dpath, frames2);

  s2.set_dtheta_threshold(a_dtheta_threshold());
  s2.set_distance_threshold(a_distance_threshold());

  if (a_prune_by_length())
    s2.set_min_length_per_curve_frag(a_min_length_per_curve_frag());
  else
    s2.set_min_samples_per_curve_frag(a_min_samples_per_curve_frag());
  s2.set_min_inliers_per_view(a_min_inliers_per_view()) ;
  s2.set_min_epiangle(a_min_epiangle()) ;
  s2.set_min_epipolar_overlap(a_min_epipolar_overlap()) ;
  s2.set_min_total_inliers(a_min_total_inliers()) ;
  s2.set_min_first_to_second_best_ratio(a_min_first_to_second_ratio()) ;
  s2.set_lonely_threshold(a_lonely_threshold()) ;
  s2.set_use_curvelets(a_use_curvelets());
  s2.set_min_num_inlier_edgels_per_curvelet(a_min_num_inliers_per_curvelet());

  //: How many stereo matchers can run simultaneously
  s2.set_max_concurrent_matchers(a_max_concurrent_matchers());
  s2.set_isFirstRun(true);   
  s2.set_usedCurves(usedCurveIDs);

  retval = s2.init();
  MW_ASSERT("Stereo driver init return value", retval, true);

  //: Run many pairwise stereo programs, as many as
  // frames_to_match.num_instances();
  retval = s2.run();
  MW_ASSERT("Stereo driver run return value", retval, true);

  s2.get_curve_sketch(&csk_second_round);
  //: Write 3D curves and attributes to file.
  retval = csk_second_round.write_dir_format(a_prefix() + std::string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);//*/

  //Write out the used curve samples in a text file
  for (unsigned u=0; u<numConf+2; ++u) {
    std::stringstream used_stream;
    used_stream << "used_samples_";
    used_stream << u;
    used_stream << ".txt";
    std::string used_fname = used_stream.str();
    std::ofstream used_file(used_fname.c_str());

    unsigned numImageCurvesProcessed = 0;
    for (unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc) {
      if (!usedCurvesAll[u][imc].empty())
        numImageCurvesProcessed++;
    }

    used_file << numImageCurvesProcessed << std::endl;
    for (unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc) {
      if (!usedCurvesAll[u][imc].empty()) {
          used_file << imc << " " << usedCurvesAll[u][imc].size() << " ";
          for (unsigned s=0; s<usedCurvesAll[u][imc].size(); ++s)
            used_file << usedCurvesAll[u][imc][s] << " ";
          used_file << std::endl;
      }
    }
    used_file.close();
  }

  return 0;
}

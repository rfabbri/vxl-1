#include <vul/vul_arg.h>
#include <dbul/dbul_arg.h>
#include <mw/mw_util.h>
#include <mw/pro/dbmcs_stereo_driver.h>


#define MW_ASSERT(msg, a, b) if ((a) != (b)) { vcl_cerr << (msg) << vcl_endl; exit(1); }

int
main(int argc, char **argv)
{
  vcl_string prefix_default(".");

  vul_arg<vcl_string> a_prefix("-prefix", 
      "path to directory of files",prefix_default.c_str());
  vul_arg<vcl_string> a_cam_type("-cam_type",
      "camera type: intrinsic_extrinsic or projcamera","intrinsic_extrinsic");
  vul_arg<vcl_string> a_out_dir("-outdir", "output directory relative to -prefix", "out/");
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
  vcl_cout << "\n";

  mw_util::camera_file_type cam_type;

  if (a_cam_type() == "intrinsic_extrinsic") {
    cam_type = mw_util::MW_INTRINSIC_EXTRINSIC;
  } else {
    if (a_cam_type() == "projcamera")
      cam_type = mw_util::MW_3X4;
    else  {
      vcl_cerr << "Error: invalid camera type " << a_cam_type() << vcl_endl;
      return 1;
    }
  }

  mw_curve_stereo_data_path dpath;
  bool retval = 
    mw_data::read_frame_data_list_txt(a_prefix(), &dpath, cam_type);
  if (!retval) return 1;
  vcl_cout << "Dpath:\n" << dpath << vcl_endl;

  dbmcs_stereo_instance_views frames_to_match;

  retval = dbmcs_view_set::read_txt(
      a_prefix() + vcl_string("/mcs_stereo_instances.txt"), 
      &frames_to_match);
  MW_ASSERT("frames to match from file", retval, true);
  vcl_cout << "Instances:\n" << frames_to_match << vcl_endl;

  if (a_use_curvelets() && !dpath.has_curvelets()) {
    vcl_cerr << "Error: curvelets requested, but no file names found.\n";
  }

  //Anil: First stereo instance is used to compute the total number of 
  //confirmation views used for the elongation iterations
  unsigned numConf = frames_to_match.instance(0)->num_confirmation_views();

  vcl_vector<vcl_vector<unsigned> > emptyCurveIDs(2);

  //STEP #1: Run 2-view stereo with confirmation views

  dbmcs_concurrent_stereo_driver s(dpath, frames_to_match);

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
  s.set_usedCurves(emptyCurveIDs);

  //: How many stereo matchers can run simultaneously
  s.set_max_concurrent_matchers(a_max_concurrent_matchers());

  retval = s.init();
  MW_ASSERT("Stereo driver init return value", retval, true);

  //: Run many pairwise stereo programs, as many as
  // frames_to_match.num_instances();
  retval = s.run();
  MW_ASSERT("Stereo driver run return value", retval, true);

  dbmcs_curve_3d_sketch csk;
  s.get_curve_sketch(&csk);

  //STEP #2: Process the 3D curves to remove the segments that did not gather sufficient edge support
  //A segment is removed only if its size is 3 samples or more
  //A curve segment is created only if its size is 8 samples or more
  vcl_vector<dbdif_1st_order_curve_3d> allCurves = csk.curves_3d();
 
  vcl_vector<dbdif_1st_order_curve_3d> supportedSegments;
  vcl_vector<dbmcs_curve_3d_attributes> supportedAttr;

  //Anil: Attributes contain mate curve information, get it from the curve sketch data structure 
  const vcl_vector<dbmcs_curve_3d_attributes> attrVec = csk.attributes();

  unsigned seedCurveSize = attrVec.front().origCurveSize_;

  //Anil: Data structure for stitching 3D curves together at their corresponding samples
  //1st index is the image curve ID, 2nd index is for different 3D curves and 3rd index is for sample IDs 
  vcl_vector<vcl_vector<vcl_vector<dbdif_1st_order_point_3d> > > cumulativeCurve(2000);
  vcl_vector<vcl_vector<vcl_vector<dbdif_1st_order_point_3d> > > dmy_cumulativeCurve(2000);

  unsigned numCurves = attrVec.size();
  dbmcs_curve_3d_attributes seedAttr = attrVec.front();

  //Anil: Get the view set information
  dbmcs_stereo_views_sptr seedViews = seedAttr.v_;

  //Anil: Data structure to store the marked portions of each image curve in each view
  vcl_vector<vcl_vector<vcl_vector<bool> > > usedCurvesAll(numConf+2);

  for(unsigned v=0; v<numConf+2; ++v)
    usedCurvesAll[v].resize(2000);

  for(unsigned c=0; c<allCurves.size(); ++c)
  {
    dbdif_1st_order_curve_3d curCurve = allCurves[c];
    dbmcs_curve_3d_attributes curAttr = attrVec[c];
    vcl_vector<unsigned> curSupp = curAttr.edgeSupportCount_;
    unsigned offset = curAttr.imageCurveOffset_;
    unsigned offset_v1 = curAttr.imageCurveOffset_v1_;
    unsigned origID = curAttr.orig_id_v0_;
    unsigned origID_v1 = curAttr.orig_id_v1_;
    unsigned origCurveSize = curAttr.origCurveSize_;
    unsigned origCurveSize_v1 = curAttr.origCurveSize_v1_;
    vcl_vector<unsigned> usedSamples_v1 = curAttr.used_samples_v1_;
    vcl_vector<bool> certaintyFlags = curAttr.certaintyFlags_;
    unsigned v0_seed = seedViews->stereo0();
    unsigned v1_seed = seedViews->stereo1();
    
    if(cumulativeCurve[origID].empty())
      cumulativeCurve[origID].resize(origCurveSize);

    unsigned breakLength = 0;
    unsigned initPoint = 0;
    int breakPoint = -1;

    for(unsigned s=0; s<curCurve.size(); ++s)
      if(!certaintyFlags[s])
	curSupp[s] = 0;
    
    for(unsigned s=0; s<curCurve.size(); ++s)
    {
      if(curSupp[s]<6)
      {
	if(breakPoint==-1)
	  breakPoint = s;
	breakLength++;
      }
      else
      {
	if(breakLength>2)
	{
	  if(breakPoint>initPoint+7)
	  {
	    dbdif_1st_order_curve_3d newCurve;
	    for(int p=initPoint; p<breakPoint; ++p){
	      newCurve.push_back(curCurve[p]);
	      cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

	      if(usedCurvesAll[v0_seed][origID].empty())
		usedCurvesAll[v0_seed][origID].resize(origCurveSize);

	      if(usedCurvesAll[v1_seed][origID_v1].empty())
		usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

	      usedCurvesAll[v0_seed][origID][p+offset]=true;
	      usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
	    }
	    supportedSegments.push_back(newCurve);
	    supportedAttr.push_back(curAttr);
	  }
	  initPoint = s;
	}
	breakLength = 0;
	breakPoint = -1;
      }

      if(s==(curCurve.size()-1))
      {
	if(curSupp[s]<6)
	{
	  if(breakPoint>initPoint+7)
	  {
	    dbdif_1st_order_curve_3d newCurve;
	    for(int p=initPoint; p<breakPoint; ++p){
	      newCurve.push_back(curCurve[p]);
	      cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

	      if(usedCurvesAll[v0_seed][origID].empty())
		usedCurvesAll[v0_seed][origID].resize(origCurveSize);

	      if(usedCurvesAll[v1_seed][origID_v1].empty())
		usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

	      usedCurvesAll[v0_seed][origID][p+offset]=true;
	      usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
	    }
	    supportedSegments.push_back(newCurve);
	    supportedAttr.push_back(curAttr);
	  }
	}
	else
	{
	  if(s>initPoint+6)
	  {
	    dbdif_1st_order_curve_3d newCurve;
	    for(int p=initPoint; p<s+1; ++p){
	      newCurve.push_back(curCurve[p]);
	      cumulativeCurve[origID][p+offset].push_back(curCurve[p]);

	      if(usedCurvesAll[v0_seed][origID].empty())
		usedCurvesAll[v0_seed][origID].resize(origCurveSize);

	      if(usedCurvesAll[v1_seed][origID_v1].empty())
		usedCurvesAll[v1_seed][origID_v1].resize(origCurveSize_v1);

	      usedCurvesAll[v0_seed][origID][p+offset]=true;
	      usedCurvesAll[v1_seed][origID_v1][usedSamples_v1[p]+offset_v1]=true;
	    }
	    supportedSegments.push_back(newCurve);
	    supportedAttr.push_back(curAttr);
	  }
	}
      }
    }
  }

  dbmcs_curve_3d_sketch supportedCurves(supportedSegments,supportedAttr);

  //Anil: Container for all the mate curves
  //First index is views and second index is the image curves in v0()
  vcl_vector<vcl_vector<vcl_set<int> > > cumulativeMates;
  cumulativeMates.resize(numConf);

  //STEP #3: Loop over all the confirmation views to gather all mate curves together
  /*for(unsigned v=0; v<numConf; ++v)
  {
    cumulativeMates[v].resize(2000);
    unsigned curView = seedViews->confirmation_view(v);

    for(unsigned c=0; c<numCurves; ++c){
      dbmcs_curve_3d_attributes curAttr = attrVec[c];
      unsigned origID = curAttr.orig_id_v0_;
      vcl_set<int> curMates = curAttr.mate_curves_[v];

      for(vcl_set<int>::iterator mit=curMates.begin(); mit!=curMates.end(); ++mit)
	cumulativeMates[v][origID].insert(*mit);
    }
  }

  //STEP #4: Loop over all the confirmation views to process the cumulative mate curves in each one
  for(unsigned v=0; v<numConf; ++v)
  {
    dbmcs_curve_3d_sketch csk_elong;
    unsigned curView = seedViews->confirmation_view(v);
    vcl_cout << "LOOKING FOR CUES IN VIEW: " << curView << vcl_endl;
    dbmcs_stereo_instance_views curFrames;
    dbmcs_stereo_views_sptr curInstance = new dbmcs_stereo_views();
    curInstance->set_stereo0(seedViews->stereo0());
    curInstance->set_stereo1(curView);
    curInstance->reserve_num_confirmation_views(numConf);

    for(unsigned cv=0; cv<numConf; ++cv)
      if(cv != v)
	curInstance->add_confirmation_view(seedViews->confirmation_view(cv));

    curInstance->add_confirmation_view(seedViews->stereo1());
    curFrames.add_instance(curInstance);

    dbmcs_concurrent_stereo_driver cur_s(dpath, curFrames);

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
    cur_s.set_usedCurves(emptyCurveIDs);

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
    vcl_vector<dbdif_1st_order_curve_3d> cur_allCurves = csk_elong.curves_3d();
 
    vcl_vector<dbdif_1st_order_curve_3d> cur_supportedSegments;
    vcl_vector<dbmcs_curve_3d_attributes> cur_supportedAttr;

    const vcl_vector<dbmcs_curve_3d_attributes> cur_attrVec = csk_elong.attributes();

    for(unsigned c=0; c<cur_allCurves.size(); ++c)
      {
	dbdif_1st_order_curve_3d curCurve = cur_allCurves[c];
	dbmcs_curve_3d_attributes curAttr = cur_attrVec[c];
	vcl_vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	unsigned cur_offset = curAttr.imageCurveOffset_;
	unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	unsigned cur_origID = curAttr.orig_id_v0_;
	unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	unsigned cur_origCurveSize = curAttr.origCurveSize_;
	unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	vcl_vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	vcl_vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	unsigned cur_v0_seed = seedViews->stereo0();
	unsigned cur_v1_seed = curView;

	unsigned breakLength = 0;
	unsigned initPoint = 0;
	int breakPoint = -1;

	for(unsigned s=0; s<curCurve.size(); ++s)
	  if(!cur_certaintyFlags[s])
	    curSupp[s] = 0;

	for(unsigned s=0; s<curCurve.size(); ++s)
	  {
	    if(curSupp[s]<6)
	      {
		if(breakPoint==-1)
		  breakPoint = s;
		breakLength++;
	      }
	    else
	      {
		if(breakLength>2)
		  {
		    if(breakPoint>initPoint+7)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<breakPoint; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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

	    if(s==(curCurve.size()-1))
	      {
		if(curSupp[s]<6)
		  {
		    if(breakPoint>initPoint+7)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<breakPoint; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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
		    if(s>initPoint+6)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<s+1; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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

  //STEP #5: Make each confirmation view the second anchor to try and reconstruct unused curves
  for(unsigned vi=0; vi<numConf; ++vi) {

    dbmcs_curve_3d_sketch csk_iterate;
    unsigned curView = seedViews->confirmation_view(vi);
    vcl_cout << "SWITCHING SECOND ANCHOR TO VIEW: " << curView << vcl_endl;
    dbmcs_stereo_instance_views curFrames;
    dbmcs_stereo_views_sptr curInstance = new dbmcs_stereo_views();
    curInstance->set_stereo0(seedViews->stereo0());
    curInstance->set_stereo1(curView);
    curInstance->reserve_num_confirmation_views(numConf);

    for(unsigned cv=0; cv<numConf; ++cv)
      if(cv != vi)
	curInstance->add_confirmation_view(seedViews->confirmation_view(cv));

    curInstance->add_confirmation_view(seedViews->stereo1());
    curFrames.add_instance(curInstance);

    vcl_vector<vcl_vector<unsigned> > usedCurveIDs(2);
    /*for(unsigned imc=0; imc<usedCurvesAll[seedViews->stereo0()].size(); ++imc){
      unsigned numUsed = 0;
      for(unsigned s=0; s<usedCurvesAll[seedViews->stereo0()][imc].size(); ++s)
	if(usedCurvesAll[seedViews->stereo0()][imc][s])
	  numUsed++;

      double ratio = double(numUsed)/double((usedCurvesAll[seedViews->stereo0()][imc].size()));
      if(ratio >= 0.75)
	usedCurveIDs[0].push_back(imc);

    }

    for(unsigned imc=0; imc<usedCurvesAll[curView].size(); ++imc){
    
      unsigned numUsed = 0;
      for(unsigned s=0; s<usedCurvesAll[curView][imc].size(); ++s)
	if(usedCurvesAll[curView][imc][s])
	  numUsed++;

      double ratio = double(numUsed)/double((usedCurvesAll[curView][imc].size()));
      if(ratio >= 0.75)
	usedCurveIDs[1].push_back(imc);

    }*/
    
    dbmcs_concurrent_stereo_driver cur_s(dpath, curFrames);

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
    cur_s.set_usedCurves(usedCurveIDs);

    retval = cur_s.init();
    MW_ASSERT("Stereo driver init return value", retval, true);

    //: Run many pairwise stereo programs, as many as
    // frames_to_match.num_instances();
    retval = cur_s.run();
    MW_ASSERT("Stereo driver run return value", retval, true);

    cur_s.get_curve_sketch(&csk_iterate);

    //Anil: Same as before - Process the 3D curves to remove the segments that did not gather sufficient edge support
    //A segment is removed only if its size is 3 samples or more
    //A curve segment is created only if its size is 8 samples or more
    vcl_vector<dbdif_1st_order_curve_3d> cur_allCurves = csk_iterate.curves_3d();
    unsigned cur_numCurves = cur_allCurves.size();
 
    vcl_vector<dbdif_1st_order_curve_3d> cur_supportedSegments;
    vcl_vector<dbmcs_curve_3d_attributes> cur_supportedAttr;

    const vcl_vector<dbmcs_curve_3d_attributes> cur_attrVec = csk_iterate.attributes();
    vcl_cout << "Got the attributes" << vcl_endl;

    for(unsigned c=0; c<cur_allCurves.size(); ++c)
      {
	vcl_cout << "Curve #" << c << vcl_endl;
	dbdif_1st_order_curve_3d curCurve = cur_allCurves[c];
	dbmcs_curve_3d_attributes curAttr = cur_attrVec[c];
	vcl_vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	unsigned cur_offset = curAttr.imageCurveOffset_;
	unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	unsigned cur_origID = curAttr.orig_id_v0_;
	unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	unsigned cur_origCurveSize = curAttr.origCurveSize_;
	unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	vcl_vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	vcl_vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	unsigned cur_v0_seed = seedViews->stereo0();
	unsigned cur_v1_seed = curView;

	if(cumulativeCurve[cur_origID].empty())
	  cumulativeCurve[cur_origID].resize(cur_origCurveSize);

	unsigned breakLength = 0;
	unsigned initPoint = 0;
	int breakPoint = -1;

	for(unsigned s=0; s<curCurve.size(); ++s)
	  if(!cur_certaintyFlags[s])
	    curSupp[s] = 0;

	for(unsigned s=0; s<curCurve.size(); ++s)
	  {
	    if(curSupp[s]<6)
	      {
		if(breakPoint==-1)
		  breakPoint = s;
		breakLength++;
	      }
	    else
	      {
		if(breakLength>2)
		  {
		    if(breakPoint>initPoint+7)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<breakPoint; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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

	    if(s==(curCurve.size()-1))
	      {
		if(curSupp[s]<6)
		  {
		    if(breakPoint>initPoint+7)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<breakPoint; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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
		    if(s>initPoint+6)
		      {
			dbdif_1st_order_curve_3d newCurve;
			for(int p=initPoint; p<s+1; ++p){
			  newCurve.push_back(curCurve[p]);
			  cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			  if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
			    usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			  if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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
    
    //Anil: Container for all the mate curves
    //First index is views and second index is the image curves in v0()
    vcl_vector<vcl_vector<vcl_set<int> > > cur_cumulativeMates;
    cur_cumulativeMates.resize(numConf);

    //Loop over all the confirmation views to gather all mate curves together
    /*for(unsigned v=0; v<numConf; ++v)
      {
	cur_cumulativeMates[v].resize(2000);
	//unsigned curView = curInstance->confirmation_view(v);

	for(unsigned c=0; c<cur_numCurves; ++c){
	  dbmcs_curve_3d_attributes curAttr = cur_attrVec[c];
	  unsigned origID = curAttr.orig_id_v0_;
	  vcl_set<int> curMates = curAttr.mate_curves_[v];

	  for(vcl_set<int>::iterator mit=curMates.begin(); mit!=curMates.end(); ++mit)
	    cur_cumulativeMates[v][origID].insert(*mit);
	}
      } 

    //Loop over all the confirmation views to process the cumulative mate curves in each one
    for(unsigned v=0; v<numConf; ++v)
      {
	dbmcs_curve_3d_sketch csk_iter_elong;
	unsigned iterView = curInstance->confirmation_view(v);
	vcl_cout << "LOOKING FOR CUES IN VIEW: " << iterView << vcl_endl;
	dbmcs_stereo_instance_views iterFrames;
	dbmcs_stereo_views_sptr iterInstance = new dbmcs_stereo_views();
	iterInstance->set_stereo0(curInstance->stereo0());
	iterInstance->set_stereo1(iterView);
	vcl_cout << curInstance->stereo0() << " " << iterView << vcl_endl;
	iterInstance->reserve_num_confirmation_views(numConf);

	for(unsigned cv=0; cv<numConf; ++cv)
	  if(cv != v){
	    iterInstance->add_confirmation_view(curInstance->confirmation_view(cv));
	    vcl_cout << curInstance->confirmation_view(cv) << " ";
	  }
	
	iterInstance->add_confirmation_view(curInstance->stereo1());
	vcl_cout << curInstance->stereo1() << vcl_endl;
	iterFrames.add_instance(iterInstance);

	dbmcs_concurrent_stereo_driver iter_s(dpath, iterFrames);

	vcl_vector<vcl_vector<unsigned> > iter_usedCurveIDs(2);
	for(unsigned imc=0; imc<usedCurvesAll[curInstance->stereo0()].size(); ++imc){
	  unsigned numUsed = 0;
	  for(unsigned s=0; s<usedCurvesAll[curInstance->stereo0()][imc].size(); ++s)
	    if(usedCurvesAll[curInstance->stereo0()][imc][s])
	      numUsed++;

	  double ratio = double(numUsed)/double((usedCurvesAll[curInstance->stereo0()][imc].size()));
	  if(ratio >= 0.75)
	    iter_usedCurveIDs[0].push_back(imc);

	}

	for(unsigned imc=0; imc<usedCurvesAll[iterView].size(); ++imc){
    
	  unsigned numUsed = 0;
	  for(unsigned s=0; s<usedCurvesAll[iterView][imc].size(); ++s)
	    if(usedCurvesAll[iterView][imc][s])
	      numUsed++;

	  double ratio = double(numUsed)/double((usedCurvesAll[iterView][imc].size()));
	  if(ratio >= 0.75)
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
	vcl_vector<dbdif_1st_order_curve_3d> iter_allCurves = csk_iter_elong.curves_3d();
 
	vcl_vector<dbdif_1st_order_curve_3d> iter_supportedSegments;
	vcl_vector<dbmcs_curve_3d_attributes> iter_supportedAttr;

	const vcl_vector<dbmcs_curve_3d_attributes> iter_attrVec = csk_iter_elong.attributes();

	for(unsigned c=0; c<iter_allCurves.size(); ++c)
	  {
	    dbdif_1st_order_curve_3d curCurve = iter_allCurves[c];
	    dbmcs_curve_3d_attributes curAttr = iter_attrVec[c];
	    vcl_vector<unsigned> curSupp = curAttr.edgeSupportCount_;
	    unsigned cur_offset = curAttr.imageCurveOffset_;
	    unsigned cur_offset_v1 = curAttr.imageCurveOffset_v1_;
	    unsigned cur_origID = curAttr.orig_id_v0_;
	    unsigned cur_origID_v1 = curAttr.orig_id_v1_;
	    unsigned cur_origCurveSize = curAttr.origCurveSize_;
	    unsigned cur_origCurveSize_v1 = curAttr.origCurveSize_v1_;
	    vcl_vector<unsigned> cur_usedSamples_v1 = curAttr.used_samples_v1_;
	    vcl_vector<bool> cur_certaintyFlags = curAttr.certaintyFlags_;
	    unsigned cur_v0_seed = curInstance->stereo0();
	    unsigned cur_v1_seed = iterView;

	    unsigned breakLength = 0;
	    unsigned initPoint = 0;
	    int breakPoint = -1;

	    for(unsigned s=0; s<curCurve.size(); ++s)
	      if(!cur_certaintyFlags[s])
		curSupp[s] = 0;

	    for(unsigned s=0; s<curCurve.size(); ++s)
	      {
		if(curSupp[s]<6)
		  {
		    if(breakPoint==-1)
		      breakPoint = s;
		    breakLength++;
		  }
		else
		  {
		    if(breakLength>2)
		      {
			if(breakPoint>initPoint+7)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for(int p=initPoint; p<breakPoint; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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

		if(s==(curCurve.size()-1))
		  {
		    if(curSupp[s]<6)
		      {
			if(breakPoint>initPoint+7)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for(int p=initPoint; p<breakPoint; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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
			if(s>initPoint+6)
			  {
			    dbdif_1st_order_curve_3d newCurve;
			    for(int p=initPoint; p<s+1; ++p){
			      newCurve.push_back(curCurve[p]);
			      cumulativeCurve[cur_origID][p+cur_offset].push_back(curCurve[p]);

			      if(usedCurvesAll[cur_v0_seed][cur_origID].empty())
				usedCurvesAll[cur_v0_seed][cur_origID].resize(cur_origCurveSize);

			      if(usedCurvesAll[cur_v1_seed][cur_origID_v1].empty())
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

  vcl_cout << "FINISHED GATHERING!!!!" << vcl_endl;

  dbmcs_curve_3d_sketch csk_supported(supportedSegments,supportedAttr);
  //: Write 3D curves and attributes to file.
  retval = csk_supported.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);


  //STEP #6: Take the average of all the corresponding curve samples 
  //to reduce the strand to a single curve
  //process 1 image curve at a time
    
  //Anil: Containers for the reduced 3d curves
  vcl_vector<dbdif_1st_order_curve_3d> reducedCurves;
  vcl_vector<dbmcs_curve_3d_attributes> reducedAttr;

  for(unsigned i=0; i<2000; ++i)
  {
    vcl_vector<vcl_vector<dbdif_1st_order_point_3d> > cur_cumulativeCurve = cumulativeCurve[i];
    dbdif_1st_order_curve_3d averageCurve;
    if(!cur_cumulativeCurve.empty())
      {
	for(unsigned s=0; s<cur_cumulativeCurve.size(); ++s)
	  {
	    if(cur_cumulativeCurve[s].empty())
	      {
		if(!averageCurve.empty())
		  {
		    reducedCurves.push_back(averageCurve);
		    averageCurve.clear();
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
		for(unsigned p=0; p<numPoints; ++p)
		  {
		    xSum+=cur_cumulativeCurve[s][p].Gama[0];
		    ySum+=cur_cumulativeCurve[s][p].Gama[1];
		    zSum+=cur_cumulativeCurve[s][p].Gama[2];

		    txSum+=cur_cumulativeCurve[s][p].T[0];
		    tySum+=cur_cumulativeCurve[s][p].T[1];
		    tzSum+=cur_cumulativeCurve[s][p].T[2];
		  }
		dbdif_vector_3d avGama,avT,finalAvGama,finalAvT;
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
		for(unsigned p=0; p<numPoints; ++p)
		  {
		    double x=cur_cumulativeCurve[s][p].Gama[0];
		    double y=cur_cumulativeCurve[s][p].Gama[1];
		    double z=cur_cumulativeCurve[s][p].Gama[2];

		    double distSq = vcl_pow(x-avGama[0],2) + vcl_pow(y-avGama[1],2) + vcl_pow(z-avGama[2],2);
		    if(distSq<0.05)
		    //if(true)
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

		if(numSamples > 0){
		  finalAvGama[0] = xSum/numSamples;
		  finalAvGama[1] = ySum/numSamples;
		  finalAvGama[2] = zSum/numSamples;

		  finalAvT[0] = txSum/numSamples;
		  finalAvT[1] = tySum/numSamples;
		  finalAvT[2] = tzSum/numSamples;

		  dbdif_1st_order_point_3d avPoint;
		  avPoint.Gama = finalAvGama;
		  avPoint.T = finalAvT;

		  averageCurve.push_back(avPoint);
		}
		else if(!averageCurve.empty()){
		  reducedCurves.push_back(averageCurve);
		  averageCurve.clear();
		  //TODO: This is dummy attribute. Remove this and be able to write curve sketch without attributes
		  reducedAttr.push_back(seedAttr);
		}

	      }  
	  }
	if(!averageCurve.empty())
	  reducedCurves.push_back(averageCurve);
      }
  }
  

  vcl_cout << "NUMBER OF REDUCED CURVES: " << reducedCurves.size() << vcl_endl;
  dbmcs_curve_3d_sketch csk_reduced(reducedCurves,reducedAttr);
  //: Write 3D curves and attributes to file.
  vul_file::make_directory(a_prefix() + vcl_string("/") + a_out_dir());
  vul_file::make_directory(a_prefix() + vcl_string("/") + a_out_dir() + vcl_string("/10"));
  vcl_cout << a_prefix() + vcl_string("/") + a_out_dir() + vcl_string("/10") << vcl_endl;

  retval = csk_reduced.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir() + vcl_string("/10"));

  /*//: Write 3D curves and attributes to file.
  retval = csk.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*//: Write 3D curves and attributes to file.
  retval = csk.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*//: Write 3D curves and attributes to file.
  retval = supportedCurves.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);*/

  /*if (a_write_corresp()) {
    for (unsigned i=0; i < s.num_corresp(); ++i) {
      vcl_ostringstream ns;
      ns << i;
      vcl_string fname
        = a_prefix() + vcl_string("/") + a_out_dir() + vcl_string("/corresp.vsl") + ns.str();
      vsl_b_ofstream corr_ofs(fname);
      vsl_b_write(corr_ofs, s.corresp(i));
    }
  }

  //Anil: Write the edge support to different txt files
  if(!write_edge_support(a_prefix() + vcl_string("/") + a_out_dir(), csk.attributes()))
  vcl_cout << "Error writing edge support files!" << vcl_endl;*/

  /*for(unsigned u=0; u<numConf+2; ++u)
  {
    vcl_stringstream used_stream;
    used_stream << "used_samples_";
    used_stream << u;
    used_stream << ".txt";
    vcl_string used_fname = used_stream.str();
    vcl_ofstream used_file(used_fname.c_str());

    unsigned numImageCurvesProcessed = 0;
    for(unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if(!usedCurvesAll[u][imc].empty())
	  numImageCurvesProcessed++;
      }

    used_file << numImageCurvesProcessed << vcl_endl;

    for(unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if(!usedCurvesAll[u][imc].empty())
	  {
	    used_file << imc << " " << usedCurvesAll[u][imc].size() << " ";
	    for(unsigned s=0; s<usedCurvesAll[u][imc].size(); ++s)
	      used_file << usedCurvesAll[u][imc][s] << " ";
      
	    used_file << vcl_endl;
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

  /*vcl_vector<vcl_vector<unsigned> > usedCurveIDs(2);

  for(unsigned imc=0; imc<usedCurvesAll[12].size(); ++imc){
    unsigned numUsed = 0;
    for(unsigned s=0; s<usedCurvesAll[12][imc].size(); ++s)
      if(usedCurvesAll[12][imc][s])
	numUsed++;

    double ratio = double(numUsed)/double((usedCurvesAll[12][imc].size()));
    if(ratio >= 0.75)
      usedCurveIDs[0].push_back(imc);

  }

  for(unsigned imc=0; imc<usedCurvesAll[14].size(); ++imc){
    
    unsigned numUsed = 0;
    for(unsigned s=0; s<usedCurvesAll[14][imc].size(); ++s)
      if(usedCurvesAll[14][imc][s])
	numUsed++;

    double ratio = double(numUsed)/double((usedCurvesAll[14][imc].size()));
    if(ratio >= 0.75)
      usedCurveIDs[1].push_back(imc);

  }

  dbmcs_curve_3d_sketch csk_second_round;
  dbmcs_stereo_instance_views frames2;
  dbmcs_stereo_views_sptr instance2 = new dbmcs_stereo_views();
  instance2->set_stereo0(12);
  instance2->set_stereo1(14);
  instance2->reserve_num_confirmation_views(numConf);

  for(unsigned cv=0; cv<numConf+2; ++cv)
    if(cv!=12 && cv!=14)
      instance2->add_confirmation_view(cv);

  frames2.add_instance(instance2);

  dbmcs_concurrent_stereo_driver s2(dpath, frames2);

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
  retval = csk_second_round.write_dir_format(a_prefix() + vcl_string("/") + a_out_dir());
  MW_ASSERT("Error while trying to write file.\n", retval, true);//*/

  //Write out the used curve samples in a text file
  for(unsigned u=0; u<numConf+2; ++u) {
    vcl_stringstream used_stream;
    used_stream << "used_samples_";
    used_stream << u;
    used_stream << ".txt";
    vcl_string used_fname = used_stream.str();
    vcl_ofstream used_file(used_fname.c_str());

    unsigned numImageCurvesProcessed = 0;
    for(unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if(!usedCurvesAll[u][imc].empty())
	  numImageCurvesProcessed++;
      }

    used_file << numImageCurvesProcessed << vcl_endl;

    for(unsigned imc=0; imc<usedCurvesAll[u].size(); ++imc)
      {
	if(!usedCurvesAll[u][imc].empty())
	  {
	    used_file << imc << " " << usedCurvesAll[u][imc].size() << " ";
	    for(unsigned s=0; s<usedCurvesAll[u][imc].size(); ++s)
	      used_file << usedCurvesAll[u][imc][s] << " ";
      
	    used_file << vcl_endl;
	  }
      }

    used_file.close();
  }

  return 0;
}

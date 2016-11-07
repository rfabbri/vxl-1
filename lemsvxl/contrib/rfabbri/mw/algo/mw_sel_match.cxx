//#include <dbdet/sel/dbdet_sel.h>

//--------------------------------------------------------------------------------
// SEL COSTS 
//--------------------------------------------------------------------------------

void mw_point_matcher::
sel_geometry_costs(
    const vcl_vector<vcl_vector< vsol_point_2d_sptr > > &points_,
    const vcl_vector<dbdet_sel_sptr> &sel_,
    mw_discrete_corresp *corr,
    unsigned iv1, unsigned iv2 )
{
  dbdif_rig rig(cam_[iv1].Pr_, cam_[iv2].Pr_);

  unsigned  long n_fail = 0;
  unsigned  long n_corr = 0;
  unsigned  long n_no_quads_1=0, n_no_quads_2=0, n_no_bundles_1=0, n_no_bundles_2=0;
  for (unsigned i = 0; i < points_[iv1].size(); ++i) {
    // run through list of candidates
    for ( vcl_list<mw_attributed_point>::iterator itr= corr->corresp_[i].begin();
          itr != corr->corresp_[i].end(); ++itr) {

      vsol_point_2d_sptr p1 = points_[iv1][i];
      vsol_point_2d_sptr p2 = points_[iv2][itr->pt_];
      // get edgels
      assert (i < sel_[iv1]->get_edgels().size());
      dbdet_edgel *e1 = sel_[iv1]->get_edgels()[i]; //:< make sure this is a valid way of accessing an edgel
      assert (e1->pt == p1->get_p());
      assert ((unsigned )e1->id == i+1);

      assert (itr->pt_ < sel_[iv2]->get_edgels().size());
      dbdet_edgel *e2 = sel_[iv2]->get_edgels()[itr->pt_]; //:< make sure this is a valid way of accessing an edgel
      assert (e2->pt == p2->get_p());
      assert ((unsigned )e2->id == itr->pt_+1);


      // compute trinocular reproj for present point + candidate
//      TODO sel_geometry_match_cost ( p1, p2, sel_[iv1],sel_[iv2], rig, &cost);
      sel_reason reason;
      double cost;

      bool constraint_valid = sel_geometry_match_cost (e1, e2, sel_[iv1],sel_[iv2], rig, &cost, &reason);


      itr->cost_ += cost;
      ++n_corr;

      if (constraint_valid) {
//        itr->cost_ += Gamma_3dot_min;
      } else {
        ++n_fail;
        switch (reason) {
          case FAIL_NO_CURVS_P1: ++n_no_quads_1; break;
          case FAIL_NO_CURVS_P2: ++n_no_quads_2; break;
          case FAIL_NO_BUNDLE_P1: ++n_no_bundles_1; break;
          case FAIL_NO_BUNDLE_P2: ++n_no_bundles_2; break;
          case FAIL_UNDEFINED: vcl_cout << "Warning: Reason undefined\n";
                               break;
          default: break;
        }
        if (n_fail < 3) {
        vcl_cout << "SEL geometry constraint failed for hypothesis (" << 
          i << ", " << itr->pt_ << ")" << vcl_endl;
        vcl_cout << "n_no_quads_1: " << n_no_quads_1 << "\tn_no_quads_2: " << n_no_quads_2 << vcl_endl;
        vcl_cout << "n_no_bundles_1: " << n_no_bundles_1 << "\tn_no_bundles_2: " << n_no_bundles_2 << vcl_endl;
        }
      }
    }

    if (i%250 == 0) {
      vcl_cout << "Processed " <<  100*(float)i/(float)points_[iv1].size() << "% (" << i << " out of " << points_[iv1].size() << ")" << vcl_endl;
      vcl_cout << "SEL geometry constraint failed for " << 100*(float)n_fail/n_corr << "% correspondences so far processed (" << n_fail << " out of " << n_corr <<  ")\n";
      vcl_cout << "\t Failure reasons:\n\t\t" << 
        "no quads 1: " << n_no_quads_1 << " (" << 100.0*(float)n_no_quads_1/(float)n_fail << "%) \n\t\t" <<
        "no quads 2: " << n_no_quads_2 << " (" << 100.0*(float)n_no_quads_2/(float)n_fail << "%) \n\t\t" <<
        "no quads 1+2: " << n_no_quads_1 + n_no_quads_2 << " (" << 100.0*(float)(n_no_quads_1+n_no_quads_2)/(float)n_fail << "%) \n\t\t" <<
        "no bundles 1: " << n_no_bundles_1 << " (" << 100.0*(float)n_no_bundles_1/(float)n_fail << "%) \n\t\t" <<
        "no bundles 2: " << n_no_bundles_2 << " (" << 100.0*(float)n_no_bundles_2/(float)n_fail << "%) \n\t\t" <<
        "no bundles 1+2: " << n_no_bundles_1 + n_no_bundles_2 << " (" << 100.0*(float)(n_no_bundles_1+n_no_bundles_2)/(float)n_fail << "%) \n\t\t" <<
        "total failures due to no quads or no bundles (has to be equal fail number above) : " << n_no_quads_1 + n_no_quads_2 + n_no_bundles_1 + n_no_bundles_2 << vcl_endl;
    }
  }
}

// \return true if constraint applicable for both matches; false if constraint
// could not be applied in this case
bool mw_point_matcher::
sel_geometry_match_cost( 
  const dbdet_edgel *e1, 
  const dbdet_edgel *e2, 
  const dbdet_sel_sptr &/*sel_img1*/,
  const dbdet_sel_sptr &/*sel_img2*/,
  dbdif_rig &rig,
  double *cost, sel_reason *reason) const
{

#if 0
  // XXX have to adapt code to use new SEL
  
  double Gamma_3dot_min = vcl_numeric_limits<double>::infinity();
  unsigned  cl = 2; bool valid_matching_curvelets_found=false;



  while (!valid_matching_curvelets_found && cl != 1) {

    // ---- For each quad through e1
    bool dont_even_try=false;
    if (e1->local_curvelets[cl].empty()) {
      *reason = FAIL_NO_CURVS_P1;
      dont_even_try=true;
    } else {
      if (e2->local_curvelets[cl].empty()) {
        *reason = FAIL_NO_CURVS_P2;
        dont_even_try=true;
      }
    }

    bool has_valid_bundle_1=false;
    bool has_valid_bundle_2=false;

    if (!dont_even_try) {
      curvelet_list_const_iter it = e1->local_curvelets[cl].begin();
      for (; it != e1->local_curvelets[cl].end(); ++it) {
//          vcl_cout << "   ==== Curvelet #" << n << " ====" << vcl_endl;
//          vcl_cout << vcl_endl;
//          vcl_cout << "#bundles: " << (*it)->curve_bundles.size() << vcl_endl;
//            vcl_cout << " --- Bundle #" << i_cb << ":  ";
          if ( (*it)->curve_model && (*it)->ref_edgel->id == e1->id  ) {
            has_valid_bundle_1=true;
            //: Cast to ES_curve_model

            dbdet_ES_curve_model *crv_model1 = dynamic_cast<dbdet_ES_curve_model *> ((*it)->curve_model);
            if (!crv_model1) {
              vcl_cerr << "Curve model must be Eulerspiral\n";
              abort();
            }

            double kurv     = crv_model1->k;
            double kurv_dot = crv_model1->gamma;
            double theta    = crv_model1->theta;
            vgl_point_2d<double> p1_refined = crv_model1->pt;

            dbdif_3rd_order_point_2d p1_frenet;

            rig.cam[0].img_to_world(p1_refined,theta,kurv,kurv_dot, &p1_frenet);

              // ---- For each quad through e2
              curvelet_list_const_iter kit = e2->local_curvelets[cl].begin();
              for (; kit != e2->local_curvelets[cl].end(); ++kit) {
                  if ( (*kit)->curve_model && (*kit)->ref_edgel->id == e2->id  ) {

                    dbdet_ES_curve_model *crv_model2 = dynamic_cast<dbdet_ES_curve_model *> ((*kit)->curve_model);
                    if (!crv_model2) {
                      vcl_cerr << "Curve model must be Eulerspiral\n";
                      abort();
                    }

                    has_valid_bundle_2=true;
                    double kurv_2     = crv_model2->k;
                    double kurv_dot_2 = crv_model2->gamma;
                    double theta_2 = crv_model2->theta;
                    vgl_point_2d<double> p2_refined = crv_model2->pt;
                    dbdif_3rd_order_point_2d p2_frenet;
                    rig.cam[1].img_to_world(p2_refined,theta_2,kurv_2,kurv_dot_2, &p2_frenet);

                    dbdif_3rd_order_point_3d P_frenet;
                    rig.reconstruct_3rd_order(p1_frenet, p2_frenet, &P_frenet);
//                    double Gamma_3dot_tmp = P_frenet.Gamma_3dot_abs();
                    double Gamma_3dot_tmp = P_frenet.K*P_frenet.K; // XXX ignoring torsion as a test
                    if (Gamma_3dot_tmp < Gamma_3dot_min)
                      Gamma_3dot_min = Gamma_3dot_tmp;

                    // get minimum
                    // - reconstruct K, Kdot, tau, measure |\dddot \Gamma|
                    // - OR just compare the difference | |\dddot \gamma_1| - |\dddot \gamma_2| | for short baseline
                  } else {
                  }
                }
          } else {
          }
      }/* ! loop curvelets around e1 */
      if (!has_valid_bundle_1 && ! e1->local_curvelets[cl].empty())
          *reason = FAIL_NO_BUNDLE_P1;
      else {
        if (!has_valid_bundle_2 && ! e2->local_curvelets[cl].empty())
          *reason = FAIL_NO_BUNDLE_P2;
      }
    } // ! if !dont_even_try
    --cl;
    valid_matching_curvelets_found = Gamma_3dot_min != vcl_numeric_limits<double>::infinity();
  } // ! while curvelet level (trips, quads)

  *cost = Gamma_3dot_min;

  if (valid_matching_curvelets_found) {
    *reason = OK_UNDEFINED;
    return true;
  }
#endif

  vcl_cerr << "Function code is being updated.\n";
  abort();
  return false;
}

// This is dbrec/tests/test_visitors.cxx
//:
// \file
// \brief Various tests for dbrec library
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
#include <testlib/testlib_test.h>
#include <dbrec/dbrec_image_models.h>
#include <dbrec/dbrec_image_models_sptr.h>
#include <bsta/bsta_gauss_sf1.h>
#include <vcl_cmath.h>

static void test_models()
{
  float lambda = 2.0f; float k = 1.2f;
  bsta_weibull<float> w(lambda, k);
  dbrec_gaussian_appearance_model_sptr m = new dbrec_gaussian_weibull_appearance_model(lambda, k);
  TEST_NEAR("testing gaussian_weibull_appearance_model", m->prob_density(10.0f, 0, 0), w.prob_density(10.0f), 0.1f);

  vil_image_view<float> mu_img(100, 100);
  mu_img.fill(20.0f);
  vil_image_view<float> sigma_img(100, 100);
  sigma_img.fill(1.1f);
  dbrec_gaussian_appearance_model_sptr m2 = new dbrec_gaussian_bg_appearance_model(mu_img, sigma_img);
  bsta_gauss_sf1 pdbg(20.0f, 1.1f*1.1f);
  TEST_NEAR("testing gaussian_bg_appearance_model", m2->prob_density(10.0f, 10, 10), pdbg.prob_density(10.0f), 0.1f);

  dbrec_prior_model_sptr pm = new dbrec_constant_prior_model(0.7f);
  TEST_NEAR("testing dbrec_constant_prior_model", pm->prior(10, 10), 0.7f, 0.1f);

  vil_image_view<float> loc_prior(100, 100);
  loc_prior.fill(0.2f);
  dbrec_prior_model_sptr pm2 = new dbrec_loc_prior_model(loc_prior);
  TEST_NEAR("testing dbrec_loc_prior_model", pm2->prior(10, 10), 0.2f, 0.1f);

  vcl_vector<dbrec_prior_model_sptr> models; models.push_back(pm); models.push_back(pm2);
  dbrec_prior_model_sptr pm3 = new dbrec_indep_prior_model(models);
  TEST_NEAR("testing dbrec_indep_prior_model", pm3->prior(10, 10), 0.2f*0.7f, 0.1f);
}

TESTMAIN( test_models );

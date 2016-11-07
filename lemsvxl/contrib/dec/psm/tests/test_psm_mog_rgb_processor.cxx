#include <testlib/testlib_test.h>

#include <vcl_vector.h>
#include <vnl/vnl_random.h>
#include <vil/vil_rgb.h>

#include "../psm_apm_traits.h"
#include "../psm_mog_rgb_processor.h"



static void test_psm_mog_rgb_processor()
{
  START("psm_mog_rgb_processor test");

  vnl_random randgen;
  {
    vcl_vector<vil_rgb<float> > samples;
    vcl_vector<float> weights;

    psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype model;

    unsigned int nsamples = 1000;
    for (unsigned int n=0; n<nsamples; ++n) {

      float r = (float)randgen.normal() * 0.10f + 0.60f;
      float g = (float)randgen.normal() * 0.20f + 0.25f;
      float b = (float)randgen.normal() * 0.15f + 0.80f;

      samples.push_back(vil_rgb<float>(r,g,b));
      weights.push_back(1.0f);
    }


    psm_mog_rgb_processor::update_appearance(samples, weights, model, 0.1f);

    for (unsigned m=0; m<3; ++m) {
      vcl_cout << "mode " << m << " weight = " << model.weight(m) << vcl_endl;
      vcl_cout << "         mean = " << model.distribution(m).mean() << vcl_endl;
      vcl_cout << "         var = " << model.distribution(m).covar() << vcl_endl;
    }

    float p0 = psm_mog_rgb_processor::prob_density(model, vil_rgb<float>(0.2f, 0.6f, 0.85f));
    float p1 = psm_mog_rgb_processor::prob_density(model, vil_rgb<float>(0.65f, 0.12f, 0.84f));

    vcl_cout << "p0 = " << p0 << vcl_endl;
    vcl_cout << "p1 = " << p1 << vcl_endl;
  }
  {
    vcl_cout << "Testing degenerate cases " << vcl_endl;

    vcl_vector<vil_rgb<float> > samples;
    vcl_vector<float> weights;


    for (unsigned int n=0; n<10; ++n) {
      float r = (float)randgen.normal() * 0.10f + 0.60f;
      float g = (float)randgen.normal() * 0.20f + 0.25f;
      float b = (float)randgen.normal() * 0.15f + 0.80f;

      samples.push_back(vil_rgb<float>(r,g,b));
      weights.push_back((float)randgen.drand32(-1,1e-5));
    }
    psm_apm_traits<PSM_APM_MOG_RGB>::apm_datatype model;
    psm_mog_rgb_processor::update_appearance(samples, weights, model, 0.1f);

    for (unsigned m=0; m<3; ++m) {
      vcl_cout << "mode " << m << " weight = " << model.weight(m) << vcl_endl;
      vcl_cout << "         mean = " << model.distribution(m).mean() << vcl_endl;
      vcl_cout << "         var = " << model.distribution(m).covar() << vcl_endl;
    }

    float p0 = psm_mog_rgb_processor::prob_density(model, vil_rgb<float>(0.2f, 0.6f, 0.85f));
    float p1 = psm_mog_rgb_processor::prob_density(model, vil_rgb<float>(0.65f, 0.12f, 0.84f));

    vcl_cout << "p0 = " << p0 << vcl_endl;
    vcl_cout << "p1 = " << p1 << vcl_endl;

  }


  return;
}



TESTMAIN( test_psm_mog_rgb_processor );

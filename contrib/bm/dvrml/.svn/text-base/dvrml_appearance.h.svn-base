//this is /contrib/bm/dvrml/dvrml_appearance.h
#ifndef dvrml_appearance_h_
#define dvrml_appearance_h_

#include<vbl/vbl_ref_count.h>

#include<vnl/vnl_vector_fixed.h>

class dvrml_appearance: public vbl_ref_count
{
public:

    dvrml_appearance( vnl_vector_fixed<double,3> const& color_rgb = vnl_vector_fixed<double,3>(1.0,0.0,0.0),
                      double const& transparency = 0.0,
                      double const& ambientIntensity = 1.0,
                      vnl_vector_fixed<double,3> const& emissiveColor = vnl_vector_fixed<double,3>(0.0,0.0,0.0),
                      vnl_vector_fixed<double,3> const& specularColor = vnl_vector_fixed<double,3>(0.0,0.0,0.0),
                      double const& shininess = 0.0 ): 
                      diffuseColor_(color_rgb),  transparency_(transparency), ambientIntensity_(ambientIntensity),emissiveColor_(emissiveColor),
                      shininess_(shininess){}

    ~dvrml_appearance(){}

    //SETTERS
    void set_diffuseColor( vnl_vector_fixed<double,3> const& color_rgb ){ diffuseColor_ = color_rgb; }

    void set_ambientIntensity( double const& ambientIntensity ){ ambientIntensity_ = ambientIntensity; }

    void set_emissiveColor( vnl_vector_fixed<double,3> const& emissiveColor ){ emissiveColor_ = emissiveColor; }

    void set_specularColor( vnl_vector_fixed<double,3> const& specularColor ){ specularColor_ = specularColor; }

    void set_shininess( double const& shininess ){ shininess_ = shininess; }

    void set_transparency( double const& transparency ){ transparency_ = transparency; }

    //GETTERS
    vnl_vector_fixed<double,3> diffuseColor(){ return diffuseColor_; }

    double ambientIntensity(){ return ambientIntensity_; }

    vnl_vector_fixed<double,3> emissiveColor(){ return emissiveColor_; }

    vnl_vector_fixed<double,3> specularColor(){ return specularColor_; }

    double shininess(){ return shininess_; }

    double transparency(){ return transparency_; }

protected:
    vnl_vector_fixed<double,3> diffuseColor_;
    double ambientIntensity_;
    vnl_vector_fixed<double,3> emissiveColor_;
    vnl_vector_fixed<double,3> specularColor_;
    double shininess_;
    double transparency_;
};

#endif //dvrml_appearance_h_
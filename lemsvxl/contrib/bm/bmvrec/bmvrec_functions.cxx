#include "bmvrec_functions.h"

vcl_map<vcl_string,vil_image_view<float> > bmvrec_functions::load_image_map_float(const vcl_string& fullImgDirectory)
{
    vcl_map<vcl_string,vil_image_view<float> > img;
    vcl_string imgDir = fullImgDirectory + "*.png";

    for(vul_file_iterator fn = imgDir; fn; ++fn)
    {
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "Loading Image: " << filename << '\n';
        img.insert(vcl_make_pair(filename,vil_convert_cast(float(),vil_load(fn()))));
    }

    return img;    
}

vcl_map<vcl_string,vil_image_view<bool> > bmvrec_functions::load_image_map_bool(const vcl_string& fullImgDirectory)
{
    vcl_map<vcl_string,vil_image_view<bool> > gt;
    vcl_string gtDir = fullImgDirectory + "*.png";
    
    for(vul_file_iterator fn = gtDir; fn; ++fn)
    {
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "Loading Image: " << filename << '\n';
        gt.insert(vcl_make_pair(filename,vil_convert_cast(bool(),vil_load(fn()))));
    }
    return gt;
}

void bmvrec_functions::scale_float(vil_image_view<float>& img, const float& max)
{
    vil_image_view<float>::iterator vit = img.begin();
    vil_image_view<float>::iterator itEnd = img.end();
    
    for(;vit!=itEnd;++vit)
        *vit = (*vit)/max;
}

vil_image_view<float> bmvrec_functions::apply_extrema(vil_image_view<float>& img)
{
    // apply extrema operator
    // point response (output of non-maximal supression) is always returned to plane 0. 
    // if output_response_mask = true, the output response mask will go to plane 1.
    // if the output_response_mask = flase and unclipped_response = true, unclipped response goes to plane 1
    // if output_response_mask = tue and unclipped_response = true, output response goes to plane 1 and
    // unclipped response goes to plane 2

    vil_image_view<float> output;

    vcl_cout << "Applying Extrema Operator to image..." << vcl_endl;
    float lambda0 = 2.0f, lambda1 = 1.0f, theta = -45.0f;
    bool bright = true, output_response_mask = false, unclipped_response = false;
    
    output = brip_vil_float_ops::fast_extrema(img,lambda0,lambda1,theta,bright,output_response_mask,unclipped_response);

    return output;
}

void bmvrec_functions::write_op_m(vcl_ofstream& os, const vil_image_view<float>& opRep, const vcl_string& imgName)
{
    vil_image_view<float>::const_iterator vit = opRep.begin();
    vil_image_view<float>::const_iterator itEnd = opRep.end();

    vcl_cout << "Writing " << imgName << " to disk..." << vcl_endl;

    vcl_string varName = imgName + "_opRep";

    os << '\n';
    os << varName << " = [";
    for(;vit!=itEnd;++vit)
        if(*vit > 1.0e-3f)
            os << *vit << '\t';
    os << "];\n";
}

bool bmvrec_functions::check_gt(const vcl_string& currImg)
{
    if(currImg=="normalized0"||currImg=="normalized1"||currImg=="normalized3"||currImg=="normalized4"||
          currImg=="normalized6"||currImg=="normalized10"||currImg=="normalized16")
    {
        vcl_cout << currImg << " has a ground truth..." << vcl_endl;
        return true;
    }
    else
        return false;
}

void bmvrec_functions::extract_fg_opRep(const vil_image_view<float>& opRep, const vil_image_view<bool>& gt,
                                        const vcl_string& currImg, vcl_ofstream& os)
{

    unsigned ni = gt.ni(), nj = gt.nj(), repPlane = 0;
    float thresh = 1.0e-3f;
    vcl_string varName = currImg+"_fgOpRep";
    
    vcl_cout << "Writing foreground operator response to disk..." << vcl_endl;

    os << '\n';
    os << varName << " = [";

    for(unsigned i = 0; i < ni; ++i)
        for(unsigned j = 0; j < nj; ++j)                                   
            if(gt(i,j) == 1 && opRep(i,j,repPlane) > thresh)
            {
                os << opRep(i,j,repPlane) << '\t';
                
            }
           
    os <<"];\n";

    
    
    
    
}



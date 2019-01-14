#include "bmvrec_functions.h"

std::map<std::string,vil_image_view<float> > bmvrec_functions::load_image_map_float(const std::string& fullImgDirectory)
{
    std::map<std::string,vil_image_view<float> > img;
    std::string imgDir = fullImgDirectory + "*.png";

    for(vul_file_iterator fn = imgDir; fn; ++fn)
    {
        std::string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        std::cout << "Loading Image: " << filename << '\n';
        img.insert(std::make_pair(filename,vil_convert_cast(float(),vil_load(fn()))));
    }

    return img;    
}

std::map<std::string,vil_image_view<bool> > bmvrec_functions::load_image_map_bool(const std::string& fullImgDirectory)
{
    std::map<std::string,vil_image_view<bool> > gt;
    std::string gtDir = fullImgDirectory + "*.png";
    
    for(vul_file_iterator fn = gtDir; fn; ++fn)
    {
        std::string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        std::cout << "Loading Image: " << filename << '\n';
        gt.insert(std::make_pair(filename,vil_convert_cast(bool(),vil_load(fn()))));
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

    std::cout << "Applying Extrema Operator to image..." << std::endl;
    float lambda0 = 2.0f, lambda1 = 1.0f, theta = -45.0f;
    bool bright = true, output_response_mask = false, unclipped_response = false;
    
    output = brip_vil_float_ops::fast_extrema(img,lambda0,lambda1,theta,bright,output_response_mask,unclipped_response);

    return output;
}

void bmvrec_functions::write_op_m(std::ofstream& os, const vil_image_view<float>& opRep, const std::string& imgName)
{
    vil_image_view<float>::const_iterator vit = opRep.begin();
    vil_image_view<float>::const_iterator itEnd = opRep.end();

    std::cout << "Writing " << imgName << " to disk..." << std::endl;

    std::string varName = imgName + "_opRep";

    os << '\n';
    os << varName << " = [";
    for(;vit!=itEnd;++vit)
        if(*vit > 1.0e-3f)
            os << *vit << '\t';
    os << "];\n";
}

bool bmvrec_functions::check_gt(const std::string& currImg)
{
    if(currImg=="normalized0"||currImg=="normalized1"||currImg=="normalized3"||currImg=="normalized4"||
          currImg=="normalized6"||currImg=="normalized10"||currImg=="normalized16")
    {
        std::cout << currImg << " has a ground truth..." << std::endl;
        return true;
    }
    else
        return false;
}

void bmvrec_functions::extract_fg_opRep(const vil_image_view<float>& opRep, const vil_image_view<bool>& gt,
                                        const std::string& currImg, std::ofstream& os)
{

    unsigned ni = gt.ni(), nj = gt.nj(), repPlane = 0;
    float thresh = 1.0e-3f;
    std::string varName = currImg+"_fgOpRep";
    
    std::cout << "Writing foreground operator response to disk..." << std::endl;

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



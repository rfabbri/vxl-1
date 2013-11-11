#include"bmvr_functions.h"


void bmvr_functions::checkBinary(vcl_ostream &os, vil_image_view<vxl_byte>& img)
{
    bool check = true;
    os << "Checking if vxl_byte Image is Binary..." << vcl_endl;
    vil_image_view<vxl_byte>::iterator vit = img.begin();
    vil_image_view<vxl_byte>::iterator endIt = img.end();
    for( ; vit!=endIt; ++vit)
    {
        if(*vit != 0 || *vit != 1)
            check = true;
        else
            check = false;
    }
    if(check == false)
        os << "Image is not Binary!" << vcl_endl;
    else
        os << "It is Binary!" << vcl_endl;

    
}

void bmvr_functions::convertByte_Bool(const vil_image_view<vxl_byte>& in, vil_image_view<bool>& out)
{
    unsigned ni = in.ni(),
             nj = in.nj(),
             nplanes = in.nplanes();
    out.set_size(ni,nj,nplanes);

    for(unsigned i = 0; i < ni; ++i)
        for(unsigned j = 0; j < nj; ++j)
            out(i,j) = static_cast<bool>(in(i,j));
    
}

void bmvr_functions::convertBool_Byte(const vil_image_view<bool>& in, vil_image_view<vxl_byte>& out)
{
    unsigned ni = in.ni(),
             nj = in.nj(),
             nplanes = in.nplanes();
    
    out.set_size(ni,nj,nplanes);

    for(unsigned i = 0; i < ni; ++i)
        for(unsigned j = 0; j < nj; ++j)
            out(i,j) = static_cast<vxl_byte>(in(i,j));
}

bool bmvr_functions::fit_dist_to_response_hist(bsta_histogram<float>& hist, float& k, float& lambda)
{
    float mean = hist.mean(); float std_dev = (float)vcl_sqrt(hist.variance());
    bsta_weibull_cost_function wcf(mean,std_dev);
    bsta_fit_weibull<float> fw(&wcf);
    k = 1.0f;
    
    if( fw.init(k) ) {
        fw.solve(k);
        vcl_cout << "Weibull k fit with residual " << fw.residual() << '\n';
        lambda = fw.lambda(k);
        vcl_cout << "k = " << k << " lambda = " << lambda << '\n';
    }
    else
    { //weibull cannot be fit!
        return false;
    }
    return true;
}

void bmvr_functions::write_prob_dist_to_m(bsta_histogram<float>& hist, vcl_ofstream& os)
{
    float delta = hist.delta(), area = hist.area()*delta, min = hist.min(), max = hist.max(),
          nbins = hist.nbins();
    
    os << "x = [" << min;
    for(unsigned i = 1; i < nbins; ++i)
        os << ", " << min + i*delta;
    os << "];\n";

    os << "y = [" << hist.counts( (unsigned char) 0)/area;    
    for(unsigned i = 1; i < nbins; ++i)
        os << ", " << hist.counts(i)/area;
    os << "];\n";
    os << "bar(x,y,'r')\n";
}

void bmvr_functions::write_prob_dist_weibull_m(bsta_histogram<float>& hist,vcl_ofstream& os,float& scale, float& shape)
{
        float delta = hist.delta(), area = hist.area()*delta, min = hist.min(), max = hist.max(),
              nbins = hist.nbins(), weiWidth = 200;
    
    os << "x = [" << min;
    for(unsigned i = 1; i < nbins; ++i)
        os << ", " << min + i*delta;
    os << "];\n";

    os << "y = [" << hist.counts( (unsigned char) 0)/area;    
    for(unsigned i = 1; i < nbins; ++i)
        os << ", " << hist.counts(i)/area;
    os << "];\n";
    os << "bar(x,y,'r')\n\n";

    os << "scale = " << scale << ";\n";
    os << "shape = " << shape << ";\n";
    os << "xgrid = linspace(0," << max << "," << weiWidth << ");\n";
    os << "weiEst = wblpdf(xgrid," << scale  << "," << shape <<");\n";
    os << "hold on,plot(xgrid,weiEst);\n";

    
}

// void bmvr_functions::write_prob_bin_weibull_m(bsta_histogram<float>& hist,vcl_ofstream& os, float& scale, float& shape, float& minW, float& maxW)
// {
//     bsta_weibull<float>(minW
// }

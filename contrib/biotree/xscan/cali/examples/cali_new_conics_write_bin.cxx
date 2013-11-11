#include <vcl_iostream.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_conic_2d.h>
#include <vil/vil_load.h>
#include <vul/vul_file.h>
#include <vil/vil_crop.h>
#include <vil/vil_convert.h>
#include <vcl_cstddef.h>
#include <cali/cali_artf_corresponder.h>
#include <cali/cali_param.h>

#include <vul/vul_timer.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vgl/algo/vgl_fit_conics_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_sel.h>
#include <dbdet/sel/dbdet_sel_sptr.h>
#include <dbdet/edge/dbdet_third_order_edge_det.h>


conic_vector_set new_fit_conics(const cali_artf_corresponder& corresp, vil_image_resource_sptr res){

        conic_vector_set toreturn;
        //----------------------------------------------------------------------------------------------
        //third-order edge detection

        vil_image_view<vxl_uint_16> view_16 = res->get_view();
   //     vil_image_view<vxl_byte> image_view = res->get_view();
        vil_image_view<vxl_byte> image_view;
        vil_convert_stretch_range(view_16,image_view);


        //get the parameters
        unsigned grad_op, parabola_fit;
        double sigma, thresh;
        int N;
        bool reduce_tokens;
        grad_op= 0; // 0 = gaussian operator, 1 = h0, 2 = h1  
        N = 1;      //interpolation factor 2^N
        reduce_tokens = 1;
        sigma= 1;
        thresh= 3; // gradient mag. threshold
        parabola_fit = 0 ; // 0 = 3-point fit, 1 = 9-point fit

        // perfrom third-order edge detection with these parameters
        dbdet_edgemap_sptr edge_map = dbdet_detect_third_order_edges(image_view,
                        sigma,
                        thresh,
                        N,
                        parabola_fit,
                        grad_op,
                        reduce_tokens);


        //end third-order edge detection
        //----------------------------------------------------------------------------------------------
        // symbolic edge linking
        
        //neighborhood radius
        double nrad = 3;
        //pos. uncertainty
        double  dx = 0.2;
        //orientation uncertainty
        double  dt = 15;
        unsigned curve_model_type = 2;
        unsigned grouping_algo = 2; 
        unsigned linking_algo = 2;
        unsigned max_size_to_group = 7;
        unsigned min_size_to_link = 4;
        bool extract_contours = 1;
        bool output_vsol = 1;
        unsigned min_size_to_keep = 3;


        //different types of linkers depending on the curve model
        typedef dbdet_sel<dbdet_simple_linear_curve_model> dbdet_sel_simple_linear;
        typedef dbdet_sel<dbdet_linear_curve_model> dbdet_sel_linear;
        typedef dbdet_sel<dbdet_CC_curve_model> dbdet_sel_CC;
        typedef dbdet_sel<dbdet_CC_curve_model_perturbed> dbdet_sel_CC_perturbed;
        typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
        typedef dbdet_sel<dbdet_ES_curve_model_perturbed> dbdet_sel_ES_perturbed;

        //start the timer
        vul_timer t;

        //construct the linker
        dbdet_sel_sptr edge_linker;
        switch (curve_model_type)
        {
                case 0: //simple linear_model
                        edge_linker = new dbdet_sel_simple_linear(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
                case 1: //linear_model
                        edge_linker = new dbdet_sel_linear(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
                case 2: //CC_model
                        edge_linker = new dbdet_sel_CC(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
                case 3: //CC_model with discrete perturbations
                        edge_linker = new dbdet_sel_CC_perturbed(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
                case 4: //ES_model
                        edge_linker = new dbdet_sel_ES(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
                case 5: //ES_model with discrete perturbations
                        edge_linker = new dbdet_sel_ES_perturbed(edge_map, nrad, dt*vnl_math::pi/180, dx);
                        break;
        }

        //perform local edgel grouping
        switch (grouping_algo)
        {
                case 0: //combinatorial grouping
                        edge_linker->build_curvelets_using_combination_rules();
                        break;
                case 1: //hierarchical grouping (breadth-first grouping)
                        edge_linker->build_curvelets_hierarchically();
                        break;
                case 2: //greedy (depth first grouping)
                        edge_linker->build_curvelets_greedy(max_size_to_group);
                        break;
                case 3: //extra greedy (depth first grouping)
                        edge_linker->build_curvelets_greedy(max_size_to_group, true);
                        break;
        }

        double group_time = t.real() / 1000.0;
        t.mark();

        //form a link graph
        edge_linker->construct_the_link_graph(min_size_to_link, linking_algo);

        //extract contours from the link graph
        if (extract_contours)
                edge_linker->extract_image_contours();

        double link_time = t.real() / 1000.0;

        vcl_cout << "Time taken to form groups: " << group_time << " sec" << vcl_endl;
        vcl_cout << "Time taken to link: " << link_time << " sec" << vcl_endl;

        //report stats
        edge_linker->report_stats();
        //edge_linker->determine_accuracy_of_measurements();

        vcl_vector< vsol_polyline_2d_sptr > image_curves;

        for (unsigned i=0; i<edge_linker->edgel_chains().size(); i++){
                //only keep the longer contours
                if (edge_linker->edgel_chains()[i]->edgels.size() >= min_size_to_keep){
                        vcl_vector<vsol_point_2d_sptr> pts;
                        for (unsigned j=0; j<edge_linker->edgel_chains()[i]->edgels.size(); j++)
                                pts.push_back(new vsol_point_2d(edge_linker->edgel_chains()[i]->edgels[j]->pt));
                        vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);
                        image_curves.push_back(new_curve);
                }
        }

        vcl_cerr << "image curves.size() = " << image_curves.size() << "\n";

        //end  symbolic edge linking
        //---------------------------------------------------------------------------------------------- 
        //conic fitting
        vcl_vector<vsol_conic_2d_sptr> conic_segs; 
        double aspect_ratio = 4;
        int min_fit_length = 40;
        vgl_fit_conics_2d<double> fitter;//the fitting class
        fitter.set_min_fit_length(min_fit_length);
        fitter.set_rms_error_tol(1);

        for (vcl_vector<vsol_polyline_2d_sptr>::iterator curve = image_curves.begin();
                        curve != image_curves.end(); ++curve)
        {
                fitter.clear();
                vsol_polyline_2d_sptr polyline = *curve;
                if(polyline->size() < min_fit_length)
                        continue;
                int vertex_ct = 0;
                while(polyline->valid_index(vertex_ct)){
                        vsol_point_2d_sptr vertex = polyline->vertex(vertex_ct);
                        vgl_point_2d<double> p(vertex->x(),vertex->y());
                        fitter.add_point(p);
                        vertex_ct++;
                }

                fitter.fit();
                vcl_vector<vgl_conic_segment_2d<double> >& segs = fitter.get_conic_segs();

                for (vcl_vector<vgl_conic_segment_2d<double> >::iterator sit=segs.begin();
                                sit != segs.end(); sit++)
                {
                        vsol_conic_2d_sptr conic = new vsol_conic_2d(*sit);
                        vcl_cout << "Fitted a conic of type " << conic->real_type() << '\n';
                        //adding a condition on aspect ratio
                        if(conic->is_real_ellipse()){
                                if (conic->real_type() != 1)
                                {
                                        conic_segs.push_back(conic);
                                }
                                else
                                {
                                        double cx, cy, width, height, angle;
                                        conic->ellipse_parameters(cx,cy,angle,width,height);
                                        if ((width/height)<aspect_ratio)
                                                conic_segs.push_back(conic);
                                }
                        }

                }
        }

        //----------------------------------------------------------------------------------------------

        return corresp.combine_conics( conic_segs); 
}


int main(int argc, char* argv[]) {

        if(argc < 2 ) {
                vcl_cerr << "Usage: " << argv[0] << " <parameter file>\n";
                exit(1);
        }
        vcl_string path = argv[1];
        cali_param par(path);
        vcl_string fname = par.LOGFILE;
        cali_artf_corresponder corresp(par);
        int start=par.START, end=par.END;

        vcl_string txt_file =  corresp.gen_write_fname(fname, 0);
        vcl_string dir = vul_file::dirname(txt_file);
        vcl_string ext = vul_file::extension(txt_file);
        vcl_string base = vul_file::basename(txt_file,ext.c_str());

        #if defined(VCL_WIN32)
        txt_file = dir + "\\" +  base + ".log";
        #else
        txt_file = dir + "/" + base + ".log";
        #endif
        vcl_cout << "txt_file is " << txt_file << "\n";

        vcl_ofstream fstream(txt_file.c_str());



        for (int i=start; i <=end; i+=par.INTERVAL) {
                vcl_string outname = corresp.gen_write_fname(fname, i);
                dir = vul_file::dirname(outname);
                ext = vul_file::extension(outname);
                base = vul_file::basename(outname,ext.c_str());

                #if defined(VCL_WIN32)
                fname = dir + "\\" +  base + ".tif";
                #else
                fname = dir + "/" + base + ".tif";
                #endif

                vcl_cout << "loading " << fname << "\n";
                vil_image_resource_sptr img = vil_load_image_resource(fname.c_str());

//                conic_vector_set conics = corresp.fit_conics(img);
                conic_vector_set conics = new_fit_conics(corresp,img);

                // generate file name with .bin extension

                vcl_cout << "writing " << outname << "\n";
                corresp.save_conics_bin(conics, outname);

                // also save into a txt file
                fstream << "Conic File Number:" << i << "\n";
                corresp.print(fstream, conics);
        }
        return 0;

}

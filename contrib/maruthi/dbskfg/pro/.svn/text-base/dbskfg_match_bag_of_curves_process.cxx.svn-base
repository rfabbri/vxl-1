// This is brcv/shp/dbskfg/pro/dbskfg_match_bag_of_curves_process.cxx

//:
// \file

// dbskfg headers
#include <dbskfg/pro/dbskfg_match_bag_of_curves_process.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <bbas/bsol/bsol_intrinsic_curve_2d_sptr.h>
#include <bbas/bsol/bsol_intrinsic_curve_2d.h>

#include <dbcvr/dbcvr_cvmatch.h>
#include <vsol/vsol_polyline_2d.h>

#include <vnl/vnl_matrix.h>

//: Constructor
dbskfg_match_bag_of_curves_process::dbskfg_match_bag_of_curves_process()
{
    if (!parameters()->add( "Model folder:" , 
                            "-model_folder" , bpro1_filepath("", ".cem")) ||
        !parameters()->add( "Query folder:" , 
                            "-query_folder" , bpro1_filepath("", ".cem")) ||
        !parameters()->add( "Output folder:", 
                            "-output_folder", bpro1_filepath("", "")) ||
        !parameters()->add( "Output file prefix:" , 
                            "-output_prefix", vcl_string(""))||
        !parameters()->add( "Template size: " , "-template_size" , 3 )  ||
        !parameters()->add( "R:" , "-r1" , 10.0f ) 
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }

}

//: Destructor
dbskfg_match_bag_of_curves_process::~dbskfg_match_bag_of_curves_process()
{
}

//: Clone the process
bpro1_process*
dbskfg_match_bag_of_curves_process::clone() const
{
    return new dbskfg_match_bag_of_curves_process(*this);
}

vcl_string
dbskfg_match_bag_of_curves_process::name()
{
    return "Match Bag of Curves";
}

vcl_vector< vcl_string >
dbskfg_match_bag_of_curves_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
    
}

vcl_vector< vcl_string >
dbskfg_match_bag_of_curves_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.clear();
    return to_return;
}

int dbskfg_match_bag_of_curves_process::input_frames()
{
    return 1;
}

int dbskfg_match_bag_of_curves_process::output_frames()
{
    return 1;
}

bool dbskfg_match_bag_of_curves_process::execute()
{
    bpro1_filepath model_folder_filepath;
    this->parameters()->get_value("-model_folder", model_folder_filepath);
    vcl_string model_dir = model_folder_filepath.path;
    
    bpro1_filepath query_folder_filepath;
    this->parameters()->get_value("-query_folder", query_folder_filepath);
    vcl_string query_dir = query_folder_filepath.path;

    bpro1_filepath output_folder_filepath;
    this->parameters()->get_value("-output_folder", output_folder_filepath);
    vcl_string output_folder = output_folder_filepath.path;

    vcl_string output_prefix;
    parameters()->get_value("-output_prefix", output_prefix);
 
    vcl_string output_file = output_folder+"/"+output_prefix;

    int template_size=0; 
    parameters()->get_value( "-template_size" , template_size );

    float r1=0; 
    parameters()->get_value( "-r1" , r1 );

    
    //Load contours

    // Create output storage
    vcl_vector<bpro1_storage_sptr> model_contour;
    vcl_vector<bpro1_storage_sptr> query_contour;

    load_contours(model_contour,model_folder_filepath);
    load_contours(query_contour,query_folder_filepath);


    // Grab the underlying contours
    vidpro1_vsol2D_storage_sptr model_contour_storage = 
        vidpro1_vsol2D_storage_new();
    model_contour_storage.vertical_cast(model_contour[0]);
    vcl_vector< vsol_spatial_object_2d_sptr > model_vsol_list = 
        model_contour_storage
        ->all_data();
    vcl_vector<bsol_intrinsic_curve_2d_sptr> model_curves;

    for ( unsigned int i=0; i < model_vsol_list.size() ; ++i)
    {
        if ( model_vsol_list[i]->cast_to_curve()
             ->cast_to_polyline() )
        {
            vsol_polyline_2d_sptr poly_line= model_vsol_list[i]->cast_to_curve()
                ->cast_to_polyline();
            if ( poly_line->p0()->get_p() != poly_line->p1()->get_p())
            {
                bsol_intrinsic_curve_2d_sptr c1=new bsol_intrinsic_curve_2d(
                    poly_line);
                model_curves.push_back(c1);
            }
        }
    }

    // Grab the underlying contours
    vidpro1_vsol2D_storage_sptr query_contour_storage = 
        vidpro1_vsol2D_storage_new();
    query_contour_storage.vertical_cast(query_contour[0]);
    vcl_vector< vsol_spatial_object_2d_sptr > query_vsol_list = 
        query_contour_storage
        ->all_data();
    vcl_vector<bsol_intrinsic_curve_2d_sptr> query_curves;

    for ( unsigned int i=0; i < query_vsol_list.size() ; ++i)
    {
        if ( query_vsol_list[i]->cast_to_curve()
             ->cast_to_polyline() )
        {
            vsol_polyline_2d_sptr poly_line= query_vsol_list[i]->cast_to_curve()
                ->cast_to_polyline();
            if ( poly_line->p0()->get_p() != poly_line->p1()->get_p())
            {
                bsol_intrinsic_curve_2d_sptr c1=new bsol_intrinsic_curve_2d(
                    poly_line);
                query_curves.push_back(c1);
            }
        }
    }

    vnl_matrix<double> binary_sim_matrix(model_curves.size(),
                                         query_curves.size());

    // Now perform matching
    for ( unsigned int m=0; m < model_curves.size() ; ++m)
    {
        for ( unsigned int q=0; q < query_curves.size() ; ++q)
        {
            bsol_intrinsic_curve_2d_sptr curve1=model_curves[m];
            bsol_intrinsic_curve_2d_sptr curve2=query_curves[q];

            bsol_intrinsic_curve_2d_sptr curve2_flipped = new 
                bsol_intrinsic_curve_2d();
           
            for (int i = curve2->size()-1; i>=0 ; i--)
            {
                curve2_flipped->add_vertex(curve2->x(i), curve2->y(i));
            }
            
            double cost1(0),cost2(0);

            // Perform First Match curve 1 to curve 2
            {
                dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();
                curveMatch->setCurve1(curve1);
                curveMatch->setCurve2(curve2);
                curveMatch->setStretchCostFlag(true); 
                curveMatch->Match();
                cost1 = curveMatch->finalCost();

            }


            // Perform First Match curve 1 to curve 2 flipped
            {
                dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();
                curveMatch->setCurve1(curve1);
                curveMatch->setCurve2(curve2_flipped);
                curveMatch->setStretchCostFlag(true); 
                curveMatch->Match();
                cost2 = curveMatch->finalCost();

            }

            double final_cost=(cost1<cost2)?cost1:cost2;
            binary_sim_matrix[m][q]=final_cost;                   
            
            vcl_cout<<"Curve Matching Cost: "<<final_cost
                    <<" min("
                    <<cost1
                    <<","
                    <<cost2
                    <<")"
                    <<vcl_endl;
        }
    }

    vcl_string output_binary_file = output_file + 
        "_contour_similarity_matrix.bin";

    vcl_ofstream binary_sim_file;
    binary_sim_file.open(output_binary_file.c_str(),
                         vcl_ios::out | 
                         vcl_ios::binary);

    //********************* Write model curves first *****************
    double model_curves_numb=model_curves.size();
    binary_sim_file.write(reinterpret_cast<char *>(&model_curves_numb),
                          sizeof(double));
   
    // Now perform matching
    for ( unsigned int m=0; m < model_curves.size() ; ++m)
    {
        bsol_intrinsic_curve_2d_sptr curve1=model_curves[m];

        double curve_size=curve1->size();
        binary_sim_file.write(reinterpret_cast<char *>(&curve_size),
                              sizeof(double));
        
        for ( unsigned int v=0; v < curve1->size() ; ++v)
        {
            vsol_point_2d_sptr vertex=curve1->vertex(v);
            double x = vertex->x();
            double y = vertex->y();

            binary_sim_file.write(reinterpret_cast<char *>(&x),
                              sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y),
                              sizeof(double));


        }
    }

    //********************* Write query curves first *****************
    double query_curves_numb=query_curves.size();
    binary_sim_file.write(reinterpret_cast<char *>(&query_curves_numb),
                          sizeof(double));
   
    // Now perform matching
    for ( unsigned int m=0; m < query_curves.size() ; ++m)
    {
        bsol_intrinsic_curve_2d_sptr curve1=query_curves[m];

        double curve_size=curve1->size();
        binary_sim_file.write(reinterpret_cast<char *>(&curve_size),
                              sizeof(double));
        
        for ( unsigned int v=0; v < curve1->size() ; ++v)
        {
            vsol_point_2d_sptr vertex=curve1->vertex(v);
            double x = vertex->x();
            double y = vertex->y();

            binary_sim_file.write(reinterpret_cast<char *>(&x),
                              sizeof(double));
            binary_sim_file.write(reinterpret_cast<char *>(&y),
                              sizeof(double));


        }
    }

    double matrix_rows=binary_sim_matrix.rows();
    double matrix_columns=binary_sim_matrix.columns();

    binary_sim_file.write(reinterpret_cast<char *>(&matrix_rows),
                          sizeof(double));
    binary_sim_file.write(reinterpret_cast<char *>(&matrix_columns),
                          sizeof(double));

    for ( unsigned int c=0; c < binary_sim_matrix.columns() ; ++c)
    {
        for ( unsigned int r=0; r < binary_sim_matrix.rows() ; ++r)
        {
            double value=binary_sim_matrix[r][c];
            binary_sim_file.write(reinterpret_cast<char *>(&value),
                                  sizeof(double));
            
        }
    } 
    
    binary_sim_file.close();

    //Set storage with new transform graph
    clear_output();
  
    bool status=true;
    return status;
}

bool dbskfg_match_bag_of_curves_process::finish()
{
    return true;
}

void dbskfg_match_bag_of_curves_process::load_contours(
    vcl_vector<bpro1_storage_sptr>&
    vsol_contour,
    bpro1_filepath& input)
{
    
    // Call vidpro load cem process
    vidpro1_load_cem_process load_pro_cem;
    load_pro_cem.parameters()->set_value("-ceminput",input);

    // Before we start the process lets clean input output
    load_pro_cem.clear_input();
    load_pro_cem.clear_output();

    // Pass in input vsol string
    bool load_cem_status = load_pro_cem.execute();
    load_pro_cem.finish();

    // Grab output from symbolic edge linking
    if ( load_cem_status )
    {
        vsol_contour = load_pro_cem.get_output();
    }

    //Clean up after ourselves
    load_pro_cem.clear_input();
    load_pro_cem.clear_output();

}


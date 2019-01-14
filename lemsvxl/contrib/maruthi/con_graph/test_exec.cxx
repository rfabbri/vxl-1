// dbgl headers
#include <dbskfg/pro/dbskfg_prune_fragments_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vul/vul_file.h>
#include <vidpro1/process/vidpro1_load_cem_process.h>
#include <dbsk2d/pro/dbsk2d_compute_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vul/vul_timer.h>
#include <dbsk2d/algo/dbsk2d_ishock_loop_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap4_transform.h>
#include <dbsk2d/algo/dbsk2d_ishock_gap_detector.h>
#include <dbsk2d/pro/dbsk2d_prune_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_sample_ishock_process.h>
#include <dbsk2d/pro/dbsk2d_save_esf_process.h>
#include <dbsk2d/pro/dbsk2d_compute_containment_graph_process.h>
#include <vsol/vsol_line_2d.h>
#include <dbsk2d/dbsk2d_file_io.h>
#include <dbsk2d/algo/dbsk2d_ishock_grouping_transform.h>
#include <dbsk2d/dbsk2d_containment_graph.h>
#include <dbsk2d/dbsk2d_transform_manager.h>

unsigned int gap_type=1;

void prune_sample_shock(dbsk2d_shock_storage_sptr storage,
                        std::string file_name);

bool loop_transform(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                    output_shock,std::string input_extension,bool write_data,
                    unsigned int& numb_transforms);

bool loop_transform3(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                     output_shock,std::string input_extension,bool write_data,
                     unsigned int& numb_transforms);

bool gap_transform(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                    output_shock,std::string input_extension,bool write_data,
                    unsigned int& numb_transforms);

bool gap_transform(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                    output_shock,std::string input_extension,bool write_data,
                    unsigned int& numb_transforms)
{

    bool flag=true;

    std::vector< std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bpoint*> >
        gap_pairs;

    std::vector< std::pair<dbsk2d_ishock_bpoint*,dbsk2d_ishock_bline*> >
        gap4_pairs;

    dbsk2d_ishock_gap_detector detector(output_shock->get_ishock_graph());

    if ( gap_type == 1 )
    {
        
        detector.detect_gap1(gap_pairs);
    }
    else
    {
        detector.detect_gap4(gap4_pairs);
    }
    unsigned int trans=0;
    unsigned int index=0;
    if (gap_type==1 )
    {
    
        for (; index < gap_pairs.size() ; ++index)
        {

             if ( !(gap_pairs[index].first->is_an_end_point() && 
                    gap_pairs[index].second->is_an_end_point()))
             {
                 continue;
             }

            std::stringstream sstream;
            sstream<<index;
            std::string number=sstream.str();
            std::string extension;
            if ( number.size() == 1 )
            {
                extension="000"+number;
            }
            else if ( number.size() == 2 )
            {
                extension="00"+number;

            }
            else
            {
                extension="0"+number;
            }

            vul_timer timer1;
            timer1.mark();

            dbsk2d_ishock_gap_transform transformer(
                output_shock->get_ishock_graph(),
                gap_pairs[index]);
            flag = transformer.execute_transform();

            double preprocess_time = (double)(timer1.real())/1000;
            std::cout << "Done. Time Local Shock taken= " << 
                preprocess_time << " seconds\n";

            if ( write_data)
            {
                std::string filename = input_extension;
                filename=filename+"_"+extension+"_shock_graph.bnd";
            
                std::string filename2 =input_extension;
                filename2=filename2+"_"+extension+".bnd";
            
                if ( output_shock->get_shock_graph())
                {
                    output_shock->get_shock_graph()->clear();
                }
                transformer.write_boundary(filename2);
                prune_sample_shock(output_shock,filename);
            
            }

            if ( flag == false )
            {
                std::cerr<<"Local Shock computation failed for "<<
                    input_extension
                        <<std::endl;
                break;
            }            
            ++trans;


        }

        index=trans;
    }
    else if ( gap_type == 4 )
    {

    
        for (; index < gap4_pairs.size() ; ++index)
        {
            
            std::stringstream sstream;
            sstream<<index;
            std::string number=sstream.str();
            std::string extension;
            if ( number.size() == 1 )
            {
                extension="000"+number;
            }
            else if ( number.size() == 2 )
            {
                extension="00"+number;
                
            }
            else
            {
                extension="0"+number;
            }

            vul_timer timer1;
            timer1.mark();

            dbsk2d_ishock_bpoint* anchor_pt = dbsk2d_transform_manager
                ::Instance().get_anchor_pt(gap4_pairs[index]);
            
            dbsk2d_ishock_gap4_transform transformer(
                output_shock->get_ishock_graph(),
                gap4_pairs[index],
                anchor_pt);
            flag = transformer.execute_transform();
            
            double preprocess_time = (double)(timer1.real())/1000;
            std::cout << "Done. Time Local Shock taken= " << 
                preprocess_time << " seconds\n";
        
            if ( write_data)
            {
                std::string filename = input_extension;
                filename=filename+"_"+extension+"_shock_graph.bnd";
                
                std::string filename2 =input_extension;
                filename2=filename2+"_"+extension+".bnd";
                
                if ( output_shock->get_shock_graph())
                {
                    output_shock->get_shock_graph()->clear();
                }
                transformer.write_boundary(filename2);
                prune_sample_shock(output_shock,filename);
            
            }

            if ( flag == false )
            {
                std::cerr<<"Local Shock computation failed for "<<
                    input_extension
                        <<std::endl;
                break;
            }            
        
     
        }
    }

    numb_transforms=index;

    return flag;
}

bool loop_transform(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                    output_shock,std::string input_extension,bool write_data,
                    unsigned int& numb_transforms)
{
    bool flag=true;
    unsigned int index=0;
    std::vector<dbsk2d_ishock_belm*> belm_list = boundary->belm_list();
    for (unsigned int i=0;i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_point() )
        {
            dbsk2d_ishock_bpoint* bpoint = 
                dynamic_cast<dbsk2d_ishock_bpoint*>
                (belm_list[i]);

            if ( bpoint->is_an_end_point() && bpoint->is_a_GUIelm())
            {

                dbsk2d_ishock_loop_transform transformer(output_shock
                                                         ->get_ishock_graph(),
                                                         bpoint);
                vul_timer timer1;
                timer1.mark();
                flag = transformer.execute_transform();
                double preprocess_time = (double)(timer1.real())/1000;
                std::cout << "done. Time Local Shock taken= " << 
                    preprocess_time << "seconds\n";
  
                std::stringstream sstream;
                sstream<<index;
                std::string number=sstream.str();
                std::string extension;
                if ( number.size() == 1 )
                {
                    extension="000"+number;
                }
                else if ( number.size() == 2 )
                {
                    extension="00"+number;

                }
                else
                {
                    extension="0"+number;
                }

                std::string filename = input_extension;
                filename=filename+"_"+extension+"_shock_graph.bnd";
                std::string filename2 =input_extension;
                filename2=filename2+"_"+extension+".bnd";
                
                if ( flag == false )
                {
                    bpoint->getInfo(std::cerr);
                    std::cerr<<"Local Shock computation failed for "<<
                        input_extension
                            <<std::endl;
                    break;
                }
                
                if ( write_data )
                {

                    if ( output_shock->get_shock_graph())
                    {
                        output_shock->get_shock_graph()->clear();
                    }
                    transformer.write_boundary(filename2);
                    prune_sample_shock(output_shock,filename);
                }
                index++;
            }
            
        }

    }

    numb_transforms=index;
    return flag;

}


bool loop_transform3(dbsk2d_boundary_sptr boundary,dbsk2d_shock_storage_sptr 
                    output_shock,std::string input_extension,bool write_data,
                    unsigned int& numb_transforms)
{
    bool flag=true;
    unsigned int index=0;
    std::vector<dbsk2d_ishock_belm*> belm_list = boundary->belm_list();
    for (unsigned int i=0;i < belm_list.size() ; ++i)
    {
        if ( belm_list[i]->is_a_point() )
        {
            dbsk2d_ishock_bpoint* bpoint = 
                dynamic_cast<dbsk2d_ishock_bpoint*>
                (belm_list[i]);

            if ( bpoint->nLinkedElms()>=6 && bpoint->is_a_GUIelm())
            {

                dbsk2d_ishock_loop_transform transformer(output_shock
                                                         ->get_ishock_graph(),
                                                         bpoint);
                vul_timer timer1;
                timer1.mark();
                flag = transformer.execute_transform();
                double preprocess_time = (double)(timer1.real())/1000;
                std::cout << "done. Time Local Shock taken= " << 
                    preprocess_time << "seconds\n";
  
                std::stringstream sstream;
                sstream<<index;
                std::string number=sstream.str();
                std::string extension;
                if ( number.size() == 1 )
                {
                    extension="000"+number;
                }
                else if ( number.size() == 2 )
                {
                    extension="00"+number;

                }
                else
                {
                    extension="0"+number;
                }

                std::string filename = input_extension;
                filename=filename+"_"+extension+"_shock_graph.bnd";
                std::string filename2 =input_extension;
                filename2=filename2+"_"+extension+".bnd";
                
                if ( flag == false )
                {
                    bpoint->getInfo(std::cerr);
                    std::cerr<<"Local Shock computation failed for "<<
                        input_extension
                            <<std::endl;
                    break;
                }
                
                if ( write_data )
                {

                    if ( output_shock->get_shock_graph())
                    {
                        output_shock->get_shock_graph()->clear();
                    }
                    transformer.write_boundary(filename2);
                    prune_sample_shock(output_shock,filename);
                }
                index++;
            }
            
        }

    }

    numb_transforms=index;
    return flag;

}

void prune_sample_shock(dbsk2d_shock_storage_sptr storage,std::string file_name)
{
    //std::cout<<"Pruning "<<std::endl;
    dbsk2d_prune_ishock_process process;
    // process.parameters()->set_value("-threshold",(float) 0.0);
    process.clear_input();
    process.clear_output();
    process.add_input(storage);
    process.execute();
    process.finish();

    //now the edges
    dbsk2d_shock_graph_sptr shock_graph = storage->get_shock_graph();
    dbsk2d_shock_graph::edge_iterator curE = shock_graph->edges_begin();

    std::vector<vsol_spatial_object_2d_sptr> line_objects;
    for (; curE != shock_graph->edges_end(); curE++)
    {
        dbsk2d_shock_edge_sptr cur_edge = (*curE);
        std::vector<vgl_point_2d<double> > ex_pts=cur_edge->ex_pts();
        for ( unsigned int i=0; i < ex_pts.size()-1 ; ++i)
        {
            // Add in contours for front 
            vsol_spatial_object_2d_sptr obj=
                new vsol_line_2d(ex_pts[i],
                                 ex_pts[i+1]);
            line_objects.push_back(obj);
        }
    }

    dbsk2d_file_io::save_bnd_v3_0(file_name,line_objects);

    // std::cout<<"Sampling "<<std::endl;
    // dbsk2d_sample_ishock_process sample_sg_pro;
    // sample_sg_pro.clear_input();
    // sample_sg_pro.clear_output();
    // sample_sg_pro.add_input(storage);
    // sample_sg_pro.execute();
    // sample_sg_pro.finish();

    // // Grab output from sampling
    // std::vector<bpro1_storage_sptr> sample_shock_results;
    // sample_shock_results   = sample_sg_pro.get_output();

    // //Clean up after ourselves
    // sample_sg_pro.clear_input();
    // sample_sg_pro.clear_output();

    // std::cout<<"Saving "<<std::endl;
    // dbsk2d_save_esf_process save_sg_pro;           
    // bpro1_filepath output(file_name);
    // save_sg_pro.parameters()->set_value("-esfoutput",output);
    // save_sg_pro.clear_input();
    // save_sg_pro.clear_output();
    // save_sg_pro.add_input(sample_shock_results[0]);
    // save_sg_pro.execute();

    // save_sg_pro.finish();
    // save_sg_pro.clear_input();
    // save_sg_pro.clear_output();


}

int main( int argc, char *argv[] )
{

    std::stringstream stream(argv[1]);
    std::string input_cem_file;
    stream>>input_cem_file;

    std::stringstream stream2(argv[2]);
    unsigned int transform(0);
    stream2>>transform;

    std::stringstream stream3(argv[3]);
    unsigned int write_data(0);
    stream3>>write_data;

    // Create output storage
    std::vector<bpro1_storage_sptr> vsol_contour;

    {
        // Call vidpro load cem process
        vidpro1_load_cem_process load_pro_cem;

        bpro1_filepath input(input_cem_file);

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

 
    // Grab the underlying contours
    vidpro1_vsol2D_storage_sptr vsol_contour_storage = 
        vidpro1_vsol2D_storage_new();
    vsol_contour_storage.vertical_cast(vsol_contour[0]);

    if ( transform == 99 )
    {
        // Call containment graph process
        dbsk2d_compute_containment_graph_process cgraph_pro;

        // Create empty image stroage
        vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

        // Before we start the process lets clean input output
        cgraph_pro.clear_input();
        cgraph_pro.clear_output();

        // Add inputs
        cgraph_pro.add_input(vsol_contour_storage);
        cgraph_pro.add_input(image_storage);


        // Pass in input vsol string
        bool status = cgraph_pro.execute();
        cgraph_pro.finish();

        //Clean up after ourselves
        cgraph_pro.clear_input();
        cgraph_pro.clear_output();

        return 0;
    }

    dbsk2d_compute_ishock_process shock_pro;
    
    // Before we start the process lets clean input output
    shock_pro.clear_input();
    shock_pro.clear_output();

    // Create empty image stroage
    vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();

    // Use input from edge detection
    shock_pro.add_input(image_storage);
    shock_pro.add_input(vsol_contour_storage);
    bool ishock_status = shock_pro.execute();
    shock_pro.finish();

    // Grab output from symbolic edge linking
    std::vector<bpro1_storage_sptr> shock_results;
    shock_results = shock_pro.get_output();

    // Grab the underlying contours
    dbsk2d_shock_storage_sptr output_shock = dbsk2d_shock_storage_new();
    output_shock.vertical_cast(shock_results[0]);


    dbsk2d_boundary_sptr boundary = output_shock->get_boundary();
    dbsk2d_ishock_graph_sptr isg  = output_shock->get_ishock_graph();
    unsigned int numb_transforms=0;
    bool flag=true;

    if ( transform == 0 )
    {
      
        flag=loop_transform(boundary,
                            output_shock,
                            vul_file::strip_extension(input_cem_file),
                            write_data,
                            numb_transforms);
    }
    else if ( transform == 1 )
    {
        gap_type=1;
        flag=gap_transform(boundary,
                           output_shock,
                           vul_file::strip_extension(input_cem_file),
                           write_data,
                           numb_transforms);
        
    }
    else if ( transform == 2 )
    {

        gap_type=4;
        flag=gap_transform(boundary,
                           output_shock,
                           vul_file::strip_extension(input_cem_file),
                           write_data,
                           numb_transforms);


    }
    else if ( transform == 3 )
    {
        flag=loop_transform3(boundary,
                            output_shock,
                            vul_file::strip_extension(input_cem_file),
                            write_data,
                            numb_transforms);
        
    }

    if ( flag )
    {
        if ( transform == 0 )
        {
            std::cerr<<"Shock computation passed for "<<input_cem_file
                    <<" with "<<numb_transforms<<" Loop I Transforms"<<std::endl;
        }
        else if ( transform == 1)
        {
            std::cerr<<"Shock computation passed for "<<input_cem_file
                    <<" with "<<numb_transforms<<" Gap I Transforms"<<std::endl;

        }
        else if ( transform == 2 )
        {
            std::cerr<<"Shock computation passed for "<<input_cem_file
                    <<" with "<<numb_transforms<<" Gap IV Transforms"<<std::endl;

        }
        else if ( transform == 3 )
        {
            std::cerr<<"Shock computation passed for "<<input_cem_file
                    <<" with "<<numb_transforms<<" Loop III Transforms"
                    <<std::endl;

        }
    }

    return 0;
}

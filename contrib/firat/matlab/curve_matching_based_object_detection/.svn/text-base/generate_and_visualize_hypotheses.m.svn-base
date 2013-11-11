% This is /lemsvxl/contrib/firat/matlab/curve_matching_based_object_detection/generate_and_visualize_hypotheses.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Feb 23, 2011

function generate_and_visualize_hypotheses(image_name, model_id, curve_frag_id)
    %img_contours_file = ['/home/firat/Desktop/db/bottle_contours_amir/' image_name '.txt'];
    img_contours_file = ['/home/firat/Desktop/db/ethz_contours_gpb_0.10_kovesi_20/' image_name '_gpb_contours.txt'];
    img_contours = load_contours(img_contours_file);
    if exist('curve_frag_id','var')
        img_contours = img_contours(curve_frag_id);
    end
    %model_contour_file = ['/home/firat/Desktop/db/100bottles/bottle' num2str(model_id) '.txt'];
    %model_contour_file = ['/home/firat/Desktop/db/giraffe_warehouse/exemplar_' num2str(model_id) '.txt'];
    %model_contour_file = ['/home/firat/Desktop/db/mug_warehouse/exemplar_' num2str(model_id) '.txt'];
    model_contour_file = ['/home/firat/Desktop/db/bottle_warehouse/exemplar_' num2str(model_id) '.txt'];
    model_contour = load_contours(model_contour_file);
    model_contour = model_contour{1};
    alphabet = char([0:25] + 'a');
    hypo_out_file = [alphabet(unidrnd(26,1,10)) '.hypo'];
    xgraph_file = ['/home/firat/Desktop/db/100bottles/swan' num2str(model_id) '.xml'];
    ccm_model_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/bottles/bottles-ccm-model-rv4-11082010.xml';
    ccm_param_file = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/ccm_params_orient60_dist4.xml';
    edgemap_folder = '/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_pb_edges_pyramid-clean_using_kovesi-I_15-len_4';
    edgemap_ext = '_pb_edges.png';
    edgeorient_ext = '_pb_orient.txt';
    edgemap_log2_scale_ratio = 0;%0.5;    
    %img_file = ['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/' image_name '.jpg'];
    img_file = ['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals_resized/' image_name '.png'];
    [image_height, image_width, num_bands] = size(imread(img_file));    
    [hypotheses, match_scores] = generate_detection_hypotheses(img_contours, model_contour, 81, 25, 30, hypo_out_file, xgraph_file);   
    
       
    if exist('curve_frag_id','var')
        visualize_hypotheses(hypotheses, match_scores, img_file, model_contour, 5, 6, 300, img_contours{1});  
    else
        log_likelihoods = compute_log_likelihoods(ccm_model_file, ccm_param_file, image_name, edgemap_folder, edgemap_ext, edgeorient_ext, edgemap_log2_scale_ratio, image_width, 300, 1, hypo_out_file);
        visualize_hypotheses(hypotheses, log_likelihoods, img_file, model_contour, 5, 6, 300);
    end  
    
    eval(sprintf('!rm -f %s', hypo_out_file));
end

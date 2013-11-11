function [sorted_confidences, tpfp, num_gt] = evaluate(image_name, image_folder, image_ext, groundtruth_bb_folder, gt_class, detection_folders, record_prefixes, transformations, average_aspect_ratios, confidence_mus, confidence_sigmas, screenshots, output_base_folder)
    
    target_class = strtok(image_name, '_');    
    
    bbs = cell(length(detection_folders), 1);
    confidences = cell(length(detection_folders), 1);
    boundaries = [];
    for i = 1:length(detection_folders)
       [bbs{i}, confidences{i}, temp_boundaries] = predict_bb(image_name, image_folder, image_ext, detection_folders{i}, record_prefixes{i}, transformations{i}, average_aspect_ratios{i}, confidence_mus{i}, confidence_sigmas{i}, screenshots);      
       if screenshots
        if isempty(boundaries)
            boundaries = temp_boundaries;
        else
            boundaries(:,:,end+1:end+size(temp_boundaries,3)) = temp_boundaries;  
        end
       end
    end
   
    
    bbs1 = cell2mat(bbs);
    confidences1 = cell2mat(confidences);    
    
    while size(bbs1,1) > 1
         
        P = pdist(bbs1);
        D = squareform(P);
        min_val = min(P);
        if min_val > 60%-10%60%60%45
            break
        end
        [r,c] = find(D == min_val);
      
        r = r(1); c = c(1);
        cbb = [bbs1(r,:); bbs1(c,:)];
        
        %%Combine bounding boxes
        if 0
            minbb = min(cbb);
            maxbb = max(cbb);
            cbb = zeros(1,4);
            cbb(1) = minbb(1);
            cbb(2) = minbb(2);
            cbb(3) = maxbb(3);
            cbb(4) = maxbb(4);
        elseif 1
            cbb = mean(cbb);
        else
            if confidences1(r) > confidences1(c)
                cbb = bbs1(r,:);
            else
                cbb = bbs1(c,:);
            end
        end
        %%
                
        bbs1(r,:) = [];
        bbs1(c,:) = [];
        bbs1(end+1, :) = cbb;
        confidences1(end+1) = mean([confidences1(r) confidences1(c)]);
        confidences1(r) = [];
        confidences1(c) = []; 
        if screenshots
            boundaries(:,:,end+1) = boundaries(:,:,r) + boundaries(:,:,c);
            boundaries(:,:,r) = [];
            boundaries(:,:,c) = [];     
        end            
    end    
    
    [sorted_confidences, sort_index] = sort(confidences1, 'descend');
    sorted_bbs = bbs1(sort_index,:);
    
    
      
    if screenshots
        sorted_boundaries = boundaries(:,:,sort_index);
        I = imread([image_folder '/' image_name '.' image_ext]);
        mkdir([output_base_folder '/' image_name]);
        for i = 1:size(sorted_bbs,1)
            [r,c] = find(sorted_boundaries(:,:,i) > 0);
            imshow(I);
            hold on;
            bb = sorted_bbs(i,:);
            rectangle('Position', [bb(1) bb(2) bb(3)-bb(1) bb(4)-bb(2)], 'EdgeColor', 'red', 'LineWidth', 3)
            plot(c,r,'b.');            
            hold off
            saveas(gcf, [output_base_folder '/' image_name '/' image_name '_' num2str(i) '.png'], 'png');
            close all
        end
    end
    
    
    if strcmp(gt_class,target_class)
        gt_bbs = load([groundtruth_bb_folder '/' image_name '.groundtruth']);
        num_gt = size(gt_bbs,1);
        already_matched = zeros(num_gt,1);
        tpfp = zeros(size(sorted_confidences));
        for i = 1:length(sorted_confidences)        
            if all(already_matched)
                break
            end
            
            det_bb = sorted_bbs(i,:);
            for j = 1:num_gt
                if already_matched(j) == 0
                    gt_bb = gt_bbs(j,:);
                    pd = pascal_dist(gt_bb, det_bb);
                    if  pd >= 0.5
                        tpfp(i) = 1;
                        already_matched(j) = 1;  
                        break                  
                    end
                end
            end         
        end
    else
        num_gt = 0;
        tpfp = zeros(size(sorted_confidences));
    end    
end

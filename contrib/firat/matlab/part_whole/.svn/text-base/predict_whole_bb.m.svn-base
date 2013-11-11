function predict_whole_bb(image_name, image_folder, image_ext, detection_folders, record_prefixes, transformations, average_aspect_ratios, confidence_mus, confidence_sigmas)
    bbs = cell(length(detection_folders), 1);
    confidences = cell(length(detection_folders), 1);
    for i = 1:length(detection_folders)
       [bbs{i}, confidences{i}] = predict_bb(image_name, image_folder, image_ext, detection_folders{i}, record_prefixes{i}, transformations{i}, average_aspect_ratios{i}, confidence_mus{i}, confidence_sigmas{i});        
    end
    
    bbs1 = cell2mat(bbs);
    confidences1 = cell2mat(confidences);
    
    while size(bbs1,1) > 1
        P = pdist(bbs1);
        D = squareform(P);
        min_val = min(P);
        if min_val > 45
            break
        end
        [r,c] = find(D == min_val);
        r = r(1); c = c(1);
        cbb = [bbs1(r(1),:); bbs1(c(1),:)];
        
        minbb = min(cbb);
        maxbb = max(cbb);
        cbb = zeros(1,4);
        cbb(1) = minbb(1);
        cbb(2) = minbb(2);
        cbb(3) = maxbb(3);
        cbb(4) = maxbb(4);
        
                
        bbs1(r,:) = [];
        bbs1(c,:) = [];
        bbs1(end+1, :) = cbb;
        confidences1(end+1) = 2 * max([confidences1(r) confidences1(c)]);
        confidences1(r) = [];
        confidences1(c) = [];        
    end
    [sorted_confidences, sort_index] = sort(confidences1);
    keep_index = find(sorted_confidences >= 1.5);
    sort_index = sort_index(keep_index);
    sorted_bbs = bbs1(sort_index,:);
    I = imread(['/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/reorganized-data/all_originals/' image_name '.jpg']);
    for i = size(sorted_bbs,1):-1:1
        imshow(I);
        hold on;
        bb = sorted_bbs(i,:);
        rectangle('Position', [bb(1) bb(2) bb(3)-bb(1) bb(4)-bb(2)], 'EdgeColor', 'red', 'LineWidth', 3)
        hold off
        saveas(gcf, ['~/Desktop/prediction4/' image_name '_' num2str(size(sorted_bbs,1) - i + 1) '.png'], 'png');
        close all
    end
      
    
end

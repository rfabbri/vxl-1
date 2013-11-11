function [transformation, average_aspect_ratios] = learn_part_whole_bb_transformation(whole_folder, part_folder, gt_list_file, image_folder, image_ext, num_clusters)
    transformation = cell(1, num_clusters);
    A = textread(gt_list_file, '%s');
    num_train = length(A);
    aspect_ratios = zeros(1,num_train);
    part_bbs = zeros(num_train, 4);
    whole_bbs = zeros(num_train, 4);
    for i = 1:num_train
        xml_file = A{i};
        whole_xml = [whole_folder '/' xml_file];
        part_xml = [part_folder '/' xml_file];
        image_name = strtok(xml_file, '.');
        image_file = [image_folder '/' image_name '.' image_ext];
        wbb = dbsksp_get_bounding_box(whole_xml, image_file);
        pbb = dbsksp_get_bounding_box(part_xml, image_file);
        aspect_ratios(i) = (pbb(3)-pbb(1))/(pbb(4)-pbb(2));
        part_bbs(i,:) = pbb;
        whole_bbs(i,:) = wbb;
    end  
    fprintf('Begin kmeans\n'); 
    [cidx, average_aspect_ratios] = mykmeans(aspect_ratios, num_clusters);
    
    for k = 1:num_clusters
        indices = find(cidx == k);
        part_bbs_k = part_bbs(indices,:);
        whole_bbs_k = whole_bbs(indices,:);
        minx_trans = regress(whole_bbs_k(:,1), part_bbs_k);
        miny_trans = regress(whole_bbs_k(:,2), part_bbs_k);
        maxx_trans = regress(whole_bbs_k(:,3), part_bbs_k);
        maxy_trans = regress(whole_bbs_k(:,4), part_bbs_k);
        transformation{k} = struct('minx', minx_trans, 'miny', miny_trans, 'maxx', maxx_trans, 'maxy', maxy_trans);
    end
    
end

function [cidx, average_aspect_ratios] = mykmeans(aspect_ratios, num_clusters)
    try
        [cidx, average_aspect_ratios] = kmeans(aspect_ratios, num_clusters);
    catch
        [cidx, average_aspect_ratios] = mykmeans(aspect_ratios, num_clusters)
    end
    h = hist(cidx, 1:num_clusters);
    if length(find(h < 4)) > 0
        [cidx, average_aspect_ratios] = mykmeans(aspect_ratios, num_clusters);
    end
end


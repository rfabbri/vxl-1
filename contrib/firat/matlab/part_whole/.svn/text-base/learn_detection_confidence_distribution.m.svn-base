function [mu,sigma1] = learn_detection_confidence_distribution(gt_list_file, detection_folder, detection_prefix)
    addpath ~/lemsvxl/src/contrib/ntrinh/matlab/xml_io_tools/
    A = textread(gt_list_file, '%s');
    num_train = length(A);
    confidence_values = cell(1,num_train);
    for i = 1:num_train
        xml_file = A{i};
        image_name = strtok(xml_file, '.');
        detection_file = [detection_folder '/' detection_prefix image_name '.xml'];
        doc = xml_read(detection_file);
        try
            dets = doc.xshock_det;
        catch
            new_bbs = [];
            return
        end
        C = zeros(1, length(dets));
        for j = 1:length(dets)
            C(j) = dets(j).confidence;
        end
        confidence_values{i} = C;
    end
    confidences = cell2mat(confidence_values);
    mu = mean(confidences);
    sigma1 = std(confidences, 1); 
end

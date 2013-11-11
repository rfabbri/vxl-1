function [new_bbs, confidences, boundaries] = predict_bb(image_name, image_folder, image_ext, detection_folder, record_prefix, transformation, average_aspect_ratios, confidence_mu, confidence_sigma, need_boundaries)
    addpath ~/lemsvxl/src/contrib/ntrinh/matlab/xml_io_tools/
    xml_file = [detection_folder '/' record_prefix image_name '.xml'];
    doc = xml_read(xml_file);
    try
        dets = doc.xshock_det;
    catch
        new_bbs = [];
        confidences = [];
        boundaries = [];
        return
    end
    I = imread([image_folder '/' image_name '.' image_ext]);
    [numrows,numcols,numbands] = size(I);
    new_bbs = zeros(length(dets), 4);
    if need_boundaries
        boundaries = zeros(numrows,numcols,length(dets));
    else
        boundaries = [];
    end
    confidences = zeros(length(dets),1);
    for i = 1:length(dets)
        det = dets(i);
        if need_boundaries
            xgfile = det.xgraph_xml;
            xgfile = [detection_folder xgfile(2:end-1)];
            boundaries(:,:,i) = dbsksp_boundary(xgfile, [image_folder '/' image_name '.' image_ext]);   
        end     
        pbb = [det.bbox_xmin det.bbox_ymin det.bbox_xmax det.bbox_ymax];
        aspect_ratio = (pbb(3)-pbb(1))/(pbb(4)-pbb(2));
        [min_val, min_index] = min(abs(average_aspect_ratios - repmat(aspect_ratio, length(average_aspect_ratios), 1)));
        new_bb(1) = max([(pbb * transformation{min_index}.minx) 1]);
        new_bb(2) = max([(pbb * transformation{min_index}.miny) 1]);
        new_bb(3) = min([(pbb * transformation{min_index}.maxx) numcols]);
        new_bb(4) = min([(pbb * transformation{min_index}.maxy) numrows]); 
        new_bbs(i,:) = new_bb; 
        confidences(i) = (det.confidence - confidence_mu) / confidence_sigma;
        %confidences(i) = det.confidence;            
    end    
end

% This is /lemsvxl/contrib/firat/matlab/eccv2012/verification/nms_detections.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jan 20, 2012

function dets2keep = nms_detections(object_bbs, overlap)
    N = size(object_bbs,1);
    dets2keep = [];
    if N == 0        
        return
    end    
    for i = N:-1:1
        bb = object_bbs(i,:);
        delete = false;
        for j = i-1:-1:1  
            if pascal_sim(bb, object_bbs(j,:)) >= overlap
                delete = true;
                break
            end            
        end  
        if ~delete
            dets2keep = [i; dets2keep];
        end
    end
end

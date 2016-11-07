function [edgmap, edgorient, contour_matrix] = export_gPb_kovesi_contours(gpb_file, pb_thresh, len_thresh)
    addpath ~/lemsvxl/src/contrib/ntrinh/matlab/kovesi;
    load(gpb_file);
    edgmap = gPb_thin > pb_thresh;
    
    [dummy, angle_index] = max(gPb_orient, [], 3);
    edgorient = (angle_index - 1) * pi / 8;
    edgorient(~edgmap) = 0;
    
    
    edgelist = edgelink(edgmap); 
    contour_matrix = [];
    for i = 1:length(edgelist)
        L = size(edgelist{i},1);
        if L > len_thresh
            contour_matrix = [contour_matrix; L 0];
            contour_matrix = [contour_matrix; edgelist{i}(:,2) edgelist{i}(:,1)];
        end
    end
end

% This is /lemsvxl/contrib/firat/matlab/3d_reconstruction_from_edges/intersect_mesh_and_slice.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 19, 2011

function contours = intersect_mesh_and_slice(tri, pts, slice_z)
    r1 = pts(:,tri(1,:))'; 
    r2 = pts(:,tri(2,:))';   
    r3 = pts(:,tri(3,:))';
    N = repmat([0 0 1], size(r1,1), 1);
    h = slice_z;
    line_segments = intersect_triangles_and_slice(r1, r2, r3, N, h);
    old_contours = line_segments;
    while true        
        contours = combine_segments(old_contours);
        if isequal(contours, old_contours)
            break
        end
        old_contours = contours;
    end
end

function contours = combine_segments(segments)
    num = length(segments);
    contours = {};
    tol = 1e-10;
    for i = 1:num
        start_new_contour = true;
        seg = segments{i};
        for j = 1:length(contours)
            C = contours{j};
            
            if norm(seg(1,:) - C(1,:)) < tol
                contours{j} = [seg(end:-1:2,:);C];
                start_new_contour = false;
            elseif norm(seg(1,:) - C(end,:)) < tol
                contours{j} = [C;seg(2:end,:)];
                start_new_contour = false;
            elseif norm(seg(end,:) - C(1,:)) < tol
                contours{j} = [seg(1:end-1,:); C];
                start_new_contour = false;
            elseif norm(seg(end,:) - C(end,:)) < tol
                contours{j} = [C; seg(end-1:-1:1,:)];
                start_new_contour = false;                          
            end
            
            if ~start_new_contour 
                
                break
            end
        end
        if start_new_contour
            contours{end+1} = seg;
        end        
    end
end

function line_segments = intersect_triangles_and_slice(r1, r2, r3, N, h)
    r21 = r2 - r1;
    r31 = r3 - r1;
    r32 = r3 - r2;
    alpha1_num = h - sum(N.*r1, 2);
    alpha1_den = sum(N.*r21, 2);
    alpha2_num = alpha1_num;
    alpha2_den = sum(N.*r31, 2);
    alpha3_num = h - sum(N.*r2, 2);
    alpha3_den = sum(N.*r32, 2);
    %r1r2r3_on_slice = find(alpha1_den == 0 & alpha2_den == 0 & alpha3_den == 0);
    %r1r2_on_slice = find(alpha1_den == 0 & alpha2_den ~= 0 & alpha3_den ~= 0);
    %r1r3_on_slice = find(alpha1_den ~= 0 & alpha2_den == 0 & alpha3_den ~= 0);
    %r2r3_on_slice = find(alpha1_den ~= 0 & alpha2_den ~= 0 & alpha3_den == 0);
    %others = setdiff((1:size(r1,1))', [r1r2r3_on_slice; r1r2_on_slice; r2r3_on_slice; r1r3_on_slice]);
    others = (1:size(r1,1))';
    %%others
    others_alpha1 = alpha1_num(others)./alpha1_den(others);
    others_alpha2 = alpha2_num(others)./alpha2_den(others);
    others_alpha3 = alpha3_num(others)./alpha3_den(others);
    indic1 = double(others_alpha1 >= 0 & others_alpha1 <= 1);
    indic1(indic1 == 0) = nan;    
    indic2 = double(others_alpha2 >= 0 & others_alpha2 <= 1);
    indic2(indic2 == 0) = nan;
    indic3 = double(others_alpha3 >= 0 & others_alpha3 <= 1);
    indic3(indic3 == 0) = nan;
    p1 = r1(others,:) + repmat(indic1.*others_alpha1,1,3).*r21(others,:);
    p2 = r1(others,:) + repmat(indic2.*others_alpha2,1,3).*r31(others,:);
    p3 = r2(others,:) + repmat(indic3.*others_alpha3,1,3).*r32(others,:);
    line_segments = cell(size(p1,1),1);
    tbd = zeros(size(p1,1),1);
    q = 1;
    for i = 1:size(p1,1)
        line_segments{i} = zeros(2,3);
        k = 1;
        if ~any(isnan(p1(i,:)))
            line_segments{i}(k,:) = p1(i,:);
            k = k + 1;
        end
        if ~any(isnan(p2(i,:)))
            line_segments{i}(k,:) = p2(i,:);
            k = k + 1;
        end
        if ~any(isnan(p3(i,:)))
            line_segments{i}(k,:) = p3(i,:); 
            k = k + 1;           
        end
        if k == 1
            tbd(q) = i;
            q = q + 1;
        end
    end
    tbd(q:end) = [];
    line_segments(tbd) = [];
    
end

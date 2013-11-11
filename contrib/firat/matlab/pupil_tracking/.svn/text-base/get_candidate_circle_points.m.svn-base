% This is /lemsvxl/contrib/firat/matlab/pupil_tracking/get_candidate_circle_points.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Mar 10, 2011

function points = get_candidate_circle_points(I0, x, y, delta_intensity)    
    s = 25;
    J = uint8(I0(y-s:y+s,x-s:x+s));
    J = imadjust(J,stretchlim(J),[]);
    I = double(J);
    %figure;imshow(J)
    C = bwlabel(edge(I, 'canny'),8);
    %figure;imagesc(C)
    [row,col] = find(C > 0);
    P = [s+1 s+1; col row];
    D = pdist(P);
    [ign, sort_index] = sort(D(1:length(row)));
    hist_index = sort_index(1:70);
    label_hist = hist(diag(C(row(hist_index), col(hist_index))), 1:max(C(:)));
    [ign, L] = max(label_hist);
    [row1,col1] = find(C == L);
    points = [col1-s+x-1 row1-s+y-1];
    
    
   
end

% This is /lemsvxl/contrib/firat/matlab/elbow/statistics.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 29, 2011

function statistics(I)
	close all
	imagesc(I);colormap gray; axis off; axis image
    hold on;
    
    [x{1},y{1},b] = ginput(1);
    plot(x{1},y{1},'rd');
    
    [x{2},y{2},b] = ginput(1);
        
    plot([x{1} x{2}], [y{1} y{2}], 'rd-')
    t = 3;
    while b == 1
        [x{t},y{t},b] = ginput(1);
        plot([x{t-1} x{t}], [y{t-1} y{t}], 'rd-');
        t = t + 1;    
    end  
    plot([x{t-1} x{1}], [y{t-1} y{1}], 'rd-');  
    hold off;    
    bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
    figure; imagesc(bw)
    
    se = strel('disk', 2);
    A = imerode(bw, se);
    B = imdilate(bw, se);
    
    A = bw - A;
    B = B - bw;
    IA = double(I(logical(A)));
    meanA = mean2(IA)
    stdA = std(IA(:))
    IB = I(logical(B));
    
    figure;hist(IA(:),0:10:3400, 'g')
    
    figure;hist(IB(:),0:10:3400,'r')
end


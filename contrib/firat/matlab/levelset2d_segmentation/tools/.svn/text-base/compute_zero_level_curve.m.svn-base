% This is /lemsvxl/contrib/firat/matlab/levelset2d_segmentation/tools/compute_zero_level_curve.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 15, 2011

function contours = compute_zero_level_curve(phi, hx, hy)
	alphabet = char('a' + (0:25));
	[nrows, ncols] = size(phi);
	rand_in = [alphabet(unidrnd(26,1,10)) '.phi'];
	rand_out = [alphabet(unidrnd(26,1,10)) '.ctr'];
	dlmwrite(rand_in, phi,'delimiter', ' ');
	cmd = sprintf('!/home/firat/lemsvxl/bin/contrib/firat/zero_levelset_tracer/compute_zero_level_curve %s %d %d %s', rand_in, nrows, ncols, rand_out);
	eval(cmd);
	contours = load_contours(rand_out, hx, hy);
	cmd2 = sprintf('!rm -f %s %s', rand_in, rand_out);
	eval(cmd2);	
end

function ctrs = load_contours(filename, hx, hy)
    [x,y] = textread(filename,'%f%f', 'delimiter',' ');
    contours = {};
    total_length = 0;
    while length(x) > 0
        l = x(1);
        cx = x(2:l+1)*hx+1;
        cy = y(2:l+1)*hy+1;
        x(1:l+1) = [];
        y(1:l+1) = [];
        contours = [contours; [cx cy]]; 
        total_length = total_length + length(cx);          
    end
    ctrs = zeros(2, total_length + length(contours)); 
    k = 1;
    for i = 1:length(contours)
    	L = size(contours{i},1);
    	ctrs(:,k) = [0; L];
    	ctrs(:,k+1:k+L) = contours{i}';
    	k = k+L+1;
    end    
end

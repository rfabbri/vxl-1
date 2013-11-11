% This is /lemsvxl/contrib/firat/matlab/levelset2d/levelset2d_driver.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 26, 2011

function levelset2d_driver(I, initialization_method, S, Sx, Sy, beta0, beta1, alpha, delta_t, num_iter, reinit_freq)
	[nrows,ncols] = size(I);
	if strcmp(initialization_method, 'manualcontour')
		fid = figure;
		imagesc(I);colormap gray; axis image; axis off;
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
		pause(.1)  
		bw = poly2mask(cell2mat(x), cell2mat(y), size(I,1), size(I,2));
		se = strel('disk',1);
		bw2 = imerode(bw, se);
		bw3 = bw - bw2;
		phi = sbwdist(bw, bw3);
	elseif strcmp(initialization_method, 'randombubbles')
		A = zeros(nrows,ncols);
		for i = 1:20
			A = circle(A, nrows, ncols, unidrnd(nrows-6)+3, unidrnd(ncols-6)+3, 1.5, 1); 
		end
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;
		phi = -sbwdist(A, bw3);
		%imagesc(A); pause
	elseif strcmp(initialization_method, 'manualbubbles')
		fid = figure;
		imagesc(I);colormap gray; axis image; axis off;
		hold on;  		  
		[x{1},y{1},b] = ginput(1);
		plot(x{1},y{1},'r.');    
		[x{2},y{2},b] = ginput(1);        
		plot([x{1} x{2}], [y{1} y{2}], 'r.')
		t = 3;
		while b == 1
			[x{t},y{t},b] = ginput(1);
			plot([x{t-1} x{t}], [y{t-1} y{t}], 'r.');
			t = t + 1;    
		end  
		plot([x{t-1} x{1}], [y{t-1} y{1}], 'r.');  
		hold off;  
		pause(.1)  
		A = zeros(nrows,ncols);
		for i = 1:length(x)
			A = circle(A, nrows, ncols, x{i}, y{i}, 2, 1); 
		end
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;
		phi = sbwdist(A, bw3);
	end
	
	close all
	figure;
	imagesc(I); colormap gray; axis image; axis off;  
	axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
	hold off;
	for i = 1:num_iter
		if ~mod(i,reinit_freq)  
			%phi = reinitlevelset(C, phi, nrows, ncols);	
			phi = reinitlevelset_sussman(phi, 1, 1);
		end		
		imagesc(I); colormap gray; axis image; axis off;  
		axis image  
		hold on;
		[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', [1 0 0]);
		hold off;	
		title(['iter:' num2str(i)])
		phi = levelset2d_drg(phi, S, Sx, Sy, beta0, beta1, alpha, delta_t, 1);	
		pause(.1)
	end
end

function I = circle(I, M, N, x, y, r, V)
	if r > 0
		for i = 1:M
			for j = 1:N
				if (i-y)^2+(j-x)^2 < r^2
					I(i,j) = V;
				end
			end
		end
	end
end


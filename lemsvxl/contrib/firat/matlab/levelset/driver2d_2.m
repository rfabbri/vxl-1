% This is /lemsvxl/contrib/firat/matlab/levelset/driver2d_2.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 14, 2011

function driver2d_2(I)
	if size(I,3) > 1
		I = rgb2gray(I);
	end
	I = double(I);
	[nrows,ncols] = size(I);
	
	if 1 %circle
		A = zeros(nrows,ncols);
		A(round(nrows/2), round(ncols/2)) = 1;
		phi = bwdist(A);
		phi = phi - 45;
	elseif 0 % square
		A = zeros(nrows,ncols);
		A(30:70, 30:70) = 1;
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;		
		phi = sbwdist(A, bw3);
	elseif 0% noisy circle
		A = zeros(nrows,ncols);
		for i = 1:nrows
			for j = 1:ncols
				if (i-round(nrows/2))^2+(j-round(ncols/2))^2 < 30^2
					A(i,j) = 1;
				end
			end
		end	
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;		
		phi = sbwdist(A, bw3);
		imagesc(bw3);	
	elseif 0
		A = zeros(nrows,ncols);
		A(30:70, 30:70) = 1;
		%A(29, 30:70) = rand(1,41)<.6;
		%A(71, 30:70) = rand(1,41)<.6;
		%A(30:70, 29) = rand(41,1)<.6;
		%A(30:70, 71) = rand(41,1)<.6;
		A(29, 30:70) = mod(1:41,2);
		A(71, 30:70) = mod(1:41,2);
		A(30:70, 29) = mod(1:41,2)';
		A(30:70, 71) = mod(1:41,2)';
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;		
		phi = sbwdist(A, bw3);
	else
		A = imread('~/Desktop/gear2.png');
		A = double(A(:,:,1) < 255);
		whos A
		se = strel('disk',1);
		bw2 = imerode(A, se);
		bw3 = A - bw2;		
		phi = sbwdist(A, bw3);
	end	
	
	%imagesc(phi); colormap gray; axis image
	%pause
	
	sigma=1; 
	G = fspecial('gaussian',5,sigma);
	Is = conv2(I,G,'same'); 
	Ix = (circshift(Is, [0 -1]) - circshift(Is, [0 1]))/2;
	Iy = (circshift(Is, [-1 0]) - circshift(Is, [1 0]))/2;		
	J = (Ix.^2 + Iy.^2);
	S = 1 ./ (1 + J); 
		
	beta0 = 1;
	beta1 = -.1;%-1;
	delta_t = 1;
	num_iter = 125;
    close all
    figure;
    imagesc(I); colormap gray; axis image; axis off;  
    axis image  
	hold on;
	[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', rand(1,3));
    for i = 1:num_iter
    	if ~mod(i,5)  
    		phi = reinitlevelset(C, phi, nrows, ncols);	
		end	
		%if ~mod(i,9)	
			[C, H] = contour(phi, [0,0], 'LineWidth', 1, 'Color', rand(1,3));	
		%end
		phi = levelset2d(phi, S, beta0, beta1, delta_t, 1);
		
		pause(.1)
	end
    %hold off;
    
    
    %imagesc(I); colormap gray; axis image; axis off;
	%hold on;
	
	hold off;
	phi = reinitlevelset(C, phi, nrows, ncols);
	figure; imagesc(phi); axis image
    	
end


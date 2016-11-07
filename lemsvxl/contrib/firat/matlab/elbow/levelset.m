% This is /lemsvxl/contrib/firat/matlab/elbow/levelset.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jun 24, 2011

function S = levelset(Img, S0, alfa)	
	Img = double(Img);	
	timestep=6;%5;  % time step
	mu=0.2/timestep;  % coefficient of the distance regularization term R(phi)
	iter_inner=7;%5;
	iter_outer=50;%200;
	lambda=3.5; % coefficient of the weighted length term L(phi)
	if ~exist('alfa','var')
		alfa=7;%6;%6.5;  % coefficient of the weighted area term A(phi)
    end
	epsilon=2; % papramater that specifies the width of the DiracDelta function

	sigma=1.5;     % scale parameter in Gaussian kernel
	G=fspecial('gaussian',15,sigma);
	Img_smooth=conv2(Img,G,'same');  % smooth image by Gaussiin convolution
	[Ix,Iy]=gradient(Img_smooth);
	f=Ix.^2+Iy.^2; 
	
	g=1./(1+f);  % edge indicator function.	
	c0 = 2;
	initialLSF=c0*ones(size(Img));
	initialLSF(S0) = -c0;
	phi=initialLSF;
	
	potentialFunction = 'double-well';
	
	% start level set evolution
	for n=1:iter_outer
		phi = drlse_edge(phi, g, lambda, mu, alfa, epsilon, timestep, iter_inner, potentialFunction);		
	end

	% refine the zero level contour by further level set evolution with alfa=0
	iter_refine = 10;
	phi = drlse_edge(phi, g, lambda, mu, 0, epsilon, timestep, iter_refine, potentialFunction);
	
	finalLSF=phi;
	%figure;
	%imagesc(Img); axis off; axis equal; colormap(gray); hold on;  contour(phi, [0,0], 'r');
	%hold on;  contour(phi, [0,0], 'r');
	%str=['Final zero level contour, ', num2str(iter_outer*iter_inner+iter_refine), ' iterations'];
	%title(str);
	
	S = phi <= 0;
	%figure;
	%imagesc(S); colormap gray; axis image; axis off;
	
	if 1
	figure;
	mesh(-finalLSF); % for a better view, the LSF is displayed upside down
	hold on;  contour(phi, [0,0], 'r','LineWidth',2);
	view([-80 35]);
	str=['Final level set function, ', num2str(iter_outer*iter_inner+iter_refine), ' iterations'];
	title(str);
	axis on;
	[nrow, ncol]=size(Img);
	axis([1 ncol 1 nrow -5 5]);
	set(gca,'ZTick',[-3:1:3]);
	set(gca,'FontSize',14)
	end
end


% This is get_critical_regions.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Jul 31, 2012

% mask is a label matrix
% 1 -> min
% 2 -> max
% 3 -> saddle

function mask = get_critical_regions(I, sigma)
	%%options
	show_points = 1;
	reflect_first = 0;
	sensitivity = 1;
		
	%%	
	I = double(I);	
	
	if sigma > 0			
		H = fspecial('gaussian', ceil(7*sigma)*[1 1], sigma);
		I = imfilter(I, H, 'replicate');		
	end	
	if reflect_first
		I = [I(1,:); I; I(end,:)];
		I = [I(:,1), I, I(:,end)];
	else %reflect second
		I = [I(2,:); I; I(end-1,:)];
		I = [I(:,2), I, I(:,end-1)];
		I(1,1) = I(3,3);
		I(1, end) = I(3, end-2);
		I(end, 1) = I(end-2, 3);
		I(end, end) = I(end-2, end-2);
	end
	if sensitivity ~= 0
		I = double(int32(I/sensitivity));
	end
	%regmax = imregionalmax(I);
	regmin = zeros(size(I));
	regmax = zeros(size(I));
	regsad = zeros(size(I));
	%regmin = imregionalmin(I);
	%regsad = imregionalsaddle(I); %implement in c++	
	%mask = regmin + 2*regmax; %+ 3*regsad;
	values = unique(I);
	for i = 1:length(values)
		v = values(i);
		L = bwlabel(I == v);		
		for j = 1:max(L(:))
			reg = imfill(L == j, 'holes'); 
			bw = imdilate(reg, strel('square',3));
			boun = logical(bw - reg);
			if all(v < I(boun))
				regmin = regmin | reg;
			elseif all(v > I(boun))
				regmax = regmax | reg;
			else
				imagesc(boun); pause
				props = regionprops(boun, 'PixelIdxList');
				S = I(props(1).PixelIdxList)-v;
				
				regsad = regsad | reg;
			end
		end
	end 
	mask = regmin + 2*regmax;
end


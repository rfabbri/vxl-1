% This is get_surface.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date May 24, 2012

function [I] = get_surface()
%function [I, mins, maxs, saddles] = get_surface()
	syms x y
	%f = sin((x^2 - y^2 + 2*x*y - 7)/pi)*cos(x*y);
	%f = (x-y)*(x+y) - x^3 - x*y - 16;
	%f = x^2 - y^2;
	%f = -x^2 - y^2;
	%f = sin(x)^2-y^2;
	%f = sin(x)^2 + cos(y)^2 - x*y;
	f = x^3 -3*x*y^2; %monkey saddle
	%f = x^3 + 22*x^2 + 41*y^2 - 14*x*y - 176*x - 68*y +500;
	%f = x*cos(x+y) - y*sin(x-y);
	%f = 3*x^2 +4*x*y - 2*y^2 + x - 11*y - 4;
	%f = cos((x^2 - y^2)/4/pi)*x*y^2;
	%f = 4*x^4 + x*y^2 - 16*x*(y^3 - y^2 + y - 1) - 4;
	%f = cos(x - y^2) + cos(x^2 - y);
	%f = sin(x*y) + cos(x*y);
	hx = .1;
	hy = .1;
	AA = 5;
	minx = -AA;
	maxx = AA;
	miny = -AA;
	maxy = AA;
	[xgrid, ygrid] = meshgrid(minx:hx:maxx,miny:hx:maxy);
	I = subs(f, [x, y], {xgrid, ygrid});
	figure;imagesc(I); axis image; axis off; colormap gray
	return
	drawnow
	fx = diff(f, 'x');
	fy = diff(f, 'y');
	fxx = diff(fx, 'x');
	fyy = diff(fy, 'y');
	fxy = diff(fx, 'y');
	H = [fxx fxy; fxy fyy];
	Z = solve(fx, fy, 'IgnoreAnalyticConstraints', true);
	Q = eig(H);
	mins = zeros(length(Z.x), 2);
	maxs = zeros(length(Z.x), 2);
	saddles = zeros(length(Z.x), 2);
	mink = 1;
	maxk = 1;
	sadk = 1;
	for i = 1:length(Z.x)
		try
			x0 = double(Z.x(i));
			y0 = double(Z.y(i));
			if imag(x0) ~= 0 || imag(y0) ~= 0
				continue
			end
		catch
			fprintf('Not isolated:');
			[Z.x(i) Z.y(i)]
			continue
		end
		H1 = subs(H, [x,y], [x0, y0]);
		Q2 = eig(H1);
		fprintf('Point (%.4f,%.4f) is a', x0, y0);
		p = [(x0 - minx)/hx, (y0 - miny)/hy]+1;
		if all(Q2>0)
			fprintf(' min.\n');
			mins(mink, :) = p;
			mink = mink + 1;			
		elseif any(Q2>0) && all(Q2 ~= 0)
			fprintf(' saddle.\n');	
			saddles(sadk, :) = p;
			sadk = sadk + 1;		
		elseif all(Q2 < 0)
			fprintf(' max.\n');	
			maxs(maxk, :) = p;
			maxk = maxk + 1;		
		else	
			fprintf('n unknown.\n');
		end 
		
	end
	mins = mins(1:mink-1, :);
	maxs = maxs(1:maxk-1, :);
	saddles = saddles(1:sadk-1, :);
	hold on
	plot(maxs(:,1), maxs(:,2), '^', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    plot(mins(:,1), mins(:,2), 'o', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);
    plot(saddles(:,1), saddles(:,2), '+', 'MarkerSize', 5, 'MarkerEdgeColor',[0, 127, 255]/255, 'LineWidth', 2);  
    hold off
end

% This is get_bspline_critical_points.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Nov 15, 2012

function get_bspline_critical_points(f, n)
	n = 2;
	W = [1/8 3/4 1/8; -1/2 0 1/2; 1/2 -1 1/2];
	[He,Wi] = size(f);
	Bsf = myBspline(f, n);
	global BsplinePad
	Grad = gradient(Bsf);
	fx = Grad{2};
	fy = Grad{1};
	fxy_fxx = gradient(fx);
	fyy_fxy = gradient(fy);
	fxx = fxy_fxx{2};
	fxy = fxy_fxx{1};
	fyy = fyy_fxy{1};
	C = Bsf.c;
	syms u v;
	figure; imagesc(f); axis image; colormap gray; hold on
	for y = 1:He
		for x = 1:Wi
			c = C(y+BsplinePad-1:y+BsplinePad+1,x+BsplinePad-1:x+BsplinePad+1);	
			B = W*c*W';
			g = [1 v v^2]*B*[1;u;u^2];
			gu = diff(g, u);
			gv = diff(g, v);
			Sol = solve(gu == 0, gv == 0, 'Real', true);	
			Sol = double([Sol.u Sol.v]);
			Sol = Sol(Sol(:,1) >= -.5 & Sol(:,1) < .5 & Sol(:,2) >= -.5 & Sol(:,2) < .5,:);
			if ~isempty(Sol)
				p = Sol + repmat([x y], size(Sol,1),1);	
				for i = 1:size(p,1)
					H = get_hessian(fxx, fxy, fyy, p(i,1), p(i,2));			
					if det(H) > 0
						if trace(H) < 0
							plot(p(i,1),p(i,2),'g^');
						else
							plot(p(i,1),p(i,2),'go');
						end
					elseif det(H) < 0
						plot(p(i,1),p(i,2),'g+');
					else
						plot(p(i,1),p(i,2),'r.');	
					end
				end
				drawnow
			end
		end
	end
	hold off
	
	if 0
		Grad = gradient(Bsf);
		fx = Grad{2};
		fy = Grad{1};
		fxy_fxx = gradient(fx);
		fyy_fxy = gradient(fy);
		fxx = fxy_fxx{2};
		fxy = fxy_fxx{1};
		fyy = fyy_fxy{1};
		grad_thresh = .01;	
		figure; imagesc(f); axis image; colormap gray; hold on
		maxK = imregionalmax(f);
		[maxr,maxc] = find(maxK);
		plot(maxc, maxr, 'g^');
		minK = imregionalmin(f);
		[minr,minc] = find(minK);
		plot(minc, minr, 'go');
		for y = 1:He
			for x = 1:Wi
				x0 = x;
				y0 = y;
				p0 = [x0; y0];
				[gm, vfx, vfy] = gradmag(fx, fy, x0, y0);
				H = get_hessian(fxx, fxy, fyy, x0, y0);
			
				it = 0; 
				while  it < 200 && gm > grad_thresh && all(p0 >= [0.5;.5]) && all(p0 <= [Wi;He]+.5) 				
					if det(H) == 0
						break
					end
					v = 0.01*inv(H)*[vfx; vfy];
					p0 = p0 + v;
					x0 = p0(1);
					y0 = p0(2);
					[gm, vfx, vfy] = gradmag(fx, fy, x0, y0);
					H = get_hessian(fxx, fxy, fyy, x0, y0);	
					it = it + 1;
				end
			
				if all(p0 >= [.5;.5]) && all(p0 <= [Wi;He]+.5) && gm <= grad_thresh
					if det(H) > 0
						if trace(H) < 0
							plot(p0(1), p0(2), 'r^')
						else
							plot(p0(1), p0(2), 'ro')
						end
						gm
					elseif det(H) < 0
						plot(p0(1), p0(2), 'r+');
						gm
					end	
					drawnow
				end	
			end
		end
	end
	
	
	if 0
		clear
		[x,y] = meshgrid(-10:10,-10:10);
		f = (2*x-8.6).^2 + (3*y-12.3).^2;		
		Bsf = myBspline(f, 3);
		Grad = gradient(Bsf);
		gfx = Grad{2};
		gfy = Grad{1};
		fxy_fxx = gradient(gfx);
		fyy_fxy = gradient(gfy);
		gfxx = fxy_fxx{2};
		gfxy = fxy_fxx{1};
		gfyy = fyy_fxy{1};
		[x1,y1] = meshgrid(1:.1:21,1:.1:21);
		fxint = myEvalBspline(gfx, x1, y1);		
		fyint = myEvalBspline(gfy, x1, y1);		
		fx = 8*(x1-11) - 24;
		fy = 18*(y1-11) - 72;
		ind = find(sqrt(fxint.^2+fyint.^2) < 0.1);
		[x1(ind)-11 y1(ind)-11]
		
		%figure; surf(abs(fx - fxint))
		%figure; surf(abs(fy - fyint))
		%figure; surf(abs(8 - fxxint))
		
	
	end
end

function [mag, vfx, vfy] = gradmag(fx, fy, x, y)
	vfx = myEvalBspline(fx, x, y);
	vfy = myEvalBspline(fy, x, y);
	mag = norm([vfx vfy]);
end

function H = get_hessian(fxx, fxy, fyy, x, y)
	vfxx = myEvalBspline(fxx, x, y);
	vfxy = myEvalBspline(fxy, x, y);
	vfyy = myEvalBspline(fyy, x, y);
	H = [vfxx vfxy; vfxy vfyy];
end

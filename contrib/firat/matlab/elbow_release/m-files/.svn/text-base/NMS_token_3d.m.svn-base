% This is /lemsvxl/contrib/firat/matlab/third_order_edge_detector_3d/NMS_token_3d.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Aug 26, 2011


function [subpix_x, subpix_y, subpix_z, str] = NMS_token_3d(Gx, Gy, Gz, G, thresh, margin)
		
	mask = G > thresh;

	%% Perform non-max suppression at every point
	[size_y size_x size_z] = size(G);
	num_voxels = prod([size_y size_x size_z]);
	i = 1; %edgel token counter
	
	subpix_x = zeros(num_voxels,1);
	subpix_y = zeros(num_voxels,1);
	subpix_z = zeros(num_voxels,1);
	str = zeros(num_voxels,1);
	

	for x=margin+3:size_x-(margin+2)
		for y=margin+3:size_y-(margin+2)
		    for z=margin+3:size_z-(margin+2)
				%fprintf('Processing [%d,%d,%d]\n',x,y,z);
				if(mask(y,x,z)==0)
				    continue;
				end
				gx = Gx(y,x,z);
				gy = Gy(y,x,z);
				gz = Gz(y,x,z);
				if(abs(gx) < 10e-6 && abs(gy) < 10e-6 && abs(gz) < 10e-6) % invalid direction
		        	continue;
		    	end
		    	%fprintf('interpolate\n');
		    	p = [x y z];
		    	N = [gx gy gz];
		    	N = N/norm(N);
		    	s = 1/max(abs(N));
		    	pp = p + s*N;
		    	pm = p - s*N;
		    	f = G(y,x,z);
		    	
		    	%r = floor(pp);
		    	%[X,Y,Z] = meshgrid(r(1):r(1)+1, r(2):r(2)+1, r(3):r(3)+1);
				%fp = interp3(X,Y,Z,G(r(2):r(2)+1, r(1):r(1)+1, r(3):r(3)+1), pp(1), pp(2), pp(3), '*linear');		  
		    	fp = trilinear_interp(G, pp);
		    	
		    	%r = floor(pm);
				%[X,Y,Z] = meshgrid(r(1):r(1)+1, r(2):r(2)+1, r(3):r(3)+1);
				%fm = interp3(X,Y,Z,G(r(2):r(2)+1, r(1):r(1)+1, r(3):r(3)+1), pm(1), pm(2), pm(3), '*linear');	
				fm = trilinear_interp(G, pm);	    	
		    	
		    	
		    	if((f >  fm && f >  fp) || ... %abs max
				   (f >  fm && f >= fp) || ... %relaxed max
				   (f >= fm && f >  fp))

				    % fit parabola
				    A = (fm+fp-2*f)/(2*s^2);
				    B = (fp-fm)/(2*s);
				    C = f;

				    s_star = -B/(2*A); %location of max
				    max_f = A*s_star*s_star + B*s_star + C;
				    		  
				    if(abs(s_star) <= s) % significant max is within a pixel
				        % store the edgel token :
				        % store the subpixel location
				        subpix_x(i) = x + s_star * N(1);
				        subpix_y(i) = y + s_star * N(2);
				        subpix_z(i) = z + s_star * N(3);		       
						str(i) = max_f;
				        i = i+1;
				    end %valid token
				end
		    	
		    end
		end
	end
	subpix_x(i:end) = [];
	subpix_y(i:end) = [];
	subpix_z(i:end) = [];
	str(i:end) = [];
end

function w = trilinear_interp(V, p)	
	voxel_0_0_0 = floor(p);
	voxel_0_0_1 = voxel_0_0_0 + [0 0 1];
	voxel_0_1_0 = voxel_0_0_0 + [0 1 0];
	voxel_0_1_1 = voxel_0_0_0 + [0 1 1];
	voxel_1_0_0 = voxel_0_0_0 + [1 0 0];
	voxel_1_0_1 = voxel_0_0_0 + [1 0 1];
	voxel_1_1_0 = voxel_0_0_0 + [1 1 0];
	voxel_1_1_1 = voxel_0_0_0 + [1 1 1];

	dlocx = p(1) - voxel_0_0_0(1);
	dlocy = p(2) - voxel_0_0_0(2);
	dlocz = p(3) - voxel_0_0_0(3);

	value = zeros(1,8);
	value(1) = (1-dlocx)*(1-dlocy)*(1-dlocz)*...
	V(voxel_0_0_0(2), voxel_0_0_0(1), voxel_0_0_0(3));
	value(2) = (1-dlocx)*(1-dlocy)*(dlocz)*...
	V(voxel_0_0_1(2), voxel_0_0_1(1), voxel_0_0_1(3));
	value(3) = (1-dlocx)*(dlocy)*(1-dlocz)*...
	V(voxel_0_1_0(2), voxel_0_1_0(1), voxel_0_1_0(3));
	value(4) = (1-dlocx)*(dlocy)*(dlocz)*...
	V(voxel_0_1_1(2), voxel_0_1_1(1), voxel_0_1_1(3));
	value(5) = (dlocx)*(1-dlocy)*(1-dlocz)*...
	V(voxel_1_0_0(2), voxel_1_0_0(1), voxel_1_0_0(3));
	value(6) = (dlocx)*(1-dlocy)*(dlocz)*...
	V(voxel_1_0_1(2), voxel_1_0_1(1), voxel_1_0_1(3));
	value(7) = (dlocx)*(dlocy)*(1-dlocz)*...
	V(voxel_1_1_0(2), voxel_1_1_0(1), voxel_1_1_0(3));
	value(8) = (dlocx)*(dlocy)*(dlocz)*...
	V(voxel_1_1_1(2), voxel_1_1_1(1), voxel_1_1_1(3));
	
	w = sum(value);
end



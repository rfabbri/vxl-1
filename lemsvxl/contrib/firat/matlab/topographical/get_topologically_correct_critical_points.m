% This is get_topologically_correct_critical_points.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 24, 2012

function [mins,maxs,saddles] = get_topologically_correct_critical_points(I)
	[M,N] = size(I);
	DT = determine_diagonal_type(I,M,N);
	%imagesc(DT); axis image; colormap gray
	G = build_graph(I, DT,M,N);
	C = classify_grid_points(I,G,M,N);
	[min_r,min_c] = find(C==1);
	[max_r,max_c] = find(C==2);
	[sad_r,sad_c] = find(C==3);
	mins = [min_c min_r];
	maxs = [max_c max_r];
	saddles = [sad_c sad_r];	
end

function C = classify_grid_points(I,G,M,N)
	C = zeros(M,N);
	for y = 1:M
		for x = 1:N
			v = I(y,x);
			S = G{y,x}(:,3);
			if all(v < S)
				C(y,x) = 1;
			elseif all(v > S)
				C(y,x) = 2;
			else
				count = 0;
				T = (S - v) > 0;
				for i = 1:length(T)-1
					if T(i) ~= T(i+1)
						count = count + 1;
					end					
				end
				if T(1) ~= T(end)
					count = count + 1;
				end
				if count > 2
					C(y,x) = 3;
				end
			end 	
		end
	end
end

function DT = determine_diagonal_type(I,M,N)
	DT = zeros(M-1, N-1);
	for y = 1:M-1
		for x = 1:N-1
			v1 = [1 0 (I(y+1,x+1)-I(y+1,x))];
			v2 = [0 -1 (I(y,x)-I(y+1,x))];
			v3 = [-1 0 (I(y,x)-I(y,x+1))];
			v4 = [0 1 (I(y+1,x+1)-I(y,x+1))];
						
			v5 = cross(v1,v2);
			v6 = cross(v3,v4);
			v7 = cross(v2,v3);
			v8 = cross(v4,v1);
			
			m1 = acos(dot(v5,v6)/norm(v5)/norm(v6));
			m2 = acos(dot(v7,v8)/norm(v7)/norm(v8));	
			
			if m1 < m2 %\
				DT(y,x) = 2;	
			else %/
				DT(y,x) = 1;
			end				
		end
	end
end

function G = build_graph(I,DT,M,N);
	G = cell(M,N);	
	%inner grid points
	for y = 2:M-1
		for x = 2:N-1
			num = 4;
			if DT(y-1,x-1) == 1
				num = num + 1;
			end	
			if DT(y, x-1) == 2
				num = num + 1;
			end
			if DT(y, x) == 1
				num = num + 1;
			end
			if DT(y-1, x) == 2
				num = num + 1;
			end			
			G{y,x} = zeros(num, 3);
			ind = 2;
			G{y,x}(1, :) = [x y-1 I(y-1,x)];
			if DT(y-1,x-1) == 1
				G{y,x}(ind, :) = [x-1 y-1 I(y-1,x-1)];
				ind = ind + 1;
			end
			G{y,x}(ind, :) = [x-1 y I(y,x-1)];
			ind = ind + 1;
			if DT(y, x-1) == 2
				G{y,x}(ind, :) = [x-1 y+1 I(y+1,x-1)];
				ind = ind + 1;
			end
			G{y,x}(ind, :) = [x y+1 I(y+1,x)];
			ind = ind + 1;
			if DT(y, x) == 1
				G{y,x}(ind, :) = [x+1 y+1 I(y+1,x+1)];
				ind = ind + 1;
			end
			G{y,x}(ind, :) = [x+1 y I(y,x+1)];
			ind = ind + 1;
			if DT(y-1, x) == 2
				G{y,x}(ind, :) = [x+1 y-1 I(y-1,x+1)];
			end
		end
	end
	%%
	%top left
	num = 3;
	if DT(1,1) == 1
		num = 4;
	end
	G{1,1} = zeros(num, 3);
	G{1,1}(1,:) = [0 0 -Inf]; %virtual pit
	G{1,1}(2,:) = [1 2 I(2,1)];
	ind = 3;
	if DT(1,1) == 1
		G{1,1}(3,:) = [2 2 I(2,2)];
		ind = ind + 1;
	end
	G{1,1}(ind,:) = [2 1 I(1,2)];
	%%
	%top right
	num = 3;
	if DT(1, N-1) == 2
		num = 4;
	end
	G{1,N} = zeros(num, 3);	
	G{1,N}(1,:) = [0 0 -Inf];
	G{1,N}(2,:) = [N-1 1 I(1, N-1)];
	ind = 3;
	if DT(1, N-1) == 2
		G{1,N}(3,:) = [N-1 2 I(2, N-1)];
		ind = ind + 1;
	end
	G{1,N}(ind,:) = [N 2 I(2,N)]; 
	%
	%%bottom left
	num = 3;
	if DT(M-1, 1) == 2
		num = 4;
	end
	G{M,1} = zeros(num, 3);
	G{M,1}(1,:) = [0 0 -Inf];
	G{M,1}(2,:) = [2 M I(M,2)];
	ind = 3;
	if DT(M-1, 1) == 2
		G{M,1}(3,:) = [2 M-1 I(M-1,2)];
		ind = ind + 1;
	end 
	G{M,1}(ind, :) = [1 M-1 I(M-1,1)];
	%
	%%bottom right
	num = 3;
	if DT(M-1,N-1) == 1
		num = 4;
	end
	G{M,N} = zeros(num, 3);
	G{M,N}(1,:) = [0 0 -Inf];
	G{M,N}(2,:) = [N M-1 I(M-1, N)];
	ind = 3;
	if DT(M-1,N-1) == 1
		G{M,N}(2,:) = [N-1 M-1 I(M-1,N-1)];
		ind = ind + 1;
	end
	G{M,N}(ind, :) = [N-1 M I(M,N-1)];
	%
	%%top
	for x = 2:N-1
		num = 4;
		if DT(1, x-1) == 2
			num = num + 1;
		end
		if DT(1, x) == 1
			num = num + 1;
		end
		G{1,x} = zeros(num, 3);
		G{1,x}(1,:) = [0 0 -Inf];
		G{1,x}(2,:) = [x-1 1 I(1, x-1)];
		ind = 3;
		if DT(1, x-1) == 2
			G{1,x}(ind,:) = [x-1 2 I(2, x-1)];
			ind = ind + 1; 
		end
		G{1,x}(ind,:) = [x 2 I(2, x)];
		ind = ind + 1;
		if DT(1, x) == 1
			G{1,x}(ind,:) = [x+1 2 I(2, x+1)];
			ind = ind + 1;
		end
		G{1,x}(ind, :) = [x+1 1 I(1, x+1)];
	end
	%
	%%bottom
	for x = 2:N-1
		num = 4;
		if DT(M-1, x-1) == 1
			num = num + 1;
		end
		if DT(M-1, x) == 2
			num = num + 1;
		end
		G{M,x} = zeros(num, 3);
		G{M,x}(1,:) = [0 0 -Inf];
		G{M,x}(2,:) = [x+1 M I(M,x+1)];
		ind = 3;
		if DT(M-1, x) == 2
			G{M,x}(ind,:) = [x+1 M-1 I(M-1,x+1)];
			ind = ind + 1;
		end
		G{M,x}(ind, :) = [x M-1 I(M-1,x)];
		ind = ind + 1;
		if DT(M-1, x-1) == 1
			G{M,x}(ind, :) = [x-1 M-1 I(M-1,x-1)];
			ind = ind + 1;
		end
		G{M,x}(ind, :) = [x-1 M I(M, x-1)];
	end
	%
	%%left
	for y = 2:M-1
		num = 4;
		if DT(y-1, 1) == 2
			num = num + 1;
		end
		if DT(y, 1) == 1
			num = num + 1;
		end
		G{y,1} = zeros(num, 3);
		G{y,1}(1,:) = [0 0 -Inf];
		G{y,1}(2,:) = [1 y+1 I(y+1, 1)];
		ind = 3;
		if DT(y, 1) == 1
			G{y,1}(ind,:) = [2 y+1 I(y+1, 2)];
			ind = ind + 1;
		end
		G{y,1}(ind,:) = [2 y I(y, 2)];
		ind = ind + 1;
		if DT(y-1, 1) == 2
			G{y,1}(ind,:) = [2 y-1 I(y-1, 2)];
			ind = ind + 1;
		end
		G{y,1}(ind,:) = [1 y-1 I(y-1,1)];
	end
	%
	%%right
	for y = 2:M-1
		num = 4;
		if DT(y-1,N-1) == 1
			num = num + 1;
		end
		if DT(y, N-1) == 2
			num = num + 1;
		end
		G{y, N} = zeros(num, 3);
		G{y,N}(1,:) = [0 0 -Inf];
		G{y,N}(2,:) = [N y-1 I(y-1,N)];
		ind = 3;
		if DT(y-1,N-1) == 1
			G{y,N}(ind,:) = [N-1 y-1 I(y-1,N-1)];
			ind = ind + 1;
		end
		G{y,N}(ind,:) = [N-1 y I(y, N-1)];
		ind = ind + 1;
		if DT(y, N-1) == 2
			G{y,N}(ind,:) = [N-1 y+1 I(y+1,N-1)];
			ind = ind + 1;
		end
		G{y,N}(ind,:) = [N y+1 I(y+1,N)];
	end
	%
end

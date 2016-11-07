% This is get_8n_critical_points.m

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Sep 27, 2012

% min = 1
% max = 2
% saddle = 3

function C = get_8n_critical_points(I)
	%boundary padding
	I = [I(2,:); I; I(end-1,:)];
	I = [I(:,2), I, I(:,end-1)];
	I(1,1) = I(3,3);
	I(1, end) = I(3, end-2);
	I(end, 1) = I(end-2, 3);
	I(end, end) = I(end-2, end-2);
	[H,W] = size(I);
	Max = zeros(H-1,W-1);
	max_ind = 1;
	Min = zeros(H-1,W-1);
	min_ind = 1;
	Sad = zeros(H-1,W-1);
	sad_ind = 1;
	for i = 2:H-1
		for j = 2:W-1
			P = I(i-1:i+1, j-1:j+1);
			v = P(5);
			R = P([1 2 3 6 9 8 7 4 1]);
			
			 
			if all(v == R) && Max(i-2,j-2)
					
			elseif v >= max(R)
				C(i-1,j-1) = 
			end
			
			
			maxness(i,j) = v - max(P(:));
			P(5) = Inf;
			minness(i,j) = min(P(:)) - v;			
			S = P([1 2 3 6 9 8 7 4 1]) - v;
			count = 0;
			S(S == 0) = [];
			S = S > 0;
			for k = 1:length(S)-1
				if S(k) ~= S(k+1)
					count = count + 1;
				end
			end
			if count == 4
				%Hess = [Ixx(i,j) Ixy(i,j); Ixy(i,j) Iyy(i,j)];
				%Q = det(Hess);
				saddleness(i,j) = ( Ixx(i,j)*Iyy(i,j) - (Ixy(i,j)^2) ) < 0;
			%	saddleness(i,j) = 1;
			end 			 
		end
	end
end

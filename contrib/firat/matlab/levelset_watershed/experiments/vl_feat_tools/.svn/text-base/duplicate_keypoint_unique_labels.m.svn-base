function lab = duplicate_keypoint_unique_labels(pts)
	N = size(pts,1);
	lab = zeros(N, 1);
	k = 1;
	for i = 1:N		
		if lab(i) == 0
			lab(i) = k;
			for j = i+1:N
				if all(pts(i,:) == pts(j,:))
					lab(j) = k;
				else
					k = k + 1;
					break
				end 
			end
		end
	end
end

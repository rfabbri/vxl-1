function pts = remove_duplicate_keypoints(pts)
	N = size(pts,1);
	mask = true(N, 1);
	for i = 1:N
		if mask(i)
			for j = i+1:N
				if all(pts(i,:) == pts(j,:))
					mask(j) = false;
				else
					break
				end 
			end
		end
	end
	pts = pts(mask,:);
end

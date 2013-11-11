function J = smooth_image(I, sigma)
	L = round(7*sigma);
	if mod(L,2) == 0
		L = L + 1;
	end	
	L = max(L, 7);	
	% boundary extension by reflection	
    K = (L-1)/2;
    I = [I(:, K+1:-1:2), I, I(:, end-1:-1:end-K)];
    I = [I(K+1:-1:2, :); I; I(end-1:-1:end-K, :)];
		
	% smooth the image						
	H = fspecial('gaussian', [L L], sigma);
	J = imfilter(I, H, 'replicate');	
	J = J(K+1:end-K,K+1:end-K);
end

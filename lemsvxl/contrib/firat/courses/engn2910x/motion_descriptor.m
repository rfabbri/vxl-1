function desc = motion_descriptor(bb, bbref)
	if bbref(3) == bbref(1) || bbref(4) == bbref(2)
		desc = [0 0 0 0];
		return 
	end
	tx = -(bbref(1) + bbref(3))/2;
	ty = -(bbref(2) + bbref(4))/2;
	sx = 2 / (bbref(3) - bbref(1));
	sy = 2 / (bbref(4) - bbref(2));
	D = [sx sy sx sy 1];
	T = diag(D);
	T(1:(end-1), end) = [sx*tx;sy*ty;sx*tx;sy*ty];
	temp_desc = T * [bb 1]';
	desc = temp_desc(1:(end-1))'; 
end

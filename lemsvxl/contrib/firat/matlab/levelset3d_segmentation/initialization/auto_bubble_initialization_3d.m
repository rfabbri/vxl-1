function phi = auto_bubble_initialization_3d(I, edg, hx, hy, hz)        
    [nrows,ncols, nbands] = size(I);
    phi = subpixelDT_fast_3d(edg(:,1:3), nrows, ncols, nbands, hx, hy, hz) - 3;	
end



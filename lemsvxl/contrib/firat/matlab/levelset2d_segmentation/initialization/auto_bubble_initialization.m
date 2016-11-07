function phi = auto_bubble_initialization(I, edg, hx, hy)        
    [nrows,ncols] = size(I);
    phi = subpixelDT_fast(edg(:,1:2), nrows, ncols, hx, hy) - 5;	
end



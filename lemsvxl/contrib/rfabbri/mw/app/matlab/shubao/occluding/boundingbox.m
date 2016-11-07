function B = boundingbox(A, bb)

xmin = bb(1); xmax = bb(2);
ymin = bb(3); ymax = bb(4);
zmin = bb(5); zmax = bb(6);

B = A;
N = size(B,2);
for i = 1:N
    if B(1,i) < xmin
        B(1:3,i) = 0;
    elseif B(1,i) > xmax
        B(1:3,i) = 0;
    end
    
    if B(2,i) < ymin
        B(1:3,i) = 0;
    elseif B(2,i) > ymax
        B(1:3,i) = 0;
    end
    
    if B(3,i) < zmin
        B(1:3,i) = 0;
    elseif B(3,i) > zmax
        B(1:3,i) = 0;
    end
end
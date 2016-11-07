function P = pmatrix_test1(phi)

fc = 250*25;
cc = [400 300];
Mint = [fc 0 cc(1);
    0 fc cc(2);
    0 0 1];

phi = -phi;
Ry = [cos(phi) 0 -sin(phi);
        0 1 0;
        sin(phi) 0 cos(phi)];   
Rckk = [1 0 0;
        0 -1 0;
        0 0 -1];
Tckk = [0 0 -100]';
R = Rckk*Ry;
Mext = [R Tckk];

P = Mint*Mext;
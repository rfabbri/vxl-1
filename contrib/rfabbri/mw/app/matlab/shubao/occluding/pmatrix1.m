function P = pmatrix1(phi)

f = 250*25;
u0 = 400;
v0 = 300;
Mint = [f 0 u0;
        0 f v0;
        0 0 1];
Ry = [cos(-phi) 0 -sin(-phi);
        0 1 0;
        sin(-phi) 0 cos(-phi)];
Rckk = [1 0 0;
        0 -1 0;
        0 0 -1];
Tckk = [0 0 -100]';
R = Rckk*Ry;
Mext = [R Tckk];

P = Mint*Mext;


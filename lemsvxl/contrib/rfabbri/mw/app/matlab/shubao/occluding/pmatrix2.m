function P = pmatrix2(phi)

f = 250*25;
u0 = 400;
v0 = 300;
Mint = [f 0 u0;
    0 -f v0;
    0 0 1];

% Here the projection matrix is got from the formula given at http://en.wikipedia.org/wiki/3D_projection
Tc = [0 0 100]';
Alphac = 0;
Betac = 0;
Gammac = 0;

T = [eye(3) -Tc; 0 0 0 1];
Rx = [1 0 0 0;
    0 cos(Alphac) sin(Alphac) 0;
    0 -sin(Alphac) cos(Alphac) 0;
    0 0 0 1];
Ry = [cos(Betac) 0 -sin(Betac) 0;
    0   1   0   0;
    sin(Betac)  0   cos(Betac)  0;
    0   0   0   1];
Rz = [cos(Gammac)   sin(Gammac) 0   0;
    -sin(Gammac)    cos(Gammac) 0   0;
    0   0   1   0;
    0   0   0   1];
Mcam = Rx*Ry*Rz*T;

Mworld = [cos(phi)  -sin(phi)   0   0;
        sin(phi)    cos(phi)    0   0;
        0   0   1   0;
        0   0   0   1];
Mext1 = Mcam*Mworld;
Mext = Mext1(1:3,:);
P = Mint*Mext;
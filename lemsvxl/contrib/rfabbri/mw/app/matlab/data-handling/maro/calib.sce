// world units: mm

// Intrinsics
f = 11    // initial focal length, in mm
pw = 5.5e-3   // 5.5 micrometers
ph = pw
u0 = 1920/2  // 960 px
v0 = 1080/2  // 540 px


Kini = [...
f/pw 0    u0
0    f/ph v0
0    0    1
]


K=list()
R= list()
C= list()
T= list()
P= list()

// First stereo pair: cam1, cam3  =========================================
// Extrinsics cam 1
K(1) = Kini
R(1) = [...
0 1 0
0 0 -1
-1 0 0]
C(1) = [14710 7500 480]'
T(1) = -R(1)*C(1)
P(1) = K(1)*[R(1) T(1)]

// Extrinsics cam 3 (baseline 24cm down from cam 1)
K(3) = Kini
R(3) = R(1)  // TODO unsure if this is true, but from the images it looks like it
C(3) = [14710 7500 240]'
T(3) = -R(3)*C(3)
P(3) = K(3)*[R(3) T(3)]

// Second stereo pair: cam2, cam4  =========================================

// Extrinsics cam 2
K(2) = Kini
c = cos(%pi/4)
R(2) = [...
-c  c  0
 0  0 -1
-c -c  0]
C(2) = [14975 15000 480]'
T(2) = -R(2)*C(2)
P(2) = K(2)*[R(2) T(2)]

// Extrinsics cam 4 (baseline 24cm down from cam 1)
K(4) = Kini
R(4) = R(2)  // TODO unsure if this is true, but from the images it looks like it
C(4) = [14975 15000 240]'
T(4) = -R(4)*C(4)
P(4) = K(4)*[R(4) T(4)]


// Write out
fprintfMat('1.projmatrix',P(1),'%30.30lf')
fprintfMat('2.projmatrix',P(2),'%30.30lf')
fprintfMat('3.projmatrix',P(3),'%30.30lf')
fprintfMat('4.projmatrix',P(4),'%30.30lf')

// Project origin for checking

o1=P(1)*[0 0 0 1]';
o1=o1/o1(3)

o2=P(2)*[0 0 0 1]';
o2=o2/o2(3)
;
o3=P(3)*[0 0 0 1]';
o3=o3/o3(3)

o4=P(4)*[0 0 0 1]';
o4=o4/o4(3)


// Compute point of focus by intersecting the look at directions

// intermediate vertical positions:
z_out0 = (C(1)(3)+C(3)(3))/2

lookat0 = [1 0]';

// intersect
l0 = cross([C(1)(1:2); 1], [C(1)(1:2)+lookat0; 1])

lookat1 = [-1 -1]';

l1 = cross([C(2)(1:2); 1], [C(2)(1:2)+lookat1; 1])

p = cross(l0,l1)
p = p/p(3)
p = [p(1:2); z_out0]

//p  = 
//   7475.
//   7500.
//   360.

pp=P(1)*[p;1]; pp(1:2)/pp(3)
pp=P(2)*[p;1]; pp(1:2)/pp(3)
pp=P(3)*[p;1]; pp(1:2)/pp(3)
pp=P(4)*[p;1]; pp(1:2)/pp(3)


// Best optimized cameras so far ===============

exec analyze_optimal.sce;

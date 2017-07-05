// world units: mm

// Intrinsics
f = 11    // initial focal length, in mm
pw = 5.5e-3   // 5.5 micrometers
ph = pw
u0 = 1920/2  // 960 px
v0 = 1080/2  // 540 px


K = [...
f/pw 0    u0
0    f/ph v0
0    0    1
]

// First stereo pair: cam1, cam3  =========================================
// Extrinsics cam 1
K1 = K
R1 = [...
0 1 0
0 0 -1
-1 0 0]
C1 = [14710 7500 480]'
T1 = -R1*C1
P1 = K1*[R1 T1]

// Extrinsics cam 3 (baseline 24cm down from cam 1)
K3 = K
R3 = R1  // TODO unsure if this is true, but from the images it looks like it
C3 = [14710 7500 240]'
T3 = -R3*C3
P3 = K3*[R3 T3]

// Second stereo pair: cam2, cam4  =========================================

// Extrinsics cam 2
K2 = K
c = cos(%pi/4)
R2 = [...
-c  c  0
 0  0 -1
-c -c  0]
C2 = [14975 15000 480]'
T2 = -R2*C2
P2 = K2*[R2 T2]

// Extrinsics cam 4 (baseline 24cm down from cam 1)
K4 = K
R4 = R2  // TODO unsure if this is true, but from the images it looks like it
C4 = [14975 15000 240]'
T4 = -R4*C4
P4 = K4*[R4 T4]


// Write out
fprintfMat('1.projmatrix',P1,'%30.30lf')
fprintfMat('2.projmatrix',P2,'%30.30lf')
fprintfMat('3.projmatrix',P3,'%30.30lf')
fprintfMat('4.projmatrix',P4,'%30.30lf')

// Project origin for checking

o1=P1*[0 0 0 1]';
o1=o1/o1(3)

o2=P2*[0 0 0 1]';
o2=o2/o2(3)
;
o3=P3*[0 0 0 1]';
o3=o3/o3(3)

o4=P4*[0 0 0 1]';
o4=o4/o4(3)

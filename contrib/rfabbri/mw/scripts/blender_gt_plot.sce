// mat 3x3 as matrix of change of basis - rotation of coordinate systems
// euler 1x3
function mat = eul_to_mat3(eul)
	// double ci, cj, ch, si, sj, sh, cc, cs, sc, ss;

	ci = cos(eul(1));
	cj = cos(eul(2));
	ch = cos(eul(3));
	si = sin(eul(1));
	sj = sin(eul(2));
	sh = sin(eul(3));
	cc = ci * ch;
	cs = ci * sh;
	sc = si * ch;
	ss = si * sh;

	mat(1)(1) = (cj * ch);
	mat(2)(1) = (sj * sc - cs);
	mat(3)(1) = (sj * cc + ss);
	mat(1)(2) = (cj * sh);
	mat(2)(2) = (sj * ss + cc);
	mat(3)(2) = (sj * cs - sc);
	mat(1)(3) = -sj;
	mat(2)(3) = (cj * si);
	mat(3)(3) = (cj * ci);
endfunction

// Cube_057 object (chair rim)
//loc = [0.9911641 -1.213026 1.66694]'
//Rz = [0 0 1 -46.30167]
//Ry = [0 1 0 7.07923e-7]
//Rx = [1 0 0 -4.68709e-7]
//scale = diag([0.01600483 0.01510128 0.01510128]);

// Plane.008 (chair leather seat)
//loc = [0.5259647 -1.101067 1.94671]'
//Rz = [0 0 1 -46.6002];
//Ry = [0 1 0 14.5235];
//Rx = [1 0 0 -3.17702e-6];
//scale = diag([0.07752522 0.08356105 0.07752521]);

// Default cube
loc = [0 0 0]'
Rz = [0 0 1 0];
Ry = [0 1 0 0];
Rx = [1 0 0 0];
scale = diag([1 1 1]);

// proj matrix for frame 58
// WTF: P = [2.100000000000000000e+03 8.392586460104212165e-05 9.600000000000000000e+02 3.840000000000000000e+03
//0.000000000000000000e+00 2.100000000000000000e+03 5.399998168945312500e+02 2.160000000000000000e+03
//0.000000000000000000e+00 8.742277657347585773e-08 1.000000000000000000e+00 4.000000000000000000e+00]

// fixed for frame 58
//P = [-4.434745788574218750e+02 6.290440673828125000e+02 -5.841709136962890625e+00 1.551619628906250000e+03
//-1.643272552490234375e+02 -3.081280517578125000e+01 -7.031693725585937500e+02 2.237722412109375000e+03
//-9.827068448066711426e-01 -1.842668652534484863e-01 -1.825542002916336060e-02 4.085147857666015625e+00]

// proj matrix for frame 1
//P = [6.419722290039062500e+02 -4.240860290527343750e+02 -2.055912590026855469e+01 1.319760253906250000e+03
//     1.141579437255859375e+02 7.141400146484375000e+01 -7.101184082031250000e+02 2.459689941406250000e+03
//     8.460292220115661621e-01 5.292513966560363770e-01 -6.424716114997863770e-02 5.219823837280273438e+00];

// proj matrix for frame 22
//P = [-9.469497680664062500e+01 -7.373432006835937500e+02 -1.993945465087890625e+02 1.548648803710937500e+03
//-2.632128295898437500e+02 1.340713195800781250e+02 -6.596545410156250000e+02 2.303698730468750000e+03
//6.969335675239562988e-01 -3.549930155277252197e-01 -6.231080889701843262e-01 4.801393508911132812e+00];

// cube - default scene
P = [405.8409, 1057.1350,  -225.0754, 5046.0317
     156.4260, -163.2123, -1060.3268, 3433.6606
     -0.6549,    0.6107,    -0.4452,   11.2523];

// from: ground-truth-pavillion-sunset/pavillon_barcelone_v1.2-009-ground_truth-sunset.dae
//exec('/Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/3d/obj-transform-test/chair-rim-points.sce');
//exec('/Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/3d/obj-transform-test/plane_009.sce');
//exec('/Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/3d/obj-transform-test/plane_009-ungrouped_curves-010-ground_truth-sunset.sce');
exec('/Users/rfabbri/lib/data/models/pabellon_barcelona_v1/3d/obj-transform-test/cube/default-scene-cube.sce');
po=matrix(points,3,-1);

//R = eul_to_mat3([Rx(4), Ry(4), Rz(4)]*%pi/180)'
R = eul_to_mat3([Rx(4), Ry(4), Rz(4)]*%pi/180)'

// object-to-world transform
// tested formula against corresponding object's matrix_world and it matches
// perfectly
p = R*scale*po + loc*ones(1,size(po,2)) 

// transform by the projmatrix
p = [p; ones(1,size(p,2))]
px = P*p;
w = px(3,:)
w = [w; w; w]
rprj = px./w
clear w px
prj = rprj(1:2,:)
rprj = round(prj)

//img = zeros(360,640);
img = zeros(540,960);
for i = 1:size(rprj,2); 
  r = rprj(2,i) + 1;
  c = rprj(1,i) + 1;
  if r >= 1 & c >= 1 & r <= size(img,1) & c <= size(img,2)
    img(r,c) = 1; 
  end
end

SIPVIEWER = 'display'
imshow(img,[])
//im = gray_imread('/Users/rfabbri/lib/models/pabellon_barcelona_v1/3d/ground-truth-pavillion-sunset/sunset-640x-0022.png');
//im = gray_imread('/Users/rfabbri/lib/models/pabellon_barcelona_v1/3d/ground-truth-pavillion-night/night-640x-0058-opengl.png');
im = gray_imread('/Users/rfabbri/lib/data/models/pabellon_barcelona_v1/3d/obj-transform-test/cube/default_scene.png');

imshow(im + 0.5*edilate(img,3),[])


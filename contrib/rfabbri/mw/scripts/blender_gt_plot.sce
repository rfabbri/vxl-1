// mat 3x3
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
//scale = diag([0.01600483 0.01510128 0.01510128])

// Plane.008 (chair leather seat)

// XXX
loc = [0.5259647 -1.101067 1.94671]'
Rz = [0 0 1 -46.6002];
Ry = [0 1 0 14.5235];
Rx = [1 0 0 -3.17702e-6];
scale = diag(0.07752522 0.08356105 0.07752521)


// proj matrix for frame 58
//P = [2.100000000000000000e+03 8.392586460104212165e-05 9.600000000000000000e+02 3.840000000000000000e+03
//0.000000000000000000e+00 2.100000000000000000e+03 5.399998168945312500e+02 2.160000000000000000e+03
//0.000000000000000000e+00 8.742277657347585773e-08 1.000000000000000000e+00 4.000000000000000000e+00]

// proj matrix for frame 1
P = [6.419722290039062500e+02 -4.240860290527343750e+02 -2.055912590026855469e+01 1.319760253906250000e+03
     1.141579437255859375e+02 7.141400146484375000e+01 -7.101184082031250000e+02 2.459689941406250000e+03
     8.460292220115661621e-01 5.292513966560363770e-01 -6.424716114997863770e-02 5.219823837280273438e+00];

//exec('/Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/3d/obj-transform-test/chair-rim-points.sce');
exec('/Users/rfabbri/3d-curve-drawing/ground-truth/models/pabellon_barcelona_v1/3d/obj-transform-test/plane_009.sce');
po=matrix(points,3,-1);

R = eul_to_mat3([Rx(4), Ry(4), Rz(4)]*%pi/180)'

// object-to-world transform
//p = R*scale*po + loc*ones(1,size(po,2)) 
p = R*scale*po + loc*ones(1,size(po,2)) 

// transform by the projmatrix
p = [p; ones(1,size(p,2))]
px = P*p;
w = px(3,:)
w = [w; w; w]
prj = px./w
clear w px
rprj = round(prj)
rprj = rprj(1:2,:)

img = zeros(360,640);
for i = 1:size(p,2); 
  r = rprj(2,i) + 1;
  c = rprj(1,i) + 1;
  if r >= 1 & c >= 1 & r <= 360 & c <= 640
    img(r,c) = 1; 
  end
end

SIPVIEWER = 'display'
imshow(img,[])

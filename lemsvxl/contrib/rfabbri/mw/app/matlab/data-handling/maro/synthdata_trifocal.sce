cd /Users/rfabbri/lib/data/synthcurves-multiview-3d-dataset/ascii-20_views-olympus-turntable

// read 3 views
gama__1_vec_img=read('frame_0003-pts-2D.txt',-1,2)';
gama__2_vec_img=read('frame_0011-pts-2D.txt',-1,2)';
gama__3_vec_img=read('frame_0017-pts-2D.txt',-1,2)';

RC_1 = read('frame_0003.extrinsic',-1,3);
RC_2 = read('frame_0011.extrinsic',-1,3);
RC_3 = read('frame_0017.extrinsic',-1,3);

R_1 = RC_1(1:3,1:3);
R_2 = RC_2(1:3,1:3);
R_3 = RC_3(1:3,1:3);
T_1 = -R_1*RC_1(4,:)'; 
T_2 = -R_2*RC_2(4,:)'; 
T_3 = -R_3*RC_3(4,:)'; 
                 
// Compute ground-truth depth, depht depth derivatives and speeds using formulae

// Notice depths, depth derivatives and speeds are invariant to coordinate
// changes system



R_21 = R_2*R_1'
T_21 = -R_21*T_1 + T_2

R_31 = R_3*R_1'
T_31 = -R_31*T_1 + T_3
                 
// 
// Approach 3C: Transform everything relative to cam 1


// Undo the effects of K

K = read('calib.intrinsic',-1,3);


// === Specific points =====================
//
// i = 869
//

// Read 3D points

Gama_w_vec = read('crv-3D-pts.txt',-1,3)';

// Plug into equations that must be zero

//depth_ =


P_1 = K *[R_1 T_1];

// sanity check 0: project 1st point matches supplied 2D point
proj=P_1*[Gama_w_vec(:,1); 1]
proj=proj/proj($);
proj=proj(1:2)
gama__1_vec_img(:,1)
max(abs(proj-gama__1_vec_img(:,1)))

// sanity check 1: all projections to cam 1 give supplied 2D points
proj_1 = P_1 * [Gama_w_vec; ones(1,size(Gama_w_vec,2))];
proj_1 = proj_1 ./ [proj_1(3,:); proj_1(3,:); proj_1(3,:)];
proj_1 = proj_1(1:2,:);
max(abs(proj_1 - gama__1_vec_img))



// last index in underscore is view, like so:
//    symbol_samplenumber_viewnumber
//    symbol__viewnumber(samplenumber)
//    symbol(samplenumber,viewnumber)
Gama_1_vec = R_1*Gama_w_vec + T_1*ones(1,size(Gama_w_vec,2));
Gama_2_vec = R_21*Gama_1_vec + T_21*ones(1,size(Gama_w_vec,2));
Gama_3_vec = R_31*Gama_1_vec + T_31*ones(1,size(Gama_w_vec,2));

depth__1 = Gama_1_vec(3,:);
depth__2 = Gama_2_vec(3,:);
depth__3 = Gama_3_vec(3,:);

// ---------------------------------------------------------------------
// CORE POINT EQUATIONS
// must output zero:

// Starting here we treat the 2D points as 3D vectors
// Apply the inverse K matrix!

gama__1_vec_img = [gama__1_vec_img; ones(1,size(gama__1_vec_img,2))]
gama__2_vec_img = [gama__2_vec_img; ones(1,size(gama__2_vec_img,2))]
gama__3_vec_img = [gama__3_vec_img; ones(1,size(gama__3_vec_img,2))]

// gama__1_vec = inv(K)*gama__1_vec;
gama__1_vec = K\gama__1_vec_img;
gama__2_vec = K\gama__2_vec_img;
gama__3_vec = K\gama__3_vec_img;

point_ids=[689 869 968]
for i=point_ids
  depth__2(i)*gama__2_vec(:,i) - depth__1(i)*R_21*gama__1_vec(:,i) - T_21
  depth__3(i)*gama__3_vec(:,i) - depth__1(i)*R_31*gama__1_vec(:,i) - T_31
end

// ---------------------------------------------------------------------
// TANGENT EQUATIONS

// Read 3D tangents

T_w_vec = read('crv-3D-tgts.txt',-1,3)';
t__1_vec_img=read('frame_0003-tgts-2D.txt',-1,2)';
t__2_vec_img=read('frame_0011-tgts-2D.txt',-1,2)';
t__3_vec_img=read('frame_0017-tgts-2D.txt',-1,2)';

exec synthdata_trifocal_tangents.sce;


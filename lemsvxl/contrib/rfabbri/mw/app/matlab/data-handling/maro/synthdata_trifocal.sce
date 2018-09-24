cd /Users/rfabbri/lib/data/synthcurves-multiview-3d-dataset/ascii-20_views-olympus-turntable

// read 3 views
gama1_vec=read('frame_0003-pts-2D.txt',-1,2)';
gama2_vec=read('frame_0011-pts-2D.txt',-1,2)';
gama3_vec=read('frame_0017-pts-2D.txt',-1,2)';

t1_vec=read('frame_0003-tgts-2D.txt',-1,2)';
t2_vec=read('frame_0011-tgts-2D.txt',-1,2)';
t3_vec=read('frame_0017-tgts-2D.txt',-1,2)';


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

proj=P_1*[Gama_w_vec(:,1); 1]
proj=proj/proj($);
proj=proj(1:2)
gama1_vec(:,1)
max(abs(proj-gama1_vec(:,1)))

// sanity check 1: projections to cam 1 give supplied 2D points
proj_1 = P_1 * [Gama_w_vec; ones(1,size(Gama_w_vec,2))];
proj_1 = proj_1 ./ [proj_1(3,:); proj_1(3,:); proj_1(3,:)];
proj_1 = proj_1(1:2,:);
max(abs(proj_1 - gama1_vec))

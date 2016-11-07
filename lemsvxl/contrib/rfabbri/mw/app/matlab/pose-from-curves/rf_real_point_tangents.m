% ids into the edge files .edg, just look into corresponding line:
% files 25-35-37 ~/lib/data/dino-iccv2011

% Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer
% [311, 184]    6.07943 30.1887   [311.03, 183.599]   6.07943 30.1887 0.260538
% [476, 175]    6.05911 27.6294   [475.933, 174.798]   6.05911 27.6294 0.369003
 
edg_id1 = 3917;
%edg_id1 = 4852;
edg_id2 = 3561;
%edg_id2 = 3917;

Gama1 = [-0.022602327659732135423 0.030344773579267278474 -0.018201336093059686944]';
Tgt1  = [-0.66875971307387338527 0.69396296748687680278 -0.26678801683388725374]';

Gama4  = [-0.014001358089023474562 0.047657954547335561191 -0.010344651210103457048]';
Tgt4   = [-0.19016953419516979817 0.74313032230107811671 -0.6415550423312779138]';

Gama2 = [-0.00063428195635620299164 0.063829947289953697931 -0.015498824340923636131]';
Tgt2  = [-0.057501195123540162213 0.95918756133818905418 -0.27686248343439795461]';

Gama3 = [-0.022602327659732135423 0.030344773579267278474 -0.018201336093059686944]';
Tgt3  = [-0.66875971307387338527 0.69396296748687680278 -0.26678801683388725374]';

Gama_all = [Gama1';Gama2';Gama3';Gama4'];

gama1_img = [311.03, 183.599 1]';
gama4_img = [396.555, 214.571 1]';

gama2_img = [475.933, 174.798 1]';
gama3_img = [311.03, 183.599 1]';

gama_all_img = [gama1_img'; gama2_img';gama3_img';gama4_img'];
gama_all_img = gama_all_img(:,1:2);

tgt1  = [cos(6.07943) sin(6.07943) 0]';
%tgt4  = [cos(5.69937) sin(5.69937) 0]';

tgt2  = [cos(6.05911) sin(6.05911) 0]';
%tgt3  = [cos(6.07943) sin(6.07943) 0]';

tgt1_2d_pt = tgt1(1:2) + gama1_img(1:2);
tgt2_2d_pt = tgt2(1:2) + gama2_img(1:2);

% Camera

K_gt = [ 3.3104000000000001e+03   0.0000000000000000e+00   3.1673000000000002e+02
   0.0000000000000000e+00   3.3255000000000000e+03   2.0055000000000001e+02
   0.0000000000000000e+00   0.0000000000000000e+00   1.0000000000000000e+00];

% Convert to world coordinates 

Kinv = inv(K_gt);

gama1_cam = Kinv*gama1_img;
gama1_cam(1) = gama1_cam(1)./gama1_cam(3);
gama1_cam(2) = gama1_cam(2)./gama1_cam(3);
gama1_cam(3) = 1;

gama2_cam = Kinv*gama2_img;
gama2_cam(1) = gama2_cam(1)./gama2_cam(3);
gama2_cam(2) = gama2_cam(2)./gama2_cam(3);
gama2_cam(3) = 1;

tgt1_2d_pt_normal = Kinv*[tgt1_2d_pt; 1];
tgt1_2d_pt_normal(1) = tgt1_2d_pt_normal(1) ./ tgt1_2d_pt_normal(3);
tgt1_2d_pt_normal(2) = tgt1_2d_pt_normal(2) ./ tgt1_2d_pt_normal(3);
tgt1_2d_pt_normal(3) = 1;

tgt2_2d_pt_normal = Kinv*[tgt2_2d_pt; 1];
tgt2_2d_pt_normal(1) = tgt2_2d_pt_normal(1) ./ tgt2_2d_pt_normal(3);
tgt2_2d_pt_normal(2) = tgt2_2d_pt_normal(2) ./ tgt2_2d_pt_normal(3);
tgt2_2d_pt_normal(3) = 1;
   
tgt1 = tgt1_2d_pt_normal - gama1_cam;
tgt1 = tgt1/norm(tgt1);

tgt2 = tgt2_2d_pt_normal - gama2_cam;
tgt2 = tgt2/norm(tgt2);


% Ground-truth rotation and translation

R_gt = [-2.1330836966165886e-02   9.9875992918698486e-01  -4.4983802818283715e-02
-2.6706314663951503e-01   3.7667539147032428e-02   9.6294193034782027e-01
 9.6344343022363377e-01   3.2554009675712856e-02   2.6592841509119858e-01];

C_gt = [-6.3709035082348664e-01   1.0830459269250841e-02 -1.8443542793804638e-01]';
T_gt = -R_gt*C_gt;
P_gt = K_gt*[R_gt T_gt];

% Test: do the 3D project into 2D correctly?

gama1_reproj = P_gt*[Gama1; 1];
gama1_reproj(1) = gama1_reproj(1)/gama1_reproj(3);
gama1_reproj(2) = gama1_reproj(2)/gama1_reproj(3);
gama1_reproj(3) = 1;

gama2_reproj = P_gt*[Gama2; 1];
gama2_reproj(1) = gama2_reproj(1)/gama2_reproj(3);
gama2_reproj(2) = gama2_reproj(2)/gama2_reproj(3);
gama2_reproj(3) = 1;


% Use exact reprojection to compute cam, to factor out localization errors.
gama1_reproj_cam = [R_gt T_gt]*[Gama1; 1];
gama1_reproj_cam(1) = gama1_reproj_cam(1)/gama1_reproj_cam(3);
gama1_reproj_cam(2) = gama1_reproj_cam(2)/gama1_reproj_cam(3);
gama1_reproj_cam(3) = 1;

gama1_reproj_img = K_gt*gama1_reproj_cam;
gama1_reproj_img(1) = gama1_reproj_img(1)/gama1_reproj_img(3);
gama1_reproj_img(2) = gama1_reproj_img(2)/gama1_reproj_img(3);
gama1_reproj_img(3) = 1;

gama2_reproj_cam = [R_gt T_gt]*[Gama2; 1];
gama2_reproj_cam(1) = gama2_reproj_cam(1)/gama2_reproj_cam(3);
gama2_reproj_cam(2) = gama2_reproj_cam(2)/gama2_reproj_cam(3);
gama2_reproj_cam(3) = 1;

gama2_reproj_img = K_gt*gama2_reproj_cam;
gama2_reproj_img(1) = gama2_reproj_img(1)/gama2_reproj_img(3);
gama2_reproj_img(2) = gama2_reproj_img(2)/gama2_reproj_img(3);
gama2_reproj_img(3) = 1;

% Transform 3D T to local camera coordinates
Tgt1_cam = R_gt*Tgt1;
tgt1_reproj = Tgt1_cam - Tgt1_cam(3)*gama1_reproj_cam;
tgt1_reproj = tgt1_reproj/norm(tgt1_reproj);

Tgt2_cam = R_gt*Tgt2;
tgt2_reproj = Tgt2_cam - Tgt2_cam(3)*gama2_reproj_cam;
tgt2_reproj = tgt2_reproj/norm(tgt2_reproj);



%-----------
% Real image data: 
gama1 = gama1_cam;
gama2 = gama2_cam;

%-----------
% Temp! XXX
%gama1 = gama1_reproj_cam;
%gama2 = gama2_reproj_cam;

tgt1 = tgt1_reproj;
%tgt2 = tgt2_reproj;
%-----------


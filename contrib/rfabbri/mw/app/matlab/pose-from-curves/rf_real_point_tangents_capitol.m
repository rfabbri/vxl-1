% ids into the edge files .edg, just look into corresponding line:
% files 1-177-66 ~/lib/data/right-dome-subset-local-mcs-work

% Triplet 1 (right by the dome)
% Middle of selected edgel: (182.86 140.781)
% Selected view[index 2]: selected point number: (25353) out of 62996
% Previously selected view[index 0]: point number: (22700) out of 59885
% Previously selected view[index 1]: point number: (16190) out of 60346
%
%         Prec.Gama: 20.264983282540946163 -29.21160486068595219 28.097390540699617389
%         Prec.Tangent: -0.36462311318122969661 -0.92277217516084353299 -0.12466554489093507885
%
% Triplet 2 (floor)
% Middle of selected edgel: (383.2540000000000191 137.95099999999999341)
% Selected view[index 2]: selected point number: (24940) out of 62996
% Previously selected view[index 0]: point number: (26881) out of 59885
% Previously selected view[index 1]: point number: (11655) out of 60346
%         Prec.Gama: 47.36049190047523183 73.864221097343204292 -9.1104135881035599454
%         Prec.Tangent: 0.047151815788090779835 0.68133508472466774997 -0.73045137318723396547

% Triplet 3 (lower vertical corner of bulding)
%Middle of selected edgel: (279.44099999999997408 378.48700000000002319)
%Selected view[index 2]: selected point number: (60873) out of 62996
%Previously selected view[index 0]: point number: (57310) out of 59885
%Previously selected view[index 1]: point number: (53331) out of 60346
%3rd view...
%Frame index: 2
%(basic) Unassigned key: 114/r pressed.
%Computing reprojections for selected triplet.Reconstruction from views 0,1:
%         Prec.Gama: 41.460984059231002163 -61.72945320434706673 14.21490237021884262
%         Prec.Tangent: -0.074988667378282775067 -0.056303980019817762015 0.99559357249771218701
%
% Triplet 4 (dome)
%Middle of selected edgel: (187.33299999999999841 52.826000000000000512)
%Selected view[index 2]: selected point number: (8441) out of 62996
%Previously selected view[index 0]: point number: (6867) out of 59885
%Previously selected view[index 1]: point number: (1768) out of 60346
%3rd view...
%Frame index: 2
%(basic) Unassigned key: 114/r pressed.
%Computing reprojections for selected triplet.Reconstruction from views 0,1:
%         Prec.Gama: 19.001475054810271104 -36.02969355984117783 43.810136871732815678
%         Prec.Tangent: 0.19738553750727910741 0.98020027673554288583 -0.015696084560412035258




% Format :  [Pixel_Pos]  Pixel_Dir Pixel_Conf  [Sub_Pixel_Pos] Sub_Pixel_Dir Strength Uncer
% [183, 141]    2.56705 49.3656   [182.86, 140.781]   2.56705 49.3656 0.239744
% [383, 138]    1.61739 36.848   [383.254, 137.951]   1.61739 36.848 0.698369
% [279, 378]    4.867 25.7912   [279.441, 378.487]   4.867 25.7912 0.174651
% [187, 53]    5.55885 41.9503   [187.333, 52.826]   5.55885 41.9503 0.325378

 
%edg_id1 = 25353;
%edg_id1 = 60873;
edg_id1 = 24940;
edg_id2 = 8441;

Gama_25353  = [20.264983282540946163 -29.21160486068595219 28.097390540699617389]';
Tgt_25353   = [-0.36462311318122969661 -0.92277217516084353299 -0.12466554489093507885]';

Gama_24940 = [47.36049190047523183 73.864221097343204292 -9.1104135881035599454]';
Tgt_24940  = [0.047151815788090779835 0.68133508472466774997 -0.73045137318723396547]';

Gama_60873 = [41.460984059231002163 -61.72945320434706673 14.21490237021884262]';
Tgt_60873  = [-0.074988667378282775067 -0.056303980019817762015 0.99559357249771218701]';

Gama_8441  = [19.001475054810271104 -36.02969355984117783 43.810136871732815678]';
Tgt_8441   = [0.19738553750727910741 0.98020027673554288583 -0.015696084560412035258]';

gama_img_25353 = [182.86, 140.781 1]';
gama_img_24940 =  [383.254, 137.951 1]';
gama_img_60873 = [279.441, 378.487 1]';
gama_img_8441  = [187.333, 52.826 1]';

tgt_25353 = [cos(2.56705) sin(2.56705) 0]';
tgt_24940 = [cos(1.61739) sin(1.61739) 0]';
tgt_60873 = [cos(4.867) sin(4.867) 0]';
tgt_8441  = [cos(5.55885) sin(5.55885) 0]';


Gama_all = [Gama_25353'; Gama_24940'; Gama_60873'; Gama_8441'];
gama_all_img = [gama_img_25353'; gama_img_24940'; gama_img_60873'; gama_img_8441'];
gama_all_img = gama_all_img(:,1:2);

Gama1  = eval(['Gama_' num2str(edg_id1)]);
Tgt1   = eval(['Tgt_' num2str(edg_id1)]);

Gama2  = eval(['Gama_' num2str(edg_id2)]);
Tgt2   = eval(['Tgt_' num2str(edg_id2)]);

gama1_img = eval(['gama_img_' num2str(edg_id1)]);
gama2_img = eval(['gama_img_' num2str(edg_id2)]);

tgt1 = eval(['tgt_' num2str(edg_id1)]);
tgt2 = eval(['tgt_' num2str(edg_id2)]);

tgt1_2d_pt = tgt1(1:2) + gama1_img(1:2);
tgt2_2d_pt = tgt2(1:2) + gama2_img(1:2);

% Camera

K_gt = [2200 0 -60
     0 2200 190
     0 0 1 ];

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

R_gt = [ 9.5299400000000001e-01   2.6928999999999997e-01   1.3887300000000000e-01
   2.4355199999999999e-01  -4.0819100000000003e-01  -8.7980800000000003e-01
  -1.8023600000000001e-01   8.7227500000000002e-01  -4.5458999999999999e-01];


C_gt = [   4.5003113255720002e+01  -3.0601355251041002e+02   1.5559322711681997e+02 ]';
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

%tgt1 = tgt1_reproj;
%tgt2 = tgt2_reproj;
%-----------


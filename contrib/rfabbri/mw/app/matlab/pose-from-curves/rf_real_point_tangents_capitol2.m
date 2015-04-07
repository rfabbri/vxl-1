% ids of triplets to estimate pose with
id1 = 1;
id2 = 2;

A = load('/home/rfabbri/lib/data/right-dome-subset-local-mcs-work/ground-truth/edgel-corresp-1-177-66-data_matlab_v1.dat');

cd /home/rfabbri/cprg/vxlprg/lemsvxlsrc-git2/contrib/rfabbri/mw/app/matlab/pose-from-curves/results-capitol2/work/

A=reshape(A',3,11,29);

ntrips = size(A,3);

Gama_all = zeros(ntrips,3);
Tgt_all = Gama_all;
gama_all_img = Gama_all;
tgt_all_img = Gama_all;
for i=1:ntrips
  Gama_all(i,:) = A(:,9,i);
  Tgt_all(i,:) = A(:,10,i);
  gama_all_img(i,:) = A(:,7,i);
  gama_all_img(i,3) = 1;
  tgt_all_img(i,:) = A(:,8,i);
end


Gama1 = Gama_all(id1,:)';
Gama2 = Gama_all(id2,:)';
Tgt1 = Tgt_all(id1,:)';
Tgt2 = Tgt_all(id2,:)';
gama1_img = gama_all_img(id1,:)';
gama2_img = gama_all_img(id2,:)';

tgt1_2d_pt = tgt_all_img(id1,1:2)' + gama1_img(1:2);
tgt2_2d_pt = tgt_all_img(id2,1:2)' + gama2_img(1:2);

% Camera
K_gt = [2200 0 -60
     0 2200 190
     0 0 1 ];

% Convert to world coordinates 

K_gt_inv = inv(K_gt);

gama_all = K_gt_inv*gama_all_img';
gama_all = gama_all';
gama_all(:,1) = gama_all(:,1)./gama_all(:,3);
gama_all(:,2) = gama_all(:,2)./gama_all(:,3);
gama_all(:,3) = ones(size(gama_all(:,3)));

gama1_cam = K_gt_inv*gama1_img;
gama1_cam(1) = gama1_cam(1)./gama1_cam(3);
gama1_cam(2) = gama1_cam(2)./gama1_cam(3);
gama1_cam(3) = 1;

gama2_cam = K_gt_inv*gama2_img;
gama2_cam(1) = gama2_cam(1)./gama2_cam(3);
gama2_cam(2) = gama2_cam(2)./gama2_cam(3);
gama2_cam(3) = 1;

tgt1_2d_pt_normal = K_gt_inv*[tgt1_2d_pt; 1];
tgt1_2d_pt_normal(1) = tgt1_2d_pt_normal(1) ./ tgt1_2d_pt_normal(3);
tgt1_2d_pt_normal(2) = tgt1_2d_pt_normal(2) ./ tgt1_2d_pt_normal(3);
tgt1_2d_pt_normal(3) = 1;

tgt2_2d_pt_normal = K_gt_inv*[tgt2_2d_pt; 1];
tgt2_2d_pt_normal(1) = tgt2_2d_pt_normal(1) ./ tgt2_2d_pt_normal(3);
tgt2_2d_pt_normal(2) = tgt2_2d_pt_normal(2) ./ tgt2_2d_pt_normal(3);
tgt2_2d_pt_normal(3) = 1;
   
tgt1 = tgt1_2d_pt_normal - gama1_cam;
tgt1 = tgt1/norm(tgt1);

tgt2 = tgt2_2d_pt_normal - gama2_cam;
tgt2 = tgt2/norm(tgt2);

tgt_all_2d_pt = tgt_all_img(:,1:2) + gama_all_img(:,1:2);
tgt_all_2d_pt_normal = K_gt_inv*[tgt_all_2d_pt ones(size(tgt_all_2d_pt,1),1)]';
tgt_all_2d_pt_normal = tgt_all_2d_pt_normal';
tgt_all_2d_pt_normal(:,1) = tgt_all_2d_pt_normal(:,1) ./ tgt_all_2d_pt_normal(:,3);
tgt_all_2d_pt_normal(:,2) = tgt_all_2d_pt_normal(:,2) ./ tgt_all_2d_pt_normal(:,3);
tgt_all_2d_pt_normal(:,3) = tgt_all_2d_pt_normal(:,3) ./ tgt_all_2d_pt_normal(:,3);

tgt_all = tgt_all_2d_pt_normal - gama_all;

ssum = sqrt(sum(tgt_all.*tgt_all, 2));
norms =  [ssum ssum ssum];
tgt_all = tgt_all ./ norms;

if (abs(sum(tgt_all(:,3))) > 1e-7)
  error('something is wrong with normalizing tgt pert');
end

tgt_all(:,3) = zeros(size(tgt_all,1),1);


% Ground-truth rotation and translation

R_gt = [
0.95299379755510815748 0.26928978507512568719 0.13887344409816462787
0.24355214643210248981 -0.40819111776693228766 -0.87980814007628849183
-0.18023643857391638545 0.8722746259237252886 -0.45458970861625958015 ];

C_gt = [ 45.003113255720002428 -306.01355251041002248 155.59322711681997475 ]';

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


gama_all_img = gama_all_img(:,1:2);

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

errors_gt = rf_reprojection_error(K_gt*[R_gt T_gt],...
         gama_all_img, Gama_all);
disp(['Data with reproj error ' num2str(mean(errors_gt)) 'px']);


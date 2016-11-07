% sp_demo.m
%
% See instructions in README.

if ~exist('cncut')
    addpath('/home/rfabbri/lib/matlab/superpixels/yu_imncut');
end

disp(['opening ' my_fname]);
I = im2double(imread(my_fname));

N = size(I,1);
M = size(I,2);

% Number of superpixels coarse/fine.
N_sp=50;
N_sp2=100;
% Number of eigenvectors (number of segments in the initial segmentation).
N_ev=40;


% ncut parameters for superpixel computation
diag_length = sqrt(N*N + M*M);
par = imncut_sp;
par.int=0;
par.pb_ic=1;
par.sig_pb_ic=0.05;
par.sig_p=ceil(diag_length/50);
par.verbose=0;
par.nb_r=ceil(diag_length/60);
par.rep = -0.005;  % stability?  or proximity?
par.sample_rate=0.2;
par.nv = N_ev;
par.sp = N_sp;

% Intervening contour using mfm-pb
fprintf('running PB\n');
[emag,ephase] = pbWrapper(I,par.pb_timing);
emag = pbThicken(emag);
par.pb_emag = emag;
par.pb_ephase = ephase;
clear emag ephase;

st=clock;
fprintf('Ncutting...');
[Sp,Seg] = imncut_sp(I,par);
fprintf(' took %.2f minutes\n',etime(clock,st)/60);

st=clock;
fprintf('Fine scale superpixel computation...');
Sp2 = clusterLocations(Sp,ceil(N*M/N_sp2));
fprintf(' took %.2f minutes\n',etime(clock,st)/60);


I_sp = segImage(I,Sp);
I_sp2 = segImage(I,Sp2);
I_seg = segImage(I,Seg);
figure;
subplot(1,4,1);
imshow(I);
subplot(1,4,2);
imshow(I_seg);
subplot(1,4,3);
imshow(I_sp);
subplot(1,4,4);
imshow(I_sp2);

disp(['writing results/sp2-' num2str(N_sp2) '-' my_fname])
imwrite(I_sp2, ['results/sp2-' num2str(N_sp2) '-'  my_fname]);
disp(['writing results/sp-' num2str(N_sp) '-' my_fname])
imwrite(I_sp2, ['results/sp-' num2str(N_sp) '-'  my_fname]);
disp(['writing results/seg-' num2str(N_ev) '-' my_fname])
imwrite(I_sp2, ['results/seg-' num2str(N_ev) '-'  my_fname]);


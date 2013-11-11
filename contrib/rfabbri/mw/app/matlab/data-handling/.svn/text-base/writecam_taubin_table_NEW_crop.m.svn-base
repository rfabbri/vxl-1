% models world coordinates as being in the rotation plane, and the y axis of the
% world coordinate system is the axis of rotation. The cameras are not
% necessarily aligned to this axis.

load Calib_Results.mat
load base_extrinsics_rect_crop.mat

% intrinsic

fname = 'calib.intrinsic';

KK(1:2,3) = KK(1:2,3) - crop_rect(:,1);


save(fname,'KK','-ascii','-double');

% extrinsic

my_nimages = 669
stepsize   = 2*pi / 670; 
for i=0:my_nimages

  theta = -(i)*stepsize;


  % R_cam1_to_cam2 = dR^(i);

  Rot_theta = ...
  [cos(theta) 0 -sin(theta)
        0     1      0
   sin(theta) 0 cos(theta)];

  R_W0_to_Wtheta= ...
  [cos(theta)  0 sin(theta)
        0      1      0
   -sin(theta) 0 cos(theta)];

%  R_cam1_to_cam2  = dR^i;

  R_world_to_cam2 =  Rckk * R_W0_to_Wtheta;


  C2_in_world = - Rot_theta * Rckk' * Tckk;

  % See Turntable notes march 23 2006
%  C2_in_cam1 = -norm(Tckk) * [sin(theta); 0; cos(theta) - 1];

%  C2_in_world = - R_world_to_cam2' * (C2_in_cam1 - Tckk);

  n_str = int2str(i);

  while (length(n_str) < 3)
    n_str = ['0' n_str];
  end

  fname = [n_str '.extrinsic'];

  C2_in_world = C2_in_world';
  %save(fname,'R_world_to_cam2','C2_in_world','-ascii','-double');
  save(fname,'R_world_to_cam2','-ascii','-double');
  save(fname,'C2_in_world','-ascii','-double','-append');

  disp(['Writing ' fname]);
end

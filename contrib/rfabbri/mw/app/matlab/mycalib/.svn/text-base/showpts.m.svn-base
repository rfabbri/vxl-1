% read  3D points
mypath='/home/rfabbri/lib/data/lafayette-1218/';

%nblock='4';
%nview='3';
%plotfig3d = 99
display_images = true;

myprefix = ['Lafayette_' nblock '_v' nview];

img_f = [myprefix '.jpg'];
cam_f = [myprefix '_cam.txt'];
center_direction_f = [myprefix '_center-direction.txt'];
pts2d_f = [myprefix '_2D_pnts.txt'];
pts3d_f = [myprefix '_3D_pnts.txt'];


% read cam
try
  cam=load([mypath cam_f]);
catch
  disp('shopts: file doesn''t exist.'); 
  disp(['Last error message: ' lasterror.message]);
  return;
end
pts2d=load([mypath pts2d_f]);
pts3d=load([mypath pts3d_f]);
center_direction=load([mypath center_direction_f]);
c = center_direction(1,:);
fvec = center_direction(2,:); % focal vector in world coords


% read img
im=rgb2gray(imread([mypath img_f]));
if display_images
  figure();
  clf;
  imshow(im);
  title(['block ' nblock ' view ' nview]);
end

show_err

clear;

% n=7500;
% T = readPose('pose.txt',n);

% finish 'em all
T = readPose('pose.txt');
n = max(size(T));

Rosette = readRosette('rosette.proto');

view = [1,2,3,4];   % visualize the frame of view 1 and view 2. The
% number of views in rosette can be found as length(Rosette)

% P = Cameras(i).pose(:,:,j) is the mapping between the world and
% camera coordinates such that Xc = P Xw
% The intrinsic parameters are in Rosette(i).calib for i-th camera

% Fill in cameras struct
for i=1:n
    invT = [T(1:3,1:3,i)' -T(1:3,1:3,i)'*T(1:3,4,i); 0 0 0 1];
    for j=1:length(view)
       Cameras(i).pose(:,:,view(j)) = Rosette(view(j)).pose*invT;
    end;
end;

% Write the fname.extrinsic files

cv = zeros(3,n);
for i=1:n
  for j=1:length(view)
    r = Cameras(i).pose(1:3, 1:3, view(j));
    t = Cameras(i).pose(1:3, 4, view(j));

    c = -r'*t;
    if (j == 1)
      cv(:,i) = c;
    end
    rc=[r; c'];
    fname_base = ['reprojected_'  num2str(i-1,'%.5i') '_' num2str(j-1)];
%    save(['/home/rfabbri/work/cityblock/' fname_base '.extrinsic'],'rc','-ascii','-double');
  end
  % example: reprojected_04845_3.jpg
end

close all; clear;
%n = 1000; % read first 10 frames

T = readPose('pose.txt');
n = max(size(T));
Rosette = readRosette('rosette.proto');


%figure(500); hold on;
draw_scale = 1;
view = [1,2];   % visualize the frame of view 1 and view 2. The
                % number of views can be found as length(Rosette)
for i=1:n
%    R = T(1:3,1:3,i); trans = T(1:3,4,i);
%    figure(500); axis equal; box on;
%    draw_frame_scaled_cb([R trans],draw_scale,0,[0 0 1]);
end;
title('camera rosette coordinate frames');

% P = Cameras(i).pose(:,:,j) is the mapping between the world and
% camera coordinates such that Xc = P Xw
% The intrinsic parameters are in Rosette(i).calib for i-th camera

%figure(400); hold on; axis equal; box on;
for i=1:n
    invT = [T(1:3,1:3,i)' -T(1:3,1:3,i)'*T(1:3,4,i); 0 0 0 1];
    for j=1:length(view)
       Cameras(i).pose(:,:,view(j)) = Rosette(view(j)).pose*invT;
%       draw_frame_scaled_cb(inv(Cameras(i).pose(:,:,view(j))), ...
%                            draw_scale,0,[0 0 1]);
    end;
end;
%title('coordinate frames of individual views');

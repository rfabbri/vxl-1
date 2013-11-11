% Draw frame at some specified position and specified orientation
% function[] = draw_frame_scaled(T,scale)
% T - (3x4) rigid body transformation matrix T
% scale - specifies the lenght of the axis
% flag - if 0 draws the image plane perpendicular to z-axis (z-axis is drawn in red)
% color - 3x1 color vector

function[] = draw_frame_scaled_cb(T,scale, flag, colorRand)

rot = T(1:3,1:3);
trans = T(1:3,4);
plot3(trans(1),trans(2),trans(3),'k.');

% coordinates of the vectors in the init frame
i0 = [1 0 0]';
j0 = [0 1 0]';
k0 = [0 0 1]';

i = scale*T(1:3,1:3)*i0 + trans;
j = scale*T(1:3,1:3)*j0 + trans;
k = scale*T(1:3,1:3)*k0 + trans;

pos = trans;

vx = scale*T(1:3,1:3)*i0/norm(T(1:3,1:3)*i0);
vy = scale*T(1:3,1:3)*j0/norm(T(1:3,1:3)*j0);
vz = scale*T(1:3,1:3)*k0/norm(T(1:3,1:3)*k0);

h1 = quiver3(pos(1),pos(2),pos(3),vx(1),vx(2),vx(3),0);
set(h1,'Color','black');
h2 = quiver3(pos(1),pos(2),pos(3),vy(1),vy(2),vy(3),0);
set(h2,'Color','blue');
h3 = quiver3(pos(1),pos(2),pos(3),vz(1),vz(2),vz(3),0);
set(h3,'Color','red');

% flag for printing the image plane

if flag
  % transform all the end points of the image plane by T
  p1 = [scale scale 0]';
  p2 = [scale -scale 0]';
  p3 = [-scale -scale 0]';
  p4 = [-scale scale 0]';

  p1t = rot*p1 + trans;
  p2t = rot*p2 + trans;
  p3t = rot*p3 + trans;
  p4t = rot*p4 + trans;

  % plot the image plane
  if isempty(colorRand)
    colorRand = 'b';
  end;
  line([p1t(1), p2t(1)], [p1t(2) p2t(2)], [p1t(3) p2t(3)],'Color',colorRand);
  line([p2t(1), p3t(1)], [p2t(2) p3t(2)], [p2t(3) p3t(3)],'Color',colorRand);
  line([p3t(1), p4t(1)], [p3t(2) p4t(2)], [p3t(3) p4t(3)],'Color',colorRand);
  line([p4t(1), p1t(1)], [p4t(2) p1t(2)], [p4t(3) p1t(3)],'Color',colorRand);
end

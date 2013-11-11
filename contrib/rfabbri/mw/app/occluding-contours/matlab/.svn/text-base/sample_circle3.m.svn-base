% samples a circle in 3D, given its defining parameters
%
% t: vector of parameters to sample disk
function [p]=sample_circle3(center,radius,binormal,t)

if norm(binormal) < 0.99
  disp 'Error: normal is not unit'
  return
end

if abs(binormal(1)) > abs(binormal(2))
  i_big = 1
else
  i_big = 2;
end

if abs(binormal(3)) > abs(binormal(i_big))
  i_big = 3
end

i_o1 = mod(i_big+1,3);
i_o2 = mod(i_big+2,3);

u(i_o1)  = 1;
u(i_o2)  = 0;
u(i_big) = -binormal(i_o1)/binormal(i_big);
u = u/norm(u);
u = u';

v = cross(binormal,u);

p = center*ones(size(t)) +   radius* [u v]*[cos(t); sin(t)];
p = p';


% Convert a 3x3 rotation matrix to a quaternion.  NB: The solution is
% discontinuous q <-> -q for 180 degree rotations. FIXME: Handle cases
% near this more gracefully.

function q = rot_to_quat(R)
   c = sqrt(1+trace(R));
   A = R-R';
   q = [[A(3,2)-A(2,3); A(1,3)-A(3,1); A(2,1)-A(1,2)]/(4*c); c/2];
%end
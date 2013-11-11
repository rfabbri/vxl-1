% R = quat_to_rot(q)
% Convert a quaternion to the corresponding rotation matrix. Scalar
% component is q(4).

function R = quat_to_rot(q)
   q = q/norm(q);
   R = [q(4)^2+q(1)^2-q(2)^2-q(3)^2, 2*(q(1)*q(2)-q(3)*q(4)), 2*(q(4)*q(2)+q(1)*q(3));
        2*(q(4)*q(3)+q(1)*q(2)), q(4)^2-q(1)^2+q(2)^2-q(3)^2, 2*(q(2)*q(3)-q(4)*q(1));
	2*(q(1)*q(3)-q(4)*q(2)), 2*(q(4)*q(1)+q(2)*q(3)), q(4)^2-q(1)^2-q(2)^2+q(3)^2];
%end;

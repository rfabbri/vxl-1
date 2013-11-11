%to be called by rotation_experiment.m

% input p0 and p1 must have only 4 points
function  dv = det_value_for_angle(angle,e0,e1,p0,p1);

  rotm = [cos(angle), -sin(angle);
          sin(angle)  cos(angle)];

%  p4_e0_rot = rotm*e0';
  p4_e0_rot = e0';
  p4_e1_rot = rotm*e1';

%  p4_0_rot = p0*rotm';
  p4_0_rot = p0;
  p4_1_rot = p1*rotm';

  % compute det

  [M,dv] = four_line_test_cos(p4_e0_rot,p4_e1_rot,p4_0_rot,p4_1_rot);


function [pts3d, normals, conf] = rf_reconstruct_curve_point_shubao(edgel, projmatrix, nsamples)
%
% Perform Shubao's subspace-based reconstruction from tracked edgels.
% 
% Ricardo Fabbri modularized and documented the code on Fri Sep 25 17:31:36 EDT 2009
% 
% Input:
% 
% edgel: set of 3 corresponding edgels with pixel-precision position:
%   for i in 1:3
%     i=1 represents previous frame
%     i=2 represents current frame
%     i=3 represents next frame 
%     edgel{i}.x (col)
%     edgel{i}.y (row) 
%     edgel{i}.orientation in (-pi,pi] range
%
% projmatrix: the perspective projection matrix P for the 3 views:
%     projmatrix{i}
%
% nsamples: how many samples to take from the interpolation of the 3 views, e.g.
% 10. This will cause nsamples 3D points and normals to be generated, and can be
% used to generate a denser sample on the reconstructed surface.
%
% Output: 
% 
% pts3d - 3D point. pts3d(:,v) is the [x y z]' for each nsamples.
% normals - 3D normal. normals(:,v) the [x y z]' for the normal.
% conf - confidence. conf(:,v) is the confidence value for pts3d(:,v)
%
  if nsamples == 1
      vsamples = 0;
  else
      vsamples = linspace(0,1,nsamples);
  end

  indleft = 1;
  indmiddle = 2;
  indright = 3;
  ymiddle = edgel{indmiddle}.y; 
  xmiddle = edgel{indmiddle}.x; 
  thetamiddle = edgel{indmiddle}.orientation;
  pTmiddle = [-sin(thetamiddle); cos(thetamiddle); -cos(thetamiddle)*ymiddle+sin(thetamiddle)*xmiddle];
  pNmiddle = [-cos(thetamiddle); -sin(thetamiddle); sin(thetamiddle)*ymiddle+cos(thetamiddle)*xmiddle];
  PTmiddle = projmatrix{indmiddle}'*pTmiddle; PTmiddle = PTmiddle/norm(PTmiddle(1:3),2); 
%             if PTMiddle(4)~=0 
%                 PTmiddle = sign(PTmiddle(4))*PTmiddle;
%             end
  PNmiddle = projmatrix{indmiddle}'*pNmiddle; PNmiddle = PNmiddle/norm(PNmiddle(1:3),2); 
%             if PNmiddle(4)~=0
%                 PNmiddle = sign(PTmiddle(4))*PNmiddle;
%             end
  yright = edgel{indright}.y;
  xright =  edgel{indright}.x;
  thetaright = edgel{indright}.orientation;
  pTright = [-sin(thetaright); cos(thetaright); -cos(thetaright)*yright+sin(thetaright)*xright];
  pNright = [-cos(thetaright); -sin(thetaright); sin(thetaright)*yright+cos(thetaright)*xright];
  PTright = projmatrix{indright}'*pTright; 
  PTright = PTright/norm(PTright(1:3),2); 

  %PTright = sign(PTright(4))*PTright;
  PNright = projmatrix{indright}'*pNright; 
  PNright = PNright/norm(PNright(1:3),2); 
  %PNright = sign(PNright(4))*PNright;
  
  yleft =  edgel{indleft}.y;
  xleft =  edgel{indleft}.x;
  thetaleft = edgel{indleft}.orientation;
  pTleft = [-sin(thetaleft); cos(thetaleft); -cos(thetaleft)*yleft+sin(thetaleft)*xleft];
  pNleft = [-cos(thetaleft); -sin(thetaleft); sin(thetaleft)*yleft+cos(thetaleft)*xleft];
  PTleft = projmatrix{indleft}'*pTleft; 
  PTleft = PTleft/norm(PTleft(1:3),2); %PTleft = sign(PTleft(4))*PTleft;
  PNleft = projmatrix{indleft}'*pNleft; 
  PNleft = PNleft/norm(PNleft(1:3),2); %PNleft = sign(PNleft(4))*PNleft;
  
  % normalize
  PTleft = PTleft/norm(PTleft(1:3),2);
  PNleft = PNleft/norm(PNleft(1:3),2);
  PTmiddle = PTmiddle/norm(PTmiddle(1:3),2);
  PNmiddle = PNmiddle/norm(PNmiddle(1:3),2);
  PTright = PTright/norm(PTright(1:3),2);
  PNright = PNright/norm(PNright(1:3),2);
  


  % then fit a spline
  PTsp(1) = rf_quadratic(PTleft(1), PTmiddle(1), PTright(1));
  PTsp(2) = rf_quadratic(PTleft(2), PTmiddle(2), PTright(2));
  PTsp(3) = rf_quadratic(PTleft(3), PTmiddle(3), PTright(3));
  PTsp(4) = rf_quadratic(PTleft(4), PTmiddle(4), PTright(4));
                                                
  PNsp(1) = rf_quadratic(PNleft(1), PNmiddle(1), PNright(1));
  PNsp(2) = rf_quadratic(PNleft(2), PNmiddle(2), PNright(2));
  PNsp(3) = rf_quadratic(PNleft(3), PNmiddle(3), PNright(3));
  PNsp(4) = rf_quadratic(PNleft(4), PNmiddle(4), PNright(4));
      
  % then get the derivative
  dPTdvsp(1) = rf_quadratic_derivative(PTsp(1));
  dPTdvsp(2) = rf_quadratic_derivative(PTsp(2));
  dPTdvsp(3) = rf_quadratic_derivative(PTsp(3));
  dPTdvsp(4) = rf_quadratic_derivative(PTsp(4));
  
  dPNdvsp(1) = rf_quadratic_derivative(PNsp(1));
  dPNdvsp(2) = rf_quadratic_derivative(PNsp(2));
  dPNdvsp(3) = rf_quadratic_derivative(PNsp(3));
  dPNdvsp(4) = rf_quadratic_derivative(PNsp(4));
  
  PT(1,:) = rf_quadratic_eval(PTsp(1),vsamples);
  PT(2,:) = rf_quadratic_eval(PTsp(2),vsamples);
  PT(3,:) = rf_quadratic_eval(PTsp(3),vsamples);
  PT(4,:) = rf_quadratic_eval(PTsp(4),vsamples);

  PN(1,:) = rf_quadratic_eval(PNsp(1),vsamples);
  PN(2,:) = rf_quadratic_eval(PNsp(2),vsamples);
  PN(3,:) = rf_quadratic_eval(PNsp(3),vsamples);
  PN(4,:) = rf_quadratic_eval(PNsp(4),vsamples);

  dPTdv(1,:) = rf_quadratic_eval(dPTdvsp(1),vsamples);
  dPTdv(2,:) = rf_quadratic_eval(dPTdvsp(2),vsamples);
  dPTdv(3,:) = rf_quadratic_eval(dPTdvsp(3),vsamples);
  dPTdv(4,:) = rf_quadratic_eval(dPTdvsp(4),vsamples);
  
  dPNdv(1,:) = rf_quadratic_eval(dPNdvsp(1),vsamples);
  dPNdv(2,:) = rf_quadratic_eval(dPNdvsp(2),vsamples);
  dPNdv(3,:) = rf_quadratic_eval(dPNdvsp(3),vsamples);
  dPNdv(4,:) = rf_quadratic_eval(dPNdvsp(4),vsamples);
      
  Clr = nullvector(PTleft, PNleft, PTright, PNright); % cross point
  Clr = Clr/Clr(4);
  for v = 1:nsamples
      [pts3d(:,v), normal_pseudo, conf(:,v)] = recover3d(PT(:,v), PN(:,v), dPTdv(:,v), dPNdv(:,v));
      if abs(Clr'*PTmiddle)<5e-3
          normals(:,v) = [0 0 0]';
          pts3d(:,v) = Clr(1:3);
      else
          normals(:,v) = sign(Clr'*PTmiddle)*normal_pseudo; %% add the threshold checking in the future
      end
  end

function [pt3d, normal, conf] = recover3d(PT, PN, dPTdv, dPNdv)

A = [PT PN dPTdv dPNdv]';
[U,D,V] = svd(A);
pt3d_homo = V(:,4);
pt3d = pt3d_homo(1:3)/pt3d_homo(4);
conf = diag(D);
normal = PT(1:3);
normal = normal/norm(normal);

% ----------------------------------
function b = nullvector(a1,a2,a3,a4)

A = [a1 a2 a3 a4]';
[U,D,V] = svd(A);
b = V(:,4);


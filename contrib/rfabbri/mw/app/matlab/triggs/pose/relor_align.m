% Relative orientation of two calibrated cameras by rotation cancelling
% iteration. Assumes that translation is small and returns only one
% solution, but does not fail for t=0 or coplanar points.


% points, using sparse resultant. Similar but not identical to the
% method reported in [1].
%
% Let x be the direction vector and dx the distance to a 3D point in
% camera frame 1, and similarly y, dy in image 2. Let the motion between
% the camera frames be R(q),t, where q is a quaternion parametrizing the
% 3x3 inter-frame rotation R and t is the translation between the camera
% centres. Then 
%
%	dy * y = R * (dx * x) + t			(1)
%
% Also, the points will be visible iff dx>0 and dy>0.  The coplanarity
% constraint is
%
%	[y,R*x,t] = 0 

function [Rt,cond] = relor_align(ys,xs)
   ys = ys ./ (ones(3,1)*sqrt(sum(ys.^2)));
   xs = xs ./ (ones(3,1)*sqrt(sum(xs.^2)));
   R = dirns_to_rot(ys,xs);
   Rx = R*xs;
   YxX = (ys([2,3,1],:).*Rx([3,1,2],:)-ys([3,1,2],:).*Rx([2,3,1],:))';
   [U,S,V] = svd(YxX);
   t = V(:,3);
   [Rt,cond] = Rt_from_pts_nl([R,-R'*t],xs,ys);
   Rt(:,4) = -Rt(:,1:3)*Rt(:,4);


%    for it = 1:100
%       Rx = R*xs;
%       YxX = (ys([2,3,1],:).*Rx([3,1,2],:)-ys([3,1,2],:).*Rx([2,3,1],:))';
%       [U,S,V] = svd(YxX);
%       t = V(:,3);
%       A = (t*sum(ys.*Rx) - ys.*(ones(3,1)*(t'*Rx)))';
%       b = YxX*t;
%       r = -A \ b;
%       R = quat_to_rot([r/2;1])*R;
%       [it,norm(r),S(3,3)/S(1,1)]
%       if norm(r)<1e-6, break; end;
%    end;
%    Rt = [R,t];
%    cond = S(3,3)/S(1,1);
end;
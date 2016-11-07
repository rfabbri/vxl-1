% [U1,U2] = circ_pose(C,side) --- Given a calibrated image of a 3D circle, 
% return the two possible solutions for the measurable part of the
% camera pose relative to it.

% Input: a symmetric 3x3 matrix C representing the image of the circle
% as an implicit point conic in calibrated camera coordinates: x'* C * x
% = 0 for all points x on the image of the circle. Optional argument
% SIDE is any direction vector in the camera frame pointing towards
% (rather than away from) the circle's plane. (E.g. the image of any
% point on the circle, or any other finite visible point on the
% plane). It is used only to disambiguate between twisted pairs of
% rotations, as no sidedness information is contained in C itself.
% The default value is the optical axis, which will suffice for most
% applications. 

% Output: two 3x8 matrices U = [R,t,H,c] representing the two possible
% solutions. Each of these is expressed w.r.t. a nominal 3D camera frame
% centred at the estimated circle centre and looking straight into the
% circle plane (i.e. camera +z is into plane). This frame is only
% defined up to rotations about the optical axis [0;0;1], but this is the
% only remaining pose ambiguity. Distance is measured in units of the
% circle radius. R is the camera's angular pose and t its optical centre
% w.r.t. this frame, and R * [eye(3),-t] is its projection
% matrix. R(:,3) is the rotated optical axis, and t(3)<0 for realistic
% camera positions above the plane. 
% H = R * [ 1,0,-t(1); 0,1,-t(2); 0,0,-t(3) ] is the 3x3 image-to-image
% homography from a nominal image with t = [0;0;1] (i.e. looking
% straight at the circle from one radius away) to the actual image.
% c = H * [0;0;1] is the estimated image of the circle's centre. H'*C*H =
% diag([1,1,-1]) is the nominalized image of the circle.

% R, H, t are defined only up to R -> R * Rz, H -> H * Rz, t -> Rz' * t,
% where Rz = [cos(a),-sin(a),0; sin(a),cos(a),0; 0,0,1] for arbitrary a.
% R * t is always perpendicular to the long axis of the image ellipse.
% The origin for Rz is chosen so that this is the nominal y direction,
% i.e. t(2)=0.  For near-circular (i.e. near-frontoparallel) images,
% sin(a) and x=t(1) are of order sqrt(e) where e is the eccentricity
% (long_axis/short_axis-1).  Hence the estimated translation t (and less
% importantly the origin of Rz) are especially sensitive to noise for
% frontoparallel circles.  Also, the two solutions U1,U2 differ by +-x
% and coincide in this case.

% Method: The formulae in the code can be derived by brute force,
% working w.r.t. an eigenbasis E for C ~ inv(H)' * diag([1,1,-1]) *
% inv(H), where H is as above. Basically, R = E * Ry where Ry is an
% additional rotation that converts the diagonal eigenmatrix
% L=diag([L(1),L(2),L(3)]) of C into something non-diagonal of the form
% C1 = inv(H1)' * diag([1,1,-1]) * inv(H1) where H1 = [1,0,-t(1); 0,1,0;
% 0,0,-t(3)] is the homography induced by a pure translation. Calculate
% C1(H1), diagonalize it, equate to L, and back-solve for t.

% Alternatively, a 3D circle passes through its plane's two circular
% points.  These lie on the 3D absolute conic, so their images lie on the
% image of the absolute conic IAC (= I in calibrated coordinates).  Hence
% the circular point images are among the 4 points of intersection (two
% complex conjugate pairs) of the IAC and the circle's image C.
% Algebraically, the intersection points x are defined by the fact that
% they lie on any linear combination of the two conics: x'(C+lambda*IAC)x
% = 0 for all lambda. Solving this in an eigenbasis E for C gives the two
% circular points, which in turn define the R matrix above. R(:,3) is the
% image of the plane's line at infinity. The dual of this in C is the
% image of the circle's centre: c = inv(C)*R(:,3).

function [U1,U2] = circ_pose(C,side)
   
   if (nargin<2) side = [0; 0; 1]; end;
   
   % Symmetric eigendecomposition C ~ E * L * E', with sign changes to
   % make signature (++-), eigenvalues in decreasing order, and E a
   % proper rotation with E(:,3) pointing forwards.  NB: Octave doesn't
   % actually have a symmetric eigendecomposition routine so eigs()
   % fakes it (somewhat unreliably).

   if (det(C) > 0) C = -C; end;
   [E,L] = eigs(C);
   L = L / L(2);
   if (det(E) < 0) E = -E; end;
   
   % Calculate motion parameters. Disambiguate twist of R and sign of c
   % by requiring camera to roughly face direction `side'.
   
   c = sqrt( (1-L(3)) / (L(1)-L(3)) );
   s = sqrt( (L(1)-1) / (L(1)-L(3)) );
   x = sqrt( (1-L(1)) * (1-L(3)) / (L(1)*L(3)) );   
   z = 1 / sqrt( -L(1)*L(3) );

   R1 = E * [c,0,-s; 0,1,0; s,0,c];
   R2 = E * [c,0,s; 0,1,0; -s,0,c];

   c1 = E * [s/L(1); 0; -c/L(3)];         
   c2 = E * [-s/L(1); 0; -c/L(3)];   

   if (side' * R1(:,3) < 0) R1 = R1 * diag([-1,1,-1]); end;
   if (side' * R2(:,3) < 0) R2 = R2 * diag([-1,1,-1]); end;
   if (side' * c1 < 0) c1 = -c1; end;
   if (side' * c2 < 0) c2 = -c2; end;
   
   t1 = [-x; 0; -z];
   t2 = [x; 0; -z];

   H1 = R1 * [1,0,x; 0,1,0; 0,0,z];
   H2 = R2 * [1,0,-x; 0,1,0; 0,0,z];   

   U1 = [R1, t1, H1, c1/norm(c1)];
   U2 = [R2, t2, H2, c2/norm(c2)];

%end;

% [U1,U2] = circ_to_cpts(C) --- Given the image of a 3D circle in
% calibrated coordinates, return the two possible solutions for the images
% of its centre and the two circular points of its plane.

% Input: a symmetric 3x3 matrix C representing the image of the circle as
% an implicit point conic in calibrated camers coordinates: x'*C*x=0 for
% all points x on the image of the circle.

% Output: two 3x3 matrices U = [c,x,y] representing the two possible Euclidean
% structures of the circle's 3D plane in image coordinates. Each c is a
% possible image of the 3D circle's centre. x,y are associated points
% representing the images (i.e. vanishing points) of two orthonormal
% directions in the 3D plane.  The plane's horizon (image of its line at
% infinity) is the line through x and y, and the images of its circular
% points are x+-i*y where i=sqrt(-1).  The image of the 3D plane's normal
% lies on the line c-x. (NB: for near fronto-parallel planes whose
% normal projects to something near c, the direction of x is only weakly
% constrained by this).

% Theory: A 3D circle passes through its plane's two circular points.
% These lie on the 3D absolute conic, so their images lie on the image of
% the absolute conic IAC.  Hence the circular point images are among the 4
% points of intersection (two complex conjugate pairs) of the IAC and the
% circle's image C.  Algebraically, the intersection points x are defined
% by the fact that they lie on any linear combination of the two conics:
% x'(C + lambda*IAC)x = 0 for all lambda.

% In calibrated image coords IAC=I. We can work in an orthonormal
% eigenbasis for C: C*E = E*diag(L1,L2,L3), where the Li are the
% eigenvalues and the columns of E the eigenvectors.
% Also, we assume that C has signature (++-) (change sign C:=-C if
% necessary) and that the eigenvalues are sorted L1>=L2>0>L3.  In this basis
% the equations are easily solved to find the intersection points
% (+-a1,+-i,+-a3) where a1=sqrt((L2-L3)/(L1-L3)),
% a3=sqrt((L1-L2)/(L1-L3)), a1^2+a3^2=1. Up to overall sign and complex
% conjugation, this gives two essentially distinct solutions. (These
% coincide for fronto-parallel planes a3=0).  In this basis, the
% orthogonal direction vectors are then x=(a1,0,+-a3), y=(0,1,0) and the
% circle's centre is c=(a3/L1,0,+-a1/L3). For small circles of radius r,
% L3/L1 = O(r^2) and the two centres coincide to O(r^2).


function [U1,U2] = circ_to_cpts(C)

   % Sorted eigendecomposition of +-C = E*L*E'
   [E,L] = eig(C);
   L = diag(L);
   if (sum(L>0)<2) L = -L; end;  % make sure signature is (++-)
   [L,I] = sort(L);
   L = flipud(L);
   E = E(:,flipud(I));

   a1 = sqrt((L(2)-L(3))/(L(1)-L(3)));
   a3 = sqrt((L(1)-L(2))/(L(1)-L(3)));

   c1 = a3/L(1)*E(:,1)-a1/L(3)*E(:,3);
   c2 = a3/L(1)*E(:,1)+a1/L(3)*E(:,3);

   U1 = [c1/norm(c1), a1*E(:,1)+a3*E(:,3), E(:,2)];
   U2 = [c2/norm(c2), a1*E(:,1)-a3*E(:,3), E(:,2)];
end;

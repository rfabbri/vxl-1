% Build multiresultant matrix for 4 point DLT-like pose method, and
% extract the solution from its null vector.

% The input is Npolys>=4 homogeneous quadratic polynomials in the
% variables u1..u4, represented as an Npolys x 10 matrix of coefficients
% of [u1^2,u1*u2,u2^2,u1*u3,...,u3*u4,u4^2]. The output is a vector u =
% (u1..u4) approximating a common root of the polynomials, in the sense
% that it approximately minimizes some horribly twisted and suboptimal
% heuristic measure of their average deviation from 0.

% In the DLT pose+calibration application, the variables u represent a 4
% parameter family P(u) = u1*P1+..+u4*P4 of possible 3x4 projection
% matrices (where the Pi are known matrices). The first 4 polynomials
% represent the aspect_ratio=1, skew=0, principal_point=(0,0)
% calibration constraints on P(u). The remaining input polynomials (if
% any) represent further soft constraints such as (i) a prior
% distribution on focal length f, and (ii) the residual projection error
% of P(u) on the input points. These extra constraints are not (usually)
% indispensable, but they help to produce a well-conditioned solution.

function [u,cond] = dlt4mres(M)

   % Build the big multiresultant matrix from the coefficients of the
   % input polynomials. Each input polynomial generates 20 rows
   % representing 20 different monomial multiples of the polynomial.
   % Each row contains all 10 of the polynomial's coefficients, padded
   % out with zeros. Each column corresponds to one of the 56 different
   % monomials of degree 5 in 4 variables. The null vector of the
   % multiresultant matrix contains (up to a global rescaling) the
   % values of these monomials at the solution, so we can read the
   % solution off from it. The matrix was built using my MAPLE
   % multiresultant routines (available from me if you want them).

   npolys = size(M,1);
   R = zeros(20*npolys,56);
   for i = 1:npolys
      Ri = zeros(20,56);
      x = M(i,1);
      Ri(1,47)=x;  Ri(2,39)=x;  Ri(3,38)=x;  Ri(4,37)=x;  Ri(5,27)=x;
      Ri(6,26)=x;  Ri(7,25)=x;  Ri(8,24)=x;  Ri(9,23)=x;  Ri(10,22)=x;
      Ri(11,10)=x; Ri(12,9)=x;  Ri(13,8)=x;  Ri(14,7)=x;  Ri(15,6)=x;
      Ri(16,5)=x;  Ri(17,4)=x;  Ri(18,3)=x;  Ri(19,2)=x;  Ri(20,1)=x;

      x = M(i,2);
      Ri(1,48)=x;  Ri(2,41)=x;  Ri(3,40)=x;  Ri(4,38)=x;  Ri(5,30)=x;
      Ri(6,29)=x;  Ri(7,28)=x;  Ri(8,26)=x;  Ri(9,25)=x;  Ri(10,23)=x;
      Ri(11,14)=x; Ri(12,13)=x; Ri(13,12)=x; Ri(14,11)=x; Ri(15,9)=x;
      Ri(16,8)=x;  Ri(17,7)=x;  Ri(18,5)=x;  Ri(19,4)=x;  Ri(20,2)=x;

      x = M(i,3);
      Ri(1,50)=x;  Ri(2,44)=x;  Ri(3,43)=x;  Ri(4,40)=x;  Ri(5,34)=x;
      Ri(6,33)=x;  Ri(7,32)=x;  Ri(8,29)=x;  Ri(9,28)=x;  Ri(10,25)=x;
      Ri(11,19)=x; Ri(12,18)=x; Ri(13,17)=x; Ri(14,16)=x; Ri(15,13)=x;
      Ri(16,12)=x; Ri(17,11)=x; Ri(18,8)=x;  Ri(19,7)=x;  Ri(20,4)=x;

      x = M(i,4);
      Ri(1,49)=x;  Ri(2,42)=x;  Ri(3,41)=x;  Ri(4,39)=x;  Ri(5,31)=x;
      Ri(6,30)=x;  Ri(7,29)=x;  Ri(8,27)=x;  Ri(9,26)=x;  Ri(10,24)=x;
      Ri(11,15)=x; Ri(12,14)=x; Ri(13,13)=x; Ri(14,12)=x; Ri(15,10)=x;
      Ri(16,9)=x;  Ri(17,8)=x;  Ri(18,6)=x;  Ri(19,5)=x;  Ri(20,3)=x;

      x = M(i,5);
      Ri(1,51)=x;  Ri(2,45)=x;  Ri(3,44)=x;  Ri(4,41)=x;  Ri(5,35)=x;
      Ri(6,34)=x;  Ri(7,33)=x;  Ri(8,30)=x;  Ri(9,29)=x;  Ri(10,26)=x;
      Ri(11,20)=x; Ri(12,19)=x; Ri(13,18)=x; Ri(14,17)=x; Ri(15,14)=x;
      Ri(16,13)=x; Ri(17,12)=x; Ri(18,9)=x;  Ri(19,8)=x;  Ri(20,5)=x;

      x = M(i,6);
      Ri(1,52)=x;  Ri(2,46)=x;  Ri(3,45)=x;  Ri(4,42)=x;  Ri(5,36)=x;
      Ri(6,35)=x;  Ri(7,34)=x;  Ri(8,31)=x;  Ri(9,30)=x;  Ri(10,27)=x;
      Ri(11,21)=x; Ri(12,20)=x; Ri(13,19)=x; Ri(14,18)=x; Ri(15,15)=x;
      Ri(16,14)=x; Ri(17,13)=x; Ri(18,10)=x; Ri(19,9)=x;  Ri(20,6)=x;

      x = M(i,7);
      Ri(1,53)=x;  Ri(2,49)=x;  Ri(3,48)=x;  Ri(4,47)=x;  Ri(5,42)=x;
      Ri(6,41)=x;  Ri(7,40)=x;  Ri(8,39)=x;  Ri(9,38)=x;  Ri(10,37)=x;
      Ri(11,31)=x; Ri(12,30)=x; Ri(13,29)=x; Ri(14,28)=x; Ri(15,27)=x;
      Ri(16,26)=x; Ri(17,25)=x; Ri(18,24)=x; Ri(19,23)=x; Ri(20,22)=x;

      x = M(i,8);
      Ri(1,54)=x;  Ri(2,51)=x;  Ri(3,50)=x;  Ri(4,48)=x;  Ri(5,45)=x;
      Ri(6,44)=x;  Ri(7,43)=x;  Ri(8,41)=x;  Ri(9,40)=x;  Ri(10,38)=x;
      Ri(11,35)=x; Ri(12,34)=x; Ri(13,33)=x; Ri(14,32)=x; Ri(15,30)=x;
      Ri(16,29)=x; Ri(17,28)=x; Ri(18,26)=x; Ri(19,25)=x; Ri(20,23)=x;

      x = M(i,9);
      Ri(1,55)=x;  Ri(2,52)=x;  Ri(3,51)=x;  Ri(4,49)=x;  Ri(5,46)=x;
      Ri(6,45)=x;  Ri(7,44)=x;  Ri(8,42)=x;  Ri(9,41)=x;  Ri(10,39)=x;
      Ri(11,36)=x; Ri(12,35)=x; Ri(13,34)=x; Ri(14,33)=x; Ri(15,31)=x;
      Ri(16,30)=x; Ri(17,29)=x; Ri(18,27)=x; Ri(19,26)=x; Ri(20,24)=x;

      x = M(i,10);
      Ri(1,56)=x;  Ri(2,55)=x;  Ri(3,54)=x;  Ri(4,53)=x;  Ri(5,52)=x;
      Ri(6,51)=x;  Ri(7,50)=x;  Ri(8,49)=x;  Ri(9,48)=x;  Ri(10,47)=x;
      Ri(11,46)=x; Ri(12,45)=x; Ri(13,44)=x; Ri(14,43)=x; Ri(15,42)=x;
      Ri(16,41)=x; Ri(17,40)=x; Ri(18,39)=x; Ri(19,38)=x; Ri(20,37)=x;

      R(20*i-19:20*i,:) = Ri;
   end;

   % SVD the resultant matrix to estimate its null vector (i.e. its
   % right singular vector with smallest singular value, V(:,56)).  This
   % is where most of the time is spent. There are faster ways to
   % calculate null vectors, but SVD is simple and stable.

   [U,S,V] = svd(R);
   v = V(:,56);
   if nargout>1
      S = diag(S)';
      cond = [S(55)/S(1), S(56)/S(55)];
   end;

   % Read off the solution (u1,..,u4) from the null vector v. v contains
   % (up to scale) all the monomials of degree 5 in the unknowns u.i.
   % This includes many sets of 4 monomials of the form
   % (u1,..,u4)*(some_monomial). We could read off the solution from any
   % of these, but for a large signal-to-noise ratio (some_monomial)
   % should be chosen to be as large as possible. We try all (u.i)^4 for
   % i=1:4 and take the largest.  uchoice = const * [u1;u2;u3;u4] *
   % [u1,u2,u3,u4].^4

   uchoice = [ v([1;2;3;22]), ...
	       v([11;16;17;32]), ...
	       v([15;20;21;36]), ...
	       v([53;54;55;56]) ];
   nu = sum(uchoice.^2);
   i = 1;
   for j=2:4 
      if (nu(j)>nu(i)) i=j; end;
   end;
   u = uchoice(:,i);

%end;

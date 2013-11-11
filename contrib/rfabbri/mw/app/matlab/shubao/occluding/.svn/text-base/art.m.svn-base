function [A,R,t] = art(P,fsign)
% ART  factorize the  PPM P as P=A*[R;t]
%
%         [A,R,t] = art(P,fsign)  factorize the projection matrix P 
%         as P=A*[R;t] and enforce the sign  fsign for the focal lenght. 
%         By defaukt fsign=1.

%         Andrea Fusiello, 1999 (fusiello@sci.univr.it)

%         29/01/2004: sign enforcing added to fix the QR ambiguity;
%         10/04/2204: changed default focal lenght to positive
%         27/04/2004; fixed sign of B(3,3)
%         27/02/2005; another (the last?) fix regarding signs


% by default assume POSITIVE focal lenght
if nargin == 1
    fsign = 1;
end

s = P(1:3,4);
Q = inv(P(1:3, 1:3));
[U,B] = qr(Q);

% fix the sign of B(3,3). This can possibly change the sign of the resulting matrix,
% which is defined up to a scale factor, however.
sig = sign(B(3,3));
B=B*sig;
s=s*sig;

% if the sign of the focal lenght is not the required one, 
% change it, and change the rotation accordingly.

if fsign*B(1,1) < 0
     E= [-1     0     0
         0    1     0
         0     0     1];
     B = E*B;
     U = U*E;
 end
 
 if fsign*B(2,2) < 0
     E= [1     0     0
         0    -1     0
         0     0     1];
     B = E*B;
     U = U*E;
 end
 
% if U is not a rotation, fix the sign. This can possibly change the sign
% of the resulting matrix, which is defined up to a scale factor, however.
if det(U)< 0 
    U = -U;
    s= - s;
end

  
% sanity check 
if (norm(Q-U*B)>1e-10) & (norm(Q+U*B)>1e-10) 
    error('Something wrong with the QR factorization.'); end

R = U';
t = B*s;
A = inv(B);
A = A ./A(3,3);


% sanity check 
if det(R) < 0 error('R is not a rotation matrix'); end
if A(3,3) < 0 error('Wrong sign of A(3,3)'); end
% this guarantee that the result *is* a factorization of the given P, up to a scale factor
W = A*[R,t];
if (rank([P(:), W(:)]) ~= 1 )
    error('Something wrong with the ART factorization.'); end




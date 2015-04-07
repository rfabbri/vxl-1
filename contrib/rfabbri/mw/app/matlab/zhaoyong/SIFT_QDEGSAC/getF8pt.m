function F = getF8pt( a, b )
% F = getF8pt( a, b )
% retrieve F from the first 8 points of a and b

[img1pts,T1] = normalize( a(1:8,:) );
[img2pts,T2] = normalize( b(1:8,:) );
x  = img1pts(:,1);
y  = img1pts(:,2);
xp = img2pts(:,1);
yp = img2pts(:,2);

A = [ xp.*x xp.*y xp yp.*x yp.*y yp x y ones(8,1)];
[U,D,V] = svd(A,0);
f = V(:,end);
F = reshape( f,[3 3])';

% enforce the singularity constraint
[U,D,V] = svd(F);
D(3,3) = 0;             % force to zero to satisfy Frobenius norm'
D = D / D(1,1);         % scale 
F = U * D * V';
e = norm( A * reshape(F',9,1 ))^2; %algebraic error


%denormalize
F = T2' * F * T1;

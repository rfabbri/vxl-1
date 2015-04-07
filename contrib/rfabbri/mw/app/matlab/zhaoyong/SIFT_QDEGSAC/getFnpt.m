function F = getFnpt( F0, img1pts, img2pts )
% F = getFnpt( F0, img1pts, img2pts )
% refines F0 to get a better F. If F0 = [] it is computed
% with the 8 point method

%%if F0 == []
if size(F0, 1) == 0
    F0 = getF7pt( img1pts, img2pts );
end

ep = null(F0'); % left null space of F
%ep = cross( F0(:,1), F0(:,3));
epx = [  0    -ep(3)  ep(2);
        ep(3)   0    -ep(1);
       -ep(2)  ep(1)   0    ]; 
M = epx * F0;
t = ep;
P = [M t]; 
x = img1pts;
x(:,3) = 1;
xp = img2pts;
xp(:,3) = 1;
options = optimset('Display','off');
[P err] = lsqnonlin( @sampsonErrf, P, [], [], options, x, xp );
M = P(:,1:3);
t = P(:,4);
tx = [  0   -t(3)  t(2);
       t(3)   0   -t(1);
      -t(2)  t(1)   0    ]; 
F = tx * M;
F = F / norm(F);



function err = sampsonErrf( P, x, xp )

% get F from P
M = P(:,1:3);
t = P(:,4);
tx = [  0   -t(3)  t(2);
       t(3)   0   -t(1);
      -t(2)  t(1)   0    ]; 
F = tx * M;
L = F * x';
Lp = F' * xp';
num = sum(xp' .* L).^2;
den = L(1,:).^2 + L(2,:).^2 + Lp(1,:).^2 + Lp(2,:).^2;
err = num ./ den;

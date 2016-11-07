function [P1, P2]=GetPmatrixFromFmatrix(F)
% Get two projection matrices, P1 and P2,
% from the fundamental matrix F.
%
% P1=[I 0] and P2 = [[e']_x F | e']
P1=[eye(3) zeros(3,1)];

% e', epipole, null vector of F^T
[ep1 ep2]=epipole(F);

ep2=ep2/ep2(3);

% P2=[[ed]_xF | ed];

% P2=[skewsym(ep2)*F ep2]
P2=[crossM(ep2)*F+ep2*[1;1;1]' ep2];

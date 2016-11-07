function Lhat = normalizeLine( L )
% Lhat = normalizeLine( L )
%   returns a line with normalized direction. This means that 
%   the dot product of a point with w=1 gives the distance.
n = sqrt( L(1,:).^2 + L(2,:).^2 );
Lhat = L ./ repmat( n, 3, 1 );         


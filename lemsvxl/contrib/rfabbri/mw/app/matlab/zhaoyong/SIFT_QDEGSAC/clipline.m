function seg = clipline( l, windowSize )
% seg = clipline( l, windowSize )
%  clips a line to a window

if l(2) > l(1)  % horizontal
    l0 = [-1 0 0];
    l1 = [1 0 -windowSize(1)];
else            % vertical
    l0 = [ 0 -1 0];  
    l1 = [ 0 1 -windowSize(2)];
end

seg(1,:) = p2r( cross(l,l0) );
seg(2,:) = p2r( cross(l,l1) );



function r = p2r( p )
% transform rows from P^2 to R^2 space

r = p ./ repmat( p(:,3), 1, 3 );
r(:,3) = [];

    
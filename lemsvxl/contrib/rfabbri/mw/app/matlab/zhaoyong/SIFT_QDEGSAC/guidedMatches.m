function matches = guidedMatches( f1, f2, F, offs, w, ct, dt )
% matches = guidedMatches( f1, f2, F, offs, w, ct, dt )
%   returns the matches to features in  f1 with f2
%   offs  -  offset of the search window 
%   w     -  width of the search window
%   ct    -  threshold for similarity measure
%   dt    -  threshold pixel distance from epipolar line

x = f1.pos( :, [2 1] );  x(:,3) = 1;
xp = f2.pos( :, [2 1] ); xp(:,3) = 1;
L1 = normalizeLine( F * x' );

matches = [];
compares = 0;
for i = 1:f1.count
    p1 = f1.pos(i,:);
    rmw1 = f1.rmw{i};    
    window = [ p1 + offs - w / 2; p1 + offs + w / 2 ];
    dist = abs( dot(repmat(L1(:,i),1,f2.count), xp')); 
	candidx = find(f2.pos(:,1) >= window(1,1) & ...
                   f2.pos(:,1) <= window(2,1) & ...
                   f2.pos(:,2) >= window(1,2) & ...
                   f2.pos(:,2) <= window(2,2) & ...
                   dist' < dt );
	candidates = f2.pos( candidx, :);
	m = candidx;
	for j = 1:size(candidx)
        idx = candidx(j);
        rmw2 = f2.rmw{idx};    
        
        % check the windows to make sure that they are the same        
        if (rmw1(1) == rmw2(1) & rmw1(2) == rmw2(2) & ...
            rmw1(3) == rmw2(3) & rmw1(4) == rmw2(4) )
            cc = dot( f1.npixels{i}(:), f2.npixels{idx}(:) );
            compares = compares + 1;
        else
            cc = -inf;
%             rmwt = [max(rmw1(1,:), rmw2(1,:));...
%                     min(rmw1(2,:), rmw2(2,:))];
%             mw1 = 
        end
        m( j, 1 ) = idx;
        m( j, 2 ) = cc;          
	end    
    if m 
        [v,idx] = max(m(:,2));
        if v > ct
            matches(i,:) = [i m(idx,1)];   
        else
            matches(i,:) = [-1 -1];
        end
    else
        matches(i,:) = [-1 -1];
    end
end	
compares;

matches = matches(find(matches(:,1)~=-1),:);


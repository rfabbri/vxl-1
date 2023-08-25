function [dist_sq] = distance_sq_between_curve_and_edge(curve, edge)

numSamples = size(curve,1);
dist_sq = 10^10;
edgeX = edge(1,1);
edgeY = edge(1,2);

for n=1:numSamples
    curSampleX = curve(n,1);
    curSampleY = curve(n,2);
    curDistSq = (edgeX - curSampleX)^2 + (edgeY - curSampleY)^2;
    if(curDistSq < dist_sq)
        dist_sq = curDistSq;
    end
end


clear all;
close all;

read_curve_sketch_new_linker;

curveID = 211;

curve = recs{1,curveID};
[L,dmy] = size(curve);
length = 0;

for l=2:L
    
    prevPT = curve(l-1,:);
    curPT = curve(l,:);
    
    curDist = sqrt((prevPT(1,1) - curPT(1,1))^2 + (prevPT(1,2) - curPT(1,2))^2 + (prevPT(1,3) - curPT(1,3))^2);
    length = length + curDist;
end

length
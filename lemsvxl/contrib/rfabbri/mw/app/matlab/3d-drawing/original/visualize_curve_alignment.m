clear all;
close all;

anchors = [10 13];

% curveID1 = 165;
% curveID2 = 41;

curveID1 = 195;
curveID2 = 52;


anchor1 = imread(['./images/',num2str(anchors(1,1),'%08d'),'.jpg']);
anchor2 = imread(['./images/',num2str(anchors(1,2),'%08d'),'.jpg']);

[y, x, z] = size(anchor1);

combined = uint8(zeros(y,2*x,z));
combined(:,1:x,:) = anchor1;
combined(:,(x+1):(2*x),:) = anchor2;

imshow(combined);
set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
hold on;

filename1 = ['./curves_broken/',num2str(anchors(1,1),'%08d'),'.cemv'];
cons1=read_cem_file(filename1);

filename2 = ['./curves_broken/',num2str(anchors(1,2),'%08d'),'.cemv'];
cons2=read_cem_file(filename2);

curve1 = cons1{curveID1+1};
curve2 = cons2{curveID2+1};

line(curve1(:,1)+1, curve1(:,2)+1,'color','b','LineWidth',2);
line(curve2(:,1)+x+1, curve2(:,2)+1,'color','b','LineWidth',2);

fid = fopen('alignment.txt');
numSamples = fscanf(fid,'%d',[1 1]);

for n=1:numSamples
    curSamples = fscanf(fid,'%d',[1 2]);
    firstPoint = curve1(curSamples(1,1)+1,:);
    secondPoint = curve2(curSamples(1,2)+1,:);
    secondPoint(1,1) = secondPoint(1,1) + x;
    
    xx = [firstPoint(1,1)+1 secondPoint(1,1)+1];
    yy = [firstPoint(1,2)+1 secondPoint(1,2)+1];
    
    if(mod(n,5)==1)
        line(xx,yy,'color','g');
    end
end

hold off;
print_pdf('alignment.pdf');
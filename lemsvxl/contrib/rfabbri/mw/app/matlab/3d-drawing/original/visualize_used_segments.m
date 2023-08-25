clear all;
close all;

numIM = 27;

for n=1:numIM

    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    figure;imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    
    cons = draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);
    
    fid = fopen(['./used_curve_flags/all/used_samples_',num2str(n-1),'.txt'],'r');
    
    numCurves = fscanf(fid,'%d',[1 1]);
    for i=1:numCurves
        curveID = fscanf(fid,'%d',[1 1])+1;
        numSamples = fscanf(fid,'%d',[1 1]);
        flags = fscanf(fid,'%d',[1 numSamples]);
        indices = find(flags==1);
        numIndices = size(indices,2);
        startPoint=1;
        for j=2:numIndices
            if(indices(1,j)-indices(1,j-1) > 1)
                startIndex = indices(1,startPoint);
                curIndex = indices(1,j-1);
                line(cons{curveID}(startIndex:curIndex,1)+1, cons{curveID}(startIndex:curIndex,2)+1,'color', [0 1 0],'LineWidth',2);
                startPoint = j;
            end
        end
        if(startPoint~=numIndices)
            startIndex = indices(1,startPoint);
            curIndex = indices(1,numIndices);
            line(cons{curveID}(startIndex:curIndex,1)+1, cons{curveID}(startIndex:curIndex,2)+1,'color', [0 1 0],'LineWidth',2);
        end        
    end
    
    numCons = size(cons,2);
    
    for c=1:numCons
        curCon = cons{c};
        numSamples = size(curCon,1);
        curLength=0;
        for s=2:numSamples
            curLength = curLength + norm([curCon(s,1)-curCon(s-1,1) curCon(s,2)-curCon(s-1,2)]);
        end
        if(curLength<10)
            line(cons{c}(:,1)+1, cons{c}(:,2)+1,'color', [0 0 1],'LineWidth',2);
        end
    end
    
    hold off;
    print_pdf([num2str(n-1,'%08d'),'.pdf']);
    close all;
end
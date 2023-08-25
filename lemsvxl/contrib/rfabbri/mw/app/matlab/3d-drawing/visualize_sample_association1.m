clear all;
close all;

read_curve_sketch4;

numIM = 27;
nR = size(recs,2);


%Replace the reserved colors that are used for image curves

fid=fopen('curve_links.txt');
origIDs = zeros(nR,1);
offsets = zeros(nR,1);

for r=1:nR
   linkData = fscanf(fid,'%d',[1 2]);
   origIDs(r,1) = linkData(1,1); 
   offsets(r,1) = linkData(1,2);
end

queryCurve = 293;
origID_index = find(origIDs==queryCurve);
numQueryRecon = size(origID_index,1);

colors = distinguishable_colors(numQueryRecon+1);

maxLength=0;
queryRecon=0;
for r=1:numQueryRecon
    curCurve = recs{1,origID_index(r,1)};
    if(size(curCurve,1)>maxLength)
        maxLength = size(curCurve,1);
        queryRecon = origID_index(r,1);
    end
end
%queryRecon=1371;
queryOffset=offsets(queryRecon,1);

for n=5:5
    
        fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
        curP = (fscanf(fid,'%f',[4 3]))';

        %[edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);

        curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
        figure;imshow(curIM);
        set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
        hold on;
        
        queryCurve = recs{1,queryRecon};        
        numSamples = size(queryCurve,1);
        reprojCurve = zeros(numSamples,2);

        for s=1:numSamples

            curSample = [(queryCurve(s,:))';1];
            imSample = curP*curSample;
            imSample = imSample./imSample(3,1);

            reprojCurve(s,1) = imSample(1,1);
            reprojCurve(s,2) = imSample(2,1);

        end

        %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);
        for r=1:numQueryRecon   
            if(origID_index(r,1)~=queryRecon)
                curCurve = recs{1,origID_index(r,1)};
                curOffset = offsets(origID_index(r,1),1);
                curNumSamples = size(curCurve,1);
                curDiff = queryOffset - curOffset;

                curReprojCurve = zeros(curNumSamples,2);

                for s=1:curNumSamples

                    sample = [(curCurve(s,:))';1];
                    cur_imSample = curP*sample;
                    cur_imSample = cur_imSample./cur_imSample(3,1);

                    curReprojCurve(s,1) = cur_imSample(1,1);
                    curReprojCurve(s,2) = cur_imSample(2,1);
                    
                    corrSample = s-curDiff;
                    if(corrSample>0 && corrSample<=numSamples)
                        xx = [curReprojCurve(s,1) reprojCurve(corrSample,1)];
                        yy = [curReprojCurve(s,2) reprojCurve(corrSample,2)];
                        plot(xx+1,yy+1,'Color',colors(r+1,:),'Linewidth',1);
                    end

                end

                plot(curReprojCurve(:,1)+1,curReprojCurve(:,2)+1,'Color',colors(r+1,:),'LineWidth',2);
            end
        end
        
        plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','b','LineWidth',2);
        hold off;
        print_pdf([num2str(n-1,'%08d'),'.pdf']);
        %close all;
        
end
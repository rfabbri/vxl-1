clear all;
close all;

addpath ~/lemsvxl/src/contrib/ntrinh/matlab/utils/

read_curve_sketch4;

markedCurves = [];
nrecs = max(size(recs));

for i=1:nrecs
  curRec = recs{i};
  curFlags = flags{i,1};
  
  uncertainIndices = find(curFlags==0);
  numUncInd = size(uncertainIndices,2);
  
  if(numUncInd > 1)
      markedCurves = [markedCurves i];
  end
end

numIM = 50;
nR = size(recs,2);

%otherwise
for n=1:numIM

    fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
    curP = (fscanf(fid,'%f',[4 3]))';
    
    [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
    
    curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
    figure;imshow(curIM);
    set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
    hold on;
    
    draw_cons(['./curves/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0);
    
    for r=1:nR
        
        %If this is a marked curve
        if(size(find(markedCurves==r),2) > 0)
            curCurve = recs{1,r};
            numSamples = size(curCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(curCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %Plot the reprojection
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'-b','LineWidth',1);
        
            
            %Process the flags to paint the uncertain segments red
            curRec = recs{r};
            curFlags = flags{r,1};

            uncertainIndices = find(curFlags==0);
            numUncInd = size(uncertainIndices,2);

            curRecPieces = cell(0,1);
            pieceIDX = 1;
            beginIDX = 1;

            for j=2:numUncInd
                if(uncertainIndices(1,j)-uncertainIndices(1,(j-1)) > 1)
                    curRecPieces{pieceIDX,1} = curRec(beginIDX:(j-1),:);
                    pieceIDX = pieceIDX+1;
                    beginIDX = j;
                elseif(j==numUncInd)
                    curRecPieces{pieceIDX,1} = curRec(beginIDX:j,:);
                end
            end

            numPieces = size(curRecPieces,1);

            for k=1:numPieces
                if(size(curRecPieces{k,1},1)>1)
                    curCurve = curRecPieces{k,1};
                    numSamples = size(curCurve,1);

                    reprojCurve = zeros(numSamples,2);

                    for s=1:numSamples

                        curSample = [(curCurve(s,:))';1];
                        imSample = curP*curSample;
                        imSample = imSample./imSample(3,1);

                        reprojCurve(s,1) = imSample(1,1);
                        reprojCurve(s,2) = imSample(2,1);

                    end
                    plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'-r','LineWidth',1);
                end

            end
        else
            curCurve = recs{1,r};
            numSamples = size(curCurve,1);

            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples

                curSample = [(curCurve(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);

            end

            %Plot the reprojection
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'-g','LineWidth',1);
        end

    end
    
    hold off;
    print_pdf([num2str(n-1,'%08d'),'.pdf']);
    close all;
end
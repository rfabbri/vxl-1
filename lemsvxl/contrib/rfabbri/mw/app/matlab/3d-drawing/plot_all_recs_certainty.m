%mainfig=610;
%figure(mainfig);
figure;
clf;
hold on;
axis equal;
xlabel x;
ylabel y;
zlabel z;
title ('Main reconstruction');

nrecs = max(size(recs))

handles = zeros(1,nrecs);
for i=1:nrecs
  handles(i) = cplot(recs{i},'-b');
  set(handles(i),'displayname',num2str(i))
end

axis tight;
grid on;

markedCurves = [];

for i=1:nrecs
  curRec = recs{i};
  curFlags = flags{i,1};
  
  uncertainIndices = find(curFlags==0);
  numUncInd = size(uncertainIndices,2);
  
  if(numUncInd > 0)
      markedCurves = [markedCurves i];
  end
  
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
          cplot(curRecPieces{k,1},'-r');
      end
  end
  
end

% save the plot
%unix('cp all_recs.fig all_recs_old.fig');
%hgsave('all_recs.fig');
%hold off;
function draw_contours(contours, thresh, rand, col, flip, shortcol, conLengths, lengthThreshold,ID)

% rand: if this is on, coloring will be done randomly, shorter curves
% cannot be colored differently so shortcol, conLengths, lengthThreshold
% will be ignored

% col: used if rand is turned off, the color of the curves longer than
% lengthThreshold

% shortcol: used if rand is turned off, the color of the curves shorter
% than lengthThreshold

% flip: flag for flipping coordinate system

% conLengths: a vector storing the lengths of each contour.

if (nargin<2), thresh = 0; end
if (nargin<3), rand=1; end
if (nargin<4), col = [0 0 0]; end
if (nargin<5), flip = 0; end
if (nargin<6), shortcol = [0 0 0]; end

con_cnt = length(contours)

if (nargin<7), conLengths = zeros(con_cnt,1); end
if (nargin<8), lengthThreshold = 0; end

colourmp = hsv(con_cnt);    % HSV colour map with con_cnt entries
colourmp = colourmp(randperm(con_cnt),:);  % Random permutation

%selected_cnt = [222 223];
%contours{224}

if(ID(1,1)==-1)
    for i = 1:con_cnt
    %for j=1:size(selected_cnt,2)
    %    i=selected_cnt(1,j);
        if (size(contours{i},1)<thresh)
            continue;
        end

        if (flip)
            if (rand==1)
                line(contours{i}(:,2), contours{i}(:,1),'color',colourmp(i,:), 'LineWidth', 1);
            else
                if(conLengths(i,1)<lengthThreshold)
                    line(contours{i}(:,2), contours{i}(:,1),'color', shortcol,'LineWidth',1);
                else
                    line(contours{i}(:,2), contours{i}(:,1),'color', col,'LineWidth',1);
                end

            end

        else
            if (rand==1)
                line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color',colourmp(i,:), 'LineWidth', 2);
            else
                if(conLengths(i,1)<lengthThreshold)
                    line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color', shortcol,'LineWidth',2);
                else
                    line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color', col,'LineWidth',2);
                end

            end    
        end

        hold on;
        %text(contours{i}(1,1)+1,contours{i}(1,2)+1,num2str(i-1),'Color',[0.9 0.8 0.85]);
        %hold off;
    end
else
    for j=1:size(ID,2)
        i = ID(1,j)+1;
        
        if (size(contours{i},1)<thresh)
            continue;
        end

        if (flip)
            if (rand==1)
                line(contours{i}(:,2), contours{i}(:,1),'color',colourmp(i,:), 'LineWidth', 1);
            else
                if(conLengths(i,1)<lengthThreshold)
                    line(contours{i}(:,2), contours{i}(:,1),'color', shortcol,'LineWidth',1);
                else
                    line(contours{i}(:,2), contours{i}(:,1),'color', col,'LineWidth',1);
                end

            end

        else
            if (rand==1)
                line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color',colourmp(i,:), 'LineWidth', 2);
            else
                if(conLengths(i,1)<lengthThreshold)
                    line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color', shortcol,'LineWidth',2);
                else
                    line(contours{i}(:,1)+1, contours{i}(:,2)+1,'color', col,'LineWidth',2);
                end

            end    
        end

        hold on;
        %text(contours{i}(1,1)+1,contours{i}(1,2)+1,num2str(i-1),'Color',[0.9 0.8 0.85]);
        %hold off;
    end
end

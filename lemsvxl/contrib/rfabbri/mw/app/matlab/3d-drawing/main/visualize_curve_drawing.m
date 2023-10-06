% These are optional visualizations
%
% %VISUALIZE
% 
% n=15;
% 
% curIM = imread(['./images/',num2str(n-1,'%08d'),'.jpg']);
% figure;imshow(curIM);
% set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
% hold on;
% %draw_cons(['./curves_yuliang/',num2str(n-1,'%08d'),'.cemv'],[num2str(n-1,'%08d'),'.jpg'],0,-1);
% 
% fid = fopen(['./calibration/',num2str(n-1,'%08d'),'.projmatrix']);
% curP = (fscanf(fid,'%f',[4 3]))';
% 
% view_colors = distinguishable_colors(numViews);
% 
% for vv=1:numViews
%     
%     vview = all_views(1,vv)+1;
%     num_curves = size(clusters{vview,1},2);
%     
%     for cc=1:num_curves
%         curveID = clusters{vview,1}(1,cc);
%         cur_curve = all_recs{vview,1}{1,curveID};
%         numSamples = size(cur_curve,1);
%         reprojCurve = zeros(numSamples,2);
% 
%         for s=1:numSamples
% 
%             curSample = [(cur_curve(s,:))';1];
%             imSample = curP*curSample;
%             imSample = imSample./imSample(3,1);
% 
%             reprojCurve(s,1) = imSample(1,1);
%             reprojCurve(s,2) = imSample(2,1);
%         end
% 
%         plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color',view_colors(vv,:),'LineWidth',1);
%     end
% 
% end

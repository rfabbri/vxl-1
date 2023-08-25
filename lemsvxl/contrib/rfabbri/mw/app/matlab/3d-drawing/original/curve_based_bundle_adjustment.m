clear all;
close all;

read_curve_sketch4a;
% plot_all_recs;
% hold on;
nR = size(recs,2);
numIM = 27;

view_colors = distinguishable_colors(9);
curColor = view_colors(9,:);

transformed_recs = cell(1,nR);
transformed_tangs = cell(1,nR);
querySetID = 13;
queryIDVec = [177 178 179];
% queryIDVec = [1 57 58 59 211];
%queryIDVec = [177 178 179 183 59];
%queryIDVec = [33 176 69 163 31 32 29];
%queryIDVec = [1 132 46 47 165 130 131 68];
%queryIDVec = [3 4 49 136 1 2 50];

cons = read_cons(['./curves_yuliang/',num2str(querySetID,'%08d'),'.cemv'],[num2str(querySetID,'%08d'),'.jpg'],0,-1);
num_im_contours = size(cons,2);
[dummy1, dummy2, edge_support_all] = read_association_attributes(querySetID,num_im_contours,nR,numIM);

colors = distinguishable_colors(numIM+3);
visView = 9;

curIM = imread(['./images/',num2str(visView-1,'%08d'),'.jpg']);
figure;imshow(curIM);
set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
hold on;

[edg edgmap] = load_edg(['./edges/',num2str(visView-1,'%08d'),'.edg']);
disp_edg(edg,0);

for qid=1:size(queryIDVec,2)

    queryID = queryIDVec(1,qid);
    
    cur_rec = recs{1,queryID}; 
    nS = size(cur_rec,1);

    motionVectors = cell(nS,1);
    perpMotionVectors = cell(nS,1);
    for s=1:nS
       motionVectors{s,1} = cell(numIM,1); 
       perpMotionVectors{s,1} = cell(numIM,1);
    end

    edge_support = edge_support_all{queryID,1};

    for n=visView:visView

        cur_rec = recs{1,queryID}; 
        nS = size(cur_rec,1);
        
        if(~isempty(edge_support{n,1}))
            n
            [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
            cur_edge_support = edge_support{n,1};

            [K R T] = read_calibration(['./calibration_split/',num2str(n-1,'%08d'),'.projmatrix']);
            curP = K*[R T];

            numSamples = size(cur_rec,1);
            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples
                curSample = [(cur_rec(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);
            end
    
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','r','LineWidth',2);

            cur_rec = [cur_rec ones(nS,1)]';

            cur_tang = tangs{1,queryID}';

            transformed_rec = ([R T]*cur_rec)';
            transformed_tang = (R*cur_tang)';

            for s=1:nS
                if(~isempty(cur_edge_support{s,1}))
                    gama_3d = transformed_rec(s,:)';
                    vv = gama_3d./norm(gama_3d);
                    %vv = [0;0;1];
                    gama_proj = gama_3d./gama_3d(3,1);
                    tang = transformed_tang(s,:)';
                    pixel = K*gama_proj;
                    pixel = pixel(1:2,1)./pixel(3,1);

                    closest_edge = [];
                    min_edge_distance = 1000;

                    for es=1:size(cur_edge_support{s,1},2)
                        cur_edgeID = cur_edge_support{s,1}(1,es);
                        cur_edge = edg(cur_edgeID+1,1:2)' - 1;
                        if(norm(cur_edge - pixel) < min_edge_distance)
                           min_edge_distance = norm(cur_edge - pixel);
                           closest_edge = cur_edge;
                        end

                    end

                    if(min_edge_distance > 1)
                        continue;
                    end
                    
                    %90-degree rotation both ways
                    r1 = [0 -1; 1 0];
                    r2 = [0 1; -1 0];
                    
                    tang_2d = tang(1:2,1)';
                    tang_2d = tang_2d./norm(tang_2d);
                    
                    n1 = tang_2d*r1;
                    n2 = tang_2d*r2;

                    %if(mod(s,5)==1)
                        plot(closest_edge(1,1)+1,closest_edge(2,1)+1,'yx');
                        plot([closest_edge(1,1)+1 reprojCurve(s,1)+1],[closest_edge(2,1)+1 reprojCurve(s,2)+1],'y-');
                    %end
                    edge_trans = K\([closest_edge;1]);
                    edge_trans_2d = edge_trans(1:2,1);
                    delta_edge = edge_trans_2d - gama_proj(1:2,1);
                    
                    nn = [];
                    if(dot(delta_edge,n1)>0)
                        nn=n1;
                    else
                        nn=n2;
                    end
                    
                    delta_edge_perp = dot(delta_edge,nn);
                    edge_trans_perp = gama_proj(1:2,1) + delta_edge_perp.*nn';
                    edge_trans_perp = [edge_trans_perp; 1];

                    rho_bar = dot(gama_3d,vv)/dot(edge_trans,vv);
                    gamaBar_cam = edge_trans*rho_bar;
                    gamaBar = R\(gamaBar_cam - T);
                    motionVectors{s,1}{n,1} = gamaBar - cur_rec(1:3,s);
                    
                    rho_bar = dot(gama_3d,vv)/dot(edge_trans_perp,vv);
                    gamaBar_cam = edge_trans_perp*rho_bar;
                    gamaBar_perp = R\(gamaBar_cam - T);
                    perpMotionVectors{s,1}{n,1} = gamaBar_perp - cur_rec(1:3,s);

                    %if(mod(s,5)==1)
%                       cur_pixel = curP*[gamaBar;1]; 
%                       cur_pixel = cur_pixel./cur_pixel(3,1);
%                       cur_vector = cur_pixel(1:2,1) - reprojCurve(s,:)';
%                       quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,cur_vector(1,1),cur_vector(2,1),'g');
                      
                      cur_pixel = curP*[gamaBar_perp;1]; 
                      cur_pixel = cur_pixel./cur_pixel(3,1);
                      cur_vector = cur_pixel(1:2,1) - reprojCurve(s,:)';
                      quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,cur_vector(1,1),cur_vector(2,1),'Color','g');
                      
%                       quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,tang_2d(1,1),tang_2d(1,2),'r');
                      
                    %end

                end
            end
        end

    end
        

% %     % plot_all_recs_with_tangents;
% %     % hold on;
% %     for s=1:5:nS
% %         for n=1:numIM
% %             if(~isempty(motionVectors{s,1}{n,1}))
% %                 quiver3(cur_rec(1,s),cur_rec(2,s),cur_rec(3,s),motionVectors{s,1}{n,1}(1,1),motionVectors{s,1}{n,1}(2,1),motionVectors{s,1}{n,1}(3,1),'Color',colors(n+3,:));
% %             end
% %         end
% %     end
% % 
%     averageMotionVec = zeros(nS,3);
%     for s=1:nS
%         totalMotion = zeros(3,1);
%         numCues = 0;
%         for n=1:numIM
%             if(~isempty(motionVectors{s,1}{n,1}))
%                 totalMotion = totalMotion + motionVectors{s,1}{n,1};
%                 numCues = numCues+1;
%             end
%         end
%         
%         if(numCues>0)
%             averageMotionVec(s,:) = (totalMotion./numCues)';
%         else
%             averageMotionVec(s,:) = zeros(3,1);
%         end
%     end
%     
%     perpAverageMotionVec = zeros(nS,3);
%     for s=1:nS
%         totalMotion = zeros(3,1);
%         numCues = 0;
%         for n=1:numIM
%             if(~isempty(perpMotionVectors{s,1}{n,1}))
%                 totalMotion = totalMotion + perpMotionVectors{s,1}{n,1};
%                 numCues = numCues+1;
%             end
%         end
%         
%         if(numCues>0)
%             perpAverageMotionVec(s,:) = (totalMotion./numCues)';
%         else
%             perpAverageMotionVec(s,:) = zeros(3,1);
%         end
%     end
% 
%     modifiedCurve = recs{1,queryID} + averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + perpAverageMotionVec;
% 
% %     cplot(modifiedCurve,'-g');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp.mat'];
%     save(filename,'perpModifiedCurve');
%     
%     clear modifiedCurve;
%     clear perpModifiedCurve;
%     
%     modifiedCurve = recs{1,queryID} + 2.*averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + 2.*perpAverageMotionVec;
%     
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-2x.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp-2x.mat'];
%     save(filename,'perpModifiedCurve');
%     
%     clear modifiedCurve;
%     clear perpModifiedCurve;
%     
%     modifiedCurve = recs{1,queryID} + 3.*averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + 3.*perpAverageMotionVec;
%     
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-3x.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp-3x.mat'];
%     save(filename,'perpModifiedCurve');
end

%-------------------------------SECOND CURVE------------------------------

read_curve_sketch4b;
% plot_all_recs;
% hold on;
nR = size(recs,2);
numIM = 27;

view_colors = distinguishable_colors(9);
curColor = view_colors(9,:);

transformed_recs = cell(1,nR);
transformed_tangs = cell(1,nR);
querySetID = 6;
queryIDVec = [33 176];
% queryIDVec = [1 57 58 59 211];
%queryIDVec = [177 178 179 183 59];
%queryIDVec = [33 176 69 163 31 32 29];
%queryIDVec = [1 132 46 47 165 130 131 68];
%queryIDVec = [3 4 49 136 1 2 50];

cons = read_cons(['./curves_yuliang/',num2str(querySetID,'%08d'),'.cemv'],[num2str(querySetID,'%08d'),'.jpg'],0,-1);
num_im_contours = size(cons,2);
[dummy1, dummy2, edge_support_all] = read_association_attributes(querySetID,num_im_contours,nR,numIM);

colors = distinguishable_colors(numIM+3);

for qid=1:size(queryIDVec,2)

    queryID = queryIDVec(1,qid);
    
    cur_rec = recs{1,queryID}; 
    nS = size(cur_rec,1);

    motionVectors = cell(nS,1);
    perpMotionVectors = cell(nS,1);
    for s=1:nS
       motionVectors{s,1} = cell(numIM,1); 
       perpMotionVectors{s,1} = cell(numIM,1);
    end

    edge_support = edge_support_all{queryID,1};
    
%     curIM = imread(['./images/',num2str(20,'%08d'),'.jpg']);
%     figure;imshow(curIM);
%     set(figure(1),'Units','Normalized','OuterPosition',[0 0 1 1]);
%     hold on;

%    [edg edgmap] = load_edg(['./edges/',num2str(visView-1,'%08d'),'.edg']);
%     disp_edg(edg,0);

    for n=visView:visView

        cur_rec = recs{1,queryID}; 
        nS = size(cur_rec,1);
        
        if(~isempty(edge_support{n,1}))
            n
            [edg edgmap] = load_edg(['./edges/',num2str(n-1,'%08d'),'.edg']);
            cur_edge_support = edge_support{n,1};

            [K R T] = read_calibration(['./calibration_split/',num2str(n-1,'%08d'),'.projmatrix']);
            curP = K*[R T];

            numSamples = size(cur_rec,1);
            reprojCurve = zeros(numSamples,2);

            for s=1:numSamples
                curSample = [(cur_rec(s,:))';1];
                imSample = curP*curSample;
                imSample = imSample./imSample(3,1);

                reprojCurve(s,1) = imSample(1,1);
                reprojCurve(s,2) = imSample(2,1);
            end
    
            plot(reprojCurve(:,1)+1,reprojCurve(:,2)+1,'Color','m','LineWidth',2);

            cur_rec = [cur_rec ones(nS,1)]';

            cur_tang = tangs{1,queryID}';

            transformed_rec = ([R T]*cur_rec)';
            transformed_tang = (R*cur_tang)';

            for s=1:nS
                if(~isempty(cur_edge_support{s,1}))
                    gama_3d = transformed_rec(s,:)';
                    vv = gama_3d./norm(gama_3d);
                    %vv = [0;0;1];
                    gama_proj = gama_3d./gama_3d(3,1);
                    tang = transformed_tang(s,:)';
                    pixel = K*gama_proj;
                    pixel = pixel(1:2,1)./pixel(3,1);

                    closest_edge = [];
                    min_edge_distance = 1000;

                    for es=1:size(cur_edge_support{s,1},2)
                        cur_edgeID = cur_edge_support{s,1}(1,es);
                        cur_edge = edg(cur_edgeID+1,1:2)' - 1;
                        if(norm(cur_edge - pixel) < min_edge_distance)
                           min_edge_distance = norm(cur_edge - pixel);
                           closest_edge = cur_edge;
                        end

                    end

                    if(min_edge_distance > 1)
                        continue;
                    end
                    
                    %90-degree rotation both ways
                    r1 = [0 -1; 1 0];
                    r2 = [0 1; -1 0];
                    
                    tang_2d = tang(1:2,1)';
                    tang_2d = tang_2d./norm(tang_2d);
                    
                    n1 = tang_2d*r1;
                    n2 = tang_2d*r2;

                    %if(mod(s,5)==1)
                        plot(closest_edge(1,1)+1,closest_edge(2,1)+1,'cx');
                        plot([closest_edge(1,1)+1 reprojCurve(s,1)+1],[closest_edge(2,1)+1 reprojCurve(s,2)+1],'c-');
                    %end
                    edge_trans = K\([closest_edge;1]);
                    edge_trans_2d = edge_trans(1:2,1);
                    delta_edge = edge_trans_2d - gama_proj(1:2,1);
                    
                    nn = [];
                    if(dot(delta_edge,n1)>0)
                        nn=n1;
                    else
                        nn=n2;
                    end
                    
                    delta_edge_perp = dot(delta_edge,nn);
                    edge_trans_perp = gama_proj(1:2,1) + delta_edge_perp.*nn';
                    edge_trans_perp = [edge_trans_perp; 1];

                    rho_bar = dot(gama_3d,vv)/dot(edge_trans,vv);
                    gamaBar_cam = edge_trans*rho_bar;
                    gamaBar = R\(gamaBar_cam - T);
                    motionVectors{s,1}{n,1} = gamaBar - cur_rec(1:3,s);
                    
                    rho_bar = dot(gama_3d,vv)/dot(edge_trans_perp,vv);
                    gamaBar_cam = edge_trans_perp*rho_bar;
                    gamaBar_perp = R\(gamaBar_cam - T);
                    perpMotionVectors{s,1}{n,1} = gamaBar_perp - cur_rec(1:3,s);

                    %if(mod(s,5)==1)
%                       cur_pixel = curP*[gamaBar;1]; 
%                       cur_pixel = cur_pixel./cur_pixel(3,1);
%                       cur_vector = cur_pixel(1:2,1) - reprojCurve(s,:)';
%                       quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,cur_vector(1,1),cur_vector(2,1),'g');
                      
                      cur_pixel = curP*[gamaBar_perp;1]; 
                      cur_pixel = cur_pixel./cur_pixel(3,1);
                      cur_vector = cur_pixel(1:2,1) - reprojCurve(s,:)';
                      quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,cur_vector(1,1),cur_vector(2,1),'Color','k');
                      
%                       quiver(reprojCurve(s,1)+1,reprojCurve(s,2)+1,tang_2d(1,1),tang_2d(1,2),'r');
                      
                    %end

                end
            end
        end

    end
        

% %     % plot_all_recs_with_tangents;
% %     % hold on;
% %     for s=1:5:nS
% %         for n=1:numIM
% %             if(~isempty(motionVectors{s,1}{n,1}))
% %                 quiver3(cur_rec(1,s),cur_rec(2,s),cur_rec(3,s),motionVectors{s,1}{n,1}(1,1),motionVectors{s,1}{n,1}(2,1),motionVectors{s,1}{n,1}(3,1),'Color',colors(n+3,:));
% %             end
% %         end
% %     end
% % 
%     averageMotionVec = zeros(nS,3);
%     for s=1:nS
%         totalMotion = zeros(3,1);
%         numCues = 0;
%         for n=1:numIM
%             if(~isempty(motionVectors{s,1}{n,1}))
%                 totalMotion = totalMotion + motionVectors{s,1}{n,1};
%                 numCues = numCues+1;
%             end
%         end
%         
%         if(numCues>0)
%             averageMotionVec(s,:) = (totalMotion./numCues)';
%         else
%             averageMotionVec(s,:) = zeros(3,1);
%         end
%     end
%     
%     perpAverageMotionVec = zeros(nS,3);
%     for s=1:nS
%         totalMotion = zeros(3,1);
%         numCues = 0;
%         for n=1:numIM
%             if(~isempty(perpMotionVectors{s,1}{n,1}))
%                 totalMotion = totalMotion + perpMotionVectors{s,1}{n,1};
%                 numCues = numCues+1;
%             end
%         end
%         
%         if(numCues>0)
%             perpAverageMotionVec(s,:) = (totalMotion./numCues)';
%         else
%             perpAverageMotionVec(s,:) = zeros(3,1);
%         end
%     end
% 
%     modifiedCurve = recs{1,queryID} + averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + perpAverageMotionVec;
% 
% %     cplot(modifiedCurve,'-g');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp.mat'];
%     save(filename,'perpModifiedCurve');
%     
%     clear modifiedCurve;
%     clear perpModifiedCurve;
%     
%     modifiedCurve = recs{1,queryID} + 2.*averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + 2.*perpAverageMotionVec;
%     
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-2x.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp-2x.mat'];
%     save(filename,'perpModifiedCurve');
%     
%     clear modifiedCurve;
%     clear perpModifiedCurve;
%     
%     modifiedCurve = recs{1,queryID} + 3.*averageMotionVec;
%     perpModifiedCurve = recs{1,queryID} + 3.*perpAverageMotionVec;
%     
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-3x.mat'];
%     save(filename,'modifiedCurve');
%     filename = ['modifiedCurve-',num2str(querySetID),'-' num2str(queryID) 'im-plane-perp-3x.mat'];
%     save(filename,'perpModifiedCurve');
end

hold off;
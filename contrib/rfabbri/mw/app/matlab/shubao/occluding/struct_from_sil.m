function [pts3drawcell, normalrawcell, confrawcell] = struct_from_sil(data,yfactor,vfactor,datasetname)

DEBUG = 1;
neighbors_rel = -1:1;

pts3drawcell = cell(data.imnum,1);
normalrawcell = cell(data.imnum,1);
confrawcell = cell(data.imnum,1);
fprintf(1,'\n');
fprintf(1,'Matching and Reconstructing ...\n');
for i = 1:data.imnum
    fprintf(1,'[%d]',i); 
    neighbors = mod(i+neighbors_rel-1,data.imnum)+1;
    indmiddle = i; indleft = neighbors(1); indright = neighbors(3);
    
    %% match forward
    [T1, T2, Pn1, Pn2] = rectify(data.imMcell{indmiddle}, data.imMcell{indright});
    T1form = maketform('projective', T1');
    T2form = maketform('projective', T2');
    rectmap_middle = sparseimtrans(data.edgemap{indmiddle}, T1form);
    rectmap_right = sparseimtrans(data.edgemap{indright}, T2form);
    matchmap_forward = edgemapmatch(rectmap_middle, rectmap_right,...
                                    data.edgegradmap{indmiddle}, data.edgegradmap{indright},...
                                    data.edgeormap{indmiddle}, data.edgeormap{indright},...
                                    yfactor);

    %% match backword
    [T1, T2, Pn1, Pn2] = rectify(data.imMcell{indmiddle}, data.imMcell{indleft});
    T1form = maketform('projective', T1');
    T2form = maketform('projective', T2');
    rectmap_middle = sparseimtrans(data.edgemap{indmiddle}, T1form);
    rectmap_left = sparseimtrans(data.edgemap{indleft}, T2form);
    matchmap_backward = edgemapmatch(rectmap_middle, rectmap_left,...
                                     data.edgegradmap{indmiddle}, data.edgegradmap{indleft},...
                                     data.edgeormap{indmiddle}, data.edgeormap{indleft},...
                                     yfactor);

    % combine two matching results
    matchmap_forward = matchmap_forward.*(matchmap_backward~=0);
    matchmap_backward = matchmap_backward.*(matchmap_forward~=0);
    numedgepts = length(find(matchmap_forward~=0));
    [row,col] = find(matchmap_forward);

    if DEBUG
        [row1, col1, val1] = find(matchmap_forward);
        [row2, col2, val2] = find(matchmap_backward);
        xmiddle = col1; ymiddle = row1;
        xright = real(val1); yright = imag(val1);
        xleft = real(val2); yleft = imag(val2);
%         for indtmp = 1:numedgepts
%             plot([xleft(indtmp) xmiddle(indtmp) xright(indtmp)],...
%                 [yleft(indtmp) ymiddle(indtmp) yright(indtmp)], ...
%                 '-mo','MarkerSize',2); hold on;
%         end
%         plot(xleft,yleft,'.g','LineWidth',1); hold on;
%         plot(xmiddle,ymiddle,'.r','LineWidth',1); hold on;
%         plot(xright,yright,'.b','LineWidth',1); hold off;
%         axis ij; axis equal;
%         print('-dpng', ['../debug/' datasetname '/matchresult_' num2str(i) '.png']); 
        [rowtmp, coltmp, valtmp] = find(data.edgemap{indleft});
        plot(coltmp,rowtmp,'.g', 'LineWidth', 1); hold on;
        [rowtmp, coltmp, valtmp] = find(data.edgemap{indmiddle});
        plot(coltmp,rowtmp,'.b', 'LineWidth', 1); hold on;

        for indtmp = 1:3:numedgepts
            plot([xleft(indtmp) xmiddle(indtmp)],...
                [yleft(indtmp)  ymiddle(indtmp)], ...
                '-ro','MarkerSize',2); hold on;
        end
%        plot(xleft,yleft,'.g','LineWidth',1); hold on;
%        plot(xmiddle,ymiddle,'.r','LineWidth',1); hold on;
%        plot(xright,yright,'.b','LineWidth',1); hold off;
        hold off;
        axis ij; axis equal;
        print('-dpng', ['../debug/' datasetname '/matchresult_' num2str(i) '.png']); 
    end %!DEBUG

    %% interpolation and reconstruction
    pts3drawcell{i} = zeros(3,numedgepts,vfactor);
    normalrawcell{i} = zeros(3,numedgepts,vfactor);
    confrawcell{i} = zeros(4,numedgepts,vfactor);
    if vfactor == 1
        vsamples = 0;
    else
        vsamples = linspace(0,1,vfactor);
    end

    for j = 1:numedgepts
        ymiddle = row(j); xmiddle = col(j); thetamiddle = data.edgeormap{i}(ymiddle,xmiddle);
        pTmiddle = [-sin(thetamiddle); cos(thetamiddle); -cos(thetamiddle)*ymiddle+sin(thetamiddle)*xmiddle];
        pNmiddle = [-cos(thetamiddle); -sin(thetamiddle); sin(thetamiddle)*ymiddle+cos(thetamiddle)*xmiddle];
        PTmiddle = data.imMcell{i}'*pTmiddle; PTmiddle = PTmiddle/norm(PTmiddle(1:3),2); 
%             if PTMiddle(4)~=0 
%                 PTmiddle = sign(PTmiddle(4))*PTmiddle;
%             end
        PNmiddle = data.imMcell{i}'*pNmiddle; PNmiddle = PNmiddle/norm(PNmiddle(1:3),2); 
%             if PNmiddle(4)~=0
%                 PNmiddle = sign(PTmiddle(4))*PNmiddle;
%             end
        yright = imag(matchmap_forward(ymiddle,xmiddle)); xright = real(matchmap_forward(ymiddle,xmiddle)); thetaright = data.edgeormap{indright}(yright,xright);
        pTright = [-sin(thetaright); cos(thetaright); -cos(thetaright)*yright+sin(thetaright)*xright];
        pNright = [-cos(thetaright); -sin(thetaright); sin(thetaright)*yright+cos(thetaright)*xright];
        PTright = data.imMcell{indright}'*pTright; PTright = PTright/norm(PTright(1:3),2); %PTright = sign(PTright(4))*PTright;
        PNright = data.imMcell{indright}'*pNright; PNright = PNright/norm(PNright(1:3),2); %PNright = sign(PNright(4))*PNright;
        
        yleft = imag(matchmap_backward(ymiddle,xmiddle)); xleft = real(matchmap_backward(ymiddle,xmiddle)); thetaleft = data.edgeormap{indleft}(yleft,xleft);
        pTleft = [-sin(thetaleft); cos(thetaleft); -cos(thetaleft)*yleft+sin(thetaleft)*xleft];
        pNleft = [-cos(thetaleft); -sin(thetaleft); sin(thetaleft)*yleft+cos(thetaleft)*xleft];
        PTleft = data.imMcell{indleft}'*pTleft; PTleft = PTleft/norm(PTleft(1:3),2); %PTleft = sign(PTleft(4))*PTleft;
        PNleft = data.imMcell{indleft}'*pNleft; PNleft = PNleft/norm(PNleft(1:3),2); %PNleft = sign(PNleft(4))*PNleft;
        
        % normalize
        PTleft = PTleft/norm(PTleft(1:3),2);
        PNleft = PNleft/norm(PNleft(1:3),2);
        PTmiddle = PTmiddle/norm(PTmiddle(1:3),2);
        PNmiddle = PNmiddle/norm(PNmiddle(1:3),2);
        PTright = PTright/norm(PTright(1:3),2);
        PNright = PNright/norm(PNright(1:3),2);
        
        % then fit a spline
            PTsp(1) = csapi([-1 0 1], [PTleft(1) PTmiddle(1) PTright(1)]);
            PTsp(2) = csapi([-1 0 1], [PTleft(2) PTmiddle(2) PTright(2)]);
            PTsp(3) = csapi([-1 0 1], [PTleft(3) PTmiddle(3) PTright(3)]);
            PTsp(4) = csapi([-1 0 1], [PTleft(4) PTmiddle(4) PTright(4)]);
           
            PNsp(1) = csapi([-1 0 1], [PNleft(1) PNmiddle(1) PNright(1)]);
            PNsp(2) = csapi([-1 0 1], [PNleft(2) PNmiddle(2) PNright(2)]);
            PNsp(3) = csapi([-1 0 1], [PNleft(3) PNmiddle(3) PNright(3)]);
            PNsp(4) = csapi([-1 0 1], [PNleft(4) PNmiddle(4) PNright(4)]);
            
        % then get the derivative
            dPTdvsp(1) = fnder(PTsp(1));
            dPTdvsp(2) = fnder(PTsp(2));
            dPTdvsp(3) = fnder(PTsp(3));
            dPTdvsp(4) = fnder(PTsp(4));
            
            dPNdvsp(1) = fnder(PNsp(1));
            dPNdvsp(2) = fnder(PNsp(2));
            dPNdvsp(3) = fnder(PNsp(3));
            dPNdvsp(4) = fnder(PNsp(4));
            
            PT(1,:) = fnval(PTsp(1),vsamples);
            PT(2,:) = fnval(PTsp(2),vsamples);
            PT(3,:) = fnval(PTsp(3),vsamples);
            PT(4,:) = fnval(PTsp(4),vsamples);

            PN(1,:) = fnval(PNsp(1),vsamples);
            PN(2,:) = fnval(PNsp(2),vsamples);
            PN(3,:) = fnval(PNsp(3),vsamples);
            PN(4,:) = fnval(PNsp(4),vsamples);

            dPTdv(1,:) = fnval(dPTdvsp(1),vsamples);
            dPTdv(2,:) = fnval(dPTdvsp(2),vsamples);
            dPTdv(3,:) = fnval(dPTdvsp(3),vsamples);
            dPTdv(4,:) = fnval(dPTdvsp(4),vsamples);
            
            dPNdv(1,:) = fnval(dPNdvsp(1),vsamples);
            dPNdv(2,:) = fnval(dPNdvsp(2),vsamples);
            dPNdv(3,:) = fnval(dPNdvsp(3),vsamples);
            dPNdv(4,:) = fnval(dPNdvsp(4),vsamples);
            
        Clr = nullvector(PTleft, PNleft, PTright, PNright); % cross point
        Clr = Clr/Clr(4);
        for v = 1:vfactor
            [pts3drawcell{i}(:,j,v), normal_pseudo, confrawcell{i}(:,j,v)] = recover3d(PT(:,v), PN(:,v), dPTdv(:,v), dPNdv(:,v));
            if abs(Clr'*PTmiddle)<5e-3
                normalrawcell{i}(:,j,v) = [0 0 0]';
                pts3drawcell{i}(:,j,v) = Clr(1:3);
            else
                normalrawcell{i}(:,j,v) = sign(Clr'*PTmiddle)*normal_pseudo; %% add the threshold checking in the future
            end
        end
    end
end

% pts3draw = cell2matrix(pts3drawcell);
% confraw = cell2matrix(confrawcell);

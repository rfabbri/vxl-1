function maskcell = detectoutlier(pts3drawcell, confrawcell, data)
% detect the outliers by projecting the recoStructed poiVs back to the original silhouettes.
% pts3drawcell: recoStructed 3d poiVs
% data: the silhouette data
%
% @author: Shubao Liu (shubao.liu@gmail.com)
% @date: May, 2006

fprintf(1, '\nRemoving the outliers ...\n');

confraw = cell2matrix(confrawcell);
icnd = confraw(4,:)./confraw(3,:);
icndlog = -log(icnd);
icndmean = mean(icndlog);
icndstd = std(icndlog);

imnum = data.imnum;
silnum = data.silnum;
Mcell = data.silMcell;
maskcell = cell(silnum,1);
for j = 1:imnum-2
    [nouse, S, V] = size(pts3drawcell{j});
    maskcell{j} = ones(S, V);
end
for indsil = 1:silnum
    fprintf(1,'[%d]',indsil);
    sil = imread(data.silnamecell{indsil});
    SE = strel('square',7);
    sil = imdilate(sil,SE);
    for indim = 1:imnum-2
        [nouse, S, V] = size(pts3drawcell{indim});
        for v = 1:V
            for s = 1:S
                xhomo = Mcell{indsil}*[pts3drawcell{indim}(:,s,v);1];
                x = xhomo(1:2)/xhomo(3);
                if (confrawcell{indim}(3,s,v) == 0)
                    maskcell{indim}(s,v) = 0;
                elseif (-log(confrawcell{indim}(4,s,v)/confrawcell{indim}(3,s,v))<icndmean-2*icndstd)
                    maskcell{indim}(s,v) = 0;
                elseif ((round(x(2)) > size(sil,1))||...
                    (round(x(2)) <= 0)||...
                    (round(x(1)) > size(sil,2))||...
                    (round(x(1)) <= 0))
                    maskcell{indim}(s,v) = 0;
                elseif (sil(round(x(2)),round(x(1))) == 0)
                    maskcell{indim}(s,v) = 0;
                end
            end
        end
    end
end
fprintf(1,'\n');


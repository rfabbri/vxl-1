function extract_random_fragments(imfile, gtfile, Nfrags, minArea, maxArea, outfolder)
        if exist(outfolder)~=7
                mkdir(outfolder);
        end 
        GT = imread(gtfile);
        GT = double(GT > max(GT(:))/2);
        props1 = regionprops(GT, 'BoundingBox');
        bb = round(props1.BoundingBox);
        GT = imcrop(GT, bb);
        [M, N] = size(GT);
        I = imcrop(imread(imfile), bb);
        count = Nfrags;
        while 1
                if count == 0
                        break
                end
                xs = sort(2*unidrnd(ceil(N/2), 1, 2)-1);
                width = xs(2)-xs(1);
                while width == 0
                        xs = sort(2*unidrnd(ceil(N/2), 1, 2)-1);
                        width = xs(2)-xs(1);
                end
                ys = sort(2*unidrnd(ceil(M/2), 1, 2)-1);
                height = ys(2)-ys(1);
                while height == 0
                        ys = sort(2*unidrnd(ceil(M/2), 1, 2)-1);
                        height = ys(2)-ys(1);
                end
                asp = width / height;
                totalArea = width * height;
                cropBB = [xs(1) ys(1) width height];
                gt_frag = imcrop(GT, cropBB);
                figArea = sum(gt_frag(:));
                ratArea = figArea/totalArea;
                if asp < 0.7 || asp > 10/7 || ratArea < 0.15 || ratArea > 0.85 || totalArea < minArea || totalArea > maxArea
                        continue
                end            
                im_frag = imcrop(I, cropBB);
                normalized_center = [mean(xs)/bb(3), mean(ys)/bb(4)];
                count = count - 1;  
                objectname = get_obj_name(imfile);
                save([outfolder '/' objectname '_'  num2str(count) '.mat'], 'im_frag', 'gt_frag', 'objectname', 'normalized_center');                           
        end            
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end




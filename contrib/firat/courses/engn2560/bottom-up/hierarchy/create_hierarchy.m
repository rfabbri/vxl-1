function create_hierarchy(imagefile, bu_folder, nbins, outfile, saliency_measure, npad)
        I = double(imread(imagefile));
        objname = get_obj_name(imagefile);
        bu_files = dir([bu_folder '/' objname '/*.txt']);
        
        bu = zeros(size(I,1), size(I,2), length(bu_files));
        for i = 1:length(bu_files)
                if i < 10
                        bu_file = [bu_folder '/' objname '/' objname '_0000' num2str(i) '.txt'];
                else
                        bu_file = [bu_folder '/' objname '/' objname '_000' num2str(i) '.txt'];
                end
              bu(:,:,i) = load(bu_file);                
        end
        bu(:,:,end+1) = ones(size(I,1),size(I,2));
        bu_assign = unique(reshape(bu, size(I,1) * size(I,2), length(bu_files)+1), 'rows');
        fprintf('Create tree...\n');
        segtree = create_tree(bu_assign, bu, I, nbins, 1, 1, saliency_measure, npad);
        fprintf('Prune tree...\n');
        segtree = prune_tree(segtree);
        fprintf('Find the most salient ancestors...\n');
        segtree = find_most_salient_ancestors(segtree, 1, [], 0, []);
        salient_regions_table = create_salient_regions_table(segtree, segtree, segtree.leaves(end));
        fprintf('Finalizing...\n');
        segtree.bu = bu(:,:,1);
        fprintf('Saving...\n');
        save(outfile, 'segtree', 'salient_regions_table');
        fprintf('Done!\n');
end

function h = saliency_entropy(R, nbins)
        hist1 = hist([0 R 255], nbins);
        hist1([1 end]) = hist1([1 end]) - 1;
        hist1(hist1 == 0) = [];
        p = hist1 / length(R);
        h = 1 + (sum(p.*log2(p))/log2(nbins));
end

function h = saliency_histogram(R, B, nbins)
        hist1 = hist([0 R 255], nbins);
        hist1([1 end]) = hist1([1 end]) - 1;
        p1 = hist1 / length(R);
        hist2 = hist([0 B 255], nbins);
        hist2([1 end]) = hist2([1 end]) - 1;
        p2 = hist2 / length(B);
        h = sum((p1 - p2).^2)/2;
end

function salient_regions_table = create_salient_regions_table(osegtree, segtree, numLeaves)
        salient_regions_table = zeros(numLeaves, numLeaves);
        if iscell(segtree.children)
                for i = 1:length(segtree.children)
                        salient_regions_table = salient_regions_table + create_salient_regions_table(osegtree, segtree.children{i}, numLeaves);
                end
        else
                temptree = osegtree;
                for i = segtree.bestAncestor(2:end)
                        temptree = temptree.children{i};
                end
                for i = temptree.leaves
                      salient_regions_table(segtree.leaves, i) = 1;  
                end
        end
end

function segtree = create_tree(bu_assign, bu, I, nbins, level, id, saliency_measure, npad)
      %fprintf('Processing level=%d id=%d\n', level, id);
      [numLeaves, numLevels] = size(bu_assign);
      parent = bu_assign(:, numLevels - level + 1);
      rows1 = find(parent == id);
      segtree = struct;
      if numLevels ~= level
        children = bu_assign(:, numLevels - level);
        childids = unique(children(rows1));
        segtree.children = cell(length(childids), 1);
        for i = 1:length(childids)
                segtree.children{i} = create_tree(bu_assign, bu, I, nbins, level+1, childids(i), saliency_measure, npad);
        end               
      else
        segtree.children = -1;               
      end
      leaf = bu_assign(:, 1);
      segtree.leaves = unique(leaf(rows1))';
      
      switch saliency_measure
                case 'entropy' 
                        R = I(bu(:,:, numLevels - level + 1) == id);                       
                        segtree.saliency = saliency_entropy(R', nbins);
                case 'histogram'
                        if level == 1
                                segtree.saliency = 0;
                        else
                                [Iy, Ix] = size(I);
                                R = double(bu(:,:, numLevels - level + 1) == id);
                                props1 = regionprops(R, 'BoundingBox');  
                                bb = round(props1.BoundingBox);
                                bb(1:2) = bb(1:2) - npad;
                                bb(3:4) = bb(3:4) + 2*npad;
                                bb0 = bb(1:2);
                                bb0(bb0 < 1) = 1;
                                bb1 = bb(1:2) + bb(3:4);
                                if bb1(1) > Ix
                                        bb1(1) = Ix;
                                end
                                if(bb1(2) > Iy)
                                        bb1(2) = Iy;
                                end
                                BBmask = zeros(Iy, Ix);
                                BBmask(bb0(2):bb1(2), bb0(1):bb1(1)) = 1;
                                backmask = BBmask - R;
                                segtree.saliency = saliency_histogram(I(logical(R))', I(logical(backmask))', nbins);    
                        end
                case 'mhistogram'
                        if level == 1
                                segtree.saliency = 0;
                        else
                                [Iy, Ix] = size(I);
                                R = double(bu(:,:, numLevels - level + 1) == id);
                                RP = imdilate(R, strel('disk', npad)) - R;
                                segtree.saliency = saliency_histogram(I(logical(R))', I(logical(RP))', nbins);    
                        end
                case 'ehistogram'
                        if level == 1
                                segtree.saliency = 0;
                        else
                                [Iy, Ix] = size(I);
                                R = double(bu(:,:, numLevels - level + 1) == id);
                                props1 = regionprops(R, 'BoundingBox');  
                                bb = round(props1.BoundingBox);
                                bb(1:2) = bb(1:2) - npad;
                                bb(3:4) = bb(3:4) + 2*npad;
                                bb0 = bb(1:2);
                                bb0(bb0 < 1) = 1;
                                bb1 = bb(1:2) + bb(3:4);
                                if bb1(1) > Ix
                                        bb1(1) = Ix;
                                end
                                if(bb1(2) > Iy)
                                        bb1(2) = Iy;
                                end
                                BBmask = zeros(Iy, Ix);
                                BBmask(bb0(2):bb1(2), bb0(1):bb1(1)) = 1;
                                backmask = BBmask - R;
                                segtree.saliency = saliency_histogram(I(logical(R))', I(logical(backmask))', nbins)*saliency_entropy(I(logical(R))', nbins);    
                        end
                                
                otherwise
                        error('Unknown saliency measure')
      end             
end

function segtree = prune_tree(segtree)
       if iscell(segtree.children)
                if length(segtree.children) == 1
                        segtree = prune_tree(segtree.children{1});                        
                else
                        for i = 1:length(segtree.children)
                                segtree.children{i} = prune_tree(segtree.children{i});
                        end
                end
       end
end

function segtree = find_most_salient_ancestors(segtree, id, parent_index, best_saliency, salient_ancestor_index)
        new_parent_index = [parent_index, id];
        if segtree.saliency > best_saliency
                segtree.bestAncestor = new_parent_index; 
                new_best_saliency = segtree.saliency;               
        else
                segtree.bestAncestor = salient_ancestor_index;
                new_best_saliency = best_saliency;
        end
        if iscell(segtree.children)
                for i = 1:length(segtree.children)
                        segtree.children{i} = find_most_salient_ancestors(segtree.children{i}, i, new_parent_index, new_best_saliency, segtree.bestAncestor);
                end
        end
end

function objname = get_obj_name(imfile)
        name1 = dir(imfile);
        name1 = name1.name;
        dotpos = strfind(name1, '.');
        fext = dotpos(end)-1;
        objname = name1(1:fext);        
end


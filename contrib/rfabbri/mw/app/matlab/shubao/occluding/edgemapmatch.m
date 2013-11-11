function matchmap = edgemapmatch(rectmap1, rectmap2, gradmap1, gradmap2, ormap1, ormap2, yfactor)

[rownum, colnum] = size(rectmap1);
matchmap = sparse(rownum, colnum);
[row1, col1, val1] = find(rectmap1);
[row2, col2, val2] = find(rectmap2);

xmap1 = real(val1); ymap1 = imag(val1);
xmap2 = real(val2); ymap2 = imag(val2);

ymin = min(min(ymap1), min(ymap2));
ymax = max(max(ymap1), max(ymap2));

for y = ymin:yfactor:ymax
    ind1 = find(ymap1==y);
    ind2 = find(ymap2==y);
    if length(ind1)<=1 || length(ind2)<=1
        continue;
    end
    x1 = xmap1(ind1);   [x1sorted,IX1] = sort(x1);
    x2 = xmap2(ind2);   [x2sorted,IX2] = sort(x2);

    num1 = length(ind1); num2 = length(ind2);
    grad1 = zeros(num1,1); or1 = zeros(num1,1);
    for i = 1:num1
        grad1(i) = gradmap1(row1(ind1(IX1(i))), col1(ind1(IX1(i))));
        or1(i) = ormap1(row1(ind1(IX1(i))), col1(ind1(IX1(i))));
    end
    grad2 = zeros(num2,1); or2 = zeros(num2,1);
    for i = 1:num2
        grad2(i) = gradmap2(row2(ind2(IX2(i))), col2(ind2(IX2(i))));
        or2(i) = ormap2(row2(ind2(IX2(i))), col2(ind2(IX2(i))));
    end

    posmin1 = x1sorted(1); posmax1 = x1sorted(end);
    posmin2 = x2sorted(1); posmax2 = x2sorted(end);


    vec1 = []; vec2 = [];
    for i = 1:num1
        if posmin1 == posmax1
            vec1(i).relpos = 1;
        else
            vec1(i).relpos = (x1sorted(i)-posmin1)/(posmax1-posmin1);
        end
        vec1(i).relgrad = grad1(i)/255;
        vec1(i).or = or1(i);
    end
    for i = 1:num2 
        if posmin2 == posmax2
            vec2(i).relpos = 1;
        else
            vec2(i).relpos = (x2sorted(i)-posmin2)/(posmax2-posmin2);
        end
        vec2(i).relgrad = grad2(i)/255;
        vec2(i).or = or2(i);
    end
    
    [align1 align2] = NWalign(vec1, vec2);
    [alignc1, alignc2] = compact(align1, align2);
    
    num = length(alignc1);
    for i = 1:num
        matchmap(row1(ind1(IX1(alignc1(i)))),col1(ind1(IX1(alignc1(i))))) = ...
            complex(col2(ind2(IX2(alignc2(i)))),row2(ind2(IX2(alignc2(i)))));
    end
end

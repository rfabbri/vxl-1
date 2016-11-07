function rectmap = sparseimtrans(im, T)

[rownum, colnum] = size(im);
rectmap = sparse(rownum, colnum);
[row, col, val] = find(im);
for i = 1:length(row)
    [x, y] = tformfwd(T, col(i), row(i));
    rectmap(row(i),col(i)) = complex(round(x),round(y));
end
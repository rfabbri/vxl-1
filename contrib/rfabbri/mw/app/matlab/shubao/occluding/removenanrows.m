function B = removenanrows(A)

[nr0,nc] = size(A);
mask_rows = ones(nr0,1);
ind = 1;
for row = 1:nr0
    if any(isnan(A(row,:)))
        continue;
    else
        B(ind,:) = A(row,:);
        ind = ind+1;
    end
end


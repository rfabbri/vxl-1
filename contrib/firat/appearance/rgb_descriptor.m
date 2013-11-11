function desc = rgb_descriptor(im, nrows, ncols)
        if ~isnumeric(im)
            im = imread(im);
        end
        desc = zeros(1, 3*ncols*nrows);
        
            [Rs, Cs, Bs] = size(im);
            cell_heights = mypartition(Rs, nrows);
            cell_widths = mypartition(Cs, ncols);
            height_indices = [0 cumsum(cell_heights)];
            width_indices = [0 cumsum(cell_widths)];
            R = im(:,:,1);G = im(:,:,2);B = im(:,:,3);
            for i = 1:nrows
                for j = 1:ncols
                    Rdata = R(height_indices(i)+1:height_indices(i+1), width_indices(j)+1:width_indices(j+1));
                    desc(3*ncols*(i-1)+3*j - 2) = mean(Rdata(:));
                    Gdata = G(height_indices(i)+1:height_indices(i+1), width_indices(j)+1:width_indices(j+1));
                    desc(3*ncols*(i-1)+3*j - 1) = mean(Gdata(:));
                    Bdata = B(height_indices(i)+1:height_indices(i+1), width_indices(j)+1:width_indices(j+1));
                    desc(3*ncols*(i-1)+3*j) = mean(Bdata(:));                     
                end
            end
        
end

function Y = mypartition(X, a)
    Y = ones(1, a) * floor(X / a);
    rem1 = mod(X, a);
    Y(1:rem1) = Y(1:rem1) + 1;
end

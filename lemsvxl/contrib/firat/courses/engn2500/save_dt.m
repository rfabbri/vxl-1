% This is /lemsvxl/contrib/firat/courses/engn2500/save_dt.m.

% \file
% \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
% \date Apr 26, 2011

function save_dt(DT, out_file)
    [x_dim, y_dim, z_dim] = size(DT);
    fid = fopen(out_file, 'wb');
    fprintf(fid, 'float\n%d %d %d\n0 0 0 0 0 0\n0 0 0\n', z_dim, y_dim, x_dim);
    fwrite(fid, DT, 'single');
    fclose(fid);
end


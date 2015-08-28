function [mapping,query_points]=read_dc_file(filename)


fid=fopen(filename);

m_vs_q=fread(fid,2,'double=>double');

models=m_vs_q(1);
querys=m_vs_q(2);


mapping=cell(models,querys);
query_points=cell(querys,1);


for s=1:querys
    numb_orig_points=fread(fid,1,'double=>double');
    orig_points=fread(fid,numb_orig_points*2,'double=>double');
    coords=[orig_points(1:2:end) orig_points(2:2:end)]+1;
    query_points{s}=coords;

end


for m=1:models
    for q=1:querys
        mapped_points_size=size(query_points{q},1);
        orig_points=fread(fid,mapped_points_size*2,'double=>double');
        coords=[orig_points(1:2:end) orig_points(2:2:end)]+1;
        mapping{m,q}=coords;
        
    end
end



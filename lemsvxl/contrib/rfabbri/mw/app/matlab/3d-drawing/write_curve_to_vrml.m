function [] = write_curve_to_vrml(curve,color,fname)

    diff_factor = 0.0000000001;

    numSamples = size(curve,1);
    
    %Open the file
    fid = fopen(fname,'w');
    
    %Write the header
    fprintf(fid,'ply\n');
    fprintf(fid,'format ascii 1.0\n');
    fprintf(fid,'element vertex ');
    fprintf(fid,'%d',(numSamples*2)-1);
    fprintf(fid,'\n');
    fprintf(fid,'property float x\n');
    fprintf(fid,'property float y\n');
    fprintf(fid,'property float z\n');
    fprintf(fid,'property uchar red\n');
    fprintf(fid,'property uchar green\n');
    fprintf(fid,'property uchar blue\n');
    fprintf(fid,'element face ');
    fprintf(fid,'%d',(numSamples-1));
    fprintf(fid,'\n');
    fprintf(fid,'property list uchar int vertex_index\n');
    fprintf(fid,'end_header\n');
    
    %Write out the vertices
    fprintf(fid,'%f %f %f %d %d %d\n',curve(1,1),curve(1,2),curve(1,3),color(1,1),color(1,2),color(1,3));
    
    for s=2:numSamples
        fprintf(fid,'%f %f %f %d %d %d\n',(curve(s-1,1)+curve(s,1))/2,(curve(s-1,2)+curve(s,2))/2,(curve(s-1,3)+curve(s,3))/2,color(1,1),color(1,2),color(1,3));
        fprintf(fid,'%f %f %f %d %d %d\n',curve(s,1),curve(s,2),curve(s,3),color(1,1),color(1,2),color(1,3));
    end
    
    for s=1:(numSamples-1)
        fprintf(fid,'%d %d %d %d %d\n',3,(s-1)*2,(s-1)*2+1,s*2);
    end
    
    fclose(fid);

end


%build edge-curve link in text files
%numViews is defined in definitions_12 module

for v=1:numViews
    
    %write the path where cemv files will be located
    fileNames = dir('/.../*.cemv');
    fileName = fileNames(v,1).name;
    cloc = strfind(fileName,'.cemv');
    viewName = fileName(1,1:(cloc-1));

    %write the path where cemv and edg files are located
    [Mcemv,Edges_per_curve] = read_cem_file_modified(['/.../',viewName,'.cemv']);
    Medg = read_edg_file(['/.../',viewName,'.edg']);
    Edges_per_curve;
    total_curves = length(Edges_per_curve);
    total_edges = sum(Edges_per_curve);
    
    %LocResult contains the indexes in the order they were assigned
    LocResult = [];
    found  = 0;
    
    %keep the distance;
    vdist = ones(1,size(Mcemv,1));
    
    for i=1:length(Mcemv)
        for j=1:length(Medg) 
            dist = norm([Mcemv(i,1),Mcemv(i,2)]-[Medg(j,1),Medg(j,2)]);
            vdist(i) = dist;
            if dist < 10^(-2) % compute if distance between a subpixel in cemv and edg is whithin threshold
                LocResult = [LocResult, j];
                found = 1;
                break
            end
        end
        if found == 0
            LocResult = [LocResult, 0];
        end
        found = 0; % reset bool
    end
    %write the path where txt files will be located
    writematrix(LocResult,['/.../',viewName,'.txt']);
    fileID = fopen(['/.../',viewName,'.txt'],'wt');
    
    edges_indexing = 1;
    
    for i=1:total_curves
    
        string = string + newline + Edges_per_curve(i);
        
        for j=1:Edges_per_curve(i)
            string = string + newline + LocResult(edges_indexing + j-1);
        end
    
        string = string + newline;
    
        edges_indexing = edges_indexing + Edges_per_curve(i);
    end
    fprintf(fileID,"%s",string);
    fclose(fileID);
    clear;
end

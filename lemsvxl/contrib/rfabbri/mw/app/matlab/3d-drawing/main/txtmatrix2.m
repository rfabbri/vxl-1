[Mcemv,Edges_per_curve] = read_cem_file_modified("midday-640x-0001.cemv");
Medg = read_edg_file("midday-640x-0001.edg");

Mcemv4 = round(Mcemv,4,"decimals");
Medg4 = round(Medg,4,"decimals");

Edges_per_curve;
total_curves = length(Edges_per_curve);
[Result,LocResult] = ismember(Mcemv4,Medg4,'rows');

writematrix(LocResult,'midday-640x-0001.txt');

fileID = fopen('midday-640x-0001.txt','wt');
string = "Curve Edge linking file";
string = string + newline + "Made by Carlos Bião, Gabriel Andrade, Ricardo Fabbri";
%string = string + newline + "TEXTO CAPES EXIGENCIA DELES!!!!!!";
string = string + newline;
string = string + newline;

edges_indexing = 1;

for i=1:total_curves
    string = string + newline + "[BEGIN CURVE " + i + "]";

    string = string + newline + "numCurEdges=" + "["Edges_per_curve(i)"]";
    
    for j=1:Edges_per_curve(i)
        string = string + newline + "[EDGE ID " + j + "  " +LocResult(edges_indexing + j-1) +"]";
    end
    string = string + newline + "[END CURVE " + i + "]";

    string = string + newline;

    edges_indexing = edges_indexing + Edges_per_curve(i);
end
fprintf(fileID,"%s",[string]);
fclose(fileID);

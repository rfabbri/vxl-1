function points = extract_points(img,method)
%{
This functions takes two inputs and returns as output a matrix of points that we want to fix, which
will be used for reconstruction using a different function.

Inputs 
1)img = This is the input GRAYSCALE image which will specify the intensities at the points we want to fix.
2)method = This is used to specify which points we want to extract. It can have the following values
 >'maxima' extracts the maxima points and removes the connected components
 >'minima' extracts the minima points and removes the connected components
 >'watershed' extracts the indices of the watershed lines 
 >'watercourse' extracts the indices of the watercourse lines
 >'edges' extracts the indices of the edges using the default Sobel Edge Detector

Output
The output matrix is a logical matrix which has 1's at the indices which we want to fix and has 0's
at the indices that we do not want to fix.
%}

switch method
    case 'maxima'
        max=imregionalmax(img);
        ccmax=bwconncomp(max);
        smax=regionprops(ccmax,'centroid');
        centmax=cat(1,smax.Centroid);
        centmax=round(centmax);
        [a,~]=size(centmax);
        max=zeros(size(img));
        for i=1:a
            max(centmax(i,1),centmax(i,2))=1;
        end
        I=max;

    case 'minima'
        min=imregionalmin(img);
        ccmin=bwconncomp(min);
        smin=regionprops(ccmin,'centroid');
        centmin=cat(1,smin.Centroid);
        centmin=round(centmin);
        [a,~]=size(centmin);
        min=zeros(size(img));
        for i=1:a
            min(centmin(i,1),centmin(i,2))=1;
        end
        I=min;
    case 'watershed'
        %Conver the image into double 
        imgd=double(img);
        ws=watershed(imgd);
        I=ws;
        
    case 'watercourse'
        imgd=double(img);
        wc=watershed(-imgd);
        I=wc;
        
    case 'edges'
        ed=edge(img);  
        I=ed;
end

points = I;





function [features,descriptors] = extract_features(img,method)
%{
This function extract features from an input image, where method specifies the algorithm used to
extract the features.

Inputs
1)img - This is the input GRAYSCALE image from which we need to extract the keypoints and descriptors
2)method - This structure specifies the algorithm used to extract image keypoints and descriptors.
the method.keypoint variable can assume the following values
 >'SIFT' extracts sift keypoints 
 >'maxima' extract maxima keypoints from image, while removing connected components
 >'minima' extract minima keypoints from image, while removing connected components
the method.descriptor variable specifies the type of descriptors to extract. 

Outputs
1)features - is a matrix where each column is a feature frame and has the format [X;Y;S;TH], where 
X,Y is the (fractional) center of the frame, S is the scale and TH is the orientation (in radians).
2)descriptors - is the matrix where each column is the descriptor of the corresponding frame in features.
A descriptor is a 128-dimensional vector of class UINT8.
%}

switch method.keypoint
    case 'SIFT'
        img=single(img);
        [f,~]=vl_sift(img); %using vl_feat inbuilt function
        features = f ;
        
    case 'maxima'
        img=single(img);                   
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
        [a,b]=size(max);
        l=0;
        for i=1:a
            for j=1:b
                if (max(i,j)==1)
                    l=l+1;
                    f(1,l)=i;
                    f(2,l)=j;
                end
            end
        end
        f(3,:)=1;
        f(4,:)=0;
        features = f;
                
    case 'minima'
        img=single(img);
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
        [a,b]=size(min);
        l=0;
        for i=1:a
            for j=1:b
                if (min(i,j)==1)
                    l=l+1;
                    f(1,l)=i;
                    f(2,l)=j;
                end
            end
        end
        f(3,:)=1;
        f(4,:)=0;
        features = f;
       
end

switch method.descriptor
    case 'SIFT'
        if (strcmp(method.keypoint,'SIFT'))
            [~,d]=vl_sift(img);       
        else
            [~,d]=vl_sift(img,'frames',f,'orientations'); 
        end
        descriptors = d;
end



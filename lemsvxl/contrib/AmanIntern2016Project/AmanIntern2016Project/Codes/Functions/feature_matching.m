function [matches,scores] = feature_matching(d1,d2,method)
%{
This function takes the descriptors d1 and d2 of two images which have been extracted beforehand.
It outputs two matrices, each of which are described below

Inputs
1)d1 - It's the descriptor matrix of the first image where each column is the descriptor of the 
corresponding frame in features. A descriptor is a 128-dimensional vector of class UINT8.
2)d2 - It's the descriptor matrix of the second image where each column is the descriptor of the 
corresponding frame in features. A descriptor is a 128-dimensional vector of class UINT8.
3)method - This variable specifies the algorithm we want to use for matching the two images.

Outputs 
For each descriptor in d1, vl_ubcmatch finds the closest descriptor in d2 (as measured
by the L2 norm of the difference between them)
1)matches - The index of the original match and the closest descriptor is stored in each column of
matches
2)scores - The distance between the pair of matched descriptors is stored in scores. 
%}

switch method
    case 'SIFT'
        [matches,scores] = vl_ubcmatch(d1,d2);
        
end



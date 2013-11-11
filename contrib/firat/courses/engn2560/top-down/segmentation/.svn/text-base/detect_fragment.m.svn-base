function detection = detect_fragment(imfile, fragmat, maxNdet)
        I = imread(imfile);
        load(fragmat);
        try
                R = abs(normalized_cross_correlation(im_frag, I));
        catch
                detection.Score = [];
                return
        end
        [r,c] = nonmaxsuppts(R, 21, 0.3);
        [responses, rind] = sort(diag(R(r,c)), 'descend');
        if length(rind) > maxNdet
                rind = rind(1:maxNdet);
                responses = responses(1:maxNdet);
        end
        response_locations = [c(rind) r(rind)];  
        detection.Score = responses;
        detection.Loc = response_locations;
        detection.Mask = gt_frag;
        detection.ExpNorLoc = normalized_center;
        detection.SrcObj = objectname;       
end



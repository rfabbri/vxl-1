function all_detections = detect_all_fragments(imfile, fragbank, maxNdet)
        fragmats = dir([fragbank '/*.mat']);
        all_detections = cell(1, length(fragmats));
        for i = 1:length(fragmats)
                fprintf('Frag %d...\n', i);
                all_detections{i} = detect_fragment(imfile, [fragbank '/' fragmats(i).name], maxNdet);
        end
end


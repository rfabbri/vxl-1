function length = get_length(curve)

    numSamples = size(curve,1);
    length = 0;
    for s=2:numSamples
        length = length + norm(curve(s,:)-curve(s-1,:));
    end

end


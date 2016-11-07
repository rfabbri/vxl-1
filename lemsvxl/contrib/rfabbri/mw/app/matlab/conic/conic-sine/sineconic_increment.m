    disp (['Prev s0_idx: ' int2str(s0_idx)]);
    s0_idx = mod(s0_idx + istep -6, size(Gamma,1)-2*6) +6;
    disp (['Curr s0_idx: ' int2str(s0_idx)]);

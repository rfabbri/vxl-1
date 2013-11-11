% This is /lemsvxl/contrib/firat/appearance/gabor_descriptor.m.

% \file
% \author Firat Kalaycilar (firat@lems.brown.edu)
% \date Jul 12, 2010

function desc = gabor_descriptor(im, nscale, norient, minWaveLength, mult, sigmaOnf, dThetaOnSigma, sampling_rate)
    addpath ~/lemsvxl/src/contrib/firat/appearance/gabor
    %default parameters
    nscale = 4;
    norient = 8;
    minWaveLength = 3;
    mult = 2;
    sigmaOnf = 0.65;
    dThetaOnSigma = 1.5;
    sampling_rate = 3;
    %%
    if ~isnumeric(im)
        im = imread(im);
    end
    if isrgb(im)
        im = rgb2gray(im);
    end
    EO = gaborconvolve(im,  nscale, norient, minWaveLength, mult, sigmaOnf, dThetaOnSigma, 0);
    im = im(1:sampling_rate:end, 1:sampling_rate:end);
    data_size = prod(size(im));
    desc = zeros(data_size*nscale, norient);
    for s = 1:nscale
        for o = 1:norient
            res = abs(EO{s,o});
            res = res(1:sampling_rate:end, 1:sampling_rate:end);
            desc((data_size * s - data_size + 1):(data_size * s), o) = res(:);
        end
    end
end

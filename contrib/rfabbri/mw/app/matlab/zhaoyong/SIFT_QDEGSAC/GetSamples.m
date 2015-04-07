function Samples = GetSamples(Points,NoSamples)
%
% call:
%      Samples = GetSamples(Points,NoSamples)
% Returns NoSamples samples from Point array.
% every ppoint is contained only ones in the samples array
%
%

s = sort(ceil(size(Points,1)*rand(NoSamples,1)));

for i = 2 : NoSamples
  
  if (s(i-1) == s(i))
    if (i == NoSamples)
      if (s(1) > 1)
        s(i) = 1;
      else
        s(i) = s(1)+1;
      end
    else
      s(i) = s(i) + 1;
    end
  end
end 

Samples = Points(s,:);
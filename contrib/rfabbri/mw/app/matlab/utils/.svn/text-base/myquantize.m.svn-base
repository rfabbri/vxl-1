% crv: 2 columns (x,y), n rows 
% qtz: grid-quantized result (assuming fine enough sampling in crv)
function qtz = myquantize(crv)

rqtz = round(crv);

% remove dups
k=2;
qtz = rqtz(1,:);
for i=2:size(crv,1)
  if ((rqtz(i,1) ~= qtz(k-1,1)) | (rqtz(i,2) ~= qtz(k-1,2)))
    qtz(k,:) = rqtz(i,:);
    k = k+1;
  end
end

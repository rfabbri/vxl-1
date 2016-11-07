%capitol:
%inst_template = [0 120 60 150 180 240];
%delta_offset = 120;
%nf=500;

%dino: 
%inst_template = [28 206 160 133 150 361 337];
%nf = 363;
%delta_offset=178
%steeple:
%inst_template = [ 0 98 60 30 80 15 ];
%downtown: 
%inst_template = [0 30 15 45 60 115];
%nf = 172; delta_offset = 30;
%steeple: 

% Pot fragments:
%inst_template = [0 1 2 7 30 19];

% Pot subset:
%inst_template = [0 1 3 2 5 8];

%Capitol High: 
inst_template = [0 28 15 45 10 60];
nf = 253; delta_offset = 28;


%n_instances = (nf - inst_template(2) - 1) / delta_offset;
%nf = 253; delta_offset = 28;

n_instances = 30;

all_inst = zeros(n_instances, max(size(inst_template)));

for i=1:n_instances
  all_inst(i,:) = mod(inst_template + (i-1)*delta_offset, nf);
%  delta_offset = delta_offset + 1;
end

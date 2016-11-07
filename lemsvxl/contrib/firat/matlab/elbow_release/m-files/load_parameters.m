function params = load_parameters(filename)
	fid = fopen(filename, 'rt');
	params = struct;
	while 1
        tline = fgetl(fid);
        if ~ischar(tline)
        	break
        end
        [var, rest] = strtok(tline, '=');
        var = sscanf(var, ' %s ');
        if isempty(var)
        	break
        end
        value = sscanf(rest, ' = %s');
        params = setfield(params, var, value);
    end
    fclose(fid);
end

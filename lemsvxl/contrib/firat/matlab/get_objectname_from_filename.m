function objname = get_objectname_from_filename(filename, ext1)
        slashpos = strfind(filename, '/');
        if length(slashpos) > 0
            slashpos = slashpos(end);
            filename = filename(slashpos + 1 : end);
        end
        if(exist('ext1', 'var'))
            LF = length(filename);
            LE = length(ext1);
            ext2 = filename(LF - LE + 1 : LF);
            if strcmp(ext1, ext2)
                objname = filename(1 : LF - LE - 1);
            else
                objname = filename;
            end  
        else
            dotpos = strfind(filename, '.');
            dotpos = dotpos(end);
            objname = filename(1 : dotpos - 1);
        end
               
end

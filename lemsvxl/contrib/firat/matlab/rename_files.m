function rename_files(pfolder, ext, pref, ndig, ext2)
       files = dir([pfolder '/*.' ext]);
       for i = 1:length(files)
                fprintf('Original name: %s\n', files(i).name);
                destname = [pref getIndex(ndig, i) '.' ext2];
                srcfull = [pfolder '/' files(i).name];
                destfull = [pfolder '/' destname];
                eval(sprintf('!mv %s %s', srcfull, destfull));
                fprintf('Moved to %s\n', destfull);
       end 
end

function s = getIndex(ndig, i)
        numd =  floor(log10(i)) + 1;
        numz = ndig - numd;
        s = '';
        for k = 1:numz
                s = [s '0'];
        end
        s = [s num2str(i)];
end

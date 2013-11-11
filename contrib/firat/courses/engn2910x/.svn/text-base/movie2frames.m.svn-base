function movie2frames(movie_filename, output_directory, file_extension)
	eval(sprintf('!mkdir -p %s', output_directory));
	ffmpeg_cmd = sprintf('!ffmpeg -i %s %s/frame%s.%s', movie_filename,output_directory,'%10d',file_extension);	
	eval(ffmpeg_cmd);
end

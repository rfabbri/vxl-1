function frames2movie(image_directory, file_extension, output_movie_file)
	ffmpeg_cmd = sprintf('!ffmpeg -r 10 -b 1800 -i %s/frame%s.%s %s', image_directory, '%10d', file_extension, output_movie_file);
	eval(ffmpeg_cmd);	
end

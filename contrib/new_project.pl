#!/bin/sh
# -*- perl -*-
exec perl -w -x $0 ${1+"$@"}
#!perl
#line 6
# If Windows barfs at line 3 here, you will need to run perl -x this_file.pl
# You can set up as a permanent file association using the following commands
#  >assoc .pl=PerlScript
#  >ftype PerlScript=C:\Perl\bin\Perl.exe -x "%1" %*

#-----------------------------------------------------------
# Usage:
#  new_project.pl project_name
#-----------------------------------------------------------

use Cwd;
use Getopt::Std;
use File::Copy;
use File::Path;

#-----------------------------------------------------------
# rename($src_dir);
#-----------------------------------------------------------

#-----------------------------------------------------------
# copy_dir($src_dir, $dest_dir, $projectname);
#-----------------------------------------------------------

sub copy_dir 
{
  my($src_dir, $dest_dir, $projectname)=@_;

  opendir(DIR,$src_dir) || die "Can't open $src_dir\n";
  my @files = grep {not /^\./ and $_ ne 'CVS'} readdir DIR;
  closedir(DIR);

  my $file;
  my $new_file;
  foreach $file (@files){
    # Replace all instances of "starter_kit" with "$projectname"
    ($new_file = $file) =~ s/starter_kit/$projectname/g;

    # If this is a subdirectory
    if(opendir(DIR,"$src_dir/$file")){
      closedir(DIR);
      $new_dir = "$dest_dir/$new_file";
      mkpath($new_dir,0,0777) || die "Can't create $new_dir\n";
      print "Creating: $new_dir\n";
      copy_dir("$src_dir/$file",$new_dir,$projectname);
    }

    # If this is a file
    else{
      open(F_IN, "<", "$src_dir/$file") || die "Can't read $file\n";
      open(F_OUT, ">", "$dest_dir/$new_file") || die "Can't write to $new_file\n";
      print "Creating: $dest_dir/$new_file\n";
      while (<F_IN>){
        s/starter_kit/$projectname/g;
        print F_OUT $_;
      }
      close(F_IN);
      close(F_OUT);
    }

  } 
}

#-----------------------------------------------------------
# Main
#-----------------------------------------------------------

($#ARGV == 0) || die "Usage:  new_project.pl project_name\n";  
my $projectname = $ARGV[0];
chomp($projectname);

my $new_basepath = "./$projectname";
my $old_basepath = "./starter_kit";

(! -e $new_basepath) || die "Project $projectname already has a directory\n";
(-e $old_basepath) || die "Can't access the starter_kit directory\n";

mkpath($new_basepath,1,0777) || die "Can't create $new_basepath\n";

copy_dir($old_basepath, $new_basepath, $projectname);

open(CMAKE, ">>", "./CMakeLists.txt") || die "Can't read ./CMakeLists.txt\n";
print "Updating CMakeLists\n";

print CMAKE "\n# Contrib Project: $projectname\n";
print CMAKE "IF( DART_ROOT )\n";
print CMAKE "  OPTION(BUILD_CONTRIB_$projectname \"Build contrib/$projectname\" YES)\n";
print CMAKE "ELSE( DART_ROOT )\n";
print CMAKE "  OPTION(BUILD_CONTRIB_$projectname \"Build contrib/$projectname\" NO)\n";
print CMAKE "ENDIF( DART_ROOT )\n";
print CMAKE "IF(BUILD_CONTRIB_$projectname)\n";
print CMAKE "    SUBDIRS( $projectname )\n";
print CMAKE "ENDIF(BUILD_CONTRIB_$projectname)\n\n";

close(CMAKE);

exit;

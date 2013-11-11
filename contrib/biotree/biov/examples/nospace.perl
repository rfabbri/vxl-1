#!/usr/bin/perl -w
# nospace /this/dir /that/dir /those/too 

use File::Find; 
use strict; 
die "usage: nospace dir[s]\n" unless @ARGV; 

my %ext; 

find(\&remspaces, @ARGV); 

sub remspaces {
return if ($_ eq '.'); 
return if ($_ eq '..'); 
(my $new = $_) =~ tr/a-zA-Z0-9_.-/_/c; 
my $duplicate = ($new ne $_ and -e $new);
my $try = $new;

$ext{"$File::Find::dir/$try"}++ if $duplicate;

while (my $count = $ext{"$File::Find::dir/$new"}++) {
(my $with_num = $new) =~ s/(?=\.|$)/_$count/;
$new = $with_num, last if not -e $with_num;
}

$ext{"$File::Find::dir/$try"}-- if $duplicate;

rename $_ => $new
or warn "can't rename $_ to $new: $!";
} 


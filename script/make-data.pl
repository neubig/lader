#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

# get the arguments
my $MAX_LEN = 40;
GetOptions( 
    "max_len=i"=>\$MAX_LEN, # The maximum length
);

open FFILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";

my $f;
while(($f = <FFILE>)) {
    chomp $f;
    my $flen = split(/ /, $f);
    next if $flen > $MAX_LEN;
    print "$f\n";
}

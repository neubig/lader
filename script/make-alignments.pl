#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

# get the arguments
my $REVERSE = 0;
my $MAX_LEN = 40;
GetOptions( 
    "reverse!"=>\$REVERSE, # If alignments are target-source
    "max_len=i"=>\$MAX_LEN, # The maximum length
);

if(@ARGV != 3) {
    print STDERR "Usage: make-alignments.pl [SOURCE] [TARGET] [ALIGNMENTS]\n";
    exit 1;
}

open FFILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
open EFILE, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
open AFILE, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";

my ($f, $e, $a);
while(($f = <FFILE>) and ($e = <EFILE>) and ($a = <AFILE>)) {
    chomp $f; chomp $e; chomp $a;
    my $flen = split(/ /, $f);
    my $elen = split(/ /, $e);
    next if $flen > $MAX_LEN;
    if($REVERSE) {
        $a = join(" ", map { my ($af, $ae) = split(/-/); "$ae-$af" } split(/ /, $a));
    }
    print "$flen-$elen ||| $a\n";
}

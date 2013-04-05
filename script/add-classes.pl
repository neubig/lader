#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

if(@ARGV != 1) {
    print STDERR "Usage: add-classes.pl CLASS_FILE < INPUT > OUTPUT\n";
    exit 1;
}

open FILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
my %word2class = map { chomp; my ($w,$c) = split(/[\t ]/); $w => $c } <FILE>;
close FILE;

while(<STDIN>) {
    chomp;
    my @classes = map { $word2class{$_} ? $word2class{$_} : 0 } split(/ /);
    print "@classes\n";
}

#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

if(@ARGV != 0) {
    print STDERR "Usage: sort-model-features.pl < model-file.txt";
    exit 1;
}

while(<STDIN>) {
    chomp;
    last if /reorderer_model/;
}

my %feats;
while(<STDIN>) {
    chomp;
    my ($name, $val) = split(/\t/);
    $feats{$name} = $val;
}

for(sort {$feats{$b} <=> $feats{$a}} keys %feats) {
    print "$_\t$feats{$_}\n";
}

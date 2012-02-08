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

my (@feats);
$_ = <STDIN>; chomp; /feature_set/ or die "Bad line in model $_";
$_ = <STDIN>; chomp; /config_str/ or die "Bad line in model $_";
$_ = <STDIN>; chomp; /max_term/ or die "Bad line in model $_";
$_ = <STDIN>; chomp; /^\d*$/ or die "Bad line in model $_";
while(<STDIN>) {
    chomp;
    last if not $_;
    push @feats, [$_, 0];
}

$_ = <STDIN>; chomp; /^$/ or die "Bad line in model $_";
$_ = <STDIN>; chomp; /reorderer_model/ or die "Bad line in model $_";
foreach my $i (0 .. $#feats) {
    $_ = <STDIN>; chomp;
    $feats[$i]->[1] = $_;
}

for(sort { my $val = ($b->[1] <=> $a->[1]); $val = ($a->[0] cmp $b->[0]) if not $val; $val } @feats) {
    print join("\t", @$_)."\n" if $_->[1];
}

#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my $DISCOUNT = 1;
my $BONUS = 0.5;
my $MAX_LEN = 8;
my $BIG = 100000;

# Count the number of times a source phrase would be extracted
if(@ARGV != 3) {
    print STDERR "Usage: phrase-extract.pl [SOURCE] [TARGET] [ALIGN]\n";
    exit 1;
}

open EFILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
open FFILE, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
open AFILE, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";

my %counts;

my ($e, $f, $al);
my $sent = 0;
while(($f = <FFILE>) and ($e = <EFILE>) and ($al = <AFILE>)) {
    print STDERR "$sent\n" if(++$sent % 10000 == 0);
    # print "$e$f$al";
    chomp $f; chomp $e; chomp $al;
    $al =~ s/.* \|\|\| //g;
    my @es = split(/ /, $e);
    my @fs = split(/ /, $f);
    # Find @A
    my @A = map { my @arr = split(/-/); \@arr } split(/ /, $al);
    # Find c(j)
    my @cj = map { 0 } (@fs, 0);
    for(@A) { my($i, $j) = @$_; $cj[$j+1] = 1; }
    for(2 .. $#cj) { $cj[$_] += $cj[$_-1]; }
    pop @cj;
    # Find A'
    my @Aprime = map { [ $_->[0], $cj[$_->[1]] ] } @A;
    # Find Jis
    my @Ji = map { [] } @es;
    for(@Aprime) { push @{$Ji[$_->[0]]}, $_->[1]; }
    # Find psii
    my @psii = map { @$_ ? [ min(@$_), max(@$_)+1 ] : [$BIG, -1] } @Ji;
    # Cover phrases
    foreach my $k (0 .. $#es - 1) {
        my @psikl = @{$psii[$k]};
        foreach my $l ( $k+2 .. min($k+$MAX_LEN,$#es) ) {
            $psikl[0] = min($psikl[0], $psii[$l-1]->[0]);
            $psikl[1] = max($psikl[1], $psii[$l-1]->[1]);
            my $contig = ($psikl[1] != -1) ? 1 : 0;
            for(@Aprime) {
                last if not $contig;
                my ($i, $j) = @$_;
                if(($j >= $psikl[0]) and ($j < $psikl[1]) and 
                    (($i < $k) or ($i >= $l))) {
                    $contig = 0;
                }
            }
            my $str = join(" ", @es[$k .. $l-1]);
            $counts{$str} = [ 0, 0 ] if not $counts{$str};
            $counts{$str}->[0] += $contig;
            $counts{$str}->[1] += 1;
        }
    }
}

for(sort { $counts{$b}->[1] <=> $counts{$a}->[1] } keys %counts) {
    if($counts{$_}->[0] > $DISCOUNT) {
        my $l0 = log($counts{$_}->[0]);
        my $l1 = log($counts{$_}->[1]);
        printf "%s ||| %.4f %.4f %.4f\n", $_, $l0, $l1, $l0-$l1;
    }
}

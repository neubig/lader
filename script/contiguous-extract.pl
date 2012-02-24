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
my $MAX_LEN = 4;

# Count the number of times a source phrase would be extracted
if(@ARGV != 3) {
    print STDERR "Usage: phrase-extract.pl [SOURCE] [TARGET] [ALIGN]\n";
    exit 1;
}

open FFILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
open EFILE, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
open AFILE, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";

my %counts;

my ($f, $e, $al);
my $sent = 0;
while(($f = <FFILE>) and ($e = <EFILE>) and ($al = <AFILE>)) {
    print STDERR "$sent\n" if(++$sent % 10000 == 0);
    chomp $f; chomp $e; chomp $al;
    my @fs = split(/ /, $f);
    my @es = split(/ /, $e);
    my @fspan = map { [scalar(@es), -1] } @fs;
    my @espan = map { [scalar(@fs), -1] } @es;
    my %a;
    $al =~ s/.* \|\|\| //g;
    for(split(/ /, $al)) {
        my ($j, $i) = split(/-/);
        ($j >= @fs) and die "Illegal alignment $j in $_ (fs=@fs)\n";
        ($i >= @es) and die "Illegal alignment $i in $_ (es=@es)\n";
        $fspan[$j] = [ min($i, $fspan[$j]->[0]), max($i, $fspan[$j]->[1]) ];
        $espan[$i] = [ min($j, $espan[$i]->[0]), max($j, $espan[$i]->[1]) ];
    }
    # For all starting points $j1
    foreach my $j1 (0 .. $#fs) {
        my $contiguous = 1;
        # For all ending points $j2 up to a maximal length
        foreach my $j2 ($j1+1 .. min($j1+$MAX_LEN-1, $#fs)) {
            if(($j2 != $j1) and ($fspan[$j1]->[1] != -1) and ($fspan[$j2]->[1] != -1) and
               (($fspan[$j1]->[1] < $fspan[$j2]->[0]-1) or ($fspan[$j2]->[1] < $fspan[$j1]->[0]-1))) {
                $contiguous = 0;
            }
            my $str = join(" ", @fs[$j1 .. $j2]);
            $counts{$str} = [1, 0.5] if not $counts{$str};
            $counts{$str}->[0]++;
            $counts{$str}->[1]++ if $contiguous;
        }
    }
}

for(sort { $counts{$a}->[1]/$counts{$a}->[0] <=> $counts{$b}->[1]/$counts{$b}->[0] } keys %counts) {
    if($counts{$_}->[0] > $DISCOUNT) {
        print "$_ ||| ".log($counts{$_}->[1]/$counts{$_}->[0])."\n";
    }
}

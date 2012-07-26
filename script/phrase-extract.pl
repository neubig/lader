#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my $DISCOUNT = 1;
my $MAX_LEN = 10;
GetOptions(
    "discount=i" => \$DISCOUNT,
    "max-len=i" => \$MAX_LEN,
);

# Count the number of times a source phrase would be extracted
if(@ARGV != 3) {
    print STDERR "Usage: phrase-extract.pl [SOURCE] [TARGET] [ALIGN]\n";
    exit 1;
}

open FFILE, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
open EFILE, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
open AFILE, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";

my %counts;

my ($f, $e, $a);
my $sent = 0;
while(($f = <FFILE>) and ($e = <EFILE>) and ($a = <AFILE>)) {
    print STDERR "$sent\n" if(++$sent % 10000 == 0);
    chomp $f; chomp $e; chomp $a;
    my @fs = split(/ /, $f);
    my @es = split(/ /, $e);
    my @fspan = map { [scalar(@es), -1] } @fs;
    my @espan = map { [scalar(@fs), -1] } @es;
    my %a;
    for(split(/ /, $a)) {
        my ($j, $i) = split(/-/);
        ($j >= @fs) and die "Illegal alignment $j in $_ (fs=@fs)\n";
        ($i >= @es) and die "Illegal alignment $i in $_ (es=@es)\n";
        $fspan[$j] = [ min($i, $fspan[$j]->[0]), max($i, $fspan[$j]->[1]) ];
        $espan[$i] = [ min($j, $espan[$i]->[0]), max($j, $espan[$i]->[1]) ];
    }
    # For all starting points $j1
    foreach my $j1 (0 .. $#fs) {
        my @span = @{$fspan[$j1]};
        next if $span[1] == -1; # Skip phrases starting with a null
        # For all ending points $j2 up to a maximal length
        foreach my $j2 ($j1 .. min($j1+$MAX_LEN-1, $#fs)) {
            next if $fspan[$j2]->[1] == -1;
            $span[0] = min($span[0], $fspan[$j2]->[0]);
            $span[1] = max($span[1], $fspan[$j2]->[1]);
            # Check if the whole target span falls within j1 and j2
            my $consecutive = 1;
            foreach my $i ($span[0] .. $span[1]) {
                if(($espan[$i]->[0] < $j1) or ($espan[$i]->[1] > $j2)) {
                    $consecutive = 0;
                    last;
                }
            }
            next if not $consecutive;
            # The span is consecutive, so extract this and all surrounding
            # null aligned spans
            for(my $j3 = $j1; ($j3 >= 0) and (($j3 == $j1) or ($fspan[$j3]->[1] == -1)); $j3--) {
                for(my $j4 = $j2; ($j4 < @fs) and (($j4 == $j2) or ($fspan[$j4]->[1] == -1)); $j4++) {
                    last if $j4-$j3 >= $MAX_LEN;
                    my $str = join(" ", @fs[$j3 .. $j4]);
                    # print "$str\n";
                    $counts{$str}++;
                }
            }
        }
    }
}

for(sort keys %counts) {
    if($counts{$_} > $DISCOUNT) {
        print "$_ ||| ".log($counts{$_}-$DISCOUNT)."\n";
    }
}

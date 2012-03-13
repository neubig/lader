#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my $BLEU = "/home/neubig/work/mosesdecoder-tuo/scripts/generic/multi-bleu.perl";
$|++;

# get the arguments
my $ITER = 1000;
my $TYPE = "bleu";
my $FRAC = 0.5;
GetOptions( 
    "type=s"=>\$TYPE, # If alignments are target-source
    "iter=i"=>\$ITER, # The maximum length
    "frac=f"=>\$FRAC # The fraction of the data to use each time
);

if(@ARGV != 3) {
    print STDERR "Usage: bootstrap.pl REFERENCE SYS1 SYS2\n";
    exit 1;
}

my $ID = rand();

my (@ref, @sys1, @sys2);
open REF, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
@ref = <REF>;
close REF;
open SYS1, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
@sys1 = <SYS1>;
close SYS1;
open SYS2, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";
@sys2 = <SYS2>;
close SYS2;
die "sizes don't match: ref=".scalar(@ref).", sys1=".scalar(@sys1).", sys2=".scalar(@sys2)."\n" if (@ref != @sys1) or (@ref != @sys2) or (@ref == 0);

my (@eval1, @eval2);
my @wins = (0, 0, 0);
foreach my $iter (1 .. $ITER) {
    print STDERR ".";
    # Find the sentence IDs that we need
    my @sents = shuffle(0 .. $#ref);
    @sents = @sents[0 .. int($FRAC*$#ref)];
    # Print the references
    open REFOUT, ">:utf8", "/tmp/$ID.ref" or die "/tmp/$ID.ref: $!\n";
    print REFOUT join("", map { $ref[$_] } @sents);
    close REFOUT;
    open SYS1OUT, ">:utf8", "/tmp/$ID.sys1" or die "/tmp/$ID.sys1: $!\n";
    print SYS1OUT join("", map { $sys1[$_] } @sents);
    close SYS1OUT;
    open SYS2OUT, ">:utf8", "/tmp/$ID.sys2" or die "/tmp/$ID.sys2: $!\n";
    print SYS2OUT join("", map { $sys2[$_] } @sents);
    close SYS2OUT;
    # Get the evaluation
    my ($score1, $score2);
    if($TYPE eq "bleu") {
        # Perform the evaluation
        my $ret = `$BLEU /tmp/$ID.ref < /tmp/$ID.sys1`;
        $ret =~ /^BLEU = (\S*) / or die "Bad line $ret\n";
        $score1 = $1;
        $ret = `$BLEU /tmp/$ID.ref < /tmp/$ID.sys2`;
        $ret =~ /^BLEU = (\S*) / or die "Bad line $ret\n";
        $score2 = $1;
    } elsif($TYPE eq "ribes") {
        # TODO
    } elsif($TYPE eq "tau") {
        # TODO
    } elsif($TYPE eq "chunk") {
        # TODO
    } else {
        die "Unknown evaluation type $TYPE\n";
    }
    # Add the score to the total and compare
    push @eval1, $score1;
    push @eval2, $score2;
    if($score1 > $score2) { $wins[0]++; }
    elsif($score2 > $score1) { $wins[2]++; }
    else { $wins[1]++; }
}
print STDERR "\n";

sub stddev {
    my $mean = sum(@_)/@_;
    my $sqr;
    for(@_) { $sqr += ($_-$mean) ** 2; }
    return ($sqr / @_) ** 0.5;
}

my $mean1 = sum(@eval1)/$ITER;
my $stddev1 = stddev(@eval1);
print "Sys1: mean=$mean1, stddev=$stddev1\n";
my $mean2 = sum(@eval2)/$ITER;
my $stddev2 = stddev(@eval2);
print "Sys2: mean=$mean2, stddev=$stddev2\n";
print "Wins: ".join(" ", map { $_/$ITER } @wins)."\n";

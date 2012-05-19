#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use Getopt::Long;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

my $BLEU = "/home/neubig/work/mosesdecoder-tuo/scripts/generic/multi-bleu.perl";
my $RIBES = "python3 /home/neubig/usr/local/RIBES-1.02.3/RIBES.py";
my $EVALLADER = "/home/neubig/work/lader/src/bin/evaluate-lader";
$|++;

# get the arguments
my $ITER = 1000;
my $TYPE = "bleu";
my $ATTACH = "left";
my $FRAC = 0.5;
GetOptions( 
    "type=s"=>\$TYPE, # If alignments are target-source
    "attach=s"=>\$ATTACH, # Attach alignments
    "iter=i"=>\$ITER, # The maximum length
    "frac=f"=>\$FRAC # The fraction of the data to use each time
);

if((@ARGV < 3) or (@ARGV > 4)) {
    print STDERR "Usage: bootstrap.pl REFERENCE SYS1 SYS2 [ALIGN]\n";
    exit 1;
}
die "ARGV=3 and TYPE=$TYPE" if (@ARGV == 3) and ($TYPE !~ /^(bleu|ribes)/);
die "ARGV=4 and TYPE=$TYPE" if (@ARGV == 4) and ($TYPE !~ /^(tau|fuzzy)/);

my $ID = rand();

my (@ref, @sys1, @sys2, @al);
open REF, "<:utf8", $ARGV[0] or die "$ARGV[0]: $!\n";
@ref = <REF>;
close REF;
open SYS1, "<:utf8", $ARGV[1] or die "$ARGV[1]: $!\n";
@sys1 = <SYS1>;
close SYS1;
open SYS2, "<:utf8", $ARGV[2] or die "$ARGV[2]: $!\n";
@sys2 = <SYS2>;
close SYS2;
if(@ARGV == 4) {
    open AL, "<:utf8", $ARGV[3] or die "$ARGV[3]: $!\n";
    @al = <AL>;
    close AL;
}
die "sizes don't match: ref=".scalar(@ref).", sys1=".scalar(@sys1).", sys2=".scalar(@sys2)."\n" if (@ref != @sys1) or (@ref != @sys2) or (@ref == 0) or ((@ARGV >= 4) and (@al != @ref));

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
    if(@al) {
        open ALOUT, ">:utf8", "/tmp/$ID.al" or die "/tmp/$ID.al: $!\n";
        print ALOUT join("", map { $al[$_] } @sents);
        close ALOUT;
    }
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
        # Perform the evaluation
        my $ret = `$RIBES -r /tmp/$ID.ref /tmp/$ID.sys1 2> /dev/null`;
        $ret =~ /^(\S*) / or die "Bad line $ret\n";
        $score1 = $1 * 100;
        $ret = `$RIBES -r /tmp/$ID.ref /tmp/$ID.sys2 2> /dev/null`;
        $ret =~ /^(\S*) / or die "Bad line $ret\n";
        $score2 = $1 * 100;
    } elsif($TYPE eq "tau") {
        # Perform the evaluation
        my $ret = `$EVALLADER -attach_null $ATTACH /tmp/$ID.ref /tmp/$ID.sys1 /tmp/$ID.al 2> /dev/null | tail -n 1`;
        $ret =~ /tau=(\S*) / or die "Bad line $ret\n";
        $score1 = $1 * 100;
        $ret = `$EVALLADER -attach_null $ATTACH /tmp/$ID.ref /tmp/$ID.sys2 /tmp/$ID.al 2> /dev/null | tail -n 1`;
        $ret =~ /tau=(\S*) / or die "Bad line $ret\n";
        $score2 = $1 * 100;
    } elsif($TYPE eq "fuzzy") {
        # Perform the evaluation
        my $ret = `$EVALLADER -attach_null $ATTACH /tmp/$ID.ref /tmp/$ID.sys1 /tmp/$ID.al 2> /dev/null | tail -n 1`;
        $ret =~ /fuzzy=(\S*) / or die "Bad line $ret\n";
        $score1 = $1 * 100;
        $ret = `$EVALLADER -attach_null $ATTACH /tmp/$ID.ref /tmp/$ID.sys2 /tmp/$ID.al 2> /dev/null | tail -n 1`;
        $ret =~ /fuzzy=(\S*) / or die "Bad line $ret\n";
        $score2 = $1 * 100;
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

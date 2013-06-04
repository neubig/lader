#!/usr/bin/perl

# Train Lader in parallel by partitioning data and iterating once
# then recombinining the model, then repeat ad nauseum

# Jeremy Gwinnup (jeremy@gwinnup.org)

use Getopt::Std;
use strict;
use utf8;
use Cwd;
use FindBin qw($RealBin);

our($opt_i, $opt_n, $opt_s, $opt_b, $opt_a, $opt_p);
getopts("i:n:s:b:a:p:");

my $startitr = 1;

if(!$opt_i) {
    die "No number of iterations specified with -i!\n";
}

if(!$opt_n) {
    die "No number of partitions specified with -n!\n";
}

if(!$opt_b) {
    die "No annot file basename specified with -b!\n";
}

if(!$opt_a) {
    die "No align file basename specified with -a!\n";
}

if(!$opt_p) {
    die "No phrase table specified with -p!\n";
}

if($opt_s) {
    $startitr = $opt_s;
    print "Start iteration is : $startitr\n";
}

my $baseannot = $opt_b;
my $basealign = $opt_a;
my $pt = $opt_p;
my $cwd = getcwd;

for(my $i=$startitr; $i <= $opt_i; $i++) {

    my $modelstr = "";
    #create grid job
    if($i > 1){
	my $prev  = $i-1;
	$modelstr = "-m iter$prev/model.iter$prev";
    }

    print "creating lader train script for iter $i\n";
    my $cmd = "$RealBin/genTrainLaderSGE.pl -o train-lader-iter$i.sh -d iter$i -a $basealign -i $baseannot -p $pt -n $opt_n $modelstr";
    `$cmd`;

    #run grid job
    print "Running grid job for iter $i\n";
    $cmd = "qsub -S /bin/bash -sync y train-lader-iter$i.sh";
    `$cmd`;

    #recombine model
    print "Merging model for iter $i\n";
    $cmd = "$RealBin/mergeLaderModels.pl -i iter$i/model -o iter$i/model.iter$i -n $opt_n";
    `$cmd`;

}

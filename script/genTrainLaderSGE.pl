#!/usr/bin/perl

# Generate SGE job to train Lader model on n pieces of training corpus
# Only run one iteration

# Jeremy Gwinnup 

use Getopt::Std;
use strict;
use utf8;
use Cwd;

#Cbange these values to fit your installation
my $laderhome = "/path/to/lader/build/dir";
my $trainlader = "$laderhome/src/bin/train-lader";
my $queue = "all.q";
my $cwd = getcwd;
my $sgeoutdir = "$cwd/output";
my $cost ="1e-3";
my $attachnull = "right";

our($opt_n, $opt_a, $opt_i, $opt_o, $opt_m, $opt_t, $opt_p, $opt_d);
getopts("n:a:i:o:m:t:p:d:");

#check command line options
if(!$opt_o) {
    die "No output file specified with -o!\n";
}

if(!$opt_n) {
    die "No number of partitions specified with -n!\n";
}

if(!$opt_a) {
    die "No align file basename specified with -a!\n";
}

if(!$opt_i) {
    die "No input file base specified with -o!\n";
}

if(!$opt_p) {
    die "No phrase table specified with -p!\n";
}

if(!$opt_d) {
    die "No out dir specified with -d!\n";
}

my $throttle = 32; #max concurrent on Grid
if($opt_t) {
    $throttle = $opt_t;
}

#rename cmd options for clarity
my $numparts = $opt_n;
my $inbase = $opt_i;
my $alignbase = $opt_a;
my $phrasetable = $cwd . "/" . $opt_p;
my $model = $opt_m; #can be null if first itr
my $outdir = $opt_d;

#make output directory for SGE
if(!-d $sgeoutdir) {
    print "Creating output directory: $sgeoutdir\n";
    system("mkdir -p $sgeoutdir");
}
else {
    print "out dir: " . $sgeoutdir . "\n";
}

#make output directory for this iteration's models
if(!-d $outdir) {
    print "Creating model directory: $outdir\n";
    system("mkdir -p $outdir");
}
else {
    print "model out dir: " . $outdir . "\n";
}

#open outfile
open(OUTFILE, ">$opt_o") or die "Unable to open output script: $opt_o\n";

#print header
print OUTFILE "#!/bin/bash\n";
print OUTFILE "#\n";
print OUTFILE "# generated by $0 on " . localtime() . "\n";
print OUTFILE "#\n";
print OUTFILE "#\$ -t 1-$numparts\n";
print OUTFILE "#\$ -q $queue\n";
print OUTFILE "#\$ -l mem_free=4G,h_vmem=4G,hostname=quad*\n";
print OUTFILE "#\$ -v PATH\n";
print OUTFILE "#\$ -N train-lader\n";
print OUTFILE "#\$ -o $sgeoutdir\n";
print OUTFILE "#\$ -e $sgeoutdir\n";
print OUTFILE "#\$ -tc $throttle\n";
print OUTFILE "\n";

#lader feature strings...
#This is set up for a 3-'field' annot file - words, classes, parse
my $featpt = "seq=dict=$phrasetable";
my $featclass = "seq=LL%SL%ET,RR%SR%ET,LR%LR%ET,RL%RL%ET,B%SB%ET,A%SA%ET,O%SL%SR%ET,I%LR%RL%ET";
my $featparse = "cfg=LP%LP%ET,RP%RP%ET,SP%SP%ET,TP%SP%LP%RP%ET";

my $cnt = 1;
for(my $i=0; $i < $numparts; $i++) {

    my $input = sprintf("%s/%s.%03d", $cwd , $inbase, $i);
    my $align = sprintf("%s/%s.%03d", $cwd , $alignbase, $i);
    my $modelout = sprintf("%s/%s/model.%03d", $cwd , $outdir, $i);

    my $modelcmd = "";
    #watch the first iteration
    #if($opt_m) {
    my $modelcmd = "-model_in $cwd/$opt_m";
    #}

    my $cmd = "$trainlader $modelcmd -cost $cost -attach_null right -feature_profile \"$featpt|$featclass|$featparse\" -iterations 1 -model_out $modelout -source_in $input -align_in $align";

    printJob($cnt, $cmd);

    $cnt++;
}

close OUTFILE;

# Print the job line
sub printJob {
   
    #global $cnt;
    #global OUTFILE;

    my($taskid, $cmd) = @_;

    print OUTFILE "if [ \"\$SGE_TASK_ID\" = \"$taskid\" ]; then \n";
    print OUTFILE "   $cmd\n";
    print OUTFILE "fi\n\n";
	
}

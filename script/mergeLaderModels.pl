#!/usr/bin/perl

# Merge partitions of Lader reordering models
# Jeremy Gwinnup (jeremy@gwinnup.org)

use Getopt::Std;
use strict;
use utf8;
use Cwd;

our($opt_i, $opt_o, $opt_n);
getopts("i:o:n:");

if(!$opt_i) {
    die "No input file base specified with -i!\n";
}

if(!$opt_o) {
    die "No output file specified with -o!\n";
}

if(!$opt_n) {
    die "No number of partitions specified with -n!\n";
}

#basic hash map time
my %model = ();

#iterate over model chunks and increment
my $first = 1;
my $header = "";
for(my $i=0; $i < $opt_n; $i++) {
    
    my $modelfile = sprintf("%s.%03d", $opt_i, $i);
    open(MODELFILE, "$modelfile");

    my $thisheader = "";
    for(my $j=0 ; $j < 6; $j++) {
	$thisheader .= <MODELFILE>;
    }

    if($first) {
	
	$header = $thisheader;
	$first = 0;

    }

    #iterate over entries
    
    while(my $line = <MODELFILE>) {

	chomp $line;
	my @fields = split /\t/, $line;

	$model{@fields[0]} += $fields[1];

    }

    close MODELFILE;

}

print STDERR "model has " . keys(%model) . " entries\n";

#iterate over complete table and average out
#all entries

while ( my ($key, $value) = each %model) {
    
    #average (fields already summed)
    $model{$key} /= $opt_n; 

}

#output
open(OUTFILE, ">$opt_o") or die "Unable to open output script: $opt_o\n";

print OUTFILE $header;
while ( my ($key, $value) = each %model) {
    if ($key ne '') {
	print OUTFILE "$key\t$value\n";
    }
}
close OUTFILE;

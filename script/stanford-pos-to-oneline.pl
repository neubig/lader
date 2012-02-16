#!/usr/bin/perl

$/ = "\n\n";
while(<STDIN>) {
    chomp;
    s/\S*\///g;
    print "$_\n";
}

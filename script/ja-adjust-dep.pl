#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";


sub readtree {
    $_ = shift;
    # Split and remove the leading ID
    my @lines = split(/\n/);
    shift @lines if $lines[0] =~ /^ID/;
    my @ret = map { my @arr = split(/ /); $arr[0]--; $arr[1]--; \@arr } @lines;
    # Find all values that are a head
    my @ishead = map { 0 } @ret;
    for(@ret) { $ishead[$_->[1]]++ if $_->[1] >= 0; }
    # For verb phrases that look like
    #   A -> B -> C
    # reverse them so that they look like
    #   A <- B <- C
    # as long as there are no incoming dependencies
    for(my $i = $#ret; $i > 0; $i--) {
        next if ($ishead[$i] != 1) or 
                ($ret[$i]->[3] !~ /^(動詞|助動詞|語尾)$/) or
                ($ret[$i-1]->[3] !~ /^(動詞|助動詞|語尾)$/) or
                ($ret[$i-1]->[1] != $i);
        $ret[$i-1]->[1] = $ret[$i]->[1];
        $ret[$i]->[1] = $i-1;
    }
    return @ret;
}

$/ = "\n\n";
while(<STDIN>) {
    chomp;
    my @deptree = readtree($_);
    print join("\n", map { sprintf("%03d %03d %s %s %s", $_->[0]+1, $_->[1]+1, $_->[2], $_->[3], $_->[4]) } @deptree )."\n\n";
}

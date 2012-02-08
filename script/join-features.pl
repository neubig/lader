#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
use FileHandle;
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

if(@ARGV < 1) {
    print STDERR "Usage: join-features.pl [FILE1] [FILE2] ...\n";
    exit 1;
}

my @filehandles;
for (@ARGV) {
    push @filehandles, FileHandle->new($_, "<:utf8");
}

while(1) {
    my $len = 0;
    my @vals = map { my $str = <$_>; chomp $str; $len = max($len,length($str)); $str } @filehandles;
    last if not $len;
    print join("\t", @vals)."\n";
}

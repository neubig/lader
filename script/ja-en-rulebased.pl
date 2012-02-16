#!/usr/bin/perl

use strict;
use utf8;
use List::Util qw(sum min max shuffle);
binmode STDIN, ":utf8";
binmode STDOUT, ":utf8";
binmode STDERR, ":utf8";

if(@ARGV != 0) {
    print STDERR "Usage: ja-en-rulebased.pl < TREES > OUT";
    exit 1;
}

my $SUBJ = -50;
my $HEAD = 0;
my $SUFF = 1;
my $DOBJ = 50;
my $IOBJ = 70;
my $OTHER = 100;

sub gather_left_children {
    my ($tree, $root) = @_;
    my @children;
    foreach my $i (0 .. $root-1) {
        push @children, $i if($tree->[$i]->[1] == $root);
    }
    return @children;
}

sub gather_children {
    my ($tree, $root) = @_;
    my @children;
    foreach my $i (0 .. @$tree-1) {
        push @children, $i if($tree->[$i]->[1] == $root);
    }
    return @children;
}

sub get_actual_word {
    my ($tree, $root) = @_;
    if($root == 0) { return $tree->[$root]; }
    if($tree->[$root]->[2] =~ /^、$/ and $tree->[$root-1]->[1] == $root) { return $tree->[$root-1]; }
    return $tree->[$root];
}

sub restructure {
    my ($tree, $root) = @_;
    my @children = gather_children($tree, $root);
    if(@children) {
        my $head = $tree->[$root]->[1];
        my $contains_rootminus = 0;
        for(@children) { ($contains_rootminus = 1) if $_ == $root-1; }
        print "root=$root, head=$head, contains=$contains_rootminus, children=@children\n";
        if(
            # For auxiliaries and suffixes, make the previous word the head
            (($tree->[$root]->[3] =~ /^(助動詞|語尾)$/) and ($tree->[$root-1]->[3] =~ /^動詞$/) and $contains_rootminus) or
            # For さ変名詞, make the previous word the head
            (($tree->[$root]->[2] =~ /^(し|さ|する|せ)$/) and ($tree->[$root-1]->[3] =~ /^名詞$/) and $contains_rootminus)
        ) {
            print "RESTRUCT\n";
            for(@children, $root) { $tree->[$_]->[1] = $root-1; }
            $tree->[$root-1]->[1] = $head;
            $tree->[$root-1]->[3] = "動詞";
        }
        for(@children) {
            restructure($tree, $_);
        }
    }
}

sub reorder {
    my ($tree, $root) = @_;
    my @children = gather_children($tree, $root);
    # Get the scores
    my @score;
    foreach my $i (@children, $root) {
        my $score = $OTHER;
        my $actual_word = get_actual_word($tree,$i);
        if($tree->[$root]->[3] =~ /^動詞$/) {
            if($i == $root) { $score = $HEAD; }
            elsif($actual_word->[3] =~ /^(語尾|助動詞)$/) { $score = $SUFF; }
            elsif($actual_word->[3] =~ /^助詞$/) {
                if($actual_word->[2] =~ /^を$/    ) { $score = $DOBJ; }
                elsif($actual_word->[2] =~ /^[がは]$/) { $score = $SUBJ; }
                elsif($actual_word->[2] =~ /^[にへ]$/) { $score = $IOBJ; }
            }
        }
        print "child=$i, root=$root, score=$score: i={ " .join(" ", @{$tree->[$i]})." }, act={ " .join(" ", @$actual_word)." }, head={ ".join(" ", @{$tree->[$root]})." }\n";
        push @score, [$score, $i];
    }
    # Sort by score and print
    @score = sort { my $ret = ($a->[0] <=> $b->[0]); $ret = ($a->[1] <=> $b->[1]) if not $ret; $ret } @score;
    my @ret;
    foreach my $i (@score) {
        if($i->[1] == $root) {
            push @ret, $root;
        } else {
            push @ret, reorder($tree, $i->[1]);
        }
    }
    return @ret;
}

$/ = "\n\n";
while(<STDIN>) {
    chomp;
    my @tree = map { my @arr = split(/ /); $arr[0]--; $arr[1]--; \@arr } split(/\n/);
    shift @tree;
    restructure(\@tree, $#tree);
    my @order = reorder(\@tree, $#tree);
    print "@order\t".join(" ", map { $tree[$_]->[2] } @order)."\n".join(" ", map { $_->[2] } @tree)."\n";
}

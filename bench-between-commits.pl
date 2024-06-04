#!/usr/bin/env perl
use v5.20;
use utf8;
use warnings;
use feature 'signatures';
use open qw(:std :utf8);

my @benches = (
    [ 'a27f963', '7ed318b', 'phase2', 'Ofast' ],
    [ '7ed318b', 'ed93992', 'phase2', 'openmp' ],
    [ 'ed93992', '4d7f8a9', 'phase2', 'SIMD' ],
    [ '4d7f8a9', '9b74ed9', 'phase1', 'merge sort' ],
    [ '9b74ed9', 'ad27be6', 'phase1', 'parallize' ],
    [ 'ad27be6', 'e92c2dd', 'phase1', 'const' ],
);

for (@benches) {
    my ($before, $after, $phase, $desc) = @$_;
    system 'rm -f *.out';
    system qw/git checkout/, $before;
    system qw/make clean/;
    system qw/make -j/;
    rename 'main', 'before.out';
    system qw/git checkout/, $after;
    system qw/make clean/;
    system qw/make -j/;
    rename 'main', 'after.out';
    my $env = $phase eq 'phase2' ? 'HEP_SKIP_PHASE1' : 'HEP_SKIP_PHASE2';
    system "echo '##' $desc | tee -a bench.md" ;
    system "env HEP_SKIP_BASELINE=1 HEP_DATA_SIZE=99999999 $env=1 hyperfine --export-markdown - ./before.out ./after.out | tee -a bench.md";
}

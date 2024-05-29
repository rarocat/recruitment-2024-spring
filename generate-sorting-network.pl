#!/usr/bin/env perl
use v5.20;
use utf8;
use warnings;
use feature 'signatures';
use open qw(:std :utf8);

my @network = (
    [[0,5],[1,4],[2,12],[3,13],[6,7],[8,9],[10,15],[11,14]],
    [[0,2],[1,10],[3,6],[4,7],[5,14],[8,11],[9,12],[13,15]],
    [[0,8],[1,3],[2,11],[4,13],[5,9],[6,10],[7,15],[12,14]],
    [[0,1],[2,4],[3,8],[5,6],[7,12],[9,10],[11,13],[14,15]],
    [[1,3],[2,5],[4,8],[6,9],[7,11],[10,13],[12,14]],
    [[1,2],[3,5],[4,11],[6,8],[7,9],[10,12],[13,14]],
    [[2,3],[4,5],[6,7],[8,9],[10,11],[12,13]],
    [[4,6],[5,7],[8,10],[9,11]],
    [[3,4],[5,6],[7,8],[9,10],[11,12]],
);

# my @network = (
#     [[0,2],[1,3],[4,6],[5,7]],
#     [[0,4],[1,5],[2,6],[3,7]],
#     [[0,1],[2,3],[4,5],[6,7]],
#     [[2,4],[3,5]],
#     [[1,4],[3,6]],
#     [[1,2],[3,4],[5,6]],
# );

use Data::Dumper;

print <<EOF;
    __m512 input = _mm512_loadu_ps(data);
EOF

for (@network) {
    my @layer = @$_;
    my @bigger = map { $_->[1] } @layer;
    my $bigger = 0;
    for (@bigger) {
        $bigger |= 1 << $_;
    }
    my $bigger_b = sprintf "%016b", $bigger;
    $bigger = sprintf "%04X", $bigger;

    my %position = (
        (map { $_ => $_ } (0 .. 15)),
        (map { ($_->[0] => $_->[1]), ($_->[1] => $_->[0]) } @layer),
    );
    my @permute = map { $position{$_} } 0 .. 15;
    my $permute = join ", ", reverse @permute;
    print <<EOF;
    {
        const __m512i idx = _mm512_set_epi32($permute);
        const __m512 shuffled = _mm512_permutexvar_ps(idx, input);
        const __m512 min = _mm512_min_ps(shuffled, input);
        const __m512 max = _mm512_max_ps(shuffled, input);
        input = _mm512_mask_mov_ps(min, 0x$bigger /* $bigger_b */, max);
    }
EOF
}

print <<EOF;
    _mm512_storeu_ps(data, input);
EOF

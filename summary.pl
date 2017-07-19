#!/usr/bin/perl

use List::Util qw(sum);

use warnings;
use strict;

our @bndstx = (0, 0, 0);
our @bndldx = (0, 0, 0);

while (<>) {
	# SUMMARY: bndstx[0+0=>0], bndldx[3+0=>0], wraps[0] (net/xfrm/xfrm_sysctl.c)
	if (m/^SUMMARY: \s+ bndstx \[ (\d+) \+ (\d+) => (\d+) \],\s+ bndldx \[ (\d+) \+ (\d+) => (\d+) \],\s+ .*/x) {
	# if (m/^SUMMARY: \s* bndstx \[ (\d+) \+ (\d+) => (\d+) \] , \s* \[/x) {
		print $_;
		$bndstx[0] += $1;
		$bndstx[1] += $2;
		$bndstx[2] += $3;
		$bndldx[0] += $4;
		$bndldx[1] += $5;
		$bndldx[2] += $6;
	}
}

print "TOTAL: bndstx[$bndstx[0]+$bndstx[1]=>$bndstx[2]], bndldx[$bndldx[0]+$bndldx[1]=>$bndldx[2]]\n";
printf("       bndstx = %d, bndldx = %d\n", sum(@bndstx), sum(@bndldx));

#!/usr/bin/perl

my $type = $ARGV[1];

while (<>) {
	chomp;
	if (m/
		\[\s*\d+\.\d+\]\s*
		memcpy\(size=(\d+)\)=>
		\(
		legacy=(\d+)\(\d+\),
		got_bounds=(\d+)\(\d+\),
		load_bounds=(\d+)\(\d+\)
		\)
		/x) {

		print "DATE,COMMIT,type\n";
	}
}



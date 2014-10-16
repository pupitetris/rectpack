#!/usr/bin/perl

##
## Copyright (C) 2014 Arturo Espinosa Aldama <arturoea@gmail.com>
##  
## This program is free software: you can redistribute it and/or
## modify it under the terms of the GNU General Public License as
## published by the Free Software Foundation, either version 3 of the
## License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see
## <http://www.gnu.org/licenses/>.
##

# This program takes input and generates output just as the Vistaprint 
# Techchallenge requires. (http://lifeinvistaprint.com/techchallenge/)

# The purpose of this launcher is to minimize the impact on the way
# rectpack handles input and output while complying with the challenge's
# requirements.

# Configuration
my $DIMS = 2; # 2 - rectangles, 3 - cuboids.
my $THREADS = 4; # Note: threads are ignored if running on Cygwin.
my $DEBUG = 0; # Copies rectpack output to stderr for analysis.

my $CYGWIN = ($^O eq 'cygwin');
my $INPUT_FILE = $ARGV[0]; # Optional. By default, STDIN is consumed.

use strict;
use File::Basename qw(dirname);
use File::Spec::Functions qw(catfile);
use List::Util qw(max);

# Techchallenge: If the input is empty or invalid, your program should print
# Error on a line by itself and exit.
sub go_die {
    print "Error\n";
    exit 1;
}

sub decimal_places {
    my $num = shift;
    $num =~ s/0+$//;
    my $len = length ((split (/\./, $num))[1]);
    return ($len eq undef)? 0: $len;
}

sub process_input {
    # Techchallenge:

    # The program input consists of N products to be packed together
    # into the same shipping box. The number N is always between 1 and
    # 100 inclusive. Each product is a rectangular cuboid as mentioned
    # earlier. Each of its dimensions (length, width, height) is a real
    # number between 1 and 50 inclusive.

    # Each product is represented by a line of text, meaning the input
    # is N lines long. Each line has four values in order. The first is
    # an integer representing an product ID that is unique in the
    # input. It is followed by three real numbers representing the
    # length, width, and height of the product. All values are separated
    # by spaces or tabs.

    my $fd = shift;

    # A very flexible parser:
    my $power = 0;
    my $id_len = 0;
    my $items = [];
    my $itemhash = {};
    while (my $line = <$fd>) {
	if ($line !~ /^\s*(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/) {
	    go_die ();
	}
	my ($id, $length, $width, $height) = ($1, $2 + 0, $3 + 0, $4 + 0);

	# Canonize geometries by sorting dimensions.
	my @dims = sort { $a <=> $b } ($length, $width, $height);

	my $item = {
	    'ID' => $id,
	    'DIMS' => \@dims
	};
	push @$items, $item;

	# Create canonical key based on sorted dimensions.
	my $key = join ('x', @dims[0 .. $DIMS - 1]);

	# The itemhash is a hash of arrays, since more than one item may
	# have the same geometry.
	my $i = $itemhash->{$key};
	if (!$i) {
	    $itemhash->{$key} = [$item];
	} else {
	    push @{$i}, $item;
	}

	# power will help removing decimals in dimensions so the packer runs on integer dimensions only.
	$power = max ($power, decimal_places ($length), decimal_places ($width), decimal_places ($height));

	# id_len is to apply padding on id's when sorting, thus supporting arbitrary strings for IDs.
	$id_len = max ($id_len, length ($id));
    }

    $power = 10 ** $power;

    return ($items, $itemhash, $power, $id_len);
}

# We found the lines where the optimal geometry for the rectangles is defined.
# Parse it and assign it to the items that correspond to it, by canonical geometry.
sub capture_result {
    my $pipe = shift;
    my $itemhash = shift;
    my $power = shift;

    # Example of text processed:
    # 950x1200=(0,0)       950x1200=(950,0)     630x1500=(0,1200)
    # 630x1500=(630,1200)   600x430=(1260,1200)  600x430=(1260,1630)
    #  500x300=(1260,2060)  500x300=(1260,2360)
    while (my $l = <$pipe>) {
	print STDERR $l if $DEBUG;
	chomp $l;
	$l =~ tr/ \t//d;

	# An empty line represents the end of the dump.
	last if $l eq '';

	my @specs = split (/\)/, $l);
	foreach my $spec (@specs) {
	    my ($dims, $coord) = split (/=\(/, $spec);

	    # Here we divide by the power of ten to recover decimal places.
	    my @newdims = map { $_ /= $power } split (/x/, $dims);
	    my @pos = map { $_ /= $power } split (/,/, $coord);

	    # Rebuild the canonical key
	    my $key = join ('x', sort { $a <=> $b } @newdims);

	    # Finally find the item and set its new properties.
	    my $item = shift @{$itemhash->{$key}};
	    $item->{'POS'} = \@pos;
	    $item->{'NEWDIMS'} = \@newdims;
	}
    }
}

sub process_output {
    my $items = shift;
    my $id_len = shift;
    my $box = shift;

    # Techchallenge: 
    # 1- The length, width, and height of the smallest shipping box
    # that you could find to fit the items.  By "smallest," we mean a
    # box with the minimum interior surface area when closed. The
    # three numbers should be printed in order, on a single line of
    # text, separated by one space character. Real numbers should be
    # printed with two decimal places of accuracy.

    print join (' ', map { sprintf ("%0.2f", $_) } @$box) . "\n";

    # Techchallenge: 
    # 2- N lines, one for each of the N products, in increasing order
    # by product ID.  The line consists of the product ID followed by
    # the (x,y,z) coordinates for the product's eight vertices. The
    # origin of the coordinate system (0,0,0) is one bottom, interior
    # corner of the shipping box. Use one space character as a
    # separator

    # Sort items by ID, using padding to allow for correct numeric and string ID comparisons.
    foreach my $item (sort { sprintf ('%*s', $id_len, $a->{'ID'}) cmp sprintf ('%*s', $id_len, $b->{'ID'}) } @$items) {
	print $item->{'ID'} . ' ';

	my $max = 2 ** $DIMS;
	for (my $i = 0; $i < $max; $i++) {
	    print '(';
	    for (my $d = 0; $d < $DIMS; $d++) {
		if (($i / (2 ** $d)) % 2) {
		    print $item->{'POS'}->[$d] + $item->{'NEWDIMS'}->[$d];
		} else {
		    print $item->{'POS'}->[$d];
		}
		print ', ' if $d < $DIMS - 1;
	    }
	    print ')';
	    print ' ' if $i < $max - 1;
	}
	print "\n";
    }
}

# Converts items dimensions to the format required by rectpack cmd line parser.
sub items_to_arg {
    my $items = shift;
    my $power = shift;
    
    my $arg = '';

    foreach my $i (@$items) {
	my @dims = @{$i->{'DIMS'}}[0 .. $DIMS - 1];

	# rectpack handles integer dimensions better, so we move the
	# decimal point by the max number of decimal places found
	# among dimensions.
	$arg .= join ('x', map { $_ *= $power } @dims) . ',';
    }
    chop $arg;
    
    return $arg;
}

sub find_binary {
    my $profile = shift;
    my $binfile = shift;

    $binfile .= '.exe' if $CYGWIN;

    my $bin = catfile (dirname ($0), $profile, $binfile);
    return '' if ! -x $bin;
    return $bin;
}

sub main {
    my $threads = ($CYGWIN)? 1: 4; # Multi-thread not working under Cygwin.

    my $rectpack = find_binary ('release', 'rectpack');
    if (!$rectpack) {
	$rectpack = find_binary ('debug', 'rectpack');
	if (!$rectpack) {
	    die "Error: Couldn't find rectpack binary.";
	}
    }

    my $fd;
    if ($INPUT_FILE) {
	open ($fd, $INPUT_FILE);
    } else {
	$fd = *STDIN; # The default, as required by the Techchallenge.
    }

    my ($items, $itemhash, $power, $id_len) = process_input ($fd);
    close $fd;

    my $items_arg = items_to_arg ($items, $power);
    my $cmd = "$rectpack --benchmark 0 --instance $items_arg --verbose --threads $threads --unoriented --weakening 5";

    my @box;
    open (my $pipe, "$cmd|") or die "Error: Can't run rectpack command:\n$cmd\n$!";

    # Consume the output data we need from the verbose output:
    while (my $l = <$pipe>) {
	print STDERR $l if $DEBUG;
	chomp $l;
	if ($l =~ /Rectangles were placed/) {
	    # The geometry of the optimum placements:
	    capture_result ($pipe, $itemhash, $power);
	} elsif ($l =~ /Optimal Solutions : (.*)/) {
	    # The dimensions of the smallest shipping box:
	    @box = map { $_ /= $power } split (/x/, $1);
	}
    }
    close $fd;

    process_output ($items, $id_len, \@box);
}

main ();

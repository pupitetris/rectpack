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
my $THREADS = 4; # Note: threads are ignored under Cygwin.
my $VERBOSE = 0; # 0 - quiet, 1 - normal, 2 - verbose

my $CYGWIN = ($^O eq 'cygwin');
my $INPUT_FILE = $ARGV[0];

use strict;
use File::Basename qw(dirname);
use File::Spec::Functions qw(catfile);
use List::Util qw(max);

sub get_decimals {
    my $num = shift;
    $num =~ s/0+$//;
    my $len = length ((split ('\.', $num))[1]);
    return ($len eq undef)? 0: $len;
}

# Techchallenge: If the input is empty or invalid, your program should print
# Error on a line by itself and exit.
sub go_die {
    print "Error\n";
    exit 1;
}

sub process_input {
    # Techchallenge:

    # The program input consists of N products to be packed together
    # into the same shipping box. The number N is always between 1 and
    # 100 inclusive. Each product is a rectangular cuboid as mentioned
    # earlier. Each of its dimensions (length, width, height) is a real
    # number between 1 and 50 inclusive. When we test your program, the
    # number and dimensions of products will all be generated randomly.  

    # Each product is represented by a line of text, meaning the input
    # is N lines long. Each line has four values in order. The first is
    # an integer representing an product ID that is unique in the
    # input. It is followed by three real numbers representing the
    # length, width, and height of the product. All values are separated
    # by spaces or tabs.

    my $fd = shift;

    # A very flexible parser:
    my $power = 0;
    my $cuboids = [];
    while (my $line = <$fd>) {
	if ($line !~ /^\s*(\S+)\s+(\S+)\s+(\S+)\s+(\S+)/) {
	    go_die ();
	}
	my ($id, $length, $width, $height) = ($1, $2 + 0, $3 + 0, $4 + 0);
	push @$cuboids, [$id, $length, $width, $height];
	$power = max ($power, get_decimals ($length), get_decimals ($width), get_decimals ($height));
    }

    return ($cuboids, $power);
}

sub cuboids_to_arg {
    my $cuboids = shift;
    my $power = shift;
    
    my $arg = '';

    if ($power == 1) {
	foreach my $c (@$cuboids) {
	    $arg .= join ('x', $c->[1], $c->[2]) . ',';
	}
    } else {
	# rectpack only handles integer dimensions.
	my $power10 = 10 ** $power;
	foreach my $c (@$cuboids) {
	    $arg .= join ('x', $c->[1] * $power10, $c->[2] * $power10) . ',';
	}
    }
    chop $arg;
    
    return $arg;
}

sub find_bin {
    my $profile = shift;
    my $binfile = shift;

    $binfile .= '.exe' if $CYGWIN;

    my $bin = catfile (dirname ($0), $profile, $binfile);
    return '' if ! -x $bin;
    return $bin;
}

sub main {
    my $threads = ($CYGWIN)? 1: 4; # Multi-thread not working under Cygwin.
    my $verbose = ($VERBOSE == 1)? '': (($VERBOSE == 2)? '--verbose': '--quiet');

    my $rectpack = find_bin ('release', 'rectpack');
    if (!$rectpack) {
	$rectpack = find_bin ('debug', 'rectpack');
	if (!$rectpack) {
	    die "Error: Couldn't find rectpack binary.";
	}
    }

    my $fd;
    if ($INPUT_FILE) {
	open ($fd, $INPUT_FILE);
    } else {
	$fd = *STDIN;
    }

    my ($cuboids, $power) = process_input ($fd);
    my $cuboids_arg = cuboids_to_arg ($cuboids, $power);

    my $cmd = "$rectpack --benchmark 0 --instance $cuboids_arg $verbose --threads $threads --unoriented -w 5";
    open (my $pipe, "$cmd|") or die "Error: Can't run retpack command:\n$cmd\n$!";
    while (my $l = <$pipe>) {
	print $l;
    }
    close $fd;
}

main ();

rectpack
========

Optimal rectangle packing

This code implements Huang and Korf's (2008, 2009, 2010, 2011) optimal rectangle packer. It also implements Korf's (2003, 2004) and Korf et al.'s (2009) optimal rectangle packer.

The software handles rectangles rotated to be aligned to the x and y axes, handles both oriented and unoriented rectangles, rectangles of high-precision dimensions, and finds the bounding box of smallest area that can contain the input rectangles without overlap.

Dependencies
------------

* Boost program options, boost graph library, boost spirit v2, boost thread
* GNU Multi-Precision library (gmp), and C++ wrapper (gmpxx)

Reference
---------

* 2003: [Optimal Rectangle Packing: Initial Results](http://www.aaai.org/Papers/ICAPS/2003/ICAPS03-029.pdf)
* 2004: [Optimal Rectangle Packing: New Results](http://www.aaai.org/Papers/ICAPS/2004/ICAPS04-019.pdf)
* 2009: [New Improvements in Optimal Rectangle Packing](http://ijcai.org/papers09/Papers/IJCAI09-092.pdf)
* 2010: [Optimal Rectangle Packing on Non-Square Benchmarks](http://www.aaai.org/ocs/index.php/AAAI/AAAI10/paper/view/1678/1933)
* 2011: [Optimal Packing of High-Precision Rectangles](http://www.aaai.org/ocs/index.php/AAAI/AAAI11/paper/view/3581/3820)
* 2012: [Optimal Rectangle Packing: An Absolute Placement Approach](http://www.jair.org/media/3735/live-3735-6794-jair.pdf)
* [Richard E. Korf homepage](http://www.cs.ucla.edu/~korf/)
* [Eric Huang homepage](http://www.erichuang.net/)

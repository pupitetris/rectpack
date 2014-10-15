/**
 * Copyright (C) 2011 Eric Huang
 *
 * This file is part of rectpack.
 *
 * rectpack is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * rectpack is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with rectpack. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Parameters.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>

/**
 * Spirit classic.
 */

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_push_back_actor.hpp>

/**
 * Spirit v2.
 */

#include <boost/bind.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_eps.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>

Parameters::Parameters() {
  m_nRandom = 0;
  m_bKorf = false;
  m_bScheduling = false;
  m_nWeakening = 0;
  m_nThreads = 0;
  m_nTrials = 0;
  m_nSubsetSize = 0;
  m_nRangeSize = 0;
  m_bFitScreen = false;
  m_bVerbose = false;
  m_nArgc = 0;
  m_pArgv = NULL;
  m_nJValue = -2.0;
  m_nGeometryWidth = 400;
  m_nGeometryHeight = 250;
  m_sYScale = "0.55";
  m_nYScale.initialize(55, 100);
  m_bQuiet = false;
  initBox();
}

Parameters::Parameters(const Parameters& p) :
  m_nRandom(p.m_nRandom),
  m_sBox(p.m_sBox),
  m_Box(p.m_Box),
  m_nNthBox(p.m_nNthBox),
  m_nMthBox(p.m_nMthBox),
  m_bSingleRun(p.m_bSingleRun),
  m_bKorf(p.m_bKorf),
  m_bScheduling(p.m_bScheduling),
  m_nWeakening(p.m_nWeakening),
  m_nThreads(p.m_nThreads),
  m_sCra(p.m_sCra),
  m_sExtend(p.m_sExtend),
  m_Log(p.m_Log),
  m_sRegression(p.m_sRegression),
  m_nTrials(p.m_nTrials),
  m_nSubsetSize(p.m_nSubsetSize),
  m_nRangeSize(p.m_nRangeSize),
  m_vInstance(p.m_vInstance),
  m_vUnscaled(p.m_vUnscaled),
  m_bFitScreen(p.m_bFitScreen),
  m_sFix(p.m_sFix),
  m_vFixed(p.m_vFixed), 
  m_sCommand(p.m_sCommand),
  m_bVerbose(p.m_bVerbose),
  m_nOrdering(p.m_nOrdering),
  m_s1(p.m_s1),
  m_s2(p.m_s2),
  m_nArgc(p.m_nArgc),
  m_pArgv(p.m_pArgv),
  m_nJValue(p.m_nJValue),
  m_sGeometry(p.m_sGeometry),
  m_nGeometryWidth(p.m_nGeometryWidth),
  m_nGeometryHeight(p.m_nGeometryHeight),
  m_sYScale(p.m_sYScale),
  m_nYScale(p.m_nYScale),
  m_bQuiet(p.m_bQuiet) {
}

const Parameters& Parameters::operator=(const Parameters& p) {
  m_nRandom = p.m_nRandom;
  m_sBox = p.m_sBox;
  m_Box = p.m_Box;
  m_nNthBox = p.m_nNthBox;
  m_nMthBox = p.m_nMthBox;
  m_bSingleRun = p.m_bSingleRun;
  m_bKorf = p.m_bKorf;
  m_bScheduling = p.m_bScheduling;
  m_nWeakening = p.m_nWeakening;
  m_nThreads = p.m_nThreads;
  m_sCra = p.m_sCra;
  m_sExtend = p.m_sExtend;
  m_Log = p.m_Log;
  m_sRegression = p.m_sRegression;
  m_nTrials = p.m_nTrials;
  m_nSubsetSize = p.m_nSubsetSize;
  m_nRangeSize = p.m_nRangeSize;
  m_vInstance = p.m_vInstance;
  m_vUnscaled = p.m_vUnscaled;
  m_bFitScreen = p.m_bFitScreen;
  m_sFix = p.m_sFix;
  m_vFixed = p.m_vFixed; 
  m_sCommand = p.m_sCommand;
  m_bVerbose = p.m_bVerbose;
  m_nOrdering = p.m_nOrdering;
  m_s1 = p.m_s1;
  m_s2 = p.m_s2;
  m_nArgc = p.m_nArgc;
  m_pArgv = p.m_pArgv;
  m_nJValue = p.m_nJValue;
  m_sGeometry = p.m_sGeometry;
  m_nGeometryWidth = p.m_nGeometryWidth;
  m_nGeometryHeight = p.m_nGeometryHeight;
  m_sYScale = p.m_sYScale;
  m_nYScale = p.m_nYScale;
  m_bQuiet = p.m_bQuiet;
  return(*this);
}

Parameters::~Parameters() {
}

int Parameters::parseIJCAI(int argc, char** argv) {
  saveCommandLine(argc, argv);
  m_vInstance.m_nBenchmark = 3;
  m_vInstance.m_sInstance = "1";
  if(argc == 2)
    m_vInstance.m_sInstance = argv[1];
  else return(-1);
  m_nJValue = -2.0;
  m_nOrdering = 8;
  m_nRandom = 1;
  m_nThreads = 1;
  m_nWeakening = 2;
  m_bKorf = false;
  m_bScheduling = false;
  m_bFitScreen = false;
  m_bVerbose = false;
  m_vInstance.m_bUnoriented = false;
  m_vInstance.parseInstance(m_vInstance.m_sInstance);

  /**
   * @TODO This is just a quick patch, because our previous packers
   * preferred deciding the y-coordinate prior to the
   * x-coordinates. Therefore, the best automatic ordering for them is
   * by height and not width.
   */

  if(m_nOrdering == 8 && m_nWeakening < 3) {
    if(m_vInstance.m_bUnoriented)
      m_vInstance.sortDecreasingArea();
    else
      m_vInstance.sortDecreasingHeight();
  }
  else
    m_vInstance.sort(m_nOrdering);
  m_vInstance.inferInstanceProperties();
  m_vUnscaled = m_vInstance;
  return(0);
}

void Parameters::saveCommandLine(int argc, char** argv) {
  m_nArgc = argc;
  m_pArgv = argv;
  m_sCommand = argc > 1 ? argv[1] : "";
  for(int i = 2; i < argc; ++i) {
    m_sCommand += ' ';
    m_sCommand += argv[i];
  }
}

int Parameters::parseCommand(int argc, char** argv) {

  /**
   * Save the invocation parameters.
   */
  
  saveCommandLine(argc, argv);

  /**
   * Instantiate the description.
   */

  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  getDescription(desc);

  /**
   * Parse the command line.
   */

  po::variables_map vm;
  po::store(po::parse_command_line(argc, argv, desc), vm);
  po::notify(vm);
  parseAux(desc, vm);
  return(0);
}

int Parameters::parseCommand(std::vector<std::string>& vc) {

  /**
   * Save the invocation parameters.
   */
  
  m_sCommand = vc[0];
  for(size_t i = 1; i < vc.size(); ++i)
    m_sCommand += " " + vc[i];
  m_nArgc = 0;
  m_pArgv = NULL;

  /**
   * Instantiate the description.
   */

  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  getDescription(desc);

  /**
   * Parse the command line.
   */

  po::variables_map vm;
  po::store(po::command_line_parser(vc).options(desc).run(), vm);
  po::notify(vm);
  parseAux(desc, vm);
  return(0);
}

void Parameters::initBox() {

  /**m
   * Set the initial values, which is the widest allowable range.
   */
  
  m_nNthBox = 0;
  m_nMthBox = std::numeric_limits<size_t>::max();
  m_Box.clear();
  m_bSingleRun = false;
}

void Parameters::parseGeometry(const std::string& s) {
  if(s.empty()) return;

  /**
   * Define the grammar for parsing the box specifications.
   */

  bool bParsed;
  using namespace boost::spirit::classic;

  /**
   * Define some range.
   */

  size_t w, h;
  bParsed = parse(s.c_str(),
		  uint_p[assign_a(w)] >>
		  ch_p('x') >>
		  uint_p[assign_a(h)]).full;
  if(bParsed) return;

  std::cout << "I don't understand your geometry specification: "
	    << s << std::endl;
  exit(0);
}

void Parameters::parseBox(const std::string& s) {
  if(s.empty()) return;
  
  size_t t = s.find_first_of(':');
  if(t < s.size()) {
    std::string n(s.substr(0, t));
    std::string m(s.substr(t + 1));
    m_nNthBox = boost::lexical_cast<size_t>(n);
    m_nMthBox = boost::lexical_cast<size_t>(m);
    m_bSingleRun = false;
  }
  else {
    m_Box.load(s);
    m_bSingleRun = true;
  }
}

void Parameters::parseFix(const std::string& s) {
  if(s.empty()) return;
  using namespace boost::spirit::classic;
  Fixed f;
  rule<> wxh = (uint_p[assign_a(f.m_nWidth)] >>
		ch_p('x') >>
		uint_p[assign_a(f.m_nHeight)] >>
		ch_p('=') >> ch_p('(') >>
		uint_p[assign_a(f.m_nX)] >> ch_p(',') >>
		uint_p[assign_a(f.m_nY)] >> ch_p(')'))
    [push_back_a(m_vFixed, f)];
  bool bParsed = parse(s.c_str(), wxh >> *(ch_p(',') >> wxh)).full;
  if(!bParsed) {
    std::cout << "I don't understand your fixed items specification: "
	      << s << std::endl;
    exit(0);
  }
}

void Parameters::parseRegression(const std::string& s) {
  if(s.empty()) return;
  typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;
  boost::char_separator<char> sep(",");
  tokenizer tokens(s, sep);
  tokenizer::const_iterator i = tokens.begin();
  m_nTrials = boost::lexical_cast<size_t>(*i); ++i;
  m_nSubsetSize = boost::lexical_cast<size_t>(*i); ++i;
  m_nRangeSize = boost::lexical_cast<size_t>(*i);
}

void Parameters::rerollRegression() {
  m_vInstance = m_vUnscaled;
  std::deque<UInt> p;
  if(m_sRegression.find_first_of('d') < m_sRegression.size()) {
    p.resize(m_nSubsetSize);
    for(size_t i = 0; i < m_nSubsetSize; ++i)
      p[i] = (random() % m_nRangeSize) + 1;
  }
  else {
    p.resize(m_nRangeSize);
    for(size_t i = 0; i < m_nRangeSize; ++i)
      p[i] = i + 1;
    std::random_shuffle(p.begin(), p.end());
    p.resize(m_nSubsetSize);
  }
  m_vInstance.parseInstance(p);
  if(m_nOrdering == 8 && m_nWeakening < 3) {
    if(m_vInstance.m_bUnoriented)
      m_vInstance.sortDecreasingArea();
    else
      m_vInstance.sortDecreasingHeight();
  }
  else
    m_vInstance.sort(m_nOrdering);
  m_vInstance.inferInstanceProperties();
  m_vUnscaled = m_vInstance;
}

void Parameters::print() const {
  std::cout
    << "Command line = " << m_sCommand << std::endl
    << "Random seed = " << m_nRandom << std::endl
    << "Box specification = " << m_sBox << std::endl
    << "Box range = [" << m_nNthBox << ", " << m_nMthBox << "]" << std::endl
    << "Algorithm = ";
  if(m_bKorf) std::cout << "Korf's";
  else std::cout << m_nWeakening;
  if(m_bScheduling)
    std::cout << " for cumulative scheduling";
  std::cout << std::endl;
  if(m_nThreads > 1)
    std::cout << "Threads = " << m_nThreads << std::endl;
  if(!m_sCra.empty())
    std::cout << "Contiguous resource solution file = " << m_sCra << std::endl;
  if(!m_sExtend.empty())
    std::cout << "Extension file = " << m_sExtend << std::endl;
  if(!m_sRegression.empty()) {
    std::cout
      << "Regression specification = " << m_sRegression << std::endl
      << "Regression trials, subset size, and range size = "
      << m_nTrials << ", "
      << m_nSubsetSize << ", "
      << m_nRangeSize << std::endl;
  }
  std::cout
    << "Instance specification = " << m_vInstance.m_sInstance << std::endl
    << "Instance = ";
  m_vInstance.print();
  std::cout << std::endl;
  std::cout
    << "Scale original:current = " << m_vInstance.m_nScale << std::endl
    << "Total area = " << m_vInstance.m_nTotalArea << std::endl
    << "Smallest rectangular area, width, height, and dimension = "
    << m_vInstance.m_nMinArea << ", "
    << m_vInstance.m_nMin.m_nWidth << ", "
    << m_vInstance.m_nMin.m_nHeight << ", "
    << m_vInstance.m_nMinDim << std::endl
    << "Largest rectangular area, width, height, and dimension = "
    << m_vInstance.m_nMaxArea << ", "
    << m_vInstance.m_nMax.m_nWidth << ", "
    << m_vInstance.m_nMax.m_nHeight << ", "
    << m_vInstance.m_nMaxDim << std::endl
    << "Stacked height and width of rectangles = "
    << m_vInstance.m_nStacked.m_nHeight << ", "
    << m_vInstance.m_nStacked.m_nWidth << std::endl
    << "Benchmark = " << m_vInstance.m_nBenchmark << std::endl
    << "Rectangles "
    << (m_vInstance.m_bSequential ? "are" : "aren't") << " sequential, "
    << (m_vInstance.m_bUnique ? "are" : "aren't") << " unique, "
    << (m_vInstance.m_bAllSquares ? "are" : "aren't") << " all squares, "
    << "are " << (m_vInstance.m_bUnoriented ? "unoriented" : "oriented") << ", "
    << (m_vInstance.m_bDiagonalSymmetry ? "are" : "aren't") << " diagonally symmetric." << std::endl;
  if(!m_vFixed.empty()) {
    std::cout
      << "Fixed specification = " << m_sFix << std::endl
      << "Fixed rectangles = ";
    for(std::deque<Fixed>::const_iterator i = m_vFixed.begin();
	i != m_vFixed.end(); ++i)
      std::cout << *i << " ";
    std::cout << std::endl;
  }
  std::cout
    << "Output " << (m_bVerbose ? "is" : "isn't") << " verbose"
    << " and " << (m_bFitScreen ? "is" : "isn't") << " full screen." << std::endl;
}

void Parameters::rescaleInstance() {
  m_vInstance.m_nScale = (URational) 1;
  m_vInstance.scaleDiscrete();
  if(m_s0.find_first_of('r') >= m_s0.size())
    m_vInstance.scaleDivisor();
  m_vInstance.inferInstanceProperties();
  m_Box.rescale(m_vInstance.m_nScale);
  moveUnitRectangles();
}

void Parameters::moveUnitRectangles() {

  /**
   * Find and save a copy of the unit rectangle from each
   * sequence. Then compress the vectors, and fill in the remainder
   * with the saved unit rectangle.
   */

  for(size_t i = 0; i < m_vInstance.size(); ++i) {

    /**
     * We found a unit rectangle. Perform the compression and end the
     * iteration.
     */

    if(m_vInstance[i].unit()) {

      /**
       * Save a copy of the unit rectangle.
       */

      RDimensions r(m_vInstance[i]);
      RDimensions s(m_vUnscaled[i]);

      /**
       * nCurrent points to the next location which we will be copying
       * over, while j iterates over the remaining portion of the
       * instance and filters out all unit rectangles. We increment
       * nCurrent only after we've written over the unit rectangle
       * with a non-unit rectangle.
       */

      size_t nCurrent(i);
      for(size_t j = i + 1; j < m_vInstance.size(); ++j)
	if(!m_vInstance[j].unit()) {
	  m_vInstance[nCurrent] = m_vInstance[j];
	  m_vUnscaled[nCurrent] = m_vUnscaled[j];
	  ++nCurrent;
	}

      /**
       * Now let's fill the remaining part of the vector with the unit
       * rectangles that we've previously saved.
       */

      for(size_t j = nCurrent; j < m_vInstance.size(); ++j) {
	m_vInstance[j] = r;
	m_vUnscaled[j] = s;
      }
      break;
    }
  }
}

bool Parameters::breakVerticalSymmetry() const {
  return(m_s1.find_first_of('s') >= m_s1.size());
}

void Parameters::getDescription(boost::program_options::options_description& desc) {

  /**
   * Describe each option.
   */

  const char p0[] =
    "Specifies additional options governing the search in the search "
    "for a bounding box by a string that includes any of the "
    "following case-sensitive symbols:\n\n"
    "  c: \tdisables conflict learning for subset sums\n"
    "  i: \texplore all integer boxes\n"
    "  l: \tenables the computation of a low-resolution lower bound solution (experimental)\n"
    "  m: \tdisable mutual exclusion checking between rectangle widths and heights\n"
    "  n: \tdisables normalization of the instance to remove denominators and reduce factors\n"
    "  r: \tdisables reduction scaling of the instance -- scaling up is still allowed\n"
    "  s: \tforces diagonal symmetry status to false\n"
    "  S: \tforces diagonal symmetry status to true\n"
    "  w: \tdisables bounding box pruning based on the wasted space of the top-level rectangle (only applies to high-precision benchmark)\n";

  const char p1[] =
    "Specifies additional options governing the search in the 1st "
    "dimension by a string that includes any of the following "
    "case-sensitive symbols:\n\n"
    "  d: \tdisables narrow strip domination and automatic table building\n"
    "  D: \tdisables simple duplicate handling logic\n"
    "  g: \tdisables the gap filling search space (where applicable)\n"
    "  i: \tdisables intervals\n"
    "  m: \tdisables mutex exclusion constraints during search\n"
    "  p: \tdisables constraint propagation\n"
    "  r: \tdisables range consideration of domination entries\n"
    "  s: \tdisables the confining the first rectangle to one quadrant of the bounding box\n"
    "  v: \tdisables dynamic variable ordering\n"
    "  w: \tdisables the wasted space heuristic\n";

  const char p2[] =
    "Specifies additional options governing the search in the 2nd "
    "dimension by a string that includes any of the following "
    "case-sensitive symbols:\n\n"
    "  1: \tuses only one corner for perfect packing\n"
    "  a: \tdisables rectangle adjacency search space\n"
    "  c: \tdisables non-empty composition inference rules\n"
    "  d: \tdisables all-pairs-shortest-path distance rules\n"
    "  D: \tdisables immediate placement of rectangles in dominated positions\n"
    "  e: \tdisables empty space composition rules\n"
    "  i: \tdisables the implicit representation of the remaining empty space\n"
    "  s: \tdisables subset sums inference rules\n"
    "  w: \tdisables the wasted space heuristic\n"
    "  y: \tdisables the dynamic subset sums rules.\n";

  const char pBenchmark[] =
    "Specifies the benchmark to use (or to accept a free-form "
    "description of the problem instance. The following integer-"
    "labeled list describes the available benchmarks (default is 1):\n\n"
    "  0: \tfor no benchmark, so you must give a comma-delimited list of rectangles WxH\n"
    "  1: \tfor squares 1x1, 2x2, ..., NxN\n"
    "  2: \tfor rectangles 1x2, 2x3, ..., Nx(N+1)\n"
    "  3: \tfor rectangles 1xN, 2x(N-1), ..., Nx1 (note that this benchmark has duplicates if they are unoriented)\n"
    "  4: \tfor rectangles 1x(2N-1), 2x(2N-2), ..., Nx(2N-N)\n"
    "  5: \tfor rectangles 1x1/2, 1/2x1/3, ..., 1/Nx1/(N+1)\n"
    "  6: \tfor rectangles (N+1)x(N^2+1), (2N+1)x(N^2-N+1), (3N+1)x(N^2-2N+1), ..., (N^2+1)x(N+1)\n"
    "  7: \tfor rectangles 2x4, 4x6, ..., 2Nx2(N+1)\n"
    "  8: \tfor rectangles 1x2, 3x4, ..., (2N-1)x(2N)\n";

  const char pBox[] =
    "In the case of iterative weakening, specifies the m boxes to "
    "skip, n:m to try the nth through mth boxes, or WxH to try only "
    "the specified box. In the case of an anytime algorithm, the "
    "previous descriptions (except for the cardinal specifications) "
    "are used to specify the initial bounds.\n";

  const char pCra[] =
    "Work on the contiguous resource allocation problems in the given "
    "file. The file is expected to have been generated by the "
    "cumulative scheduling solutions enumeration packer, which should "
    "be a text file where each line in the file is a cumulative "
    "scheduling solution in the format of comma-delimited y-"
    "coordinates for each square in a decreasing consecutive order.\n";

  const char pFit[] =
    "Rescale the result to fit the screen's width.\n";

  const char pFix[] =
    "Fix the placement of certain rectangles. Specify the coordinates "
    "in a comma-delimited list such as: 2x3=(10,32),4x6=(3,4) to "
    "force a 2x3 rectangle to be placed at the (x,y) coordinates "
    "(10,32) and to force a 4x6 rectangle to be placed at the (x,y) "
    "coordinates (3,4).\n";

  const char pGeometry[] =
    "Specifies the initial window geometry as WxH in pixels.";

  const char pHelp[] =
    "Displays this help message.\n";

  const char pHuman[] =
    "Makes the number printouts human-readable by adding thousand "
    "separators.\n";

  const char pJValue[] =
    "Sets the j-value for the instance description formula: W=i, "
    "H=j(N+1) + (1-2j)i.\n";

  const char pInstance[] =
    "Describes the problem instance to the packer, in the form of a "
    "series of comma-delimited rectangles without spaces if no "
    "benchmark is chosen. Otherwise, the parameter should describe "
    "the instance size or a set of parameter values N which describe "
    "the problem according to the benchmark. For example, in the "
    "consecutive-square packing benchmark, 2,4-6 describes the "
    "problem instance 2x2,4x4,5x5,6x6. The default is 1 for all "
    "benchmarks, or 1x1 if no benchmark was specified.\n";

  const char pKorf[] =
    "Use Rich Korf's packer (overrides all other switches that "
    "specify some packing algorithm.)\n";

  const char pOrdering[] =
    "Specifies what initial variable ordering to use:\n\n"
    "  0: \tExplore all possible variable orderings. Warning: for even 10 items there will be about 3.6 million permutations and with 9 there will be 360 thousand permutations\n"
    "  1: \tDo not sort\n"
    "  2: \tSort by decreasing area\n"
    "  3: \tSort by decreasing minimum dimension\n"
    "  4: \tSort by decreasing height, then decreasing width\n"
    "  5: \tSort by decreasing width, then decreasing height\n"
    "  6: \tSort by increasing W/H\n"
    "  7: \tSort by increasing H/W\n"
    "  8: \t(Default) Sort by decreasing branching factor (width for oriented cases and area for unoriented)\n";
  
  const char pQuiet[] =
    "Be quiet.\n";

  const char pRandom[] =
    "Random seed (note that 0 and 1 generate the same behavior). This "
    "should be a non-negative integer.\n";

  const char pRegression[] =
    "Runs a regression test of the specified packer against Korf's "
    "implementation. Parameters i,m,n[,d][,e][,m] i tests by "
    "randomly choosing m rectangles from the sequence 1-n of the "
    "specified benchmark. The optional literals specify the "
    "following:\n\n"
    "  d: \tAllow duplicates in the generated problem instances\n"
    "  e: \tDisable the use of domination conditions just in the baseline packer\n"
    "  i: \tForce the testing of all integral box dimensions\n"
    "  m: \tDisable subset mutexes in the baseline packer\n";

  const char pScheduling[] =
    "Solves just the cumulative scheduling problem and returns after "
    "the first solution is found.\n";

  const char pThreads[] =
    "Number of worker threads to use. The default is 1 (the program "
    "will be single-threaded.\n";

  const char pUnoriented[] =
    "Specifies that the instance is a set of unoriented rectangles, "
    "which means we allow 90-degree rotations in the packing solution.\n";

  const char pVerbose[] =
    "Be verbose.\n";

  const char pWeakening[] =
    "Selects an iterative weakening packing algorithm from the "
    "following, integer-labeled list (default is 2):\n\n"
    "  0: \tfor the IJCAI09 packer with the ability to take advantage of duplicates but ternary segmentation\n"
    "  1: \tfor the IJCAI09 packer with duplicate handling and our interval/range bug fix\n"
    "  2: \tfor a packer that dynamically balances intervals, but uses precomputed domains\n"
    "  3: \tfor a packer that precomputes all subset sums\n"
    "  4: \tfor a packer that dynamically computes subset sums in both dimensions\n"
    "  5: \tfor the AAAI11 packer which does dynamic subset sums AND nogoods learning\n"
    "  6: \tfor Moffitt and Pollack's meta-CSP anytime solver\n";

  const char pExtend[] =
    "Extends the given log by analyzing where we left off and "
    "continuing the computation. The previous effort is duplicated "
    "and the final results will include the cumulative results from "
    "all previous runs.\n";

  const char pYScale[] =
    "Specifies the size of the interval in terms of a multiplicative "
    "factor of the rectangle height: 0.0 for single values, or 1.0 "
    "for the height of the entire rectangle (not useful since it "
    "doesn't draw any constraints, but I'm just giving you an example). Put "
    "any negative number in order to force a dynamic determination of the "
    "scaling factors.\n";

  namespace po = boost::program_options;
  desc.add_options()
    ("0,0", po::value<std::string>(&m_s0)->default_value(""), p0)
    ("1,1", po::value<std::string>(&m_s1)->default_value(""), p1)
    ("2,2", po::value<std::string>(&m_s2)->default_value(""), p2)
    ("benchmark,b", po::value<int>(&m_vInstance.m_nBenchmark)->default_value(3), pBenchmark)
    ("box,B", po::value<std::string>(&m_sBox)->default_value(""), pBox)
    ("cra,c", po::value<std::string>(&m_sCra)->default_value(""), pCra)
    ("extend,e", po::value<std::string>(&m_sExtend)->default_value(""), pExtend)
    ("fit,F", pFit)
    ("fix,f", po::value<std::string>(&m_sFix)->default_value(""), pFix)
    ("geometry,G", po::value<std::string>(&m_sGeometry)->default_value(""), pGeometry)
    ("help,h", pHelp)
    ("human,H", pHuman)
    ("instance,i", po::value<std::string>(&m_vInstance.m_sInstance)->default_value("1"), pInstance)
    ("jvalue,j", po::value<float>(&m_nJValue)->default_value(-2.0), pJValue)
    ("korf,k", pKorf)
    ("ordering,o", po::value<int>(&m_nOrdering)->default_value(8), pOrdering)
    ("quiet,q", pQuiet)
    ("random,r", po::value<int>(&m_nRandom)->default_value(1), pRandom)
    ("regression,R", po::value<std::string>(&m_sRegression)->default_value(""), pRegression)
    ("scheduling,s", pScheduling)
    ("threads,t", po::value<int>(&m_nThreads)->default_value(1), pThreads)
    ("unoriented,u", pUnoriented)
    ("verbose,v", pVerbose)
    ("weakening,w", po::value<int>(&m_nWeakening)->default_value(-1), pWeakening)
    ("yscale,y", po::value<std::string>(&m_sYScale)->default_value("0.55"), pYScale);
}

void Parameters::parseAux(boost::program_options::options_description& desc,
			  boost::program_options::variables_map& vm) {

  /**
   * Parse the switch options.
   */

  if(vm.count("help")) {
    std::cout << desc << std::endl;
    exit(0);
  }
  if(vm.count("human"))
    std::cout.imbue(std::locale());
  m_bFitScreen = (vm.count("fit") > 0);
  m_bKorf = (vm.count("korf") > 0);
  m_bQuiet = (vm.count("quiet") > 0);
  m_bScheduling = (vm.count("scheduling") > 0);
  m_bVerbose = (vm.count("verbose") > 0);
  m_vInstance.m_bUnoriented = (vm.count("unoriented") > 0);
  if(m_nWeakening < 0) m_nWeakening = 2;
  if(m_s1.find_first_of('i') < m_s1.size())
    m_sYScale = "0.0";
  m_nYScale.initialize(m_sYScale);

  /**
   * Parse the complicated options into our own internal data
   * structures.
   */

  parseBox(m_sBox);
  parseFix(m_sFix);
  parseGeometry(m_sGeometry);
  m_vInstance.parseInstance(m_vInstance.m_sInstance);
  if(m_nJValue > -1.0) m_vInstance.rewriteJValue(m_nJValue);
  if(m_nOrdering == 8 && m_nWeakening < 3) {
    if(m_vInstance.m_bUnoriented)
      m_vInstance.sortDecreasingArea();
    else
      m_vInstance.sortDecreasingHeight();
  }
  else
    m_vInstance.sort(m_nOrdering);
  m_vInstance.inferInstanceProperties();
  m_vUnscaled = m_vInstance;
  parseRegression(m_sRegression);

  /**
   * Lastly, check to see if we're resuming from a previous log. If
   * so, we need to reload the previous parameters.
   */

  if(m_Log.load(m_sExtend)) {
    m_sExtend.clear();
    parseCommand(m_Log.m_vCommand);
  }
}

URational Parameters::unscale(const UInt& ui) const {
  URational ur(ui);
  ur *= m_vInstance.m_nScale;
  return(ur);
}

URational Parameters::unscale2(const UInt& ui) const {
  URational ur(ui);
  ur *= (m_vInstance.m_nScale * m_vInstance.m_nScale);
  return(ur);
}

URational Parameters::unscale2(const URational& ur) const {
  return(ur * m_vInstance.m_nScale * m_vInstance.m_nScale);
}

UInt Parameters::scale(const URational& ur) const {
  URational u(ur / m_vInstance.m_nScale);
  return(u.get_num());
}

UInt Parameters::scale2(const URational& ur) const {
  URational u(ur / m_vInstance.m_nScale / m_vInstance.m_nScale);
  return(u.get_ui());
}

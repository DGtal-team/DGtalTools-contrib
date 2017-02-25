/**
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/
/**
 * @file LUTBasedNSDistanceTransform.cpp
 * @ingroup Tools
 * @author Nicolas Normand (\c Nicolas.Normand@polytech.univ-nantes.fr)
 * LUNAM Université, Université de Nantes, IRCCyN UMR CNRS 6597
 *
 * @date 2012/09/28
 *
 * LUTBasedNSDistanceTransform computes the 2D translated neighborhood-sequence
 * distance transform of a binary image. It reads the input images from its
 * standard input and writes the result to its standard output.
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////


//boost
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/tokenizer.hpp>

//STL
#include <iostream>
#include <vector>
#include <string>

//Path-based distances
#include "ImageReader.h"
#include "NeighborhoodSequenceDistance.h"
#include "ImageWriter.h"

#include "DGtal/base/Common.h"
using namespace DGtal;

////////////////////////////////////////////////////////////////////////////////

std::vector<int> parseSequence(std::string string) {
    std::vector<int> args;
    boost::tokenizer<> tok(string);
    for (boost::tokenizer<>::iterator beg = tok.begin(); beg != tok.end(); ++beg) {
	args.push_back(boost::lexical_cast<short>(*beg));
    }
    return args;
}

////////////////////////////////////////////////////////////////////////////////

namespace po = boost::program_options;

int main( int argc, char** argv )
{
    // parse command line ----------------------------------------------------//
    po::options_description general_opt("Options and arguments: ");
    general_opt.add_options()
	("help,h", "Display this message")
	("city-block,4", "Use the city block distance")
	("chessboard,8", "Use the chessboard distance")
	("sequence,s", po::value<std::string>(),
	 "One period of the sequence of neighborhoods given as a list of 1 "
	 "and 2 separated by \" \" or \",\". Space characters must be escaped "
	 "from the shell.")
	("ratio,r", po::value<std::string>(),
	 "Ratio of neighborhood 2 given as the rational number num/den "
	 "(with den >= num >= 0 and den > 0).")
	("center,c", "Center the distance transform (the default is an asymmetric "
	 "distance transform)")
	("output,o", po::value<std::string>(), "Output file name, optionally "
	 "prefixed with the file format and ':'")
	("outputFormat,t", po::value<std::string>(), "Output file format")
	("inputFormat,f", po::value<std::string>(), "Input file format")
	("lineBuffered,l", "Flush output after each produced row.")
	("input,i", po::value<std::string>(), "Read from file \"arg\" instead of "
	 "stdin.");
    //------------------------------------------------------------------------//

    bool parseOK = true;
    po::variables_map vm;
    try {
	po::store(po::parse_command_line(argc, argv, general_opt), vm);
    }
    catch (const std::exception& ex) {
	parseOK = false;
	trace.info() << "Error checking program options: " << ex.what() << std::endl;
    }

    po::notify(vm);
    if(!parseOK || vm.count("help") || argc <= 1 ||
       (vm.count("chessboard") +
	vm.count("city-block") +
	vm.count("ratio") +
	vm.count("sequence") != 1))
    {
	trace.info() <<
	    "Compute the 2D translated neighborhood-sequence "
	    "distance transform of a binary image" << std::endl <<
	    "Basic usage: " << std::endl
	<< "\tLUTBasedNSDistanceTransform [-i filename] [-c] (-4|-8|-r <num/den>|-s <sequence>) [-t (pgm|png)]" <<std::endl
	<< general_opt << "\n";
	return 0;
    }

    //sourceOfDT source = undefined;
    NeighborhoodSequenceDistance *dist = NULL;

    // Distance selection ----------------------------------------------------//
    if (vm.count("city-block")) {
	dist = NeighborhoodSequenceDistance::newD4Instance();
    }
    else if (vm.count("chessboard")) {
	dist = NeighborhoodSequenceDistance::newD8Instance();
    }
    else if (vm.count("ratio")) {
	boost::rational<int> ratio;
	std::istringstream iss(vm["ratio"].as<std::string>());
	iss >> ratio;
	if (ratio < 0 || ratio > 1) {
	    std::cerr <<
		"Invalid ratio " << ratio << std::endl <<
		"correct ratios num/den are between 0 and 1 inclusive" << std::endl;
	    exit(-1);
	}
	dist = NeighborhoodSequenceDistance::newInstance(ratio);
    }
    else if (vm.count("sequence")) {
	std::vector<int> sequence = parseSequence(vm["sequence"].as<std::string>());
	dist = NeighborhoodSequenceDistance::newInstance(sequence);
    }
    //------------------------------------------------------------------------//

    // Output ----------------------------------------------------------------//
    ImageConsumer<GrayscalePixelType> *output;
    {
	std::string outputFile("-");
	std::string outputFormat("");
	bool lineBuffered = false;

	if (vm.count("output")) {
	    outputFile = vm["output"].as<std::string>();
	}
	if (vm.count("outputFormat")) {
	    outputFormat = vm["outputFormat"].as<std::string>();
	}
	if (vm.count("lineBuffered")) {
	    lineBuffered = true;
	}

	output = createImageWriter(outputFile, outputFormat, lineBuffered);

	if (output == NULL) {
	    std::cerr << "Unable to create image output stream (unrecognized format?)" << std::endl;
	}

	if (vm.count("center")) {
	    output = dist->newDistanceTransformUntranslator(output);
	}
    }
    NeighborhoodSequenceDistanceTransform *dt = dist->newTranslatedDistanceTransform(output);
    //------------------------------------------------------------------------//

    // Input -----------------------------------------------------------------//
    std::string inputFile("-");
    std::string inputFormat("");
    if (vm.count("input")) {
	inputFile = vm["input"].as<std::string>();
    }
    if (vm.count("inputFormat")) {
	inputFormat = vm["inputFormat"].as<std::string>();
    }
    createImageReader(dt, inputFile, inputFormat);
    //------------------------------------------------------------------------//

    return 0;
}

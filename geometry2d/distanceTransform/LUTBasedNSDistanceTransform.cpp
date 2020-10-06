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
 * Université Bretagne Loire, Université de Nantes,
 * Laboratoire des Sciences du Numérique de Nantes (LS2N) UMR CNRS 6004
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
/**
 @page LUTBasedNSDistanceTransform
 
 @brief  Apply the Rosin Threshold algorithm.

 @b Usage:   LUTBasedNSDistanceTransform  [input]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   —-outputFormat TEXT                 Output file format
   —-inputFormat TEXT                  Input file format

 Options:
   -h,--help                             Print this help message and exit
   --input,--i TEXT                      Read from file "arg" instead of stdin.
   -4,--city-block                       Use the city block distance
   -8,--chessboard                       Use the chessboard distance
   -s,--sequence TEXT                    One period of the sequence of neighborhoods given as a list of 1 and 2 separated by " " or ",". Space characters must be escaped from the shell.
   -r,--ratio TEXT                       Ratio of neighborhood 2 given as the rational number num/den (with den >= num >= 0 and den > 0).
   -c,--center BOOLEAN                   Center the distance transform (the default is an asymmetric distance transform)
   -o,--output TEXT                      Output file name, optionally prefixed with the file format and ':'
   -t TEXT                               Output file format
   -f TEXT                               Input file format
   -l,--lineBuffered                     Flush output after each produced row.
   
 
 @see
 @ref LUTBasedNSDistanceTransform.cpp

 */

// boost
#include "CLI11.hpp"

#include <boost/tokenizer.hpp>

// STL
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

// Path-based distances
#include "ImageReader.h"
#include "ImageWriter.h"
#include "NeighborhoodSequenceDistance.h"

#include "DGtal/base/Common.h"
using namespace DGtal;

////////////////////////////////////////////////////////////////////////////////

std::vector<int> parseSequence(std::string string)
{
    std::vector<int> args;
    boost::tokenizer<> tok(string);
    for (auto beg = tok.begin(); beg != tok.end(); ++beg)
    {
        args.push_back(boost::lexical_cast<short>(*beg));
    }
    return args;
}


int main(int argc, char **argv)
{
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::stringstream ssDescr;
  ssDescr << "Compute the 2D translated neighborhood-sequence distance "
          "transform of a binary image"
          << std::endl
          << "Basic usage: "
          << std::endl
          << "\tLUTBasedNSDistanceTransform [-i filename] [-c] (-4|-8|-r "
          "<num/den>|-s <sequence>) [-t (pgm|png)]"
          << std::endl;
    
  app.description(ssDescr.str());
  bool city_block {false};
  bool chessboard {false};
  bool center {false};
  bool lineBuffered {false};
  std::string ratio_s;
  std::string outputS;
  std::string outputFormat;
  std::string inputFormat;
  std::string input;
  std::string sequence;

  app.add_option("--input,--i", input, "Read from file \"arg\" instead of stdin.");
  app.add_flag("--city-block,-4",city_block, "Use the city block distance");
  app.add_flag("--chessboard,-8",chessboard, "Use the chessboard distance");
  app.add_option("--sequence,-s", sequence, "One period of the sequence of neighborhoods given as a list of 1 "
               "and 2 separated by \" \" or \",\". Space characters must be escaped "
               "from the shell.");
  auto ratioOpt = app.add_option("--ratio,-r", ratio_s, "Ratio of neighborhood 2 given as the rational number num/den "
                 "(with den >= num >= 0 and den > 0).");
  app.add_option("--center,-c", center, "Center the distance transform (the default is an "
                 "asymmetric distance transform)");

  app.add_option("--output,-o",outputS, "Output file name, optionally "
               "prefixed with the file format and ':'");
  
  app.add_option("—-outputFormat,-t", outputFormat,"Output file format");
  app.add_option("—-inputFormat,-f", outputFormat,"Input file format");
  app.add_flag("--lineBuffered,-l", lineBuffered, "Flush output after each produced row.");

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

        
  if ((chessboard + city_block + ratioOpt->count() + sequence.size() !=0 ) != 1){
    trace.info() << "You need to choose one unique distance, use --help for help.\n";
    return 1;
  }
   

    //sourceOfDT source = undefined;
    NeighborhoodSequenceDistance *dist = NULL;

    // Distance selection ----------------------------------------------------//
    if (city_block)
    {
        dist = NeighborhoodSequenceDistance::newD4Instance();
    }
    else if (chessboard)
    {
        dist = NeighborhoodSequenceDistance::newD8Instance();
    }
    else if (ratioOpt->count())
    {
        boost::rational<int> ratio;
        std::istringstream iss(ratio_s);
        iss >> ratio;
        if (ratio < 0 || ratio > 1)
        {
            std::cerr
                << "Invalid ratio " << ratio << std::endl
                << "correct ratios num/den are between 0 and 1 inclusive"
                << std::endl;
            exit(-1);
        }
        dist = NeighborhoodSequenceDistance::newInstance(ratio);
    }
    else if (sequence.size() != 0)
    {
        std::vector<int> sequenceV = parseSequence(sequence);
        dist = NeighborhoodSequenceDistance::newInstance(sequenceV);
    }
    //------------------------------------------------------------------------//

    // Output ----------------------------------------------------------------//
    ImageConsumer<GrayscalePixelType> *output;
    {
        std::string outputFile("-");
        std::string outputFormat("");
        bool lineBuffered = false;

        if (outputS!="")
        {
            outputFile = outputS;
        }
        
        
        output = createImageWriter(outputFile, outputFormat, lineBuffered);

        if (output == NULL)
        {
            std::cerr
                << "Unable to create image output stream (unrecognized format?)"
                << std::endl;
        }

        if (center)
        {
            output = dist->newDistanceTransformUntranslator(output);
        }
    }
    NeighborhoodSequenceDistanceTransform *dt =
        dist->newTranslatedDistanceTransform(output);
    //------------------------------------------------------------------------//

    // Input -----------------------------------------------------------------//
    createImageReader(dt, input, inputFormat);
    //------------------------------------------------------------------------//

    return 0;
}

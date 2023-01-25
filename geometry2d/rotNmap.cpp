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
 * @file
 * @ingroup geometry2d
 * @author Bertrand Kerautret (\c bertrand.kerautret@univ-lyon2.fr )
 *
 *
 * @date 2023/01/17
 *
 * Source file of the tool rotNmap
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include <sstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/GenericReader.h"
#include "DGtal/io/writers/GenericWriter.h"

#include "CLI11.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page rotNmap rotNmap
 
 @brief
 Applies a rotation in the input normal map (it rotates both the position and normals orientation to be consistant.
 Usage: ./geometry2d/rotNmap [input]
 Typical use example:
 rotNmap normalMap.png normalRotated 1.5
 
 Usage: ./geometry2d/rotNmap [OPTIONS] 1 2 [3]
 
 Positionals:
 1 TEXT:FILE REQUIRED                  Input file
 2 TEXT REQUIRED                       Output SDP filename
 3 FLOAT=3.14                          a double angle
 
 Options:
 -h,--help                             Print this help message and exit
 -i,--input TEXT:FILE REQUIRED         Input file
 -o,--output TEXT REQUIRED             Output SDP filename
 -a,--angle FLOAT=3.14                 a double angle
 
 @b Allowed @b options @b are :
 @code
 
 -h,--help                             Print this help message and exit
 -i,--input TEXT:FILE REQUIRED         Input file
 -o,--output TEXT REQUIRED             Output SDP filename
 -a,--angle FLOAT=3.14                 a double angle
 @endcode
 
 
 */


int main( int argc, char** argv )
{
    double alpha {3.14};
    std::string inputFileName;
    std::string outputFileName;
    std::stringstream usage;
    bool onlyNormal {false};
    bool fillFlat {false};
    
    usage << "Usage: " << argv[0] << " [input]\n"
    << "Typical use example:\n \t rotNmap normalMap.png normalRotated 1.5 \n";
    // parse command line using CLI-------------------------------------------------------
    CLI::App app;
    app.description("Applies a rotation in the input normal map (it rotates both the position and normals orientation to be consistant. \n" + usage.str() );
    app.add_option("--input,-i,1", inputFileName, "Input file")->required()->check(CLI::ExistingFile);
    app.add_option("--output,-o,2", outputFileName, "Output SDP filename")->required();
    app.add_option("--angle,-a, 3", alpha, "a double angle", true);
    app.add_flag("--onlyNormal,-n", onlyNormal, "rotate only normal not the map itself");
    app.add_flag("--fillFlat,-f", fillFlat, "fill area with missing information by vertical normal (ie Color (128, 128, 255))");
    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    // END parse command line using CLI ----------------------------------------------
    
    // Some nice processing  --------------------------------------------------
    typedef ImageContainerBySTLVector<Z2i::Domain, Color> ColorImage;
    
    
    trace.info() << "Starting " << argv[0]  << "with input: " <<  inputFileName
    << " and output :" << outputFileName
    << " angle " <<std::endl;
    
    trace.info() << "Reading input image color:";
    ColorImage inputImage = STBReader<ColorImage>::import(inputFileName);
    ColorImage outputImage (inputImage.domain());
    trace.info() << " [done] size:"
    <<  inputImage.domain().lowerBound()-inputImage.domain().upperBound()
    << std::endl;
    // Rotating normals with angle:
    for (auto p: outputImage.domain()){
        Z2i::Point center (inputImage.domain().upperBound()[0]/2,
                           inputImage.domain().upperBound()[1]/2);
        Z2i::RealPoint n (0.0, 0.0);
        Z2i::Point pr ((p[0]-center[0])*cos(-alpha)  - (inputImage.domain().upperBound()[1]-p[1]-center[1])*sin(-alpha),
                       (inputImage.domain().upperBound()[1]-p[1]-center[1])*cos(-alpha)
                       + (p[0]-center[0])*sin(-alpha));
        pr[0] = pr[0]+center[0];
        pr[1] = inputImage.domain().upperBound()[1]-(pr[1]+center[1]);
        if (outputImage.domain().isInside(pr)|| onlyNormal){
            n[0] = ((double)inputImage(onlyNormal ? p:pr).red()/255.0)*2.0-1.0;
            n[1] = ((double)inputImage(onlyNormal ? p:pr).green()/255.0)*2.0-1.0;
            double x = n[0];
            double y = n[1];
            n[0] = x*cos(-alpha) - y*sin(-alpha);
            n[1] = y*cos(-alpha) + x*sin(-alpha);
            if (onlyNormal){
                Color c (static_cast<unsigned int>(std::min(255.0,(n[0]+1.0)*128.0)),
                         static_cast<unsigned int>(std::min(255.0,(n[1]+1.0)*128.0)),
                         inputImage(p).blue());
                outputImage.setValue(p, c);
            }
            else{
                Color c (static_cast<unsigned int>(std::min(255.0,(n[0]+1.0)*128.0)),
                         static_cast<unsigned int>(std::min(255.0,(n[1]+1.0)*128.0)),
                         inputImage(pr).blue());
                outputImage.setValue(p, c);
            }
        }else if (fillFlat){
            Color c (128, 128, 255);
            outputImage.setValue(p, c);
        }
    }
    
    outputImage >> outputFileName ;
    return 0;
}

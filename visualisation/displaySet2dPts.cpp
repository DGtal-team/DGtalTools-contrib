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
 * @file displaySet2dPts.cpp
 * @author Bertrand Kerautret (\c kerautre@univ-lyon2.fr )
 * LIRIS (CNRS, UMR 7503), University of Lyon 2, France
 *
 * @date 2018/12/26
 *
 * Source file of the tool displaySet2dPts
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/base/Common.h"

//STL
#include <vector>
#include <string>
#include <cstdlib>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"

#include "DGtal/shapes/ShapeFactory.h"
#include "DGtal/shapes/Shapes.h"
#include "DGtal/topology/helpers/Surfaces.h"

//image
#include "DGtal/images/imagesSetsUtils/ImageFromSet.h"
#include "DGtal/images/imagesSetsUtils/SetFromImage.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/images/ImageSelector.h"
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/Color.h"
#include "DGtal/io/colormaps/HueShadeColorMap.h"
#include "DGtal/io/readers/GenericReader.h"


#include "DGtal/io/boards/Board2D.h"


#include "CLI11.hpp"
///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////

/**
 @page displaySet2dPts displaySet2dPts
 
 @brief  Description of the tool...
 
 @b Usage:   displaySet2dPts [input] [output]
 
 @b Allowed @b options @b are :
 
 @code
 Usage: ./visualisation/displaySet2dPts [input] [output]
 Display different sets of points from input file where each set is represented in one line. 
 Typical use example:    ./visualisation/displaySetOf2dPts -i example.sdp  -o export.eps 

Usage: ./displaySet2dPts [OPTIONS] 1 2

Positionals:
  1 TEXT:FILE REQUIRED                  the input file, each line containing one set points.
  2 TEXT REQUIRED                       save output file automatically according the file format extension.

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         the input file, each line containing one set points.
  -o,--outputFile TEXT REQUIRED         save output file automatically according the file format extension.
  --backgroundImage TEXT                display image as background.
  --alphaBG FLOAT=1                     alphaBG <value> 0-1.0 to display the background image in transparency (default 1.0), (transparency works only if cairo is available)
  --scale FLOAT=1                       scale <value> 1: normal; >1 : larger ; <1 lower resolutions)
 
 Typical use example:
 ./visualisation/displaySetOf2dPts -i example.sdp  -o export.eps
 
 @endcode
 
 @b Example:
 
 @code
 displaySet2dPts -i  $DGtal/examples/samples/....
 @endcode
 
 @image html resdisplayLineSegments.png "Example of result. "
 
 @see
 @ref displaySet2dPts.cpp
 
 */


int main( int argc, char** argv )
{
  // parse command line CLI-------------------------------------------------------
  CLI::App app;
  app.description("Display different sets of points from input file where each set is represented in one line. \n Typical use example: \t ./visualisation/displaySetOf2dPts -i example.sdp  -o export.eps \n");
  std::string fileName;
  std::string imageName;
  std::string outputFileName = "";
  std::string extension = "";
  double scale {1.0};
  double alpha {1.0};

  app.add_option("-i,--input,1", fileName, "the input file, each line containing one set points.")->required()->check(CLI::ExistingFile);
  app.add_option("--outputFile,-o,2", outputFileName, "save output file automatically according the file format extension.")->required();
  auto backgroundImageOpt = app.add_option("--backgroundImage", imageName, "display image as background.");
  app.add_option("--alphaBG", alpha, "alphaBG <value> 0-1.0 to display the background image in transparency (default 1.0), (transparency works only if cairo is available)");
  app.add_option("--scale", scale, "scale <value> 1: normal; >1 : larger ; <1 lower resolutions)");

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  Board2D aBoard;
  aBoard.setUnit (0.05*scale, LibBoard::Board::UCentimeter);
  
  unsigned int height = 0;
  if(backgroundImageOpt->count() > 0)
  {
    typedef ImageSelector<Z2i::Domain, unsigned char>::Type Image;
    Image img = DGtal::GenericReader<Image>::import( imageName );
    Z2i::Point ptInf = img.domain().lowerBound();
    Z2i::Point ptSup = img.domain().upperBound();
    unsigned int width = abs(ptSup[0]-ptInf[0]+1);
    height = abs(ptSup[1]-ptInf[1]+1);
    aBoard.drawImage(imageName, 0-0.5,height-0.5, width, height, -1, alpha );
  }
  
  std::vector<std::vector<Z2i::RealPoint>> setOfPoints;
  
  extension = outputFileName.substr(outputFileName.find_last_of(".") + 1);
  

  setOfPoints= PointListReader<Z2i::RealPoint>::getPolygonsFromFile(fileName);
  if (setOfPoints.size() != 0)
  { 
    HueShadeColorMap<int> hueMap = HueShadeColorMap<int>(0, setOfPoints.size());
    for(auto s: setOfPoints )
    {
      // use max limit of 512 to avoid xfig bug display
      if (extension == "fig")
      {
        aBoard.setPenColor(hueMap(rand()%(min<unsigned int>(setOfPoints.size(),512))));
      }
      else
      {
        aBoard.setPenColor(hueMap(rand()%setOfPoints.size()));
      }
      for(auto p: s)
      {
        aBoard.fillRectangle(p[0], p[1], 1, 1);
      }
    }
  }
  
  if(extension=="svg")
  {
    aBoard.saveSVG(outputFileName.c_str());
  }
#ifdef WITH_CAIRO
  else
    if (extension=="eps")
    {
      aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoEPS );
    }
    else if (extension=="pdf")
    {
      aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPDF );
    } else if (extension=="png")
    {
      aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPNG );
    }
#endif
    else if(extension=="eps")
    {
      aBoard.saveEPS(outputFileName.c_str());
    }
    else if(extension=="fig")
    {
      aBoard.saveFIG(outputFileName.c_str(),LibBoard::Board::BoundingBox, 10.0, true );
    }
  
  return 0;
}


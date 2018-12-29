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


#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;


/**
 @page displayLineSegments displayLineSegments
 
 @brief  Description of the tool...

 @b Usage:   displayLineSegments [input]

 @b Allowed @b options @b are :
 
 @code
 Usage: ./visualisation/displaySet2dPts [input] [output]
 Display different sets of points from input file where each set is represented in one line.Allowed options are: :
 -h [ --help ]           display this message
 -i [ --input ] arg      the input file, each line containing one set points.
 -o [ --outputFile ] arg  <filename> save output file automatically according
 the file format extension.
 --backgroundImage arg   backgroundImage <filename> : display image as
 background
 --alphaBG arg           alphaBG <value> 0-1.0 to display the background image
 in transparency (default 1.0), (transparency works
 only if cairo is available)
 --scale arg             scale <value> 1: normal; >1 : larger ; <1 lower
 resolutions  )
 
 Typical use example:
 ./visualisation/displaySetOf2dPts -i example.sdp  -o export.eps

 @endcode

 @b Example: 

 @code
   	displayLineSegments -i  $DGtal/examples/samples/....
 @endcode

 @image html resdisplayLineSegments.png "Example of result. "

 @see
 @ref displayLineSegments.cpp

 */


int main( int argc, char** argv )
{
  // parse command line -------------------------------------------------------
// parse command line ----------------------------------------------
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "the input file, each line containing one set points. ")
    ("outputFile,o", po::value<std::string>(), " <filename> save output file automatically according the file format extension.")
    ("backgroundImage", po::value<std::string>(), "backgroundImage <filename> : display image as background ")
    ("alphaBG", po::value<double>(), "alphaBG <value> 0-1.0 to display the background image in transparency (default 1.0), (transparency works only if cairo is available)")
    ("scale", po::value<double>(), "scale <value> 1: normal; >1 : larger ; <1 lower resolutions  )");
  
  
  bool parseOK=true;
  po::variables_map vm;
  try
    {
      po::store(po::parse_command_line(argc, argv, general_opt), vm);
    }
  catch(const std::exception& ex)
    {
      parseOK=false;
      trace.info()<< "Error checking program options: "<< ex.what()<< endl;
    }
  

  // check if min arguments are given and tools description ------------------
  po::notify(vm);
  if( !parseOK || vm.count("help")||argc<=1)
    {
      std::cout << "Usage: " << argv[0] << " [input] [output]\n"
                << "Display different sets of points from input file where each set is represented in one line."
                << general_opt << "\n"
                << "Typical use example:\n \t ./visualisation/displaySetOf2dPts -i example.sdp  -o export.eps \n";
      return 0;
    }
   if(! vm.count("input"))
    {
      trace.error() << " The file name was not defined" << endl;
      return 1;
    }


   double scale=1.0;
   if(vm.count("scale")){
     scale = vm["scale"].as<double>();
   }
  
   Board2D aBoard;
   aBoard.setUnit (0.05*scale, LibBoard::Board::UCentimeter);
  
   double alpha=1.0;
   if(vm.count("alphaBG")){
     alpha = vm["alphaBG"].as<double>(); 
   }
  unsigned int height = 0;
   if(vm.count("backgroundImage")){
     std::string imageName = vm["backgroundImage"].as<std::string>();
     typedef ImageSelector<Z2i::Domain, unsigned char>::Type Image;
     Image img = DGtal::GenericReader<Image>::import( imageName );
     Z2i::Point ptInf = img.domain().lowerBound(); 
     Z2i::Point ptSup = img.domain().upperBound(); 
     unsigned int width = abs(ptSup[0]-ptInf[0]+1);
     height = abs(ptSup[1]-ptInf[1]+1);
     aBoard.drawImage(imageName, 0-0.5,height-0.5, width, height, -1, alpha );
   }

    
   std::vector<std::vector<Z2i::RealPoint>> setOfPoints;
   if(vm.count("input")){
     std::string fileName = vm["input"].as<std::string>();
     setOfPoints= PointListReader<Z2i::RealPoint>::getPolygonsFromFile(fileName);
       HueShadeColorMap<int> hueMap = HueShadeColorMap<int>(0, setOfPoints.size());
       for(auto s: setOfPoints )
     {
         aBoard.setPenColor(hueMap(random()%(min<uint>(setOfPoints.size(),512))));
         for(auto p: s)
         {
             aBoard.fillRectangle(p[0], p[1], 1, 1);
         }
     }
   }
 
 
  
   if(vm.count("outputFile")){
     std::string outputFileName= vm["outputFile"].as<std::string>();
     std::string extension = outputFileName.substr(outputFileName.find_last_of(".") + 1);

     if(extension=="svg"){
       aBoard.saveSVG(outputFileName.c_str());
     }
     #ifdef WITH_CAIRO
     else
       if (extension=="eps"){
         aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoEPS );
       } else 
         if (extension=="pdf"){
           aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPDF );
         } else 
           if (extension=="png"){
             aBoard.saveCairo(outputFileName.c_str(),Board2D::CairoPNG );
           }
     #endif
     else if(extension=="eps"){
       aBoard.saveEPS(outputFileName.c_str());
     }else if(extension=="fig"){
       aBoard.saveFIG(outputFileName.c_str(),LibBoard::Board::BoundingBox, 10.0, true );
     }
   }
    
     if (vm.count("outputStreamSVG")){
     aBoard.saveSVG(std::cout);
   } else   
       if (vm.count("outputStreamFIG")){
     aBoard.saveFIG(std::cout, LibBoard::Board::BoundingBox, 10.0,  true);
   } else
         if (vm.count("outputStreamEPS")){
     aBoard.saveEPS(std::cout);
   } 
    
  return 0;
}


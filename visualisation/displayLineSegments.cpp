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
 * @file displayLineSegments.cpp
 * @ingroup visualisation
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * @date 2016/08/26
 *
 * Source file of the tool displayLineSegments
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

 #include "DGtal/io/readers/GenericReader.h"





//contour
#include "DGtal/geometry/curves/FreemanChain.h"

//processing
#include "DGtal/geometry/curves/ArithmeticalDSSComputer.h"
#include "DGtal/geometry/curves/GreedySegmentation.h"
#include "DGtal/geometry/curves/SaturatedSegmentation.h"
#include "DGtal/geometry/curves/FP.h"
#include "DGtal/geometry/curves/StabbingCircleComputer.h"
#include "DGtal/geometry/curves/SaturatedSegmentation.h"
#include "DGtal/geometry/curves/SegmentComputerUtils.h"

#include "DGtal/io/boards/Board2D.h"
#include "DGtal/io/boards/CDrawableWithBoard2D.h"


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
 Usage: ./visualisation/displayLineSegments [input]
 Display line segments from input file where each segment is represented in one line.Allowed options are: :

  -h [ --help ]           display this message
  -i [ --input ] arg      the input file containing the segments x1 y1 x2 y2 to be displayed.
  -s [ --inputSecSet ] arg the second input set file containing the segments x1 y1 x2 y2 to be displayed.
  --SDPindex arg          specify the sdp index of segment endpoints (by
                          default 0,1,2,3).
  --lineWidth arg (=1)    Define the linewidth of the segments.
  --lineWidthSecSet arg (=1) Define the linewidth of the segments (second Set).
  --noXFIGHeader           to exclude xfig header in the resulting output
                            stream (no effect with option -outputFile).
 --customLineColor arg   set the R, G, B, A components of the colors of
                          the mesh faces and eventually the color R, G, B,
                          A of the mesh edge lines (set by default to black).
  --customLineColorSecSet arg  set the R, G, B, A components of the colors of
                          the mesh faces and eventually the color R, G, B,
                          A of the mesh edge lines (set by default to black).
  --customPointColor arg  set the R, G, B, A components of the colors of
                           the mesh faces and eventually the color R, G, B,
                           A of the mesh edge lines (set by default to black).
  -e [ --noDisplayEndPoints ]  to not displays segment end points.
  -o [ --outputFile ] arg  <filename> save output file automatically according
                          the file format extension.
  --outputStreamEPS        specify eps for output stream format.
  --outputStreamSVG        specify svg for output stream format.
  --outputStreamFIG        specify fig for output stream format.
  --invertYaxis            invertYaxis invert the Y axis for display contours
                          (used only with --SDP)
  --backgroundImage arg   backgroundImage <filename> : display image as
                          background
  --alphaBG arg           alphaBG <value> 0-1.0 to display the background image
                          in transparency (default 1.0), (transparency works
                          only if cairo is available)
  --scale arg             scale <value> 1: normal; >1 : larger ; <1 lower
                          resolutions  )

Typical use example:
 	 ./visualisation/displayLineSegments -i ../Samples/lines.sdp  --SDPindex 1 2 3 4  -o lines.eps --invertYaxis
 @endcode

 @b Example:

 @code
   	displayLineSegments -i  $DGtal/examples/samples/....
 @endcode

 @image html resdisplayLineSegments.png "Example of result. "

 @see
 @ref displayLineSegments.cpp

 */


void displayLineSet(po::variables_map vm, std::string nameInput,
                    std::string nameArgCol, Board2D &aBoard,
                    unsigned int height, bool invertYaxis)
{
    std::string fileName = vm[nameInput].as<std::string>();
    std::vector<unsigned int > vectPos;
    if(vm.count("SDPindex"))
    {
        vectPos = vm["SDPindex"].as<std::vector<unsigned int > >();
        if(vectPos.size()!=4)
        {
            trace.error() << "you need to specify the four indexes of vertex." << std::endl;
            return ;
        }
    }else
    {
        vectPos = {0,1,2,3};
    }
    std::vector<unsigned int> vectPos1; vectPos1.push_back(vectPos[0]); vectPos1.push_back(vectPos[1]);
    std::vector<Z2i::RealPoint> vectPt1 = PointListReader<Z2i::RealPoint>::getPointsFromFile(fileName, vectPos);
    std::vector<unsigned int> vectPos2; vectPos2.push_back(vectPos[2]); vectPos2.push_back(vectPos[3]);
    std::vector<Z2i::RealPoint> vectPt2 = PointListReader<Z2i::RealPoint>::getPointsFromFile(fileName, vectPos2);
    
    DGtal::Color lineColor = DGtal::Color::Red;
    DGtal::Color pointColor = DGtal::Color::Blue;
    if(vm.count(nameArgCol)){
        std::vector<unsigned int > vectCol = vm[nameArgCol].as<std::vector<unsigned int> >();
        if(vectCol.size()!=3 ){
            trace.error() << "colors specification should contain R,G,B values (using default red)."<< std::endl;
        }
        lineColor.setRGBi(vectCol[0], vectCol[1], vectCol[2], 255);
    }
    if(vm.count("customPointColor")){
        std::vector<unsigned int > vectCol = vm["customPointColor"].as<std::vector<unsigned int> >();
        if(vectCol.size()!=3 ){
            trace.error() << "colors specification should contain R,G,B values (using default red)."<< std::endl;
        }
        pointColor.setRGBi(vectCol[0], vectCol[1], vectCol[2], 255);
    }
    
    
    for(unsigned int i=0; i<vectPt1.size(); i++){
        Z2i::Point pt1 (vectPt1[i][0], invertYaxis? height - vectPt1[i][1]: vectPt1[i][1] );
        Z2i::Point pt2 (vectPt2[i][0], invertYaxis? height - vectPt2[i][1]: vectPt2[i][1] );
        if(!vm.count("noDisplayEndPoints"))
        {
            aBoard << CustomStyle(vectPt1[i].className(), new CustomColors(pointColor, pointColor));
            aBoard << pt1 ;
            aBoard << pt2;
            aBoard.setPenColor(lineColor);
        }
        aBoard.drawLine(pt1[0], pt1[1], pt2[0], pt2[1]);
    }
    
}


int main( int argc, char** argv )
{
  // parse command line -------------------------------------------------------
// parse command line ----------------------------------------------
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "the input file containing the segments x1 y1 x2 y2 to be displayed. ")
    ("inputSecSet,s", po::value<std::string>(), "the second input set file containing the segments x1 y1 x2 y2 to be displayed. ")
    ("SDPindex", po::value<std::vector <unsigned int> >()->multitoken(), "specify the sdp index of segment endpoints (by default 0,1,2,3).")
    ("domain", po::value<std::vector <int> >()->multitoken(), "limit the export to a given domain (xmin ymin xmax ymax).")
    ("lineWidth", po::value<double>()->default_value(1.0), "Define the linewidth of the segments.")
    ("lineWidthSecSet", po::value<double>()->default_value(1.0), "Define the linewidth of the segments (second set).")
    ("noXFIGHeader", " to exclude xfig header in the resulting output stream (no effect with option -outputFile).")
    ("customLineColor",po::value<std::vector<unsigned int> >()->multitoken(), "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). " )
    ("customLineColorSecSet",po::value<std::vector<unsigned int> >()->multitoken(), "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). " )
    ("customPointColor",po::value<std::vector<unsigned int> >()->multitoken(), "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). " )
    ("noDisplayEndPoints,e", "to not displays segment end points.")
    ("outputFile,o", po::value<std::string>(), " <filename> save output file automatically according the file format extension.")
    ("outputStreamEPS", " specify eps for output stream format.")
    ("outputStreamSVG", " specify svg for output stream format.")
    ("outputStreamFIG", " specify fig for output stream format.")
    ("invertYaxis", " invertYaxis invert the Y axis for display contours (used only with --SDP)")

#ifdef WITH_CAIRO
    ("outputPDF", po::value<std::string>(), "outputPDF <filename> specify pdf format. ")
    ("outputPNG", po::value<std::string>(), "outputPNG <filename> specify png format.")
#endif

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
      std::cout << "Usage: " << argv[0] << " [input]\n"
                << "Display line segments from input file where each segment is represented in one line."
                << general_opt << "\n"
                << "Typical use example:\n \t ./visualisation/displayLineSegments -i ../Samples/lines.sdp  --SDPindex 1 2 3 4  -o lines.eps --invertYaxis  \n";
      return 0;
    }
  if(! vm.count("input"))
    {
      trace.error() << " The file name was not defined" << endl;
      return 1;
    }


   double lineWidth =  vm["lineWidth"].as<double>();
   double lineWidthSetB =  vm["lineWidthSetB"].as<double>();

    double scale=1.0;
   if(vm.count("scale")){
     scale = vm["scale"].as<double>();
   }
  
   Board2D aBoard;
   aBoard.setUnit (0.05*scale, LibBoard::Board::UCentimeter);
   if(vm.count("domain"))
     {
       std::vector<int> vectDom = vm["domain"].as<std::vector<int > >();
       if(vectDom.size()!=4){
         trace.error() << "you need to specify the four values for the domain." << std::endl;
         return 0;
       }
       aBoard.setClippingRectangle(vectDom[0],vectDom[1],vectDom[2],vectDom[3]);
     }

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

 
  bool invertYaxis = vm.count("invertYaxis");

  if(vm.count("input")){
      aBoard.setLineWidth(lineWidth);
      displayLineSet(vm, "input","customLineColor", aBoard, height, invertYaxis);
   }
   if(vm.count("inputSecSet")){
      aBoard.setLineWidth(lineWidthSetB);
      displayLineSet(vm, "inputSecSet","customLineColorSecSet", aBoard, height, invertYaxis);
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
       aBoard.saveFIG(outputFileName.c_str(),LibBoard::Board::BoundingBox, 10.0, !vm.count("noXFIGHeader") );
     }
   }
    
     if (vm.count("outputStreamSVG")){
     aBoard.saveSVG(std::cout);
   } else
       if (vm.count("outputStreamFIG")){
     aBoard.saveFIG(std::cout, LibBoard::Board::BoundingBox, 10.0,  !vm.count("noXFIGHeader"));
   } else
         if (vm.count("outputStreamEPS")){
     aBoard.saveEPS(std::cout);
   }
    

  

  return 0;
}


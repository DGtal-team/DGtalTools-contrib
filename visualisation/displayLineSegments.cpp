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
#include <sstream>

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

#include "CLI11.hpp"


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page displayLineSegments displayLineSegments
 
 @brief  Description of the tool...

 @b Usage:   displayLineSegments [input]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  the input file containing the segments x1 y1 x2 y2 to be displayed.
   2 TEXT                                <filename> save output file automatically according the file format extension.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         the input file containing the segments x1 y1 x2 y2 to be displayed.
   -s,--inputSec TEXT                    the second input set file containing the segments x1 y1 x2 y2 to be displayed.
   --SDPindex UINT x 4                   specify the sdp index of segment endpoints (by default 0,1,2,3).
   --domain UINT x 4                     limit the export to a given domain (xmin ymin xmax ymax).
   --lineWidth FLOAT                     Define the linewidth of the segments.
   --lineWidthSec FLOAT                  Define the linewidth of the segments.
   --noXFIGHeader                         to exclude xfig header in the resulting output stream (no effect with option -outputFile).
   --customLineColor UINT x 4            set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black).
   --customLineColorSec UINT x 4         set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black).
   --customPointColor UINT x 4           set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black).
   -e,--noDisplayEndPoints BOOLEAN       to not display segment end points.
   --noDisplayEndPointsSec               to not display segment end points of second set.
   -o,--outputFile TEXT                  <filename> save output file automatically according the file format extension.
   --outputStreamEPS                     specify eps for output stream format.
   --outputStreamSVG                     specify svg for output stream format.
   --outputStreamFIG                     specify fig for output stream format.
   --invertYaxis                         invertYaxis invert the Y axis for display contours (used only with --SDP)
   --backgroundImage TEXT                backgroundImage <filename> : display image as background
   --alphaBG FLOAT                       alphaBG <value> 0-1.0 to display the background image in transparency (default 1.0), (transparency works only if cairo is available)
   --scale FLOAT=1                       scale <value> 1: normal; >1 : larger ; <1 lower resolutions  )
   
 
 @endcode

 @b Example:

 @code
 ./visualisation/displayLineSegments  ../Samples/lines.sdp  --SDPindex 1 2 3 4 --invertYaxis  lines.eps
 @endcode

 @image html resdisplayLineSegments.png "Example of result. "

 @see
 @ref displayLineSegments.cpp

 */



void displayLineSet(std::string fileName, std::vector<unsigned int > vectPos,
                    std::vector<unsigned int > vectColPt,
                    std::vector<unsigned int > vectCol, std::string nameInput,
                    std::string nameArgCol, Board2D &aBoard,
                    unsigned int height, bool invertYaxis,
                    bool displayEndPoint, int lineWidth=2)
{
    
  if(vectPos.size() != 0)
  {
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
  if(vectCol.size() != 0)
  {
    if(vectCol.size()!=3 )
    {
      trace.error() << "colors specification should contain R,G,B values (using default red)."<< std::endl;
    }
    lineColor.setRGBi(vectCol[0], vectCol[1], vectCol[2], 255);
  }
  if(vectColPt.size() != 0)
  {
    if(vectColPt.size()!=3)
    {
      trace.error() << "colors specification should contain R,G,B values (using default red)."<< std::endl;
    }
    pointColor.setRGBi(vectColPt[0], vectColPt[1], vectColPt[2], 255);
  }
    
    
  for(unsigned int i=0; i<vectPt1.size(); i++)
  {
    Z2i::Point pt1 (vectPt1[i][0], invertYaxis? height - vectPt1[i][1]: vectPt1[i][1] );
    Z2i::Point pt2 (vectPt2[i][0], invertYaxis? height - vectPt2[i][1]: vectPt2[i][1] );
    if(displayEndPoint)
    {
      aBoard.setPenColor(pointColor);
      aBoard.fillCircle(pt1[0], pt1[1], 2*lineWidth);
      aBoard.fillCircle(pt2[0], pt2[1], 2*lineWidth);
    }
    aBoard.setPenColor(lineColor);
    aBoard.setLineWidth(lineWidth);
    aBoard.drawLine(pt1[0], pt1[1], pt2[0], pt2[1]);
  }
    
}


int main( int argc, char** argv )
{
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::stringstream ssDescr;
  ssDescr << "Display line segments from input file where each segment is represented in one line."
          << "Typical use example:\n \t  ./visualisation/displayLineSegments  ../Samples/lines.sdp  --SDPindex 1 2 3 4 --invertYaxis  lines.eps \n";
  
  app.description(ssDescr.str());
  std::string inputFileName;
  std::string inputSec;
  std::string output;
  std::string outputPNG;
  std::string outputEPS;
  std::string outputSVG;
  std::string backgroundImage;
  std::string outputStreamEPS;
  std::string outputStreamFIG;
  std::string outputStreamSVG;
  
  double lineWidth {1.0};
  double lineWidthSetB {1.0};
  double scale=1.0;
  double alpha=1.0;
  bool invertYaxis {false};
  bool noXFIGHeader {false};
  bool noDisplayEndPoints {false};
  bool noDisplayEndPointsSec {false};
  unsigned int height = 0;
  std::vector<unsigned int> sdpIndexVect;
  std::vector<unsigned int> vectDomain;
  std::vector<unsigned int> vectCustomLineColor;
  std::vector<unsigned int> vectCustomLineColorSec;
  std::vector<unsigned int> vectCustomPointColor;
  
  app.add_option("-i,--input,1", inputFileName, "the input file containing the segments x1 y1 x2 y2 to be displayed. " )
      ->required()
      ->check(CLI::ExistingFile);
  
  app.add_option("--inputSec,-s", inputSec,
                 "the second input set file containing the segments x1 y1 x2 y2 to be displayed. ");
  app.add_option("--SDPindex", sdpIndexVect, "specify the sdp index of segment endpoints (by default 0,1,2,3).")
  ->expected(4);
  app.add_option("--domain",vectDomain, "limit the export to a given domain (xmin ymin xmax ymax)." )
  ->expected(4);
  app.add_option("--lineWidth", lineWidth, "Define the linewidth of the segments.");
  app.add_option("--lineWidthSec", lineWidthSetB, "Define the linewidth of the segments.");
  app.add_flag("--noXFIGHeader",noXFIGHeader, " to exclude xfig header in the resulting output stream (no effect with option -outputFile).");
  app.add_option("--customLineColor",vectCustomLineColor, "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). ")
  ->expected(4);
  app.add_option("--customLineColorSec", vectCustomLineColorSec, "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). ")
  ->expected(4);
  app.add_option("--customPointColor", vectCustomPointColor,  "set the R, G, B, A components of the colors of the mesh faces and eventually the color R, G, B, A of the mesh edge lines (set by default to black). " )
  ->expected(4);
  app.add_flag("--noDisplayEndPoints,-e", noDisplayEndPoints, "to not display segment end points.");
  app.add_flag("--noDisplayEndPointsSec", noDisplayEndPointsSec, "to not display segment end points of second set.");
  app.add_option("--outputFile,-o,2",output, "<filename> save output file automatically according the file format extension.");
  
  app.add_flag("--outputStreamEPS", outputStreamEPS, "specify eps for output stream format.");
  app.add_flag("--outputStreamSVG", outputStreamSVG, "specify svg for output stream format.");
  app.add_flag("--outputStreamFIG", outputStreamFIG, "specify fig for output stream format.");
  app.add_flag("--invertYaxis", invertYaxis, "invertYaxis invert the Y axis for display contours (used only with --SDP)");


#ifdef WITH_CAIRO
  app.add_option("--outputPNG", outputPNG, "outputPNG <filename> specify pdf format.");
#endif

  app.add_option("--backgroundImage", backgroundImage, "backgroundImage <filename> : display image as background");
  app.add_option("--alphaBG", alpha, "alphaBG <value> 0-1.0 to display the background image in transparency (default 1.0), (transparency works only if cairo is available)");
  
  app.add_option("--scale",scale, "scale <value> 1: normal; >1 : larger ; <1 lower resolutions  )", true );
  
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  
  Board2D aBoard;
  aBoard.setUnit (0.05*scale, LibBoard::Board::UCentimeter);
  if(vectDomain.size()>0)
  {
    if(vectDomain.size()!=4)
    {
      trace.error() << "you need to specify the four values for the domain." << std::endl;
      return 0;
    }
    aBoard.setClippingRectangle(vectDomain[0],vectDomain[1],vectDomain[2],vectDomain[3]);
  }
  

  if(backgroundImage != "")
  {
    typedef ImageSelector<Z2i::Domain, unsigned char>::Type Image;
    Image img = DGtal::GenericReader<Image>::import( backgroundImage );
    Z2i::Point ptInf = img.domain().lowerBound();
    Z2i::Point ptSup = img.domain().upperBound();
    unsigned int width = abs(ptSup[0]-ptInf[0]+1);
    height = abs(ptSup[1]-ptInf[1]+1);
    aBoard.drawImage(backgroundImage, 0-0.5,height-0.5, width, height, -1, alpha );
  }


  if(inputFileName != "")
  {
 
    displayLineSet(inputFileName, sdpIndexVect, vectCustomPointColor, vectCustomLineColor, "input","customLineColor",  aBoard, height, invertYaxis, !noDisplayEndPoints, lineWidth);
  }
  if(inputSec != "")
  {
    displayLineSet(inputSec,sdpIndexVect, vectCustomPointColor, vectCustomLineColorSec , "inputSec","customLineColorSec", aBoard, height, invertYaxis,!noDisplayEndPointsSec, lineWidthSetB);
  }

 
  
  if(output != "")
  {
    std::string extension = output.substr(output.find_last_of(".") + 1);
     
    if(extension=="svg")
    {
      aBoard.saveSVG(output.c_str());
    }
#ifdef WITH_CAIRO
    else
      if (extension=="eps")
      {
        aBoard.saveCairo(output.c_str(),Board2D::CairoEPS );
      }
      else if (extension=="pdf")
      {
        aBoard.saveCairo(output.c_str(),Board2D::CairoPDF );
      }
      else if (extension=="png")
      {
        aBoard.saveCairo(output.c_str(),Board2D::CairoPNG );
      }
#endif
      else if(extension=="eps")
      {
        aBoard.saveEPS(output.c_str());
      }
      else if(extension=="fig")
      {
        aBoard.saveFIG(output.c_str(),LibBoard::Board::BoundingBox, 10.0, !noXFIGHeader);
      }
  }
    
  if (outputStreamSVG != "")
  {
    aBoard.saveSVG(std::cout);
  }
  else if (outputStreamFIG != "")
  {
    aBoard.saveFIG(std::cout, LibBoard::Board::BoundingBox, 10.0,  !noXFIGHeader);
  }
  else if (outputStreamEPS != "")
  {
    aBoard.saveEPS(std::cout);
  }
  return 0;
}


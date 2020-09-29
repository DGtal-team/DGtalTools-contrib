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
 * @file displayTgtCoverAlphaTS.cpp

 * @author Bertrand Kerautret (\c kerautre@loria.fr)
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/17/04
 *
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>

#include "CLI11.hpp"

//STL
#include <vector>
#include <string>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/topology/helpers/Surfaces.h"

//image
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/Color.h"
#include "DGtal/io/readers/GenericReader.h"

#include "DGtal/geometry/curves/SaturatedSegmentation.h"
#include "DGtal/io/boards/Board2D.h"
#include "DGtal/io/boards/CDrawableWithBoard2D.h"

// For tangential cover
#include "DGtal/geometry/curves/SaturatedSegmentation.h"


using namespace DGtal;


///////////////////////////////////////////////////////////////////////////////
/**

 @code
 Display tangentical cover. 
 Typical use example:    displayTgtCoverAlphaTS [options] --input  <fileName> 

Usage: ./displayTgtCoverAlphaTS [OPTIONS] 1 2

Positionals:
  1 TEXT:FILE REQUIRED                  the input contour.
  2 TEXT REQUIRED                       the output eps file name.
  w FLOAT=1                             width.

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         the input contour.
  -o,--output TEXT REQUIRED             the output eps file name.
  -f,--outputFIG TEXT                   the output fig file name.
  --openContour                         consider the contour as open.
  --index UINT                          the index of the interest point.
  --width FLOAT=1                       width.
  -e,--euclideanThickness               use euclidean thickness instead horizontal/vertical.
 
 */



template<typename TIterator, typename TComputer>
void 
drawPencil(Board2D &aBoard, TComputer computer,
           unsigned int index, TIterator itBegin, TIterator itEnd){  
  firstMaximalSegment(computer, itBegin+index, itBegin, itBegin);
  TComputer first (computer);
  lastMaximalSegment(computer, itBegin+index, itBegin, itEnd);
  TComputer last (computer);       
  while(first.end() != last.end()){
    aBoard << SetMode(first.className(), "BoundingBox");
    aBoard << first;
    nextMaximalSegment(first, itEnd);
  }
  aBoard << SetMode(first.className(), "BoundingBox");
  aBoard << first;
}



int main( int argc, char** argv )
{
  typedef Z2i::RealPoint Point;
  typedef std::vector<Point>::const_iterator RAConstIterator;
  typedef Circulator<RAConstIterator> ConstCirculator;
  typedef AlphaThickSegmentComputer<Point, ConstCirculator> AlphaThickSegmentComputer2D;
  typedef SaturatedSegmentation<AlphaThickSegmentComputer2D> AlphaSegmentation;

  typedef AlphaThickSegmentComputer<Point> AlphaThickSegmentComputer2DOpen;
  typedef SaturatedSegmentation<AlphaThickSegmentComputer2DOpen> AlphaSegmentationOpen;
 
  
  // parse command line CLI----------------------------------------------
  CLI::App app;
  app.description("Display tangentical cover. \n Typical use example: \t displayTgtCoverAlphaTS [options] --input <fileName> \n");
  std::string fileName;
  std::string outFileName;
  std::string outFileNameFIG;
  double width {1.0};
  bool openContour {false};
  unsigned int index;

  app.add_option("-i,--input,1", fileName, "the input contour (FreemanChain).")->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outFileName, "the output eps file name.")->required();
  auto outFileNameFIGOpt = app.add_option("--outputFIG,-f", outFileNameFIG, "the output fig file name.");
  auto openContourOpt = app.add_flag("--openContour","consider the contour as open.");
  auto indexOpt = app.add_option("--index", index, "the index of the interest point.");
  app.add_option("--width,-w", width, "width.", true);
  auto euclideanThicknessOpt = app.add_flag("--euclideanThickness,-e", "use euclidean thickness instead horizontal/vertical.");
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
    
  double scale = 4.0;
  Board2D aBoard;
  aBoard.setUnit (0.5*scale, LibBoard::Board::UCentimeter);
  functions::Hull2D::ThicknessDefinition def = functions::Hull2D::HorizontalVerticalThickness;
  if(euclideanThicknessOpt->count()>0){
    def = functions::Hull2D::EuclideanThickness;
  }

  if(openContourOpt->count()>0){
    openContour=true;
  }
  
  std::vector<Point> aContour = PointListReader<Point>::getPointsFromFile(fileName);
   
   // Display the source contour as pixel
   for(unsigned int i =0; i< aContour.size(); i++){
     aBoard << aContour[i];
   }

   if (!openContour){
     Circulator<RAConstIterator> circu (aContour.begin(), aContour.begin(), aContour.end());
     AlphaThickSegmentComputer2D computer(width, def);
     if(indexOpt->count()>0){
       drawPencil(aBoard, computer, index, circu, circu);
     }else{
         aBoard << SetMode(computer.className(), "BoundingBox");
         AlphaSegmentation segmentator(circu, circu, computer);
         AlphaSegmentation::SegmentComputerIterator i = segmentator.begin();
         AlphaSegmentation::SegmentComputerIterator end = segmentator.end();
         for ( ; i != end; ++i) {
           AlphaThickSegmentComputer2D current(*i);
           aBoard << current;
         }
     }
   }else{
     AlphaThickSegmentComputer2DOpen computer(width, def);
     computer.init(aContour.begin());
     if(indexOpt->count()>0){
       drawPencil(aBoard, computer, index, aContour.begin(), aContour.end());
     }else{
         aBoard << SetMode(computer.className(), "BoundingBox");
         AlphaSegmentationOpen segmentator(aContour.begin(), aContour.end(), computer);
         AlphaSegmentationOpen::SegmentComputerIterator i = segmentator.begin();
         AlphaSegmentationOpen::SegmentComputerIterator end = segmentator.end();
         for ( ; i != end; ++i) {
           AlphaThickSegmentComputer2DOpen current(*i);
           aBoard << current;
         }
     }
   }
     
  
  aBoard.saveEPS(outFileName.c_str());
  
  if(outFileNameFIGOpt->count()>0){  
    aBoard.saveFIG(outFileNameFIG.c_str());
  }
  
}







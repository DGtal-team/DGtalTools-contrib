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

//boost
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

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
namespace po = boost::program_options;



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
 
  
  // parse command line ----------------------------------------------
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "the input contour. ")
    ("output,o", po::value<std::string>(), "the output eps file name. ")
    ("outputFIG,f", po::value<std::string>(), "the output fig file name. ")
    ("openContour", "consider the contour as open.")
    ("index", po::value<unsigned int>(), "the index of the interest point.")
    ("euclideanThickness,e", "use euclidean thickness instead horizontal/vertical")
    ("width,w", po::value<double>()->default_value(1.0), "width.");

  
  bool parseOK=true;
  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, general_opt), vm);  
  }catch(const std::exception& ex){
    parseOK=false;
    trace.info()<< "Error checking program options: "<< ex.what()<< std::endl;
  }

  po::notify(vm);    
  if(!parseOK||vm.count("help")||argc<=1 || (!(vm.count("input")) ))
    {
      trace.info()<< "Display Tgt cover . " <<std::endl << "Basic usage: "<<std::endl
		  << "\t Tgt cover [options] --FreemanChain  <fileName>  "<<std::endl
		  << general_opt << "\n";
      return 0;
    }
  double scale = 4.0;
  Board2D aBoard;
  aBoard.setUnit (0.5*scale, LibBoard::Board::UCentimeter);
  functions::Hull2D::ThicknessDefinition def = functions::Hull2D::HorizontalVerticalThickness;
  if(vm.count("euclideanThickness")){
    def = functions::Hull2D::EuclideanThickness;
  }
  
  std::string fileName = vm["input"].as<std::string>();
  double width = vm["width"].as<double>();
  bool openContour = vm.count("openContour");
  std::vector<Point> aContour = PointListReader<Point>::getPointsFromFile(fileName);
  std::string outFileName = vm["output"].as<std::string>();
   
   // Display the source contour as pixel
   for(unsigned int i =0; i< aContour.size(); i++){
     aBoard << aContour[i];
   }

   if (!openContour){
     Circulator<RAConstIterator> circu (aContour.begin(), aContour.begin(), aContour.end());
     AlphaThickSegmentComputer2D computer(width, def);
     if(vm.count("index")){
       unsigned int index = vm["index"].as<unsigned int>();
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
     if(vm.count("index")){
       unsigned int index = vm["index"].as<unsigned int>();
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
  
  if(vm.count("outputFIG")){
    std::string outFileNameFIG = vm["outputFIG"].as<std::string>();  
    aBoard.saveFIG(outFileNameFIG.c_str());
  }
  
}







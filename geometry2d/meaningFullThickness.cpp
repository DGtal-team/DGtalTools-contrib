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
 * @file displayMeaningfullThickness.cpp
 
 * @author Bertrand Kerautret (\c kerautre@loria.fr)
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/11/09
 *
 * This file is part of the DGtal library.
 */

///////////////////////////////////////////////////////////////////////////////
#include <iostream>

//boost
#include "CLI11.hpp"


//STL
#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"

#include "DGtal/shapes/ShapeFactory.h"
#include "DGtal/shapes/Shapes.h"
#include "DGtal/topology/helpers/Surfaces.h"

//image
#include "DGtal/io/readers/PointListReader.h"
#include "DGtal/io/Color.h"
#include "DGtal/io/readers/GenericReader.h"


#include "DGtal/io/boards/Board2D.h"
#include "DGtal/io/boards/CDrawableWithBoard2D.h"

// For tangential cover
#include "DGtal/geometry/curves/AlphaThickSegmentComputer.h"
#include "DGtal/geometry/curves/SaturatedSegmentation.h"
#include "DGtal/geometry/tools/Hull2DHelpers.h"
#include "DGtal/math/Profile.h"
#include "DGtal/math/MeaningfulScaleAnalysis.h"
#include "DGtal/base/Circulator.h"
#include "DGtal/math/MultiStatistics.h"

/**
 @page meaningFullThickness
 
 @brief  Apply the Rosin Threshold algorithm.

 @b Usage:   meaningFullThickness  [input]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  an input file.

 Options:
 Positionals:
   1 TEXT:FILE REQUIRED                  the input contour.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         the input contour.
   -o,--outputDat TEXT=output.dat        the output dat file name.
   -e,--outputEPS TEXT=output.dat        the output eps file name.
   -f,--outputFIG TEXT=output.dat        the output fig file name.
   -m,--scaleMax FLOAT=10                scale step to compute MT.
   --drawingScale FLOAT=1                scale step to compute MT.
   -s,--scaleStep FLOAT=1                scale step to compute MT.
   -E,--EuclThickness                    use the euclidean thickness instead vertical/horizontal one.
@endcode

 @b Example:

 @code
     meaningFullThickness  -i ./Samples/ellipseBruit2.sdp   -e temp.eps  -o res.dat -E
 @endcode

 @image html resmeaningFullThickness .png "Example of result. "

 @see
 @ref meaningfullThickness.cpp

 */


using namespace DGtal;

typedef functions::Hull2D::ThicknessDefinition ThickDef;

struct LogFct{
 double operator()(const double &a) const {
   return log(a);
  }
};


template <typename TInputIterator>
std::vector<Profile<LogFct> >
getScaleProfiles(std::vector<double> vectScale, TInputIterator begin, TInputIterator end, ThickDef thDef){
  unsigned int size = std::distance(begin, end);
  std::vector<Profile<LogFct> > vectResult;
  // Initialisation of the resulting scale profiles
  for (TInputIterator it = begin; it != end; it++) {
    Profile<LogFct> s;
    s.init(vectScale.size());
    vectResult.push_back(s);
  }
  // For each scale i.e thickness of MS:
  // Construct one step of the multiscale profile from the maximal tangential cover.
  MultiStatistics statistics(size);
  for(unsigned int s = 0; s < vectScale.size(); s++){
    trace.info() << "[ scale " << vectScale[s]<< " :";
    statistics.init(size, false);
    typedef Circulator<TInputIterator> ConstCirculator;
    typedef AlphaThickSegmentComputer<typename std::iterator_traits<TInputIterator>::value_type, ConstCirculator> AlphaThickSegmentComputer2D;
    typedef  SaturatedSegmentation<AlphaThickSegmentComputer2D> AlphaSegmentation;
    
    Circulator<TInputIterator> circu (begin, begin, end);
    Circulator<TInputIterator> circuInit = circu;
    AlphaThickSegmentComputer2D computer(vectScale[s], thDef);
    
    AlphaSegmentation segmentator(circu, circu, computer);
    typename AlphaSegmentation::SegmentComputerIterator it = segmentator.begin();
    typename AlphaSegmentation::SegmentComputerIterator endSeg = segmentator.end();
    
    for( ; it != endSeg; ++it){
      trace.info() << ".";
      AlphaThickSegmentComputer2D seg(*it);
      double lengthSegment = seg.getSegmentLength();
      ConstCirculator circuS = seg.begin();
      for ( ; circuS != seg.end(); ++circuS) {
        unsigned int i = std::distance(circuInit, circuS);
        statistics.addValue(i, lengthSegment/vectScale[s]);
      }
    }
    statistics.terminate();
    trace.info()<< "]" << std::endl;
    for (unsigned int i =0; i<vectResult.size(); i++) {
      vectResult[i].addValue(s, statistics.mean(i));
    }
    statistics.clear();
  }
  return vectResult;
}


template <typename TInputPoint>
std::vector<double>
getNoiseLevels(const std::vector<TInputPoint> &vectContour,
               const std::vector<double> & vectScalesValues, ThickDef thDef){
  
  std::vector<double> vectResults;
  std::vector<Profile<LogFct> > vScaleProfile = getScaleProfiles(vectScalesValues, vectContour.begin(), 
                                                                 vectContour.end(), thDef);
  for(unsigned int i =0 ;i < vectContour.size(); i++){
    MeaningfulScaleAnalysis<Profile<LogFct> > msa (vScaleProfile[i]);
    vectResults.push_back(vectScalesValues[msa.noiseLevel(1)]);
  }
  return vectResults;
}




int main( int argc, char** argv )
{
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::stringstream ssDescr;
  ssDescr << "Display Meaningful Thickness " <<std::endl << "Basic usage: "<<std::endl
  << "\t  " << argv[0] <<" -i ./Samples/ellipseBruit2.sdp   -e temp.eps  -o res.dat -E"<<std::endl;
  app.description(ssDescr.str());
  std::string inputFileName;
  std::string outFileNameFIG {"output.fig"};
  std::string outFileNameEPS {"output.eps"};
  std::string outFileNamDAT {"output.dat"};
  double scaleMax {10.0};
  double scaleDrawing {1.0};
  double scaleStep {1.0};
  bool useEuclThickness {false};
  app.add_option("-i,--input,1", inputFileName, "the input contour." )
      ->required()
  ->check(CLI::ExistingFile);
  
  auto optOutDAT = app.add_option("-o,--outputDat", outFileNamDAT, "the output dat file name.", true );
  auto optOutEPS = app.add_option("-e,--outputEPS", outFileNamDAT, "the output eps file name.", true );
  auto optOutFIG = app.add_option("-f,--outputFIG", outFileNamDAT, "the output fig file name.", true );
  app.add_option("--scaleMax,-m", scaleMax, "scale step to compute MT.", true);
  app.add_option("--drawingScale", scaleDrawing,  "scale step to compute MT.", true);
  app.add_option("--scaleStep,-s",scaleStep, "scale step to compute MT.", true);
  app.add_flag("--EuclThickness,-E", useEuclThickness, "use the euclidean thickness instead vertical/horizontal one.");
 
  functions::Hull2D::ThicknessDefinition thicknessDef = useEuclThickness ? functions::Hull2D::EuclideanThickness:
  functions::Hull2D::HorizontalVerticalThickness;
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  
  Board2D aBoard;
  aBoard.setUnit (0.5*scaleDrawing, LibBoard::Board::UCentimeter);
  
  std::vector<DGtal::Z2i::RealPoint> aContour = PointListReader<Z2i::RealPoint>::getPointsFromFile(inputFileName);
  
  
  
  
  // Getting the noise level and displaying it:
  std::vector<double> vectScales;
  for (double s = 1.0; s < scaleMax; s = s + scaleStep) {
    vectScales.push_back(s);
  }
  std::vector<double> noiseLevel = getNoiseLevels(aContour,
                                                  vectScales, thicknessDef);
  
  // displaying the noise level as box of size N:
  for(unsigned int i =0; i< aContour.size(); i++){
    aBoard.setFillColor(DGtal::Color(200, 200, 255));
    aBoard.setPenColor(DGtal::Color(150, 150, 255));
    double n = noiseLevel[i];
    aBoard.drawRectangle(aContour[i][0]-(n/2.0), aContour[i][1]+(n/2.0), n, n);
  }
  
  // Display the source contour as lines
  aBoard.setPenColor(DGtal::Color(150, 50, 50));
  for(unsigned int i =0; i< aContour.size(); i++){
    aBoard.drawLine(aContour[i][0], aContour[i][1],
                    aContour[(i+1)%aContour.size()][0], aContour[(i+1)%aContour.size()][1]);
  }
  
  aBoard.saveEPS(outFileNameEPS.c_str());
  
  if(optOutFIG->count() >0){
    aBoard.saveFIG(outFileNameFIG.c_str());
  }
  
  if(optOutDAT->count() > 0){
    std::ofstream f;
    f.open(outFileNameFIG.c_str());
    f<< "Meaningfull thickness generated from meaningfullThickness" << std::endl;
    for(unsigned int i =0; i< aContour.size(); i++){
      f << aContour[i][0] << " " <<  aContour[i][1] << " " << noiseLevel[i] << std::endl;
    }
  }
  
}


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
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

//STL
#include <vector>
#include <string>
#include <fstream>
#include <iostream>


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
#include "DGtal/geometry/helpers/ScaleProfile.h"
#include "DGtal/geometry/tools/Hull2DHelpers.h"
#include "DGtal/base/Circulator.h"
#include "DGtal/math/MultiStatistics.h"

using namespace DGtal;

typedef functions::Hull2D::ThicknessDefinition ThickDef;


template <typename TInputIterator>
std::vector<ScaleProfile>
getScaleProfiles(std::vector<double> vectScale, TInputIterator begin, TInputIterator end, ThickDef thDef){
  unsigned int size = std::distance(begin, end);
  std::vector<ScaleProfile> vectResult;
  // Initialisation of the resulting scale profiles
  for (TInputIterator it = begin; it != end; it++) {
    ScaleProfile s;
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
  std::vector<ScaleProfile> vScaleProfile = getScaleProfiles(vectScalesValues, vectContour.begin(), vectContour.end(), thDef);
  for(unsigned int i =0 ;i < vectContour.size(); i++){
    vectResults.push_back(vectScalesValues[vScaleProfile[i].noiseLevel(1)]);
  }
  return vectResults;
}






///////////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;

int main( int argc, char** argv )
{
  
  
  // parse command line ----------------------------------------------
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
  ("help,h", "display this message")
  ("input,i", po::value<std::string>(), "the input contour. ")
  ("outputDat,o", po::value<std::string>()->default_value("output.dat"), "the output dat file name. ")
  ("outputEPS,e", po::value<std::string>()->default_value("output.eps"), "the output eps file name. ")
  ("outputFIG,f", po::value<std::string>(), "the output fig file name. ")
  ("scaleMax,m", po::value<double>()->default_value(10.0), "scale step to compute MT.")
  ("drawingScale", po::value<double>()->default_value(1.0), "change the scale of out eps.")
  ("scaleStep,s", po::value<double>()->default_value(1.0), "scale step to compute MT.")
  ("EuclThickness,E", "use the euclidean thickness instead vertical/horizontal one.") ;
  
  
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
    trace.info()<< "Display Meaningful Thickness " <<std::endl << "Basic usage: "<<std::endl
                << "\t  " << argv[0] <<" -i ./Samples/ellipseBruit2.sdp   -e temp.eps  -o res.dat -E"<<std::endl
		  << general_opt << "\n";
    return 0;
  }
  
  double scaleMax = vm["scaleMax"].as<double>();
  double scaleStep = vm["scaleStep"].as<double>();
  double scaleDrawing = 1;
  std::string fileName = vm["input"].as<std::string>();
  functions::Hull2D::ThicknessDefinition thicknessDef = vm.count("EuclThickness") ? functions::Hull2D::EuclideanThickness:
  functions::Hull2D::HorizontalVerticalThickness;
  
  
  Board2D aBoard;
  aBoard.setUnit (0.5*scaleDrawing, LibBoard::Board::UCentimeter);
  
  std::vector<DGtal::Z2i::RealPoint> aContour = PointListReader<Z2i::RealPoint>::getPointsFromFile(fileName);
  std::string outFileName = vm["outputEPS"].as<std::string>();
  
  
  
  
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
  
  aBoard.saveEPS(outFileName.c_str());
  
  if(vm.count("outputFIG")){
    std::string outFileNameFIG = vm["outputFIG"].as<std::string>();
    aBoard.saveFIG(outFileNameFIG.c_str());
  }

  if(vm.count("outputDat")){
    std::string outFileNameFIG = vm["outputDat"].as<std::string>();
    std::ofstream f;
    f.open(outFileNameFIG.c_str());
    f<< "Meaningfull thickness generated from meaningfullThickness" << std::endl;
    for(unsigned int i =0; i< aContour.size(); i++){
      f << aContour[i][0] << " " <<  aContour[i][1] << " " << noiseLevel[i] << std::endl;
  }
    
  }
}







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
 * @file rosinThreshold.cpp
 * @ingroup geometry2d
 *
 * @author Van-Tho Nguyen (van-tho.nguyen@inra.fr)
 * LERFOB, AgroParisTech, INRA, 54000 NANCY France
 * 
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Lorraine, France
 *
 * 
 * @date 2017/04/11
 *
 * Source file of the tool rosinThreshold
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/base/Common.h"
#include "DGtal/io/readers/PointListReader.h"
#include <DGtal/io/readers/TableReader.h>
#include <DGtal/math/Statistic.h>

#include <math.h>

#include "CLI11.hpp"


///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////

/**
 @page rosinThreshold rosinThreshold
 
 @brief  Apply the Rosin Threshold algorithm.

 @b Usage:   rosinThreshold [input]

 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  an input file.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         an input file.
   -d,--dataIndex UINT=0                 the index to read input data.
   -b,--binSize FLOAT=1                  binSize for the x axis.
@endcode

 @b Example: 

 @code
   	rosinThreshold -i  $DGtal/examples/samples/....
 @endcode

 @image html resrosinThreshold.png "Example of result. "

 @see
 @ref rosinThreshold.cpp

 */



void
export2Text(const std::vector<std::pair<double, double> > &v, const std::string &filename){
  std::ofstream outStream;
  outStream.open(filename.c_str(), std::ofstream::out);
  for(unsigned int i = 0; i < v.size();i++){
    std::pair<double, double> pa = v.at(i);
    outStream << pa.first << " "<< pa.second <<std::endl;
  }
  outStream.close();
}


double
getThresholdByRosin(const std::vector<double> &data, double binSize){
    //build histogram
    double maxValue = *std::max_element(data.begin(), data.end());
    double minValue = *std::min_element(data.begin(), data.end());
    double range = maxValue - minValue;
    int nbInterval = range / binSize;

    std::vector<int> histogram(nbInterval, 0);
    for(unsigned int i = 0; i < data.size(); i++){
        int index = (data.at(i) - minValue)/binSize;
        histogram[index]++;
    }
    std::vector<int>::iterator maxFreq = std::max_element(histogram.begin(), histogram.end());
    int maxFreqValue = *maxFreq;
    int maxFreqIndex = std::distance(histogram.begin(), maxFreq);
    assert(maxFreqValue == histogram.at(maxFreqIndex));
   
    unsigned int lastIndex = histogram.size() - 1;
    int lastValue = histogram.at(lastIndex);
   
    
    for(unsigned int i = maxFreqIndex; i < histogram.size(); i++)
      {
        if(histogram.at(i) == 0)
          {
            lastIndex = i;
            lastValue = 0;
            break;
          }
      }
    double valueDiff = lastValue - maxFreqValue;
    double valueDiff2 = valueDiff *valueDiff;
    double indexDiff = lastIndex - maxFreqIndex;
    double indexDiff2 = indexDiff * indexDiff;
    double bestThresIndex = maxFreqIndex;
    double bestDist = 0;
    //line between maxFreq and last element of historgram
    double a = (lastValue - maxFreqValue)*1.0/(lastIndex - maxFreqIndex);
    double b = maxFreqValue - a * maxFreqIndex;
    
    for (unsigned int i = maxFreqIndex; i < lastIndex; i++)
      {
        double dist = std::abs(valueDiff *  i - indexDiff*histogram.at(i) + maxFreqValue*lastIndex - maxFreqIndex * lastValue)/
          sqrt(valueDiff2 + indexDiff2 );
        if(dist > bestDist){
          bestDist = dist;
          bestThresIndex = i;
        }
      }
    //convert index to real value
    /**
    trace.info()<<"bestIndex: "<< bestThresIndex<<std::endl; 
    trace.info()<<"maxIndex: "<< maxFreqIndex<<std::endl; 
    trace.info()<<"last val:"<<lastValue<<std::endl;
    trace.info()<<"last ind"<<lastIndex * binSize + minValue<<std::endl;
    trace.info()<<"max val:"<<maxFreqValue<<std::endl;
    trace.info()<<"max ind:"<<maxFreqIndex * binSize + minValue<<std::endl;
    trace.info()<<"bestValue: "<< histogram.at(bestThresIndex)<<std::endl; 
    trace.info()<<"bestIndex: "<< bestThresIndex * binSize + minValue<<std::endl; 
    */

    
    int bestVal = histogram.at(bestThresIndex);
    double x2 = (bestVal + bestThresIndex/a - b)/(a + 1/a);
    double y2 = b + a*x2;
    std::vector<std::pair<double, double>> forPlot;
    std::pair<double, double> maxPoint(maxFreqIndex * binSize + minValue, maxFreqValue);
    std::pair<double, double> lastPoint(lastIndex * binSize + minValue, lastValue);
    std::pair<double, double> bestPoint(bestThresIndex * binSize + minValue, bestVal);
    std::pair<double, double> projBestPoint(x2* binSize + minValue, y2);
    forPlot.push_back(maxPoint);
    forPlot.push_back(lastPoint);
    forPlot.push_back(bestPoint);
    forPlot.push_back(projBestPoint);

    
    export2Text(forPlot, "pointFile");
    //histogram
    std::vector<std::pair<double, double>> histForPlot;
    for(unsigned int i = 0; i< histogram.size(); i++){
        std::pair<double, double> aBin(i* binSize + minValue, histogram.at(i));
        histForPlot.push_back(aBin);
    }
    
    export2Text(histForPlot, "hist2d");
    return bestThresIndex*binSize + minValue;
}



int main( int argc, char** argv )
{
  
  
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputFileName;
  app.description("Applies the Rosin Treshold on an image.");
  
  double binSize {1.0};
  unsigned int indexData {0};
  
  
  app.add_option("-i,--input,1", inputFileName, "an input file." )
      ->required()
      ->check(CLI::ExistingFile);
  app.add_option("-d,--dataIndex",indexData, "the index to read input data.");
  app.add_option("--binSize,-b", binSize, "binSize for the x axis.");
    
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  // Some nice processing  --------------------------------------------------
  
  std::vector<double> vectData = TableReader<double>::getColumnElementsFromFile(inputFileName, indexData);
  double value = getThresholdByRosin(vectData, binSize);  
  DGtal::trace.info() << "Rosin Threshold: " << value << std::endl;

  return 0;
}



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
 * @file volLocalMax.cpp
 * @author Bertrand Kerautret (\c bertrand.kerautret@loria.fr)
 *
 * @date 2015/12/22
 *
 *
 * This file is part of the project DGtal-Tools-Contrib of the DGtal library.
 */


///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/GenericReader.h"
#include "DGtal/io/boards/Board3D.h"
#include "DGtal/io/DrawWithDisplay3DModifier.h"
#include "DGtal/shapes/implicit/ImplicitBall.h"
#include "DGtal/shapes/GaussDigitizer.h"


#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace std;
using namespace DGtal;
using namespace Z3i;

///////////////////////////////////////////////////////////////////////////////
namespace po = boost::program_options;

typedef ImageContainerBySTLVector<Z3i::Domain, unsigned char> Image3D;



template<typename TImage>
bool
isStrictLocalMax(const TImage &anImage, const Z3i::Point &aPoint, const double aRadius){
  typedef DGtal::ImplicitBall<DGtal::Z3i::Space> EuclideanBall;
  typedef DGtal::GaussDigitizer<DGtal::Z3i::Space, EuclideanBall> DigitalShape;
  EuclideanBall aBall (DGtal::Z3i::Point(aPoint[0], aPoint[1], aPoint[2]), aRadius);
  DigitalShape  gaussDig;
  gaussDig.attach (aBall);
  gaussDig.init(DGtal::Z3i::Point(aPoint[0]-(int)aRadius-1, aPoint[1]-(int)aRadius-1, aPoint[2]-(int)aRadius-1),
                DGtal::Z3i::Point(aPoint[0]+(int)aRadius+1, aPoint[1]+(int)aRadius+1, aPoint[2]+(int)aRadius+1), 1);
  DGtal::Z3i::Domain dom = gaussDig.getDomain();
  for( DGtal::Z3i::Domain::ConstIterator it = dom.begin(); it!=dom.end(); it++){
      if(gaussDig(*it) && anImage.domain().isInside(*it) ){
        if (anImage(aPoint) <= anImage(*it) && aPoint != *it) {
          return false;
        }
      }
  }
  return  true;
}





int main( int argc, char** argv )
{
  // parse command line ----------------------------------------------
  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
  ("help,h", "display this message")
  ("input,i", po::value<std::string>(), "vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)) file." )
  ("output,o", po::value<std::string>(), "Output SDP filename" )
  ("ballSize,b",  po::value<double>()->default_value(3.0), "set the ball size." );
  
  
  bool parseOK=true;
  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  } catch(const std::exception& ex)
  {
    parseOK=false;
    trace.info()<< "Error checking program options: "<< ex.what()<< endl;
  }
  po::notify(vm);
  if( !parseOK || vm.count("help")||argc<=1)
  {
    std::cout << "Usage: " << argv[0] << " [input-file]\n" << "extraction of (strict) local maxima of a vol image within a spherical kernel of radius '--ballSize' \n"
    << general_opt << "\n";
    return 0;
  }
  
  if(! vm.count("input"))
  {
    trace.error() << " The input filename was not defined" << endl;
    return 0;
  }
  if(! vm.count("output"))
  {
    trace.error() << " The output filename was not defined" << endl;
    return 0;
  }
  
  string inputFilename = vm["input"].as<std::string>();
  string outputFilename = vm["output"].as<std::string>();
  std::ofstream outStream;
  outStream.open(outputFilename.c_str());
  Image3D image = GenericReader<Image3D>::import (inputFilename );
  double ballSize = vm["ballSize"].as<double>();
  outStream << "# coords of local maximas (from tools volLocalMax) obtained with a ball of radius " << ballSize << std::endl;
  trace.progressBar(0, image.domain().size());
  unsigned int pos = 0;
  for( auto & pt : image.domain()){
    pos++;
    trace.progressBar(pos, image.domain().size());
    if (isStrictLocalMax(image,pt, ballSize)) {
      outStream << pt[0] << " " << pt[1] << " " << pt[2] << " " << (int)image(pt) << std::endl;
    }
  }
  outStream.close();
 
  
  return 0;
}



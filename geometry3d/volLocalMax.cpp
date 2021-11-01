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

#include "CLI11.hpp"

using namespace std;
using namespace DGtal;
using namespace Z3i;

/**
@code:
extraction of (strict) local maxima of a vol image within a spherical kernel of radius '--ballSize'.
 Usage: ./volLocalMax [input-file] [output-file]

Usage: ./volLocalMax [OPTIONS] 1 2

Positionals:
  1 TEXT:FILE REQUIRED                  vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)) file
  2 TEXT REQUIRED                       Output SDP filename

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)) file
  -o,--output TEXT REQUIRED             Output SDP filename
  -b,--ballSize FLOAT=3                 set the ball size
*/ 
///////////////////////////////////////////////////////////////////////////////

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
  // parse command line CLI-------------------------------------------------------
  CLI::App app;
  app.description("extraction of (strict) local maxima of a vol image within a spherical kernel of radius '--ballSize'.\n Usage: ./volLocalMax [input-file] [output-file]\n");
  std::string inputFilename;
  std::string outputFilename;
  double ballSize {3.0};
  
  app.add_option("--input,-i,1", inputFilename, "vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)) file")->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFilename, "Output SDP filename")->required();
  app.add_option("--ballSize,-b", ballSize, "set the ball size", true);

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  std::ofstream outStream;
  outStream.open(outputFilename.c_str());
  Image3D image = GenericReader<Image3D>::import (inputFilename );
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



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
 * @file basicMorphoFilter.cpp

 * @author Bertrand Kerautret (\c kerautre@loria.fr)
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/28/11
 *
 */


/**
 @page basicMorphoFilter basicMorphoFilter
 
 @brief  Apply the Rosin Threshold algorithm.

 @b Usage:   basicMorphoFilter [input]

 @b Allowed @b options @b are :
 
 @code
 Positionals:
   1 TEXT:FILE REQUIRED                  input file name in 3d volume.
   2 TEXT=result.vol                     export the filtered volume extracted

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         input file name in 3d volume.
   -o,--output TEXT=result.vol           export the filtered volume extracted
   -s,--sizeFilter UINT                  size of the filter
   -e,--erode                            apply erosion
   -m,--median                           apply median filter
   -d,--dilate                           apply dilatation
   -c,--closure                          apply closure
   -n,--nbRepeat UINT=1                  repeat the selected type of operation

@endcode

 @b Example:

 @code
 basicMorphoFilter ${DGtal}/examples/samples/lobster.vol -e lobsErode2_2.vol -s 2 -n 2
 @endcode

 @see
 @ref basicMorphoFilter.cpp

 */

#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/VolReader.h"
#include "DGtal/io/writers/GenericWriter.h"

#include "DGtal/images/ImageContainerBySTLVector.h"

#include "CLI11.hpp"


using namespace DGtal;

typedef typename Z3i::Point Point3D;
typedef ImageContainerBySTLVector<Z3i::Domain, unsigned char> Image3D;


// size= width/2
template<typename TImage>
TImage
applyMedian(const TImage &anImage, unsigned int size){
  unsigned int imageSize = (anImage.domain().upperBound()[0]-anImage.domain().lowerBound()[0])*
    (anImage.domain().upperBound()[1]-anImage.domain().lowerBound()[1])*
    (anImage.domain().upperBound()[2]-anImage.domain().lowerBound()[2]);
  
  trace.progressBar(0, imageSize);
  typedef typename TImage::Domain::ConstIterator ImageDomIterator;
  TImage imageRes (anImage.domain());
  unsigned int cpt = 0;
  
  for(ImageDomIterator it = anImage.domain().begin(); it != anImage.domain().end(); ++it){
    trace.progressBar(cpt, imageSize);
    Point3D pt = *it;
    std::vector<unsigned char> vectVal;
    for (int k = -(int)size; k <= (int) size; k++) {
      for (int l = -(int)size; l <= (int) size; l++) {
        for (int m = -(int)size; m <= (int) size; m++) {
          Point3D p (((int)(pt[0]))+k, (int)(pt[1])+l, ((int)(pt[2]))+m);
          if(anImage.domain().isInside(p) && (p-pt).norm() < size ){
            vectVal.push_back(anImage(p));
          }          
        }
      }
    }
    std::sort(vectVal.begin(), vectVal.end());
    imageRes.setValue(pt, vectVal[vectVal.size()/2.0]);
    cpt++;
  }
  return imageRes;
}



// size= width/2
template<typename TImage>
TImage
applyErodeDilate(const TImage &anImage, unsigned int size, bool isErode){
  unsigned int imageSize = (anImage.domain().upperBound()[0]-anImage.domain().lowerBound()[0])*
    (anImage.domain().upperBound()[1]-anImage.domain().lowerBound()[1])*
    (anImage.domain().upperBound()[2]-anImage.domain().lowerBound()[2]);
 
  trace.progressBar(0, imageSize);
  typedef typename TImage::Domain::ConstIterator ImageDomIterator;
  TImage imageRes (anImage.domain());
  unsigned int cpt = 0;

  for(ImageDomIterator it = anImage.domain().begin(); it != anImage.domain().end(); ++it){
    trace.progressBar(cpt, imageSize);
    Point3D pt = *it;
    unsigned char extremVal = isErode?  std::numeric_limits<unsigned char>::max():0;
    for (int k = -(int)size; k <= (int) size; k++) {
      for (int l = -(int)size; l <= (int) size; l++) {
        for (int m = -(int)size; m <= (int) size; m++) {
          Point3D p (((int)(pt[0]))+k, (int)(pt[1])+l, ((int)(pt[2]))+m);
          if (anImage.domain().isInside(p) && (p-pt).norm() < size ) {
            if(extremVal > anImage(p) && isErode){
              extremVal = anImage(p);
            }
            if(extremVal < anImage(p) && !isErode){
              extremVal = anImage(p);
            }
          }
        }
      }
    }
    
    imageRes.setValue(pt, extremVal);
    cpt++;
  }
  return imageRes;
}



int
main(int argc,char **argv)

{
  typedef typename Image3D::Domain::ConstIterator ImageDomIterator;
  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  app.description("Apply basic morpho filter from a ball structural element.\n Example:"
                  "./geometry3d/basicMorphoFilter ${DGtal}/examples/samples/lobster.vol -e lobsErode2_2.vol -s 2 -n 2");
  std::string inputFileName;
  std::string outputFileName {"result.vol"};
  unsigned int sizeFilter;
  unsigned int nbRepeat {1};
  bool erode {false};
  bool dilate {false};
  bool median {false};
  bool closure {false};
  
  
  app.add_option("-i,--input,1", inputFileName, "input file name in 3d volume." )
  ->required()
  ->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFileName, "export the filtered volume extracted");
  app.add_option("--sizeFilter,-s", sizeFilter, "size of the filter");
  app.add_flag("--erode,-e", erode, "apply erosion");
  app.add_flag("--median,-m", median, "apply median filter");
  app.add_flag("--dilate,-d", dilate, "apply dilatation");
  app.add_flag("--closure,-c", "apply closure");
  app.add_option("--nbRepeat,-n",nbRepeat, "repeat the selected type of operation");
  
  
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------
  
  Image3D inputImage = VolReader<Image3D>::importVol(inputFileName);
  Image3D imageRes(inputImage);

  for(ImageDomIterator it = inputImage.domain().begin(); it != inputImage.domain().end(); it++){
    imageRes.setValue(*it,inputImage(*it));
  }

  for (unsigned int i=0; i<nbRepeat; i++) {    
    if(median){
      trace.info() << std::endl;
      trace.info() << "applying " << "median" << std::endl;
      imageRes = applyMedian(imageRes, sizeFilter);
      trace.info() << std::endl;
      
    }
    else if (!closure) {
      trace.info() << std::endl;
      trace.info() << "applying " << (erode ? "erosion": "dilatation") << std::endl;
      imageRes = applyErodeDilate(imageRes, sizeFilter, !dilate);
      trace.info() << std::endl;

    }else if(!median) {
      trace.info() << "applying " << "closure" << std::endl;
      trace.info() << "applying " << "dilate" << std::endl;
      imageRes = applyErodeDilate(imageRes, sizeFilter, false);
      trace.info() << "applying " << "erode" << std::endl;
      imageRes = applyErodeDilate(imageRes, sizeFilter, true);
    }
  }

  trace.info() << std::endl;
  imageRes >>  outputFileName;
    
}

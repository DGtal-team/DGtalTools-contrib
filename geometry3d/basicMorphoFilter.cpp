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


#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/VolReader.h"
#include "DGtal/io/writers/GenericWriter.h"

#include "DGtal/images/ImageContainerBySTLVector.h"
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

using namespace DGtal;
namespace po = boost::program_options;

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
          if(anImage.domain().isInside(p)){
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
          if (anImage.domain().isInside(p)) {
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

  po::options_description general_opt("Allowed options are: ");
  general_opt.add_options()
    ("help,h", "display this message")
    ("input,i", po::value<std::string>(), "input file name in 3d volume.")
    ("sizeFilter,s", po::value<unsigned int>(), "size of the filter")
    ("erode,e",  "apply erosion")
    ("median,m",  "apply median filter")
    ("dilate,d",  "apply dilatation")
    ("closure,c", "apply closure")
    ("nbRepeat,n", po::value<unsigned int>()->default_value(1), "repeat the selected type of operation" )
    ("output,o", po::value<std::string>(),  "export the filtered volume extracted");
    
  bool parseOK=true;
  po::variables_map vm;
  try{
    po::store(po::parse_command_line(argc, argv, general_opt), vm);
  }catch(const std::exception& ex){
    trace.info()<< "Error checking program options: "<< ex.what()<< std::endl;
    parseOK=false;
  }
  po::notify(vm);
  if(vm.count("help")||argc<=1|| !parseOK )
  {
    trace.info()<< "Apply basic morpho filter from a cubical structurant element" <<std::endl << "Options: "<<std::endl
		  << general_opt << "\n";
    return 0;
  }
    
  std::string inputVol = vm["input"].as<std::string>();
  std::string outputVol = vm["output"].as<std::string>();
  bool erode = vm.count("erode");
  bool dilate = vm.count("dilate");
  bool closure = vm.count("closure");
  bool median = vm.count("median");
  unsigned int nbRepeat = vm["nbRepeat"].as<unsigned int>();
  unsigned int size = vm["sizeFilter"].as<unsigned int>();    
  
  Image3D inputImage = VolReader<Image3D>::importVol(inputVol);
  Image3D imageRes(inputImage);

  for(ImageDomIterator it = inputImage.domain().begin(); it != inputImage.domain().end(); it++){
    imageRes.setValue(*it,inputImage(*it));
  }

  for (unsigned int i=0; i<nbRepeat; i++) {    
    if(median){
      trace.info() << std::endl;
      trace.info() << "applying " << "median" << std::endl;
      imageRes = applyMedian(imageRes, size);
      trace.info() << std::endl;
      
    }
    else if (!closure) {
      trace.info() << std::endl;
      trace.info() << "applying " << (erode ? "erosion": "dilatation") << std::endl;
      imageRes = applyErodeDilate(imageRes, size, !dilate);
      trace.info() << std::endl;

    }else if(!median) {
      trace.info() << "applying " << "closure" << std::endl;
      trace.info() << "applying " << "dilate" << std::endl;
      imageRes = applyErodeDilate(imageRes, size, false);
      trace.info() << "applying " << "erode" << std::endl;
      imageRes = applyErodeDilate(imageRes, size, true);
    }
  }

  trace.info() << std::endl;
  imageRes >>  outputVol;
    
}





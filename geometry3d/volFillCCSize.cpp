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
 * @file volFillCCSize.cpp
 * @author Bertrand Kerautret (\c bertrand.kerautret@univ-lyon2.fr)
 *
 * @date 2021/10/07
 *
 *
 * This file is part of the project DGtal-Tools-Contrib of the DGtal library.
 */


///////////////////////////////////////////////////////////////////////////////
#include <iostream>
#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/GenericReader.h"
#include "DGtal/io/writers/GenericWriter.h"
#include "DGtal/images/ConstImageAdapter.h"

#include "CLI11.hpp"

using namespace std;
using namespace DGtal;
using namespace Z3i;

/**
 @code:
Fills each Connected Components by using as intensity the number of voxels of the CC. The input file is supposed to be segmented (ie each CC is represented by its labels (integer)).
Usage: ./geometry3d/volFillCCSize [OPTIONS] 1 2

Positionals:
  1 TEXT:FILE REQUIRED                  vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims))
  2 TEXT REQUIRED                       Output volume file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)).

Options:
  -h,--help                             Print this help message and exit
  -i,--input TEXT:FILE REQUIRED         vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims))
  -o,--output TEXT REQUIRED             Output volume file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)).
  --bgValue INT=0                       Consider this value as background in order to ignore it from the filling.
  --rescale                             Rescale the output to fit unsigned char image representation.

 */
///////////////////////////////////////////////////////////////////////////////

typedef ImageContainerBySTLVector<Z3i::Domain, unsigned char> Image3D;
typedef ImageContainerBySTLVector<Z3i::Domain, unsigned int> Image3DI;



template<typename TImage, typename TImageOut>
unsigned int
intensityFromNbVoxCC(const TImage &anImage,  TImageOut &anImageOut, unsigned int bg = 0){
  auto maxLabel = *(std::max_element(anImage.begin(), anImage.end()));
  //tab representing for each intensity label, the number of voxels that belongs to the CC.
  std::vector< DGtal::uint64_t> nbVox(static_cast< DGtal::uint64_t>(maxLabel));
  DGtal::trace.beginBlock("Counting CC size");
  DGtal::trace.info() << "Found #CC: " << static_cast<unsigned int>(maxLabel) << std::endl;
  DGtal::trace.progressBar(0, anImage.domain().size());
  unsigned int i = 0;
  unsigned int max = 0;
  for(auto &p: anImage.domain()){
    DGtal::trace.progressBar(i, anImage.domain().size());
    nbVox[anImage(p)]++;
    if (nbVox[anImage(p)] > max && anImage(p) != bg )
    {
      max = nbVox[anImage(p)];
    }
    i++;
  }
  DGtal::trace.endBlock();

  DGtal::trace.beginBlock("Filling CC using #CC");
  DGtal::trace.progressBar(0, anImage.domain().size());
  i=0;
  for(auto &p: anImage.domain()){
    DGtal::trace.progressBar(i, anImage.domain().size());
    if (anImage(p) != bg){
      anImageOut.setValue(p,  static_cast< DGtal::uint64_t>(nbVox[anImage(p)]));
    }
    i++;
  }
  DGtal::trace.endBlock();
  return max;

}

int main( int argc, char** argv )
{
  // parse command line CLI-------------------------------------------------------
  CLI::App app;
  app.description("Fills each Connected Component with a value corresponding to the number of voxels of the CC. The input file is supposed to be segmented (ie each CC is represented by its labels (integer)).");
  std::string inputFilename;
  std::string outputFilename;
  int bgValue {0};
  bool rescale {false};
  
  app.add_option("--input,-i,1", inputFilename, "vol file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims))")->required()->check(CLI::ExistingFile);
  app.add_option("--output,-o,2", outputFilename, "Output volume file (.vol) , pgm3d (.p3d or .pgm3d, pgm (with 3 dims)).")->required();
  app.add_option("--bgValue",bgValue, "Consider this value as background in order to ignore it from the filling.", true );
  app.add_flag("--rescale", rescale, "Rescale the output to fit unsigned char image representation." );
  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------

  Image3DI image = GenericReader<Image3DI>::import(inputFilename);

  if(!rescale)
  {
    Image3DI imageOut (image.domain());
    unsigned int m = intensityFromNbVoxCC(image, imageOut, bgValue);
    DGtal::trace.info() << "nb CC max: " << m << std::endl;
    GenericWriter<Image3DI>::exportFile(outputFilename, imageOut);
  }
  else
  {
    Image3DI imageOut (image.domain());
    unsigned int m = intensityFromNbVoxCC(image, imageOut, bgValue);
    DGtal::trace.info() << "nb CC max: " << m << std::endl;
    typedef DGtal::functors::Rescaling<unsigned int ,unsigned char> RescalFCT;
    typedef ConstImageAdapter<Image3DI, Image3D::Domain, functors::Identity, unsigned char, RescalFCT> ImageAdapt;
    functors::Identity id;
    RescalFCT rescale (0, m, 0,255);
    ImageAdapt img (imageOut,imageOut.domain(), id, rescale );
    GenericWriter<ImageAdapt>::exportFile(outputFilename, img);
  }
    return 0;
}



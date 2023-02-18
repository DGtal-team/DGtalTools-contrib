#include <iostream>
#include <fstream>

#include "DGtal/base/Common.h"
#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"

#include "CLI11.hpp"


#include "DGtal/shapes/Mesh.h"

using namespace DGtal;


/**
 @page basicEditMesh meshBasicEdit.cpp
 
 @brief  Apply the Rosin Threshold algorithm.

 @b Usage:   meshBasicEdit.cpp [input]

 @b Allowed @b options @b are :
 
 @code
 Positionals:
   1 TEXT:FILE REQUIRED                  input file name of mesh vertex given as OFF format.
   —-output TEXT                       arg = file.off : export the resulting mesh associated to the fiber extraction.

 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         input file name of mesh vertex given as OFF format.
   -o TEXT                               arg = file.off : export the resulting mesh associated to the fiber extraction.
   -s,--shrinkArea FLOAT x 7             arg = <dist> <bounding box> apply a mesh shrinking on the defined area.
   -b,--shrinkBallArea FLOAT x 5         arg = <dist> <x> <y> <z> <radius> apply a mesh shrinking on the  area defined by a ball centered at x y z.
   --filterVisiblePart FLOAT             arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx, ny, nz and a maximal angle).
   -x,--nx FLOAT=0                       arg = define the nx of the direction of filtering, see --filterVisiblePart.
   -y,--ny FLOAT=0                       arg = define the ny of the direction of filtering, see --filterVisiblePart.
   -z,--nz FLOAT=1                       arg = define the nz of the direction of filtering, see --filterVisiblePart.
   --scale FLOAT                         change the scale factor
   --filterFirstFaces FLOAT              arg= X : filters the X% of the first faces of the input mesh.
   --filterNbFaces FLOAT                 arg = X % limits the number of face by keeping only X percent of faces.


@endcode

 @b Example:

 @code
   meshBasicEdit.cpp ${DGtal}/examples/samples/tref.off --filterVisiblePart 0.3 toto.offmeshBasicEdit.cpp -i
 @endcode


 @see
 @ref meshBasicEdit.cpp.cpp

 */
int
main(int argc,char **argv)

{
  typedef typename Z3i::RealPoint TPoint;

  // parse command line using CLI ----------------------------------------------
  CLI::App app;
  std::string inputMeshName;
  std::string outputMeshName{"result.obj"};
  std::vector<double> vectDistAndBBox;
  std::vector<double> paramBallArea;
  double theMaxAngle;
  double nx {0};
  double ny {0};
  double nz {1.0};
  double scale;
  double percentFirst;
  double percent;
  unsigned int rescaleToCube {100};
  std::vector<unsigned int> rescaleInterToCube;

  app.description("Apply basic mesh edition (scale change, mesh face contraction, face filtering).\n"
                  "Example: ./geometry3d/meshBasicEdit.cpp ${DGtal}/examples/samples/tref.off --filterVisiblePart 0.3 resultFiltered.off");

  app.add_option("-i,--input,1", inputMeshName, "input file name of mesh vertex given as OFF format." )
      ->required()
      ->check(CLI::ExistingFile);
  
  app.add_option("-o,--output,2",outputMeshName,"arg = file.off : export the resulting mesh associated to the fiber extraction.");
  app.add_option("--shrinkArea,-s",vectDistAndBBox,"arg = <dist> <bounding box> apply a mesh shrinking on the defined area.")
  ->expected(7);
  app.add_option("--shrinkBallArea,-b", paramBallArea,  "arg = <dist> <x> <y> <z> <radius> apply a mesh shrinking on the  area defined by a ball centered at x y z.")
  ->expected(5);
  auto filterVisOpt = app.add_option("--filterVisiblePart", theMaxAngle , "arg = angle nx ny nz: filter the mesh visible part (according the mesh part in the direction  nx, ny, nz and a maximal angle).");
  
  app.add_option("--nx,-x", nx , "arg = define the nx of the direction of filtering, see --filterVisiblePart.", true);
  app.add_option("--ny,-y", ny, "arg = define the ny of the direction of filtering, see --filterVisiblePart.", true);
  app.add_option("--nz,-z", nz, "arg = define the nz of the direction of filtering, see --filterVisiblePart.", true);
  auto scaleOpt = app.add_option("--scale",scale, "change the scale factor" );
  auto rescaleToCubeOpt = app.add_option("--rescaleToCube", rescaleToCube, "change the scale factor of the input mesh such that its bounding box size corresponds to the size of a cube given as argument.", true);
  app.add_option("--rescaleInterToCube", rescaleInterToCube, "same than rescaleToCube but only if the bounding box max size is outside the interval given as parameters.", false)
    ->expected(2);

  auto filterFF = app.add_option("--filterFirstFaces",percentFirst,"arg= X : filters the X% of the first faces of the input mesh.");
  auto filterNBF = app.add_option("--filterNbFaces",percent,  "arg = X % limits the number of face by keeping only X percent of faces." );
  

  app.get_formatter()->column_width(40);
  CLI11_PARSE(app, argc, argv);
  // END parse command line using CLI ----------------------------------------------


 
  TPoint aNormal;
  unsigned int moduloLimitFace;
  TPoint ptUpper, ptLower;
  double distanceShrink;
  if (vectDistAndBBox.size()==7){
    distanceShrink = vectDistAndBBox[0];
    ptLower[0] = vectDistAndBBox[1];
    ptLower[1] = vectDistAndBBox[2];
    ptLower[2] = vectDistAndBBox[3];
    ptUpper[0] = vectDistAndBBox[4];
    ptUpper[1] = vectDistAndBBox[5];
    ptUpper[2] = vectDistAndBBox[6];
  }

  
  TPoint ballCenter;
  double  radius;
  if (paramBallArea.size()==5){
    distanceShrink = paramBallArea[0];
    radius = paramBallArea[4];
    ballCenter[0] = (int) paramBallArea[1];
    ballCenter[1] = (int) paramBallArea[2];
    ballCenter[2] = (int) paramBallArea[3];
  }

  
  if(filterVisOpt->count()>0){
    std::vector<double> vectNormalAndAngle;  
    aNormal[0] = nx;
    aNormal[1] = ny;
    aNormal[2] = nz;
    aNormal /= aNormal.norm();
  }
  if(filterNBF->count()>0){
    moduloLimitFace = (int)(100.0/percent);
  } 
  
  DGtal::Mesh<Z3i::RealPoint> theMesh(true);
  theMesh << inputMeshName;
  if (rescaleToCubeOpt->count() >0  || rescaleInterToCube.size() != 0 )
  {
      auto bb = theMesh.getBoundingBox();
      auto s = bb.second-bb.first;
      auto maxSize = *s.maxElement();
      if (rescaleInterToCube.size() == 0 || (maxSize > rescaleInterToCube[1] || maxSize < rescaleInterToCube[0])) {
          scale = rescaleToCube/(double)maxSize;
      }
  }

  DGtal::Mesh<Z3i::RealPoint> theNewMesh(true);

  
  trace.info()<< "reading the input mesh ok: "<< theMesh.nbVertex() <<  std::endl;
  
  
  for (DGtal::Mesh<Z3i::RealPoint>::VertexStorage::const_iterator it = theMesh.vertexBegin();
       it != theMesh.vertexEnd(); it++){
    theNewMesh.addVertex(*it);
  }
  unsigned int numMaxFaces = theMesh.nbFaces();
  if (filterFF-> count() >0 )
    {
      numMaxFaces = (numMaxFaces/100.0)*percentFirst;
    }
  unsigned int num =0;
  for (DGtal::Mesh<Z3i::RealPoint>::FaceStorage::const_iterator it = theMesh.faceBegin();  
       it!= theMesh.faceEnd(); it++){
    num++;
    if(num>numMaxFaces){
      break;
    }
    DGtal::Mesh<Z3i::RealPoint>::MeshFace aFace = *it;
    bool okOrientation = true;
    TPoint p0 = theMesh.getVertex(aFace.at(1));
    TPoint p1 = theMesh.getVertex(aFace.at(0));
    TPoint p2 = theMesh.getVertex(aFace.at(2));
    TPoint vectNormal = ((p1-p0).crossProduct(p2 - p0)).getNormalized();    
    vectNormal /= vectNormal.norm();
    if (filterVisOpt->count()>0){
      okOrientation = vectNormal.dot(aNormal) > cos(theMaxAngle);
    }
    if( okOrientation && (filterNBF->count()==0 || num%moduloLimitFace == 0 )){
      theNewMesh.addFace(aFace);
    }
    if( paramBallArea.size() == 5){
      TPoint ptCenter = (p0+p1+p2)/3.0;
      if((ptCenter-ballCenter).norm() <= radius){
        for(unsigned int i =0; i<3; i++){
          TPoint &aVertex = theNewMesh.getVertex(aFace.at(i));
          if(aVertex==theMesh.getVertex(aFace.at(i))){
            aVertex-=vectNormal*distanceShrink;
          }
        }        
      } 
    }

    if( vectDistAndBBox.size() == 7 ){
      Z3i::Domain aDomain(ptLower, ptUpper);
      Z3i::Point ptCenter( (p0+p1+p2)/3.0, functors::Round<>());
      if(aDomain.isInside(ptCenter)){
        for(unsigned int i =0; i<3; i++){
          TPoint &aVertex = theNewMesh.getVertex(aFace.at(i));
          if(aVertex==theMesh.getVertex(aFace.at(i))){
            aVertex-=vectNormal*distanceShrink;
          }
        }
      }
    }
    
  }  

  if( scaleOpt->count()>0 || theMesh.nbFaces() != 0)
    {
      for(unsigned int i =0; i<theNewMesh.nbVertex(); i++)
        {
          theNewMesh.getVertex(i) *= scale;
 
        }
    }
  trace.info()<< "nbFaces init: " << theMesh.nbFaces() << std::endl;
  trace.info()<< "New nbFaces: " << theNewMesh.nbFaces() << std::endl;
  theNewMesh >> outputMeshName; 
  
  
}

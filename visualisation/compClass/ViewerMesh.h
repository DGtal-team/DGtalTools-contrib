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
 * @file ViewerMesh.h
 * @author Bertrand Kerautret (\c kerautre@loria.fr )
 * LORIA (CNRS, UMR 7503), University of Nancy, France
 *
 * @date 2015/12/20
 *
 * An simple mesh viewer which allow basic edition (coloring, face removing).
 *
 * This file is part of the DGtal library.
 */

#if defined(ViewerMesh_RECURSES)
#error Recursive header files inclusion detected in ViewerMesh.h
#else // defined(ViewerMesh_RECURSES)
/** Prevents recursive inclusion of headers. */
#define ViewerMesh_RECURSES


#if !defined ViewerMesh_h
#define ViewerMesh_h
/** Prevents repeated inclusion of headers. */


#include "DGtal/helpers/StdDefs.h"
#include "DGtal/io/viewers/Viewer3D.h"
#include "DGtal/io/Display3D.h"
#include "DGtal/images/ImageSelector.h"
#include "DGtal/images/ImageContainerBySTLVector.h"
#include "DGtal/images/ConstImageAdapter.h"
#include <deque>

enum EditMode {SELECT_MODE, COLOR_MODE};


template < typename Space = DGtal::Z3i::Space, typename KSpace = DGtal::Z3i::KSpace>
class ViewerMesh: public DGtal::Viewer3D <Space, KSpace>
{

  static const unsigned int MAXUNDO=10;
  typedef DGtal::Mesh<DGtal::Z3i::RealPoint> RealMesh;


public:
  
  ViewerMesh(RealMesh &aMesh, std::string outMeshName): myPenScale(1.0), myPenColor(DGtal::Color::Blue),myPenSize(5.0),
                                                        myMesh(aMesh), myOutMeshName(outMeshName),
                                                        myMode(COLOR_MODE)
  {
  }
  
  void postSelection(const QPoint& point);
  void deleteFacesFromDist(DGtal::Z3i::RealPoint p);
  void addToSelected(DGtal::Z3i::RealPoint p);
  void deleteCurrents();
  void displaySelectionOnMesh();
  void setSelectMode();
  void setColorMode();
  void undo();
  void save();
  void filterVisibleFaces(const double anAngleMax);
  
  
  double myPenScale;
  DGtal::Color myPenColor;
  double myPenSize;

  
protected:
  virtual QString helpString() const;  
  virtual void keyPressEvent ( QKeyEvent *e );
  virtual void init();
  void addCurrentMeshToQueue();  
  
  RealMesh &myMesh;
  std::string myOutMeshName;
  EditMode myMode;
  std::vector<unsigned int> myVectFaceSelected;
  std::deque<RealMesh> myUndoQueue;
  std::deque<std::vector<unsigned int>> myUndoQueueSelected;
  
};


#endif // undefined viewer3dimage
#undef ViewerMesh_RECURSES
#endif // else defined(ViewerMesh_RECURSES)







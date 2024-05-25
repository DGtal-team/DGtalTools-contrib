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
 * @file
 * @ingroup visualisation
 * @author Bertrand Kerautret (\c bertrand.kerautret@univ-lyon2.fr )
 *
 *
 * @date 2023/11/17
 *
 * Source file of the tool polyMeshColorize
 *
 * This file is part of the DGtal library/DGtalTools-contrib Project.
 */

///////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/helpers/Shortcuts.h>
#include <DGtal/helpers/ShortcutsGeometry.h>
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"

#include <polyscope/polyscope.h>
#include <polyscope/surface_mesh.h>
#include <polyscope/pick.h>
#include <polyscope/point_cloud.h>

#include <algorithm>

#include "CLI11.hpp"

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace DGtal;
///////////////////////////////////////////////////////////////////////////////


/**
 @page polyMeshColorize polyMeshColorize
 
 @brief  polyMeshColorize tool to colorize a mesh (faces). Note that the process relies on the halfedge data structure that can fail if the input is not topologically consistent. If you want use other type of mesh, you can use meshViewerEdit that is based on the simple soup of triangles process (slower selection process).


 @b Usage:   polyMeshColorize [OPTIONS] 1 [2]


 @b Allowed @b options @b are :
 
 @code
 
 Positionals:
   1 TEXT:FILE REQUIRED                  an input mesh file in .obj or .off format.
   2 TEXT:FILE=result.obj                an output mesh file in .obj or .off format.
   
 
 Options:
   -h,--help                             Print this help message and exit
   -i,--input TEXT:FILE REQUIRED         an input mesh file in .obj or .off format.
   -o,--output TEXT:FILE=result.obj      an output mesh file in .obj or .off format.
 @endcode

 @b Example:

 @code
    polyMeshColorize $DGtal/examples/samples/bunnyhead.obj  bunnyEdited.obj
 @endcode

 @image html respolyMeshColorize.png "Example of result. "

 @see
 @ref polyMeshColorize.cpp

 */


typedef PolygonalSurface<Z3i::RealPoint> PolySurface;

static PolySurface currentPolysurf;

static float minPaintRad = 1.0;
static float maxPaintRad = 100.0;
static float paintRad = 1.0;

static float color1Selected[] = {0.0,0.0,1.0};
static float color2Selected[] = {0.0,1.0,0.0};
static float color3Selected[] = {0.0,1.0,1.0};

static std::map<DGtal::Color, unsigned int> indexColoring;
static DGtal::Mesh<DGtal::Z3i::RealPoint> srcInputMesh (true);
static DGtal::Mesh<DGtal::Z3i::RealPoint> inputMesh (true);

std::vector<unsigned long> vectSel;
std::vector<std::array<double, 3>> fColor;

static std::string outputFileName {"result.obj"};


void updateColorDisplay(){
    polyscope::removeStructure("fColor");
    auto digsurf = polyscope::getSurfaceMesh("InputMesh");
    digsurf->addFaceColorQuantity("fColor", fColor);
    digsurf->setAllQuantitiesEnabled(true);
}


void importMeshColor(const DGtal::Mesh<DGtal::Z3i::RealPoint> &aMEsh){
    fColor.resize(aMEsh.nbFaces());
    for ( unsigned int i = 0; i< aMEsh.nbFaces(); i++ )
    {
        auto c = aMEsh.getFaceColor(i);
        fColor[i] = {c.red()/255.0, c.green()/255.0, c.blue()/255.0};
    }
}


void addSurfaceInPolyscope(PolySurface &psurf){
    std::vector<std::vector<std::size_t>> faces;
    for(auto &face: psurf.allFaces())
    {
        faces.push_back(psurf.verticesAroundFace( face ));
    }
    auto digsurf = polyscope::registerSurfaceMesh("InputMesh",
                                                  psurf.positions(), faces);
    digsurf->addFaceColorQuantity("fColor", fColor);
    digsurf->setAllQuantitiesEnabled(true);

}

Z3i::RealPoint
getFaceBarycenter(const PolySurface &polysurff, const PolySurface::Face &aFace) {
    Z3i::RealPoint res(0.0, 0.0, 0.0);
    for(auto const &v: polysurff.verticesAroundFace(aFace)){
        res += polysurff.position(v);
    }
    return res/polysurff.verticesAroundFace(aFace).size();
}

// Helper function
std::vector<PolySurface::Face> faceAround(const PolySurface &polysurff,
                                          PolySurface::Face faceId,
                                          double radius){
    std::vector<PolySurface::Face> result;
    std::queue<PolySurface::Vertex> q;
    std::map<PolySurface::Face, bool> fVisited;
    std::map<PolySurface::Face, bool> vVisited;

    for (auto const &v : polysurff.verticesAroundFace(faceId) )
    {
        q.push(v);
    }
    fVisited[faceId] = true;
    bool addNewFaces = true;
    while (!q.empty() )
    {
        PolySurface::Vertex v = q.front(); q.pop();
        auto listFace = polysurff.facesAroundVertex(v);
        for (auto const & f : listFace)
        {
            if (fVisited.count(f)==0)
            {
                if((getFaceBarycenter(polysurff, f) -
                    getFaceBarycenter(polysurff, faceId)).norm() < radius)
                {
                    fVisited[f]=true;
                    result.push_back(f);
                    for (auto const & v : polysurff.verticesAroundFace(f))
                    {
                        if (vVisited.count(v)==0)
                        {
                            vVisited[v]=true;
                            q.push(v);
                        }
                    }
                }
            }
        }
    }
    return result;
}


std::map<DGtal::Color, unsigned int>
getIndexColors(const DGtal::Mesh<DGtal::Z3i::RealPoint> &aMEsh){
    std::map<DGtal::Color, unsigned int> indexCol;
    for ( unsigned int i = 0; i< aMEsh.nbFaces(); i++ )
    {
        auto c = aMEsh.getFaceColor(i);
        if(! indexCol.count(c))
        {
            indexCol[c] += 1;
        }
    }
    unsigned int max = 0;
    for (auto it = indexCol.begin(); it != indexCol.end(); it++)
    {
        if ((*it).second > max)
        {
            max = (*it).second;
        }
    }
    return indexCol;
}



void
drawSelection() {
    for(auto i: vectSel){
        auto c = inputMesh.getFaceColor(i);
        fColor[i] = {c.red()/255.0-0.3, c.green()/255.0-0.3, c.blue()/255.0-0.3};
    }
}

void
resetSelection() {
    for ( unsigned int i = 0; i< vectSel.size(); i++ )
    {
        auto c = inputMesh.getFaceColor(vectSel[i]);
        fColor[vectSel[i]] = {c.red()/255.0, c.green()/255.0, c.blue()/255.0};
    }
    vectSel.clear();
}

void
colorizeSelectedFaces(const DGtal::Color &c) {
    for(auto i: vectSel){
        inputMesh.setFaceColor(i, c);
        fColor[i] = {c.red()/255.0, c.green()/255.0, c.blue()/255.0};
    }
    vectSel.clear();
}
  
void callbackFaceID() {
    srand((unsigned) time(NULL));
    ImGui::Begin("Editing tools");
    ImGui::Text("Setting selection size:");
    ImGui::SliderFloat("radius values", &paintRad, minPaintRad, maxPaintRad, "size = %.3f");
    ImGui::Separator();
  
    ImGui::Text("Action:");
    if (ImGui::Button("Clear selection"))
    {
        resetSelection();
    }
    ImGui::SameLine();
   
    ImGui::Text("Color selection ");
    ImGui::ColorEdit3("color 1", color1Selected);
    if (ImGui::Button("colorize 1"))
    {
        colorizeSelectedFaces(DGtal::Color(color1Selected[0]*255,
                                           color1Selected[1]*255,
                                           color1Selected[2]*255) );
        updateColorDisplay();

    }
    ImGui::ColorEdit3("color 2", color2Selected);
    if (ImGui::Button("colorize 2"))
    {
        colorizeSelectedFaces(DGtal::Color(color2Selected[0]*255,
                                           color2Selected[1]*255,
                                           color2Selected[2]*255) );
        updateColorDisplay();

    }

    ImGui::ColorEdit3("color 3", color3Selected);
    if (ImGui::Button("colorize 3"))
    {
        colorizeSelectedFaces(DGtal::Color(color3Selected[0]*255,
                                           color3Selected[1]*255,
                                           color3Selected[2]*255) );
        updateColorDisplay();

    }
    
    ImGui::Separator();
    ImGui::Text("IO");

    if (ImGui::Button("save in .obj"))
    {
        inputMesh >> outputFileName;
    }
    ImGui::SameLine();

    if (ImGui::Button("reload src"))
    {
        inputMesh = srcInputMesh;
        importMeshColor(inputMesh);
    }
    ImGui::Separator();
    ImGui::Text("Polyscope interface:");

    if (ImGui::Button("show "))
    {
        polyscope::options::buildGui=true;
    }
    ImGui::SameLine();
    if (ImGui::Button("hide"))
    {
        polyscope::options::buildGui=false;
    }

    ImGuiIO& io = ImGui::GetIO();
    if (io.MouseDoubleClicked[0])
    {
        unsigned long indexSelect = polyscope::pick::getSelection().second;
        unsigned long nb = 0;
        // face selected
        if (indexSelect >= currentPolysurf.nbVertices())
        {
            nb = (unsigned long) polyscope::pick::getSelection().second - currentPolysurf.nbVertices();
        }
        else
        {
            // vertex selected (selecting a face connected to it)
            if(currentPolysurf.facesAroundVertex(polyscope::pick::getSelection().second)
               .size()> 0)
            {
                nb = currentPolysurf.facesAroundVertex(polyscope::pick::getSelection().second)[0];
            }
        }

        if (nb > 0 && nb < fColor.size())
        {
            auto fVois = faceAround(currentPolysurf, nb, paintRad);
            vectSel.push_back(nb);
            for (auto f: fVois)
            {
                vectSel.push_back(f);
            }
            drawSelection();
            updateColorDisplay();
        }
    }
    ImGui::End();
}





int main(int argc, char** argv)
{
    std::string inputFileName {""};
    
    // parse command line using CLI ----------------------------------------------
    CLI::App app;
    app.description("polyMeshColorize tool to colorize a mesh (faces). Note that the process relies on the halfedge data structure that can fail if the input is not topologically consistant. If you want use other type of mesh, you can use meshViewerEdit that is based on the simple soup of triangles process (slower selection process). \n"
                    " polyMeshColorize $DGtal/examples/samples/bunnyhead.obj  bunnyColored.obj \n");
    app.add_option("-i,--input,1", inputFileName, "an input mesh file in .obj or .off format." )
    ->required()
    ->check(CLI::ExistingFile);
    app.add_option("-o,--output,2", outputFileName, "an output mesh file in .obj or .off format.", true );
    
    
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    polyscope::options::programName = "polyMeshColorize - (DGtalToolsContrib)";
    polyscope::init();
    polyscope::options::buildGui=false;
    // read input mesh
    inputMesh << inputFileName;
    inputMesh.removeIsolatedVertices();
    srcInputMesh = inputMesh;
    
    auto bb = inputMesh.getBoundingBox();
    indexColoring = getIndexColors(inputMesh);
    auto it = indexColoring.begin();
    if (it!= indexColoring.end()) {
        color1Selected[0] = (*it).first.red()/255.0;
        color1Selected[1] = (*it).first.green()/255.0;
        color1Selected[2] = (*it).first.blue()/255.0;
        it++;
    }
    if (it!= indexColoring.end()) {
        color2Selected[0] = (*it).first.red()/255.0;
        color2Selected[1] = (*it).first.green()/255.0;
        color2Selected[2] = (*it).first.blue()/255.0;
        it++;
        if (it!= indexColoring.end()) {
            color3Selected[0] = (*it).first.red()/255.0;
            color3Selected[1] = (*it).first.green()/255.0;
            color3Selected[2] = (*it).first.blue()/255.0;
        }
    }
    // Setting scale mesh dependant parameters
    minPaintRad = (bb.second - bb.first).norm()/1000.0;
    maxPaintRad = (bb.second - bb.first).norm()/2.0;
    paintRad = (bb.second - bb.first).norm()/50.0;
    
    DGtal::MeshHelpers::mesh2PolygonalSurface(inputMesh, currentPolysurf);
    importMeshColor(inputMesh);
    polyscope::state::userCallback = callbackFaceID;
    addSurfaceInPolyscope(currentPolysurf);

    polyscope::show();
    return 0;
    
}

#include <iostream>
#include <DGtal/base/Common.h>
#include <DGtal/helpers/StdDefs.h>
#include <DGtal/helpers/Shortcuts.h>
#include <DGtal/helpers/ShortcutsGeometry.h>
#include "DGtal/io/readers/MeshReader.h"
#include "DGtal/io/writers/MeshWriter.h"
#include "DGtal/io/readers/PointListReader.h"


#include <polyscope/polyscope.h>
#include <polyscope/surface_mesh.h>
#include <polyscope/pick.h>
#include <polyscope/point_cloud.h>

#include "CLI11.hpp"


using namespace DGtal;
using namespace Z3i;

typedef PolygonalSurface<Z3i::RealPoint> PolySurface;

static PolySurface polysurf;
static PolySurface reconsSelectSurf;
static  std::vector<std::vector<std::size_t>> facesSelectSurf;

static std::vector<int> vectSelection;

static  int shiftFaceIndex  = 0;

static Z3i::RealPoint
getFaceBarycenter(PolySurface &polysurf, const PolySurface::Face &aFace){
    Z3i::RealPoint res(0.0, 0.0, 0.0);
    for(auto const &v: polysurf.verticesAroundFace(aFace)){
        res += polysurf.positions()[v];
    }
    double s = (double) polysurf.verticesAroundFace(aFace).size();
    return res/s;
}

// Helper function
std::vector<PolySurface::Face> faceAround( PolySurface &polysurf, PolySurface::Face faceId,
                                    double radius){
    std::vector<PolySurface::Face> result;
    std::queue<PolySurface::Vertex> q;
    std::map<PolySurface::Face, bool> fVisited;
    std::map<PolySurface::Face, bool> vVisited;

    for (auto const &v : polysurf.verticesAroundFace(faceId) ){
        q.push(v);
    }
    fVisited[faceId] = true;
    bool addNewFaces = true;
    while (!q.empty() ) {
        PolySurface::Vertex v = q.front(); q.pop();
        auto listFace = polysurf.facesAroundVertex(v);
        for (auto const & f : listFace){
            if (fVisited.count(f)==0){
                if((getFaceBarycenter(polysurf, f) -
                    getFaceBarycenter(polysurf, faceId)).norm() < radius){
                    fVisited[f]=true;
                    result.push_back(f);
                    for (auto const & v : polysurf.verticesAroundFace(f)){
                        if (vVisited.count(v)==0){
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


void callbackFaceID() {
    ImGuiIO& io = ImGui::GetIO();
    auto digsurf = polyscope::getSurfaceMesh("InputMesh");
    if (io.MouseDoubleClicked[0]) {
        unsigned long indexSelect = polyscope::pick::getSelection().second;
       if (indexSelect < shiftFaceIndex){
            std::cout << "vertices selected" << std::endl;
        }else{
            int nb = (int) polyscope::pick::getSelection().second - shiftFaceIndex;
            std::cout << "face selected " << nb  << std::endl;
            if (nb > 0 && nb < vectSelection.size()){
                auto fVois = faceAround(polysurf, nb, 2);
                std::cout << "face around " << fVois.size()  << std::endl;
                vectSelection[nb] = 0;
                for (auto f: fVois){
                    vectSelection[f] = 50;
                }
            }
        }
    }
    digsurf -> removeQuantity("selection");
    digsurf -> addFaceScalarQuantity("selection", vectSelection);
}





int main(int argc, char** argv)
{
    std::string inputFileName {""};
    std::string colorFileName; // The file containing the index to be colored in the colors

    // parse command line using CLI ----------------------------------------------
    CLI::App app;
      
    app.description("Test display accumulation\n"
                    " testPolyscope -i file.obj  \n");
    app.add_option("-i,--input,1", inputFileName, "an input mesh file in .obj format." )
    ->required()
    ->check(CLI::ExistingFile);
   
   
    app.get_formatter()->column_width(40);
    CLI11_PARSE(app, argc, argv);
    polyscope::init();

    // read input mesh
    DGtal::Mesh<DGtal::Z3i::RealPoint> aMesh(true);
    aMesh << inputFileName;
    
    DGtal::MeshHelpers::mesh2PolygonalSurface(aMesh, polysurf);
    DGtal::MeshHelpers::mesh2PolygonalSurface(aMesh, reconsSelectSurf);
    shiftFaceIndex = aMesh.nbVertex();
    std::vector<std::vector<std::size_t>> faces;
    for(auto &face: polysurf.allFaces()){
        faces.push_back(polysurf.verticesAroundFace( face ));
        vectSelection.push_back(200);
    }
    polyscope::state::userCallback = callbackFaceID;
    vectSelection[0]=0;
    auto digsurf = polyscope::registerSurfaceMesh("InputMesh", polysurf.positions(), faces);
    digsurf->addFaceScalarQuantity("selection", vectSelection);
    digsurf->setAllQuantitiesEnabled(true);
    polyscope::show();

    return 0;
}


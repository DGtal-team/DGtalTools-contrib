# DGtalTools-contrib  1.5 (beta)

- *global*
  - Continuous integration fix using new version on conan following DGtal changes.
    (Bertrand Kerautret [#90](https://github.com/DGtal-team/DGtalTools-contrib/pull/90))



# DGtalTools-contrib  1.4 
- *build*
  - Remove STBimage preprocessor instruction used to fix MVSC that is 
    no more usefull since DGtal PR [175](https://github.com/DGtal-team/DGtal/pull/1715) 
    (Bertrand Kerautret [#79](https://github.com/DGtal-team/DGtalTools-contrib/pull/79))

- *global*
  - Continuous integration on windows does not use Appveyopr anymore but Github Actions.
   (Bertrand Kerautret [#77](https://github.com/DGtal-team/DGtalTools-contrib/pull/77))

- *Geometry2d*
  - rotNmap: new tools to apply a rotation in the input normal map (it
    rotates both the position and normals orientation to be
    consistant. (Bertrand Kerautret [#70](https://github.com/DGtal-team/DGtalTools-contrib/pull/70))
    
- *Geometry3d*
  - basicEditMesh: improvement of mesh read using generic reader/writer. 
    (Bertrand Kerautret [#72](https://github.com/DGtal-team/DGtalTools-contrib/pull/72))
  - trunkMeshTransform: new tools to transform trunk mesh from input centerline and cylinder coordinates.
    (Bertrand Kerautret [#82](https://github.com/DGtal-team/DGtalTools-contrib/pull/82))

- *visualisation*
  - graphViewer: new options to apply a scale factors on the radius input values
    and to display constant tube section. (Bertrand Kerautret [#74](https://github.com/DGtal-team/DGtalTools-contrib/pull/74))
  - polyMeshColorize: new tool to colorize a mesh. (Bertrand Kerautret [#83](https://github.com/DGtal-team/DGtalTools-contrib/pull/83)
  - polyMeshEdit: new tool to edit mesh based on polyscope (Bertrand Kerautret [#78](https://github.com/DGtal-team/DGtalTools-contrib/pull/78)
   
 
# DGtalTools-contrib  1.3 
 
  - meshBasicEdit: new name of basicEditMesh and addition of a new
    option to rescale a shape according to its bounding box and a new size.
    (Bertrand Kerautret [#73](https://github.com/DGtal-team/DGtalTools-contrib/pull/73))
 
- *global*
  - Continuous integration does not use Travis anymore but Github Actions.
   (Bertrand Kerautret [#58](https://github.com/DGtal-team/DGtalTools-contrib/pull/58))

 - *Geometry3d*
  - basicMorphoFilter: fix a bug on the dilate/erode options. 
    (Bertrand Kerautret [#74](https://github.com/DGtal-team/DGtalTools-contrib/pull/74))  
  - graph2vol: new tool to transform graph based object into volumetric object. 
    (Bertrand Kerautret [#76](https://github.com/DGtal-team/DGtalTools-contrib/pull/76))
  - meshAxisCutter: new tool that cuts the input mesh according one axis into several separate meshes.
   (Bertrand Kerautret [#71](https://github.com/DGtal-team/DGtalTools-contrib/pull/71))
  - meshAxisCutter: new option to select range meshes.
   (Bertrand Kerautret [#80](https://github.com/DGtal-team/DGtalTools-contrib/pull/80))
  - computeMeshDistances: Reduce the complexity using face mapping.
   (Bertrand Kerautret [#81](https://github.com/DGtal-team/DGtalTools-contrib/pull/81))

 - *visualisation*
  - polyMeshEdit: tool to edit a mesh (add local noise, remove selected faces).
    (Bertrand Kerautret [#78](https://github.com/DGtal-team/DGtalTools-contrib/pull/78))
  - polyMeshEdit: fix preformance issue in display loop.
    (Bertrand Kerautret [#84](https://github.com/DGtal-team/DGtalTools-contrib/pull/84))
  - polyMeshColorize: tool to colorize a mesh.
    (Bertrand Kerautret [#83](https://github.com/DGtal-team/DGtalTools-contrib/pull/83))

# DGtalTools-contrib  1.3 

- *global*
  - Continuous integration does not use Travis anymore but Github Actions.
   (Bertrand Kerautret [#58](https://github.com/DGtal-team/DGtalTools-contrib/pull/58))

- *Geometry3d*
  - volFillCCSize: new tool to fill each Connected Component with a value corresponding to the number of voxels of the CC.
    (Bertrand Kerauret [60](https://github.com/DGtal-team/DGtalTools-contrib/pull/60)
  - vol2meshAndNormals: new tool to estimate normal vectors (using VCM) from digital surface defined in 3D volume. It exports the mesh and the normal vectors. (backport Jacques-Olivier Lachaud from VCM repository and update with CLI)
    (Bertrand Kerautret and Jacques-Olivier Lachaud [#65](https://github.com/DGtal-team/DGtalTools-contrib/pull/65))
  - obj2off: converts an obj mesh to the .off format.
    (Bertrand Kerautret [#61](https://github.com/DGtal-team/DGtalTools-contrib/pull/61))
    
- *visualisation*
  - displayLineSegments: fix variable name and remove unused variables.
    (Phuc Ngo [#59](https://github.com/DGtal-team/DGtalTools-contrib/pull/59))
  - Fix autoEdge option of GraphViewer  (inverted interpretation from CLI transform)
    (Bertrand Kerautret with Craig Hiernard [#64](https://github.com/DGtal-team/DGtalTools-contrib/pull/64))



# DGtalTools-contrib  1.2

- *global*
  - Fix Appveyor and Travis  wioth old default osx_image with xcode12.2 and remove non used boost
    cmake references. (Bertrand Kerautret [#53](https://github.com/DGtal-team/DGtalTools-contrib/pull/53)) 




# DGtalTools-contrib  1.1

- *global*
  -  New way to handle command line options of DGtalTools-contrib by using CLI11 instead
     boost program option. (Phuc Ngo and Bertrand Kerautret 
     [#51](https://github.com/DGtal-team/DGtalTools-contrib/pull/51))     

- *visualisation*
  - graphViewer: fix compilation issue.
    (Phuc Ngo  [#47](https://github.com/DGtal-team/DGtalTools-contrib/pull/47))
  - graphViewer: fix display issue.
    (Phuc Ngo  [#46](https://github.com/DGtal-team/DGtalTools-contrib/pull/46))
    
- *global*
  - fix Cmake and CPP11 check
  (Bertrand Kerautret [#48](https://github.com/DGtal-team/DGtalTools-contrib/pull/48))


- *Geometry3d*
  - fix off2obj: the colors of .off are now well exported in obj.
   (Bertrand Kerautret [#49](https://github.com/DGtal-team/DGtalTools-contrib/pull/49))   


# DGtalTools-contrib  1.0
- *visualisation*
  - displayLineSegments: new option to display a second set of lines.
    (Bertrand Kerautret, 
    [#43](https://github.com/DGtal-team/DGtalTools-contrib/pull/43))
 - displaySet2dPts: new tool to display sets of 2d points.
    (Bertrand Kerautret, 
    [#44](https://github.com/DGtal-team/DGtalTools-contrib/pull/44))
- *global*
  - Fix PointVector implicit conversion (in link to DGtal PR #1345)
    (Bertrand Kerautret 
    [#45](https://github.com/DGtal-team/DGtalTools/pull/45))

# DGtalTools-contrib  0.9.4
- *global*
  - Fix for the last QGLViewer version (2.7). (Bertrand Kerautret, [#40](https://github.com/DGtal-team/DGtalTools-contrib/pull/40))

- *visualisation*
  - graphViewer: new option to generate edges from vertex points and to 
    customize vertex color (Bertrand Kerautret, 
    [#33](https://github.com/DGtal-team/DGtalTools-contrib/pull/33))

- *Geometry2d*
  - LUTBasedNSDistanceTransform: Removed the dependency to libNetPBM. (Nicolas Normand,
  [#32](https://github.com/DGtal-team/DGtalTools-contrib/pull/32))
  - thresholdRosin: implementation of Rosin's algorithm to compute threshold of an unimodal intensity histogram (Van-Tho
    Nguyen and Bertrand Kerautret, [#36](https://github.com/DGtal-team/DGtalTools-contrib/pull/36))

- *Geometry3d*
 - off2sdp: a new tool to convert a mesh into a set of points (.sdp). It can
    extract the mesh vertices (by default) or the face centers. 
    (Bertrand Kerautret, [33](https://github.com/DGtal-team/DGtalTools-contrib/pull/33)) 
 - off2obj: tool to convert a mesh represented in off format into obj format.
  (Bertrand Kerautret, [#35](https://github.com/DGtal-team/DGtalTools-contrib/pull/32))
  

# DGtalTools-contrib  0.9.3

- *visualisation*
  - displayLineSegments: add a new tool allowing to display line segment in an output image. (Bertrand Kerautret)
  - meshViewerEdit: add a button to invert the current selection (Bertrand Kerautret)
  
- *Geometry2d*
  - houghLineDetect: to detect line segment from Hough transform (using OpenCV). 
  

# DGtalTools-contrib  0.9.2

- *visualisation*
  - graphViewer: add a new tool allowing to display graph from edges, vertex and radii.
    (Adrien Krahenbuhl)

- *Geometry3d*
  - xyzScale: a basic tool to adjust the scale of an xyz file. (Bertrand Kerautret)


# DGtalTools-contrib  0.9.1

- *Geometry2d*
 - meaningFullThickness: to display the meaningful thickness of digital contour. (Bertrand Kerautret)
 - LUTBasedNSDistanceTransform: Compute the 2D translated neighborhood-sequence distance transform of a binary image. (Nicolas Normand)
 - CumulativeSequenceTest and RationalBeattySequenceTest: tests from LUTBasedNSDistanceTransform. (Nicolas Normand)

- *Geometry3d*
 - basicEditMesh: to apply basic mesh edition (scale change, mesh face contraction, face filtering). (Bertrand Kerautret)
 - computeMeshDistances: computes for each face of a mesh A the minimal distance to another mesh B. (Bertrand Kerautret)
 - volLocalMax: extract the local maximas of a vol image within a spherical kernel. (Bertrand Kerautret)
 - basicMorphoFilter: apply basic morpho filter from a ball structural element. (Bertrand Kerautret)


- *visualisation*
 - displayTgtCoverAlphaTS: to display alpha-thick segment given on a simple contour. (Bertrand Kerautret)
 - meshViewerEdit: tool to visualize a mesh and to apply simple edits (face removal, color edits...). (Bertrand Kerautret)


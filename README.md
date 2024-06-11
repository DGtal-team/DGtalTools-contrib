DGtalTools-contrib
==================


Various tools from the community built using DGtal. Contrary to the DGtalTools project, these tools are not necessary final but can be considered as development tools or prototypes used to share recent research in progress.


More Information
----------------
* Release 1.3 [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.7392143.svg)](https://doi.org/10.5281/zenodo.7392143)
* Release 1.2 [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4893600.svg)](https://doi.org/10.5281/zenodo.4893600)
* Release 1.1.1 DOI [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4395059.svg)](https://doi.org/10.5281/zenodo.4395059)
* Release 0.9.4.1 DOI [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1207828.svg)](https://doi.org/10.5281/zenodo.1207828)
* Release 0.9.4 DOI [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1203421.svg)](https://doi.org/10.5281/zenodo.1203421)
* Release 0.9.3 DOI [![DOI](https://zenodo.org/badge/43562168.svg)](https://zenodo.org/badge/latestdoi/43562168)
* Release 0.9.2 DOI [![DOI](https://zenodo.org/badge/4888/DGtal-team/DGtalTools-contrib.svg)](https://zenodo.org/badge/latestdoi/4888/DGtal-team/DGtalTools-contrib)
* Release 0.9.1 DOI [![DOI](https://zenodo.org/badge/doi/10.5281/zenodo.45131.svg)](http://dx.doi.org/10.5281/zenodo.45131)
* Continuous Integration (Linux/MacOS/Windows) ![Build status](https://github.com/DGtal-team/DGtalTools-contrib/actions/workflows/build.yml/badge.svg)


Organisation
============

This project is actually organized as follows:

 - [Geometry2d](#geometry2d)

 - [Geometry3d](#geometry3d)

 - [Visualisation](#visualisation)



Geometry2d
----------

This section can contains various utilities related to 2d geometry (2d estimators, 2d contour tools, ...):
   - houghLineDetect: line detection from the hough transform (from OpenCV implementation, use cmake option:-DWITH_OPENCV=true).
   - meaningFullThickness: to display the meaningful thickness of digital contour.
     [Bertrand Kerautret, Jacques-Olivier Lachaud and  Mouhammad Said;
      ~Meaningful Thickness Detection on Polygonal Curve~ ;
      Proceedings of the 1st International Conference on Pattern Recognition Applications and Methods
       2012 pp. 372--379)]

   - distanceTransform:

      - LUTBasedNSDistanceTransform: Compute the 2D translated
        neighborhood-sequence distance transform of a binary image.
      - CumulativeSequenceTest and RationalBeattySequenceTest: tests from
        LUTBasedNSDistanceTransform.
   - rosinThreshold : applies the Rosin Threshold algorithm on a simple file of data values.
   - rotNmap: Applies a rotation in the input normal map (it rotates both the position and normals orientation to be consistant).
   
| ![](https://cloud.githubusercontent.com/assets/772865/12481234/048994c0-c048-11e5-8c64-0e6baea4c62c.png)  |
| :-: |
| meaningFullThickness |




Geometry3d
----------

As the previous section but in 3d, it contains actually these tools:

   - basicMorphoFilter: apply basic morpho filter from a ball structural element.
   - computeMeshDistances: computes for each face of a mesh A the minimal distance to another mesh B.
   - graph2vol: converts graph object into volumetric image.
   - meshAxisCutter: cuts the input mesh according one axis into sereral separate meshes.
   - meshBasicEdit: to apply basic mesh edition (scale change, mesh face contraction, face filtering).
   - volLocalMax: extract the local maximas of a vol image within a spherical kernel.
   - xyzScale: a basic tool to adjust the scale of an xyz file.
   - off2obj: tool to convert a mesh represented in off format into obj format.
   - obj2off: tool to convert a .obj mesh into the .off format.
   - off2sdp: converts a mesh into a set of points (.sdp).
   - trunkMeshTransform: tools to transform trunk mesh from input centerline and cylinder coordinates.
   - volFillCCSize: fills each Connected Component with a value corresponding to the number of voxels of the CC.

<table><tr>
<td> <img width="268" src="https://cloud.githubusercontent.com/assets/772865/12481207/d20d246c-c047-11e5-8986-ae17a582c977.png"  </td>
<td> <img width="268" alt="result after olFillCSSize" src="https://user-images.githubusercontent.com/772865/138257925-60b3d30e-3a00-46ee-b9bc-fc44dff65f65.png">
<img width="268" alt="input vol" src="https://user-images.githubusercontent.com/772865/138257856-5d7a134f-3d0c-41ce-86a4-bfff6b126d14.png"></td>
</tr>
<tr>
<td> computeMeshDistances </td> <td>volFillCCSize  (visualisation of input (left) and result (right)</td>
</table>


Visualisation
-------------

This section, can contain all tools related to visualisation:
   - displayLineSegments: to display lines as vectorial image.
   - displayTgtCoverAlphaTS: to display alpha-thick segment given on a simple contour.
   - meshViewerEdit: tool to visualize a mesh and to apply simple edits (face removal, color edits...).
   - graphViewer: tool to display graphs from a list of edges, a list of vertex and an optionnal list of radius for each edge.
   - polyMeshEdit: tool to edit a mesh (add local noise, remove selected faces).
   - polyMeshColorize: tool to  colorize a mesh.
<table>
<tr>
  <td><img width="260" src="https://cloud.githubusercontent.com/assets/772865/12538777/cd8c2d28-c2e2-11e5-93ab-cb4a6cfadc8e.png"></td>
  <td> <img width="260" src="https://cloud.githubusercontent.com/assets/772865/12523276/22205f46-c156-11e5-827d-ec788baf7029.png"> </td>
  <td><img width="260" src="https://cloud.githubusercontent.com/assets/772865/13536787/02f16df6-e242-11e5-8541-88f8ca3f0a48.png"></td>
  <td><img width="260" src="https://github.com/DGtal-team/DGtalTools-contrib/assets/772865/10592c26-40b4-4cd4-8118-b30a82ab0be3"></td>
</tr> 
 <tr>
 <td>displayTgtCoverAlphaTS</td>
 <td>meshViewerEdit</td>
 <td>graphViewer</td>
 </tr>
  <tr>
   <td><img width="260" src="https://github.com/DGtal-team/DGtalTools-contrib/assets/772865/5da7e052-d332-4c1e-95bd-bfd06d4cd1e7"></td>   
  </tr>
 <tr>
 <td>polyMeshEdit</td><td>polyMeshColorize</td>
 </tr>
</table>

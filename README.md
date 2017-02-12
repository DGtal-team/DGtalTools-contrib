DGtalTools-contrib
==================


Various tools from the community built using DGtal. Contrary to the DGtalTools project, these tools are not necessary final but can be considered as development tools or prototypes used to share recent research in progress.


More Information
----------------
* Release 0.9.3 DOI [![DOI](https://zenodo.org/badge/43562168.svg)](https://zenodo.org/badge/latestdoi/43562168)
* Release 0.9.2 DOI [![DOI](https://zenodo.org/badge/4888/DGtal-team/DGtalTools-contrib.svg)](https://zenodo.org/badge/latestdoi/4888/DGtal-team/DGtalTools-contrib)
* Release 0.9.1 DOI [![DOI](https://zenodo.org/badge/doi/10.5281/zenodo.45131.svg)](http://dx.doi.org/10.5281/zenodo.45131)
* Linux/MacOS build: [![Build Status](https://api.travis-ci.org/DGtal-team/DGtalTools-contrib.svg?branch=master)](https://travis-ci.org/DGtal-team/DGtalTools-contrib)
* Windows build: [![Build status](https://ci.appveyor.com/api/projects/status/sq9w7r1xs8078g7j?svg=true)](https://ci.appveyor.com/project/kerautret/dgtaltools-contrib)

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

| ![](https://cloud.githubusercontent.com/assets/772865/12481234/048994c0-c048-11e5-8c64-0e6baea4c62c.png)  |
| :-: |
| meaningFullThickness |




Geometry3d
----------

As the previous section but in 3d, it contains actually these tools:

   - basicEditMesh: to apply basic mesh edition (scale change, mesh face contraction, face filtering).
   - computeMeshDistances: computes for each face of a mesh A the minimal distance to another mesh B.
   - volLocalMax: extract the local maximas of a vol image within a spherical kernel.
   - basicMorphoFilter: apply basic morpho filter from a ball structural element.
   - xyzScale: a basic tool to adjust the scale of an xyz file.

| ![](https://cloud.githubusercontent.com/assets/772865/12481207/d20d246c-c047-11e5-8986-ae17a582c977.png)  |
| :-: |
| computeMeshDistances |



Visualisation
-------------

This section, can contain all tools related to visualisation:
   - displayLineSegments: to display lines as vectorial image.
   - displayTgtCoverAlphaTS: to display alpha-thick segment given on a simple contour.
   - meshViewerEdit: tool to visualize a mesh and to apply simple edits (face removal, color edits...).
   - graphViewer: tool to display graphs from a list of edges, a list of vertex and an optionnal list of radius for each edge.


|![](https://cloud.githubusercontent.com/assets/772865/12538777/cd8c2d28-c2e2-11e5-93ab-cb4a6cfadc8e.png)| ![](https://cloud.githubusercontent.com/assets/772865/12523276/22205f46-c156-11e5-827d-ec788baf7029.png) |<img width="260" alt="capture d ecran 2016-03-04 a 19 46 54" src="https://cloud.githubusercontent.com/assets/772865/13536787/02f16df6-e242-11e5-8541-88f8ca3f0a48.png">|
| :-: | :-: | :-: |
|displayTgtCoverAlphaTS|meshViewerEdit|graphViewer|

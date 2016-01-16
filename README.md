# DGtalTools-contrib

Various tools from the community built using DGtal. Contrary to the DGtalTools project, these tools are not necessary final but can be considered as development tools or prototypes used to share recent research in progress.


This project is actually organized as follows:

 - [Geometry2d](#geometry2d)

 - [Geometry3d](#geometry3d)

 - [Visualisation](#visualisation)



Geometry2d
----------

This section can contains various utilities related to 2d geometry (2d estimators, 2d contour tools, ...):

   - meaningFullThickness: to display the meaningful thickness of digital contour.
     [Bertrand Kerautret, Jacques-Olivier Lachaud and  Mouhammad Said;
      ~Meaningful Thickness Detection on Polygonal Curve~ ;
      Proceedings of the 1st International Conference on Pattern Recognition Applications and Methods
       2012 pp. 372--379)]

<center>
<table>
<tr>
<td> <img height=150 src="https://cloud.githubusercontent.com/assets/772865/12368518/6026fd32-bbec-11e5-8fa3-d1c9b51ba389.png"> </td>

</tr>
<tr>
<td>meaningFullThickness</td>
</tr>

</table>

</center>



Geometry3d
----------

As the previous section but in 3d, it contains actually these tools:

   - basicEditMesh: to apply basic mesh edition (scale change, mesh face contraction, face filtering).
   - computeMeshDistances: computes for each face of a mesh A the minimal distance to another mesh B.
   - volLocalMax: extract the local maximas of a vol image within a spherical kernel.

<center>
<table>
<tr>
<td> <img height=150 src="https://cloud.githubusercontent.com/assets/772865/12368559/cdb1da0c-bbec-11e5-8076-0a39d962f759.png"> </td>

</tr>
<tr>
<td>computeMeshDistances</td>
</tr>

</table>

</center>
Visualisation
-------------

This section, can contain all tools related to visualisation:

   - displayTgtCoverAlphaTS: to display alpha-thick segment given on a simple contour.





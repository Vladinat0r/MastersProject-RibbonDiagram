# Ribbon-Diagram-Visualisation
A Ribbon Diagram Visualisation for Interactive Molecular Docking. ("boost" library is required to run: https://www.boost.org/)

This software is written in C++ with OpenGL API and creates Ribbon Diagram visualisations of protein structures at runtime based on the PDB and DSSP files from the RCSB PDB Protein Data Bank at https://www.rcsb.org/
The files to be loaded can be changed by altering directory paths in the "BasicOpenGLTemplate.cpp" file in the "void init()" function.

Controls: -Use arrowkeys to move the camera
          -Press "O" and "P" keys to rotate the Ribbon Diagram
          -Press "+" and "-" keys to zoom in/out
          -Press "C" to toggle the Ribbon Diagram visibility
          -Press "1" and "2" keys to switch the colour schemes
          -Press "V" to toggle passive rotation of the Ribbon Diagram
          -Press "T" to toggle transparency.
          -Press "M" to toggle the Molecule model visibility

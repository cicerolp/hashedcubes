**Hashedcubes: Simple, Low Memory, Real-Time Visual Exploration of Big Data**

Hashedcubes is formulated on the simple idea that a combination of progressive partitioning and sorting, along with a compact representation of data subsets, allows to create a data structure that efficiently stores and query spatiotemporal datasets.

The underlying concept behind Hashedcubes makes it natural to create links between the data structure and original records, thus encouraging to complement the visual queries with additional information stored in an external database.

**How To Build Hashedcubes**

Hashedcubes depends on Boost 1.62 or later and CMake 3.0 or later, both on Windows and Linux.

To build and run on Linux:

1- extract csv.7z and data.7z

2- cd build

3- cmake -DCMAKE_BUILD_TYPE=Release ..

4- make

5- cd .. && cp build/hashedcubes .

6- ./hashedcubes

__Web interface: [http://localhost:8000]__

__To list all command line parameters: ./hashedcubes -h__

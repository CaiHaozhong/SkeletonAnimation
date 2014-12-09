#pragma once
#include <vector>
#include <map>
#include "boost/shared_ptr.hpp"
using std::map;
class ColladaGeometry;
class ColladaPolylist;
class ColladaTriangles;
class Point3d;
class ColladaVertex;
typedef std::string String;
typedef std::map<String, boost::shared_ptr<ColladaGeometry> > GeometriesMap;
typedef std::vector<boost::shared_ptr<ColladaPolylist> > PolylistArray;
typedef std::vector<boost::shared_ptr<ColladaTriangles> > TrianglestArray;
typedef std::vector<Point3d > Point3DArray;
typedef std::vector<boost::shared_ptr<ColladaVertex> > VertexArray;

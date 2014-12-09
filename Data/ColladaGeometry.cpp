#include "ColladaGeometry.h"
#include <stdio.h>
ColladaGeometry::ColladaGeometry()
{
}

ColladaGeometry::~ColladaGeometry()
{
}

bool ColladaGeometry::initWithGeometry(daeElement* geometry )
{	
	daeTArray<domMeshRef> meshes = geometry->getChildrenByType<domMesh>();		
	for(int i = 0; i < meshes.getCount(); i++)
	{
		if(! initWithMesh(meshes.get(i)))
			return false;
	}
	return true;
}
bool ColladaGeometry::initWithMesh(daeElement* mesh)
{
	/** 先处理一下source **/
// 	std::vector<daeElement*> sources = mesh->getDAE()->getDatabase()->typeLookup(domSource::ID());
// 	std::map<String, domList_of_floats > sourceIdFloatsMap;
// 	for(int i = 0; i < sources.size(); i++)
// 	{
// 		domSource* eachSource = (domSource*)(sources.at(i));
// 		String id = eachSource->getID();
// 		domList_of_floats f = eachSource->getFloat_array()->getValue();
// 		sourceIdFloatsMap.insert(std::map<String, domList_of_floats >::value_type(id,f));
// 	}
	/** 先存储一下顶点 **/
	daeElement* vertices = mesh->getChild("vertices");
	domListOfFloats data = getSourceArrayById(mesh,getVertexPosId(vertices));
	for(int i =0; i < data.getCount();i+=3)
	{
		boost::shared_ptr<ColladaVertex> v(new ColladaVertex);
		v->position.x = data.get(i);
		v->position.y = data.get(i+1);
		v->position.z = data.get(i+2);
		mVertexs.push_back(v);
		mSkinnedVerticesArray.push_back(v->position);
	}	
	domListOfFloats normals = ((domMesh*)mesh)->getSource_array()[1]->getFloat_array()->getValue();
	domListOfFloats texcoords = ((domMesh*)mesh)->getSource_array()[2]->getFloat_array()->getValue();

	/** 处理三角形和多边形 **/
	daeTArray<domTrianglesRef> triangles = mesh->getChildrenByType<domTriangles>();
	for(int i = 0; i < triangles.getCount(); i++)
	{
		domTrianglesRef tri = triangles.get(i);
		domPRef p = tri->getP();
		domListOfUInts pValue = p->getValue();
		for(int j = 0; j < tri->getCount(); j++)
		{
			boost::shared_ptr<ColladaTriangles> triangle(new ColladaTriangles);
			int offset = 9*j;
			triangle->vertexIndexA = pValue.get(0+offset);
			triangle->vertexIndexB = pValue.get(3+offset);
			triangle->vertexIndexC = pValue.get(6+offset);
			
			/*triangle->normal.push_back(getPointFromArrayByIndex(normals,pValue.get(1+offset)));
			triangle->normal.push_back(getPointFromArrayByIndex(normals,pValue.get(4+offset)));
			triangle->normal.push_back(getPointFromArrayByIndex(normals,pValue.get(7+offset)));			

			triangle->texcoord.push_back(getPointFromArrayByIndex(texcoords,pValue.get(2+offset)));
			triangle->texcoord.push_back(getPointFromArrayByIndex(texcoords,pValue.get(6+offset)));
			triangle->texcoord.push_back(getPointFromArrayByIndex(texcoords,pValue.get(8+offset)));		
			*/
			mTriangles.push_back(triangle);
		}
	}
// 
// 	std::vector<daeElement*> polylists = mesh->getDAE()->getDatabase()->typeLookup(domPolylist::ID());
// 	for(int i = 0; i < polylists.size(); i++)
// 	{
// 
// 	}
	return true;
}

String ColladaGeometry::getVertexPosId( daeElement* vertex )
{
	daeElement* input = vertex->getChild("input");
	String id = input->getAttribute("source");
	return id;
}

domListOfFloats ColladaGeometry::getSourceArrayById(daeElement* mesh, String id )
{
	id = id.substr(1,id.size());
	daeElement* source = mesh->getDAE()->getDatabase()->idLookup(id,mesh->getDocument());	
	assert(daeSafeCast<domSource>(source));
	return ((domSource*)source)->getFloat_array()->getValue();
}

Point3d ColladaGeometry::getPointFromArrayByIndex( domListOfFloats arr, int index )
{
	return Point3d(arr.get(index),arr.get(index+1),arr.get(index+2));
}

void ColladaGeometry::computeByBindShapeMatrix( const Matrix& bsm )
{
	for(int i = 0; i < mVertexs.size(); i++)
	{
		mVertexs.at(i)->computeByBindShapeMatrix(bsm);
	}
}

void ColladaVertex::computeByBindShapeMatrix( const Matrix& bsm )
{
	position = matrixMutilpySelf(bsm);
}

#pragma once
#include "CustomizedType.h"
#include "ColladaDomHeader.h"
#include "Point3d.h"
#include "Matrix.h"
#include "SkeletonData.h"
class ColladaVertex
{
public:
	Point3d position;

	
	/** �����ؽ� **/
	String jointNames[4];

	/** ������Ӧ��Ȩ�� **/
	double weights[4];

	int mJointCount;

	/*����ı��Լ�*/
	Point3d matrixMutilpySelf(const Matrix& matrix)
	{
		Matrix tmp;
		tmp.fromPoint3d(position);
		tmp = matrix*tmp;
		return tmp.getPoint();		
	}

	/*�ı����Լ�*/
	void computeByBindShapeMatrix(const Matrix& bsm);

	ColladaVertex()
	{

	}
protected:
private:
};
class ColladaPolylist 
{
public:
	int a,b,c,d;

	/*�ĸ�������*/
	Point3DArray normal;
	/*�ĸ���������*/
	Point3DArray texcoord;
};
class ColladaTriangles
{
public:
	int vertexIndexA,vertexIndexB,vertexIndexC;
	/*����������*/
	Point3DArray normal;
	/*������������*/
	Point3DArray texcoord;
};
class ColladaGeometry
{
public:
	ColladaGeometry();
	~ColladaGeometry();

	bool initWithGeometry(daeElement* geometry);
	
	String mId;

	/** ԭʼ����λ�� **/
	VertexArray mVertexs; 

	/** ���ڹ������������ݹ������κ�Ķ���λ�� **/
	Point3DArray mSkinnedVerticesArray;


	PolylistArray mPolylists; 
	TrianglestArray mTriangles;

	void computeByBindShapeMatrix(const Matrix& bsm);
	
	Point3DArray& getSkinedVerticesRef(){
		return mSkinnedVerticesArray;
	}

	const VertexArray& getOriginalVerticesRef(){
		return mVertexs;
	}

	bool initSkinInformation(daeElement* controller)
	{
		if(controller->typeID() != domController::ID())
			return false;
		domController* domC = (domController*)controller;
		domSkin* domSk = domC->getSkin();
		domListOfNames jointNames = domSk->getSource_array()[0]->getName_array()->getValue();
		domListOfFloats weights = domSk->getSource_array()[2]->getFloat_array()->getValue();
		domListOfUInts vcount = domSk->getVertex_weights()->getVcount()->getValue();
		domListOfInts v = domSk->getVertex_weights()->getV()->getValue();
		int offset = 0;
		for (int j = 0; j < vcount.getCount(); j++)
		{
			boost::shared_ptr<ColladaVertex> vertex = mVertexs.at(j);
			vertex->mJointCount = vcount.get(j);
			for (int k = 0; k < vertex->mJointCount; k++)
			{
				vertex->jointNames[k] = jointNames.get(v.get(offset+k*2+0));
				vertex->weights[k] = weights.get(v.get(offset+k*2+1));
			}
			offset+=vertex->mJointCount*2;
		}
		return true;
	}	

private:
	bool initWithMesh(daeElement* mesh);
	String getVertexPosId(daeElement* vertex);
	domListOfFloats getSourceArrayById(daeElement* mesh, String id);
	Point3d getPointFromArrayByIndex(domListOfFloats arr, int index);
};


#pragma once
#include "ColladaGeometry.h"
#include "SkeletonData.h"
class Animator
{
public:
	Animator();
	~Animator();
	Animator(ColladaGeometry* geo, SkeletonData* ske);
	ColladaGeometry* geometry;
	SkeletonData* skeleton;
	
	/*对每一个顶点，先左乘一个Bind Shape Matrix*/
	void preProcessVertex()
	{			
		VertexArray vs = geometry->mVertexs;
		for(VertexArray::iterator it2 = vs.begin(); it2 != vs.end(); it2++)
		{
			(*it2)->computeByBindShapeMatrix(skeleton->mBindShapeMatrix);
		}		
	}

	void setupPose()
	{
		VertexArray originalVertices = geometry->getOriginalVerticesRef();
		for (int j = 0; j < originalVertices.size(); j++)
		{
			boost::shared_ptr<ColladaVertex> originalV = originalVertices.at(j);			

			Point3d tmp(0,0,0);
			double totalWeight = 0;
			for(int i = 0; i < originalV->mJointCount; i++)
			{
				Joint* joint = skeleton->getJointsByName(originalV->jointNames[i]);
				assert(joint != NULL);
				tmp += originalV->matrixMutilpySelf(joint->mSkinMatrix) * originalV->weights[i];
				totalWeight += originalV->weights[i];
			}
			if(totalWeight != 1.0)
			{
				tmp *= 1.0/totalWeight;
			}

			geometry->getSkinedVerticesRef()[j] = tmp;
		}
	}
private:

};


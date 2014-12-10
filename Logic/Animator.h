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
	void printMatrix(Matrix& m)
	{
		vector< vector<double> > arr = m.getArray();
		for (int r = 0; r < 4; r++)
		{
			for(int col = 0; col < 4; col++)
				printf("%lf ",arr[r][col]);
			printf("\n");
		}
		printf("\n\n");
	}
	void compute(int frame)
	{
		for(int i = 0; i < skeleton->mJoints.size(); i++)
		{
			Joint* joint = skeleton->mJoints.at(i);
			if(joint->mHasAnimation)
			{
				Matrix a = joint->mJointMatrix;
				Matrix b = joint->keyframes.at(frame).mTransformMatrix;								
				//printMatrix(a);
				//printMatrix(b);
				joint->mJointMatrix = joint->keyframes.at(frame).mTransformMatrix;

			}
		}

// 		Matrix m;
// 		m.set44(1,0,0,0,
// 			    0,1,0,0,
// 				0,0,1,0,
// 				2,0,0,1);
// 		skeleton->mRootJoint->mJointMatrix = m*skeleton->mRootJoint->mJointMatrix;// skeleton->mRootJoint->mChildren[0]->keyframes.at(frame).mTransformMatrix;
		skeleton->setUpWorldMatrix();
		skeleton->computeSkinMatrix();
		//setupPose();
	}
	
private:

};


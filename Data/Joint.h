#pragma once
#include "Matrix.h"
#include "ColladaDomHeader.h"
#include "CustomizedType.h"
#include "KeyFrame.h"
class Joint
{
public:
	enum JointType
	{
		NODE,
		JOINT
	};
	Joint();
	~Joint();

	bool initWithNode(daeElement* node)
	{
		domNode* domN = NULL;
		if(node->typeID() == domNode::ID())
		{
			domN = (domNode*)node;
		}
		else
		{
			return false;
		}
		domNodeType type = domN->getType();
		if(type == domNodeType::NODETYPE_JOINT)
		{
			this->mType = JOINT;
			this->mName = domN->getSid();
		}
		else if(type == domNodeType::NODETYPE_NODE)
			this->mType = NODE;

		domMatrix_Array matrixArray = domN->getMatrix_array();
		domFloat4x4 floatArray = matrixArray.get(0)->getValue();
		double arr[16];
		for(int i = 0; i < 16; i++)
			arr[i] = floatArray[i];
		this->mJointMatrix.fromArray(arr);
		return true;
	}

	void computeSkinMatrix()
	{
		if(mType == JOINT)
			mSkinMatrix = mWorldMatrix*mInvertBindMatrix;
	}
	/*对于一个骨骼，这个是固定的*/
	Matrix mInvertBindMatrix;

	/*骨骼局部变换矩阵*/
	Matrix mJointMatrix;

	/*骨骼的世界变换矩阵，等于该骨骼的局部变换矩阵乘以父骨骼的世界变换矩阵*/
	Matrix mWorldMatrix;

	Joint* mParent;
	vector<Joint*> mChildren;

	/*对于每一个骨骼，计算好这个矩阵，来对顶点进行变换*/
	Matrix mSkinMatrix;

	/*关键帧存储了时间和变换矩阵*/
	vector<KeyFrame*> keyframes;

	JointType mType;

	String mName;
private:

};

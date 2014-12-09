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
	/*����һ������������ǹ̶���*/
	Matrix mInvertBindMatrix;

	/*�����ֲ��任����*/
	Matrix mJointMatrix;

	/*����������任���󣬵��ڸù����ľֲ��任������Ը�����������任����*/
	Matrix mWorldMatrix;

	Joint* mParent;
	vector<Joint*> mChildren;

	/*����ÿһ�����������������������Զ�����б任*/
	Matrix mSkinMatrix;

	/*�ؼ�֡�洢��ʱ��ͱ任����*/
	vector<KeyFrame*> keyframes;

	JointType mType;

	String mName;
private:

};

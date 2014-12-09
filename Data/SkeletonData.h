#pragma once
#include "Matrix.h"
#include "Joint.h"
#include "ColladaDomHeader.h"
#include "CustomizedType.h"
#include <stack>
typedef std::stack<daeElement*> DaeStack;
typedef std::stack<Joint*> JointStack;
class SkeletonData
{
public:
	SkeletonData();
	~SkeletonData();
	Matrix mBindShapeMatrix;

	/*���йؽڣ�����JOINT��NODE*/
	vector<Joint*> mJoints;
	Joint* mRootJoint;

	/*���������ֵĹؽڣ���JOINT*/
	map<String, Joint*> mJointsMap;

	bool setUpInvertBindMatrixAndBindShapeMatrix(daeElement* controller)
	{
		if(controller->typeID() != domController::ID())
			return false;
		domController* domC = (domController*)controller;		
		const domSkin::domBind_shape_matrixRef bsm = domC->getSkin()->getBind_shape_matrix();
		domFloat4x4 floatArray = bsm->getValue();
		double arr[16];
		for(int i = 0; i < 16; i++)
			arr[i] = floatArray[i];
		mBindShapeMatrix.fromArray(arr);

		domName_arrayRef jointNames = domC->getSkin()->getSource_array()[0]->getName_array();
		domFloat_arrayRef floatRef = domC->getSkin()->getSource_array()[1]->getFloat_array();
		for (int i = 0; i < jointNames->getCount(); i++)
		{
			assert(mJointsMap.find(jointNames->getValue()[i]) != mJointsMap.end());
			Joint* joint = mJointsMap.at(jointNames->getValue()[i]);
			double arr[16];
			for (int j = 0; j < 16; j++)
			{
				arr[j] = floatRef->getValue()[i*16+j];
			}
			joint->mInvertBindMatrix.fromArray(arr);
		}
	}

	bool initWithRootNode(daeElement* node)
	{
		DaeStack daeStack;
		JointStack parentStack;
		daeStack.push(node);
		Joint* parent = NULL;
		parentStack.push(parent);
		while (! daeStack.empty())
		{
			daeElement* top = daeStack.top();			
			daeStack.pop();

			Joint* joint = new Joint;
			joint->initWithNode(top);
			mJoints.push_back(joint);
			if(joint->mType == Joint::JOINT)
			{
				mJointsMap.insert(map<String, Joint*>::value_type(joint->mName,joint));
			}

			//���ø��ڵ�
			joint->mParent = parentStack.top();
			parentStack.pop();
			//���븸�ڵ�ĺ�������
			if(joint->mParent != NULL)
			{
				joint->mParent->mChildren.push_back(joint);
			}
			else//��������ǿգ���ô�ýڵ��Ǹ��ڵ�
			{
				mRootJoint = joint;
			}
			

			daeTArray<domNodeRef> childrenNode = top->getChildrenByType<domNode>();
			for (int j = 0; j < childrenNode.getCount(); j++)
			{
				daeStack.push(childrenNode.get(j));
				parentStack.push(joint);
			}
		}
		return true;
	}

	/*�������±������й�������������任����*/
	void setUpWorldMatrix()
	{		
		JointStack jointStack;
		jointStack.push(mRootJoint);
		while (! jointStack.empty())
		{
			Joint* top = jointStack.top();
			jointStack.pop();
			if(top->mParent == NULL)
				top->mWorldMatrix = top->mJointMatrix;
			else
				top->mWorldMatrix = top->mParent->mWorldMatrix*top->mJointMatrix;

			vector<Joint*> childrenJoints = top->mChildren;
			for (int i = 0; i < childrenJoints.size(); i++)
			{
				jointStack.push(childrenJoints.at(i));
			}
		}
	}

	/*��setUpInvertBindMatrixAndBindShapeMatrix��setUpWorldMatrix֮����ã�������Ƥ�������������ڶ���*/
	void computeSkinMatrix()
	{
		for(map<String,Joint*>::iterator it = mJointsMap.begin(); it != mJointsMap.end(); it++)
		{
			it->second->computeSkinMatrix();
		}
	}

	bool initKeyFrames(daeElement* library_animations)
	{
		if(library_animations->typeID() != domLibrary_animations::ID())
		{
			return false;
		}
		domLibrary_animations* domLa = (domLibrary_animations*)library_animations;
		domAnimation_Array animations = domLa->getAnimation_array();
		for (int i = 0; i < animations.getCount(); i++)
		{
			domAnimationRef ani = animations.get(i);//һ��animation

			domChannel_Array channels = ani->getChannel_array();
			
			for(int j = 0; j < channels.getCount(); j++)
			{
				String tar = channels.get(j)->getTarget();
			}
			

			
		}
	}
	Joint* getJointsByName(String name){
		map<String, Joint*>::iterator ret = mJointsMap.find(name);
		if(ret != mJointsMap.end())
			return mJointsMap.at(name);
		return NULL;			
	}
	int getJointsCount(){return mJoints.size();}
private:

};

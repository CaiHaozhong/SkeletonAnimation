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

	/*所有关节，包括JOINT和NODE*/
	vector<Joint*> mJoints;
	Joint* mRootJoint;

	/*仅仅有名字的关节，即JOINT*/
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

			//设置父节点
			joint->mParent = parentStack.top();
			parentStack.pop();
			//加入父节点的孩子数组
			if(joint->mParent != NULL)
			{
				joint->mParent->mChildren.push_back(joint);
			}
			else//如果父亲是空，那么该节点是根节点
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

	/*从上至下遍历所有骨骼，计算世界变换矩阵*/
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

	/*在setUpInvertBindMatrixAndBindShapeMatrix和setUpWorldMatrix之后调用，计算蒙皮矩阵，用来作用于顶点*/
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
			domAnimationRef ani = animations.get(i);//一个animation

			domChannel_Array channels = ani->getChannel_array();

			/*对于只有一个channel的，就跳过，不处理*/
			if(channels.getCount() <= 1)
				continue;

			vector<domListOfFloats> timesArrays;
			vector<domListOfFloats> transformsArrays;
			vector<MatrixCoordinate> matrixPos;
			Joint* targetJoint = NULL;
			for(int j = 0; j < channels.getCount(); j++)
			{
				String tar = channels.get(j)->getTarget();
				String targetName = _getRealTargetFromTarget(tar);
				domNode* node = (domNode*)(library_animations->getDAE()->getDatabase()->idLookup(targetName,library_animations->getDocument()));
				String targetSid = node->getSid();
				assert(mJointsMap.find(targetSid) != mJointsMap.end());
				targetJoint = mJointsMap.at(targetSid);
				targetJoint->mHasAnimation = true;
				MatrixCoordinate coord = _getCoordinateFromTarget(tar);
				domURIFragmentType sou = channels.get(j)->getSource();
				domSampler* sampler = (domSampler*)(library_animations->getDAE()->getDatabase()->idLookup(sou.getFragment(),library_animations->getDocument()));
				daeString timeId = sampler->getInput_array()[0]->getSource().getFragment();
				daeString transformId = sampler->getInput_array()[1]->getSource().getFragment();
				domListOfFloats timeFloats = ((domSource*)(ani->getDAE()->getDatabase()->idLookup(timeId,ani->getDocument())))->getFloat_array()->getValue();
				domListOfFloats transformFloats = ((domSource*)(ani->getDAE()->getDatabase()->idLookup(transformId,ani->getDocument())))->getFloat_array()->getValue();
				timesArrays.push_back(timeFloats);
				transformsArrays.push_back(transformFloats);
				matrixPos.push_back(coord);
			}	
			for(int keyF = 0; keyF < 36; keyF++)
			{
				double transformMatrix[16];
				float t;
				for(int c = 0; c < channels.getCount(); c++)
				{
					domListOfFloats times = timesArrays.at(c);
					domListOfFloats trans = transformsArrays.at(c);
					MatrixCoordinate coor = matrixPos.at(c);
					if(times.getCount() == 36)
					{
						t = times.get(keyF);

						/*这里需要注意，y是行*/
						transformMatrix[coor.x+coor.y*4] = trans.get(keyF);
					}
					else
					{
						/*这里需要注意，y是行*/
						transformMatrix[coor.x+coor.y*4] = trans.get(0);
					}
				}				
				Matrix m;

				/*这尼玛坑爹啊，文件中为0，得手动设置为1，不然动画都乱了*/
				transformMatrix[15] = 1;

				m.fromArray(transformMatrix);

				targetJoint->keyframes[keyF].set(t,m);
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
	struct MatrixCoordinate
	{
		int x,y;
		MatrixCoordinate(int a, int b):x(a),y(b){}
	};
	String _getRealTargetFromTarget(String target)
	{
		int slash = target.find_first_of('/');
		return target.substr(0,slash);
	}
	MatrixCoordinate _getCoordinateFromTarget(String target)
	{
		int len = target.length();
		int x = target.at(len-5)-'0';
		int y = target.at(len-2)-'0';
		return MatrixCoordinate(x,y);
	}
};

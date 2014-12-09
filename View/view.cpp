#include <iostream>
#include <climits>
#include<vector>
#include <GL/freeglut.h>
#include <time.h>
#include <crtdbg.h>
#include "ColladaData.h"
#include "Animator.h"
#include "Matrix.h"

#ifdef _DEBUG
#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif
using namespace std;
#define THREHOLD 5.0
#define RANDOM_DATA 0X0001
#define REGION_RANDOM_DATA 0X0002
#define K_MEANS_2 0X0007
#define K_MEANS_3 0X0003
#define K_MEANS_4 0X0004
#define K_MEANS_5 0X0005
#define K_MEANS_6 0X0006
boost::shared_ptr<ColladaData> colladaData;
Point3d eyeCenter;
Point3d eyePos;
void drawGeometries();
void drawJoints();
void drawPoint(Point3d);
void drawLine(Point3d from, Point3d to);
GLdouble ez = 1.70;
float colorlist[][3]={
	{1,0,0},{0,1,0},{0,0,1},{1,1,0},{1,0,1},{0,1,1},{1,1,1}
};
void reshape(int w, int h)
{
	glViewport(0,0,(GLsizei)w,(GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
	gluPerspective(40,(GLdouble)w/(GLdouble)h,0.0001,30);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(0,0,-eyePos.z+eyeCenter.z);
	//glRotated(180,0,1,0);
	glRotated(-90,1,0,0);		
	glTranslated(0,0,eyePos.z-eyeCenter.z);
	gluLookAt(eyePos.x,eyePos.y,eyePos.z,eyeCenter.x,eyeCenter.y,-1,0,1,0);	
}

void display()
{	
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1,0,0);	
	glPolygonMode(GL_FACE_TYPE_FRONT_AND_BACK,GL_LINE);	
	drawGeometries();		
	drawJoints();
//	glBegin(GL_TRIANGLES);
// 	glVertex3d(-0.078233, -0.040380, 1.677108);
// 	glVertex3d(-0.078350, -0.039491, 1.671379);
// 	glVertex3d(-0.075811, -0.038438, 1.658429);
// 	glVertex3d(-0.078233, -0.040380, 1.677108);
// 	glVertex3d(0, 0, 1.671379);
// 	glVertex3d(-0.078350, 0, 1.658429);
//	glEnd();

	glFlush();
}

/*
菜单事件处理
*/
void menuEvent(int value)
{
	switch (value)
	{
	case RANDOM_DATA:

		glutPostRedisplay();
		break;
	case REGION_RANDOM_DATA:

		glutPostRedisplay();
		break;
	case K_MEANS_2:

		glutPostRedisplay();
		break;
	case K_MEANS_3:

		glutPostRedisplay();
		break;
	case K_MEANS_4:

		glutPostRedisplay();
		break;
	case K_MEANS_5:

		glutPostRedisplay();
		break;
	case K_MEANS_6:

		glutPostRedisplay();
		break;
	}
}

/*
创建菜单
*/
void createPopMenu()
{
	glutCreateMenu(menuEvent);
	glutAddMenuEntry("随机产生200个点",RANDOM_DATA);
	glutAddMenuEntry("随机产生有3个集群的点",REGION_RANDOM_DATA);
	glutAddMenuEntry("K-Means:k=2",K_MEANS_2);
	glutAddMenuEntry("K-Means:k=3",K_MEANS_3);
	glutAddMenuEntry("K-Means:k=4",K_MEANS_4);
	glutAddMenuEntry("K-Means:k=5",K_MEANS_5);
	glutAddMenuEntry("K-Means:k=6",K_MEANS_6);	

	// 把菜单和鼠标右键关联起来。
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}
void initData()
{
	colladaData = ColladaData::createColladaData("astroBoy_walk_Max.dae");
	GeometriesMap geoMap = colladaData->getGeometries();
	GLdouble x1 = FLT_MAX, y1 = FLT_MAX, z1 = FLT_MAX;
	GLdouble x2 = FLT_MIN ,y2 = FLT_MIN ,z2 = FLT_MIN ;
	for(GeometriesMap::iterator it = geoMap.begin(); it != geoMap.end(); it++)
	{
		boost::shared_ptr<ColladaGeometry> geo = it->second;
		VertexArray vs = geo->mVertexs;
		for(VertexArray::iterator it2 = vs.begin(); it2 != vs.end(); it2++)
		{
			if(x1 > (*it2)->position.x)
				x1 = (*it2)->position.x;
			if(x2 < (*it2)->position.x)
				x2 = (*it2)->position.x;

			if(y1 > (*it2)->position.y)
				y1 = (*it2)->position.y;
			if(y2 < (*it2)->position.y)
				y2 = (*it2)->position.y;

			if(z1 > (*it2)->position.z)
				z1 = (*it2)->position.z;
			if(z2 < (*it2)->position.z)
				z2 = (*it2)->position.z;
		}	
	}
	eyeCenter.x = (x1+x2)/2;
	eyeCenter.y = (y1+y2)/2;
	eyeCenter.z = (z1+z2)/2;

	eyePos.x = (x1+x2)/2;
	eyePos.y = (y1+y2)/2;
	eyePos.z = z2+5;

	Animator* animator = new Animator(colladaData->getGeometries().begin()->second.get(),colladaData->getSkeletonData());
	animator->preProcessVertex();
	animator->setupPose();
}
void drawJoints()
{
	map<String, Joint*> jointsMap = colladaData->getSkeletonData()->mJointsMap;
	vector<Joint*> jointsVector = colladaData->getSkeletonData()->mJoints;
	glPointSize(5);	
	//for(map<String, Joint*>::iterator it = jointsMap.begin(); it != jointsMap.end(); it++)
	Joint* rootJoint = colladaData->getSkeletonData()->mRootJoint;
	JointStack jointStack;
	jointStack.push(rootJoint);
	while (! jointStack.empty())
	{
		Joint* top = jointStack.top();
		jointStack.pop();

		if(top->mParent != NULL)
		{
			Matrix zero;
			zero.fromPoint3d(Point3d(0,0,0));
			Matrix result = top->mWorldMatrix * zero;
			Point3d posSelf = result.getPoint();

			result = top->mParent->mWorldMatrix * zero;
			Point3d posParent = result.getPoint();
			glColor3f(colorlist[0][0],colorlist[0][1],colorlist[0][2]);
			drawPoint(posSelf);
			drawPoint(posParent);
			glColor3f(colorlist[1][0],colorlist[1][1],colorlist[1][2]);
			drawLine(posParent,posSelf);
			
		}

		vector<Joint*> childrenJoints = top->mChildren;
		for (int i = 0; i < childrenJoints.size(); i++)
		{
			jointStack.push(childrenJoints.at(i));
		}
	}

}
void drawPoint(Point3d p)
{
	glBegin(GL_POINTS);
	glVertex3d(p.x,p.y,p.z);
	glEnd();
}
void drawLine(Point3d from, Point3d to)
{
	glBegin(GL_LINES);
	glVertex3d(from.x,from.y,from.z);
	glVertex3d(to.x,to.y,to.z);
	glEnd();
}
void drawGeometries()
{	
	GeometriesMap geoMap = colladaData->getGeometries();
	int colorIndex = 0;
	for(GeometriesMap::iterator it = geoMap.begin(); it != geoMap.end(); it++)
	{
		boost::shared_ptr<ColladaGeometry> geo = it->second;
		glColor3f(colorlist[colorIndex][0],colorlist[colorIndex][1],colorlist[colorIndex][2]);
		colorIndex++;
		int k = geo->mTriangles.size();
		for(int i = 0; i < k; i++)
		{			
			glBegin(GL_TRIANGLES);
			boost::shared_ptr<ColladaTriangles> tri = geo->mTriangles.at(i);			
			//printf("A:%lf,%lf,%lf\n",geo->getVertices().at(tri->vertexIndexA).x,geo->getVertices().at(tri->vertexIndexA).y,geo->getVertices().at(tri->vertexIndexA).z);
			glVertex3d(geo->getSkinedVerticesRef().at(tri->vertexIndexA).x,geo->getSkinedVerticesRef().at(tri->vertexIndexA).y,geo->getSkinedVerticesRef().at(tri->vertexIndexA).z);
			//printf("B:%lf,%lf,%lf\n",geo->getVertices().at(tri->vertexIndexB).x,geo->getVertices().at(tri->vertexIndexB).y,geo->getVertices().at(tri->vertexIndexB).z);
			glVertex3d(geo->getSkinedVerticesRef().at(tri->vertexIndexB).x,geo->getSkinedVerticesRef().at(tri->vertexIndexB).y,geo->getSkinedVerticesRef().at(tri->vertexIndexB).z);
			//printf("C:%lf,%lf,%lf\n",geo->getVertices().at(tri->vertexIndexC).x,geo->getVertices().at(tri->vertexIndexC).y,geo->getVertices().at(tri->vertexIndexC).z);
			glVertex3d(geo->getSkinedVerticesRef().at(tri->vertexIndexC).x,geo->getSkinedVerticesRef().at(tri->vertexIndexC).y,geo->getSkinedVerticesRef().at(tri->vertexIndexC).z);			
			glEnd();
		}
		
	}
	
}
void keyBoard(unsigned char c, int a, int b)
{
	eyePos.z += 0.1;
	//printf("ez:%lf\n",ez);
	glLoadIdentity();
	glTranslated(0,0,-eyePos.z+eyeCenter.z);
	//glRotated(180,0,1,0);
	glRotated(-90,1,0,0);		
	glTranslated(0,0,eyePos.z-eyeCenter.z);
	gluLookAt(eyePos.x,eyePos.y,eyePos.z,eyeCenter.x,eyeCenter.y,-1,0,1,0);	
	//gluLookAt(0,0,ez,0,0,-1,0,1,0);
	glutPostRedisplay();
}
int main(int a, char* b)
{
	//_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
  	initData();
	glutInit(&a,&b);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE);
	glutInitWindowPosition(100,100);
	glutInitWindowSize(960,640);	
	glutCreateWindow("K-Means");	
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyBoard);
	createPopMenu();
	glutMainLoop();
// 	Matrix ma(4,4),mb(4,4),mc(4,4);
// 	ma.set44(1.3200,   	2.1230  , 	3.1663  , 	5.2342   ,
// 		4.2500 ,  	523.1300  , 	4.2344  , 	5.2310 ,  
// 		4.2300  , 	5.1210   ,	412.3100,   	513.1200 ,  
// 		1.2300,   	212.1000,   	32.2300,   	5.3210  );
// 	mb.fromPoint3d(Point3d(23.1,12.12,123.1));
// 	mc=ma*mb;
// 	getchar();
}

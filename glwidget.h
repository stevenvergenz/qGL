/***************************************
  Name: Steven Vergenz (s2129096)
  Date: September 27, 2011
  Comment: Draws the bench model in 3d, use arrow keys to rotate
  *************************************/
	

#ifndef GLWIDGET_H
#define GLWIDGET_H

#define DRAW_FPS 60
#define M_PI 3.141592653589

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "sceneHandler.h"
#include "shadowmodel.h"
#include "lightingmodel.h"

#include <QGLWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <iostream>
#include <cmath>

using namespace std;

class GLWidget : public QGLWidget
{
Q_OBJECT

public:
	GLWidget(QWidget *parent = 0);
	~GLWidget();
	
private:
	// model information
	Scene* scene;
	glm::vec3 sceneCenter;
	float sceneDiagLength;
	
	// lighting information
	LightingModel* lighting;	
	ShadowModel* shadow;
	glm::vec4 lightPosition, lightIntensity;	
	float lightAngle;
	
	// camera information
	glm::vec3 cameraPosition, cameraDirection;
	glm::mat4 viewMat, projectionMat;
	
	// interaction state information
	float moveSpeed, lookSpeed, lightSpeed;
	glm::vec3 moveDir;
	int lightDir;
	
	// opengl handles
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
	// input handling
	void keyPressEvent(QKeyEvent* e);
	void keyReleaseEvent(QKeyEvent* e);
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	QPoint savedPoint;
	
	// draw timing elements
	void timerEvent(QTimerEvent* e);
	
};



#endif // GLWIDGET_H

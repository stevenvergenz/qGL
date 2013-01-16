#ifndef SHADOWMODEL_H
#define SHADOWMODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QObject>

#include "sceneHandler.h"

class ShadowModel : public QObject
{
Q_OBJECT
	
public:
	ShadowModel(Scene* s);
	bool initializeShadowReceiver();
	void drawShadowVolume();
	void drawIndicators();
	void calculateShadowVolume();
	
private:
	QGLShaderProgram minimal;
	glm::vec4 lightPosition;
	
	// volume info
	glm::vec3 casterQuad[4];
	QGLBuffer vertBuffer;
	GLuint vao;
	
	// uniform locations
	GLint projMinLoc, viewMinLoc;
	GLint lightPosLoc, colorLoc;
	Scene* scene;
	
public slots:
	void setViewTransforms(glm::mat4 proj, glm::mat4 view);
	void setLightingInfo(glm::vec4 lightPosition, glm::vec4 lightIntensity);
	
};

#endif // SHADOWMODEL_H

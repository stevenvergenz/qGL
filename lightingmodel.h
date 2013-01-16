#ifndef LIGHTINGMODEL_H
#define LIGHTINGMODEL_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <QApplication>
#include <QGLShaderProgram>
#include <QGLBuffer>
#include <QObject>

#include "sceneHandler.h"

class LightingModel : public QObject
{
Q_OBJECT
public:
	LightingModel(Scene* s);
	bool initializeLightingModel();
	void drawAmbient();
	void drawLit();
	
private:
	Scene* scene;
	
	// gl memory handles
	QGLShaderProgram ambient, fancy;
	GLint a_projectionLoc, a_viewLoc, a_modelLoc;
	GLint f_projectionLoc, f_viewLoc, f_modelLoc;
	GLint a_texCountLoc, a_texUnitLoc, a_diffuseRefLoc;
	GLint f_texCountLoc, f_texUnitLoc, f_diffuseRefLoc;
	
	GLint lightPosLoc, lightIntensityLoc;	
	GLint specularRefLoc, shinyLoc;

signals:

public slots:
	void setViewTransforms(glm::mat4 proj, glm::mat4 view);
	void setLightingInfo(glm::vec4 lightPosition, glm::vec4 lightIntensity);
};

#endif // LIGHTINGMODEL_H

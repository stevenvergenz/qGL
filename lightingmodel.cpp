#include "lightingmodel.h"

LightingModel::LightingModel(Scene* s)
{
	scene = s;
}

bool LightingModel::initializeLightingModel()
{
	// initialize the shaders
	fancy.addShaderFromSourceFile(QGLShader::Vertex,   "../qgl/full.vert");
	fancy.addShaderFromSourceFile(QGLShader::Fragment, "../qgl/full.frag");
	ambient.addShaderFromSourceFile(QGLShader::Vertex,   "../qgl/ambient.vert");
	ambient.addShaderFromSourceFile(QGLShader::Fragment, "../qgl/ambient.frag");
	if( !fancy.link() || !ambient.link() ){
		std::cerr << "Linker failure" << std::endl;
		exit(1);
		QApplication::exit(1);
	}
	
	// declare uniforms
	a_projectionLoc     = glGetUniformLocation(ambient.programId(), "projection");
	a_viewLoc           = glGetUniformLocation(ambient.programId(), "view");
	a_modelLoc          = glGetUniformLocation(ambient.programId(), "model");
	f_projectionLoc     = glGetUniformLocation(fancy.programId(), "projection");
	f_viewLoc           = glGetUniformLocation(fancy.programId(), "view");
	f_modelLoc          = glGetUniformLocation(fancy.programId(), "model");
	
	a_texCountLoc       = glGetUniformLocation(ambient.programId(), "texCount");
	a_texUnitLoc        = glGetUniformLocation(ambient.programId(), "texUnit");
	a_diffuseRefLoc     = glGetUniformLocation(ambient.programId(), "diffuseRef");	
	f_texCountLoc       = glGetUniformLocation(fancy.programId(), "texCount");
	f_texUnitLoc        = glGetUniformLocation(fancy.programId(), "texUnit");
	f_diffuseRefLoc     = glGetUniformLocation(fancy.programId(), "diffuseRef");
	
	lightPosLoc       = glGetUniformLocation(fancy.programId(), "lightPosition");
	lightIntensityLoc = glGetUniformLocation(fancy.programId(), "lightIntensity");
	specularRefLoc    = glGetUniformLocation(fancy.programId(), "specularRef");
	shinyLoc          = glGetUniformLocation(fancy.programId(), "shiny");
	
	return true;
}

void LightingModel::drawAmbient()
{
	ambient.bind();
	
	// begin rendering
	glUniform1i(a_texUnitLoc, 0);
	for(unsigned int i=0; i<scene->nodes.size(); i++)
	{
		// send the model transformation matrix
		glUniformMatrix4fv(a_modelLoc, 1, GL_FALSE, glm::value_ptr(scene->nodes[i].m));
		
		// load the particular mesh in the scene
		for(int j=0; j<scene->nodes[i].nMeshes; j++)
		{
			// send appropriate uniforms
			glUniform4fv(a_diffuseRefLoc, 1, glm::value_ptr( scene->nodes[i].ms[j].diffuseColor ));
			glUniform1i(a_texCountLoc, scene->nodes[i].ms[j].texCount);
			
			// bind and draw
			glBindTexture(GL_TEXTURE_2D, scene->nodes[i].ms[j].texIndex);
			glBindVertexArray(scene->nodes[i].ms[j].vao);
			glDrawElements(GL_TRIANGLES, 3*scene->nodes[i].ms[j].numFaces, GL_UNSIGNED_INT, 0);
		}
		// clear bindings
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	
	}
	
	ambient.release();
}

void LightingModel::drawLit()
{
	fancy.bind();
	
	// begin rendering
	glUniform1i(f_texUnitLoc, 0);
	for(unsigned int i=0; i<scene->nodes.size(); i++)
	{
		// send the model transformation matrix
		glUniformMatrix4fv(f_modelLoc, 1, GL_FALSE, glm::value_ptr(scene->nodes[i].m));
		
		// load the particular mesh in the scene
		for(int j=0; j<scene->nodes[i].nMeshes; j++)
		{
			// send appropriate uniforms
			glUniform4fv(f_diffuseRefLoc, 1, glm::value_ptr( scene->nodes[i].ms[j].diffuseColor ));
			glUniform4fv(specularRefLoc, 1, glm::value_ptr( scene->nodes[i].ms[j].specularColor ));
			glUniform1f(shinyLoc, scene->nodes[i].ms[j].shininess);
			glUniform1i(f_texCountLoc, scene->nodes[i].ms[j].texCount);
			
			// bind and draw
			glBindTexture(GL_TEXTURE_2D, scene->nodes[i].ms[j].texIndex);
			glBindVertexArray(scene->nodes[i].ms[j].vao);
			glDrawElements(GL_TRIANGLES, 3*scene->nodes[i].ms[j].numFaces, GL_UNSIGNED_INT, 0);
		}
		// clear bindings
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	
	}
	
	fancy.release();
}

void LightingModel::setViewTransforms(glm::mat4 proj, glm::mat4 view)
{
	ambient.bind();
	glUniformMatrix4fv(a_projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(a_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	ambient.release();
	
	fancy.bind();
	glUniformMatrix4fv(f_projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(f_viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	fancy.release();
}

void LightingModel::setLightingInfo(glm::vec4 lightPosition, glm::vec4 lightIntensity)
{
	fancy.bind();
	glUniform4fv(lightPosLoc, 1, glm::value_ptr(lightPosition));
	glUniform4fv(lightIntensityLoc, 1, glm::value_ptr(lightIntensity));
	fancy.release();
}

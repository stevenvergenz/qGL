#include "shadowmodel.h"

ShadowModel::ShadowModel(Scene* s)
{
	scene = s;
}

bool ShadowModel::initializeShadowReceiver()
{
	// load the minimal shader for rendering the floor volume
	minimal.addShaderFromSourceFile(QGLShader::Vertex, "../qgl/minimal.vert");
	minimal.addShaderFromSourceFile(QGLShader::Fragment, "../qgl/minimal.frag");
	
	// compile the shaders
	if( !minimal.link() ){
		std::cerr << "Link error" << std::endl;
		return false;
	}
	
	// allocate minimal uniforms
	projMinLoc = glGetUniformLocation(minimal.programId(), "projection");
	viewMinLoc = glGetUniformLocation(minimal.programId(), "view");
	colorLoc   = glGetUniformLocation(minimal.programId(), "color");

	// initialize the shadow caster quad
	float sceneDiag = glm::length(scene->scene_max - scene->scene_min);
	glm::vec3 sceneCenter = 0.5f*(scene->scene_min + scene->scene_max);
	glm::vec3 nearoffset = glm::vec3(0.125f*sceneDiag,
	                                 0.5f*(scene->scene_max.y-scene->scene_min.y),
	                                 0.0625*sceneDiag);
	casterQuad[0] = sceneCenter + glm::vec3(-nearoffset.x, +nearoffset.y, +nearoffset.z);
	casterQuad[1] = sceneCenter + glm::vec3(+nearoffset.x, +nearoffset.y, +nearoffset.z);
	casterQuad[2] = sceneCenter + glm::vec3(+nearoffset.x, +nearoffset.y, -nearoffset.z);
	casterQuad[3] = sceneCenter + glm::vec3(-nearoffset.x, +nearoffset.y, -nearoffset.z);
	
	// load the vertices to the GPU
	vertBuffer.setUsagePattern(QGLBuffer::DynamicDraw);
	vertBuffer.create();
	vertBuffer.bind();
	vertBuffer.allocate( 36*sizeof(float) );
	vertBuffer.release();
	
	// set up vao
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	vertBuffer.bind();
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	vertBuffer.release();
	
	glBindVertexArray(0);
	
	return true;
}

void ShadowModel::drawShadowVolume()
{
	static const unsigned int shadowVolElements[36] = {
	        0,1,3,3,1,2,
	        5,1,4,4,1,0,
	        6,2,5,5,2,1,
	        7,3,6,6,3,2,
	        4,0,7,7,0,3,
	        6,5,7,7,5,4
	};

	minimal.bind();
	glBindVertexArray(vao);
	
	glDrawElements(GL_TRIANGLES, 6*2*3, GL_UNSIGNED_INT, shadowVolElements);
	
	glBindVertexArray(0);
	minimal.release();
}

void ShadowModel::drawIndicators()
{
	static const unsigned int casterElements[24] = {
		0,1,1,2,2,3,3,0,
		0,4,1,5,2,6,3,7,
		4,5,5,6,6,7,7,4
	};
	static const unsigned int casterSource = 8;

	minimal.bind();
	glBindVertexArray(vao);

	glUniform4f(colorLoc, 1, 0, 1, 1);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, casterElements); // shadow volume
	glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, &casterSource);  // light source
	
	glBindVertexArray(0);
	minimal.release();
}

void ShadowModel::calculateShadowVolume()
{
	glm::vec3 origin = glm::vec3(lightPosition);
	glm::vec3 normal(0, 1, 0);
	glm::vec3 p0 = scene->scene_min;
	static glm::vec4 shadowVolVerts[8];

	for(int i=0; i<4; i++){
		shadowVolVerts[i] = glm::vec4(casterQuad[i], 1);
		shadowVolVerts[i+4] = glm::vec4(
		                        casterQuad[i] + (casterQuad[i]-origin) * (
		                        glm::dot(normal, (p0-casterQuad[i])) /
		                        glm::dot(normal, (casterQuad[i]-origin))
		                        ),
		                        1);
	}
	
	// flatten the vec4's to a usable format
	static float flatVertBuffer[36];
	for(int i=0; i<8; i++){
		flatVertBuffer[4*i+0] = shadowVolVerts[i].x;
		flatVertBuffer[4*i+1] = shadowVolVerts[i].y;
		flatVertBuffer[4*i+2] = shadowVolVerts[i].z;
		flatVertBuffer[4*i+3] = shadowVolVerts[i].w;
	}
	flatVertBuffer[32] = lightPosition.x;
	flatVertBuffer[33] = lightPosition.y;
	flatVertBuffer[34] = lightPosition.z;
	flatVertBuffer[35] = lightPosition.w;

	// write to vertex buffer
	vertBuffer.bind();
	vertBuffer.write(0, flatVertBuffer, 36*sizeof(float));
	vertBuffer.release();
}

void ShadowModel::setViewTransforms(glm::mat4 proj, glm::mat4 view)
{
	minimal.bind();
	glUniformMatrix4fv(projMinLoc, 1, GL_FALSE, glm::value_ptr(proj));
	glUniformMatrix4fv(viewMinLoc, 1, GL_FALSE, glm::value_ptr(view));
	minimal.release();
	
}

void ShadowModel::setLightingInfo(glm::vec4 lightPosition, glm::vec4 lightIntensity)
{
	this->lightPosition = lightPosition;
}


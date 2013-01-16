#ifndef __SCENE_H__
#define __SCENE_H__

//#define  __NOASSIMP__
//#define __NO_TEXTURE_SUPPORT__

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>


struct Mesh{
	GLuint vao;
	int numFaces;
	glm::vec4 diffuseColor;
	glm::vec4 specularColor;
	float shininess;
	int texCount;
	int texIndex;
};
struct Node{
	glm::mat4 m; // Model matrix
	int nMeshes;
	struct Mesh *ms;
};
struct Scene{
	static const GLint vertexLoc=0, normalLoc=1, texCoordLoc=2;// Vertex Attribute Locations
	std::vector <struct Node> nodes;
	glm::vec3 scene_min,scene_max;
	void *scenePointer;
};

void outputScene(const void *s,const std::string& pfile);
bool flatten(const void *s,std::vector<glm::vec4>& vertexStack);
void sceneDeInit(struct Scene *s);
bool initScene(const std::string& pathname,const std::string& filename, struct Scene *s);

#endif

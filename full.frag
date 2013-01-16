#version 330

// from the pipeline
in vec2 TexCoord;
in vec4 Position;
in vec4 Normal;
out vec4 output;

// uniforms
uniform int texCount;
uniform sampler2D texUnit;

uniform mat4 view;
uniform vec4 lightPosition;
uniform vec4 lightIntensity;
uniform vec4 diffuseRef;
uniform vec4 specularRef;
uniform float shiny;

void main()
{
	// get the color of the current texture cell, or diffuse failing that
	vec4 color;
	if( texCount == 0 ){
		color = diffuseRef;
	}
	else {
		color = texture2D(texUnit, TexCoord);
	}
	//output = diffuseRef;
	
	
	vec4 normal = normalize(Normal);
	vec4 incident = normalize(Position - view*lightPosition);
	vec4 R = normalize( reflect(incident, normal) );
	vec4 viewVec = normalize( Position );
	
	// calculate the lighting on the given fragment
	vec4 diffuse, specular, ambient;
	ambient = 0.1f * vec4(vec3(diffuseRef),1) * color;
	
	// note: dot(x,y)==cos(angle between)
	if( dot( -normal, incident) > 0 )
	{
		diffuse = diffuseRef * dot( -normal, incident) * color;
		
		if( dot(-R, viewVec) > 0 ){
			specular = pow( dot(-R, viewVec), shiny) * specularRef;
		}
		else {
			specular = vec4(0,0,0,0);
		}
	}
	else {
		diffuse = vec4(0,0,0,0);
		specular = vec4(0,0,0,0);
	}
	
	output = lightIntensity*(diffuse+specular/*+ambient*/);
	
}

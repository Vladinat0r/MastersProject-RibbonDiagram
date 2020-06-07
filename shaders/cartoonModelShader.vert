//Cartoon model vertex shader
//Author: Volodymyr Nazarenko (100174968)

#version 150

uniform mat4 ModelViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;
uniform mat3 NormalMatrix;  //inverse transpose of the 3x3 bit of modelview matrix
uniform vec4 LightPos;		//light position

in vec3 in_Position;  //Position coming in
in vec3 in_Normal;    //Vertex normal used for lighting
in vec3 in_Color;

out vec3 ex_Normal;			//Exiting normal transformed by the normal matrix
out vec3 ex_LightPos;		//Light direction vector in eye space (assuming it doesn't undergo other transformations)
out vec3 ex_Position;
out vec3 ex_PositionEye;	//vertex position in eye space (i.e. after ModelView but not projection)
out vec3 ex_Color;

void main(void)
{
	gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(in_Position, 1.0);
	
	ex_Normal = NormalMatrix * in_Normal; 

	ex_Position = in_Position;

	ex_PositionEye = vec3((ModelViewMatrix * vec4(in_Position, 1.0)));

	ex_LightPos = vec3(ViewMatrix*LightPos);

	ex_Color = in_Color;
}
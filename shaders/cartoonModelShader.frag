//Cartoon model fragment shader
//Author: Volodymyr Nazarenko (100174968)

#version 150

uniform vec4 light_ambient;    //ambient light 
uniform vec4 light_diffuse;    //diffuse property for the light
uniform vec4 light_specular;   //specular property for the light

uniform vec4 material_ambient; //ambient property for material
uniform vec4 material_diffuse; //diffuse property for the material
uniform vec4 material_specular; //specular property for the material
uniform float material_shininess; //shininess exponent for specular highlight

uniform vec3 cameraPos;
uniform bool isTransparent;

in vec3 ex_LightPos;	//light direction arriving from the vertex
in vec3 ex_Normal;		//normal arriving from the vertex
in vec3 ex_Position;
in vec3 ex_PositionEye;
in vec3 ex_Color;

out vec4 out_Color;		//colour of the pixel


void main(void) {
	vec4 color = vec4(ex_Color, 1.0) * light_ambient * material_ambient;

	vec3 n, L;
	float NdotL;
	
	n = normalize(ex_Normal);
	L = normalize(ex_LightPos - ex_PositionEye);

	vec3 v = normalize(-ex_PositionEye);
	vec3 r = normalize(-reflect(L, n));
	
	float RdotV = max(0.0, dot(r, v));

	NdotL = max(dot(n, L),0.0);

	if(NdotL > 0.0) {
		color += (light_diffuse * material_diffuse * NdotL);
		color += material_specular * light_specular * pow(RdotV, material_shininess);
	
	}

	if (isTransparent) {	//Calculates transparency intensity based on distance to the camera
		float distance = distance(-cameraPos, ex_Position);
		float opacity = clamp(distance / 500.0f, 0.2f, 1.0f);

		out_Color = vec4(color.xyz, opacity);
	}
	else {
		out_Color = color;
	}

	//out_Color = color;
}
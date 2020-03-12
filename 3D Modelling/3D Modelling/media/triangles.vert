#version 400 core

layout( location = 0 ) in vec3 vPosition;
layout( location = 1 ) in vec4 vColour;
//layout( location = 2 ) in vec3 vNormal;

uniform mat4 mv_matrix;
uniform mat4 p_matrix;

out vec4 fragColour;

void
main()
{
	vec4 P = mv_matrix * vec4(vPosition,1.0);

	// calculate normal in view-space
	//vec3 N = mat3(mv_matrix)* vNormal;
	
	// calc the view vector
	vec3 V = -P.xyz;
	
	//Normalise
	//N = normalize(N);
	
	V = normalize(V);
	gl_Position = p_matrix * P;
      
	//fragColour=vColour;
	fragColour = vec4(0.9, 0.9, 0.9, 1);
}

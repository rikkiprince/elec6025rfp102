varying vec3 normal;

void main()
{
	// normalizing the lights position to be on the safe side
	vec3 n = normalize(normal);
	
	gl_FragColor = vec4(n.x, n.y, n.z, 1.0);
} 

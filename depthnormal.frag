varying vec3 lightDir,normal;

void main()
{

	float intensity;
	vec4 color;
	
	// normalizing the lights position to be on the safe side
	
	vec3 n = normalize(normal);
	
	intensity = dot(lightDir,n);
	
	/*if (intensity > 0.95)
		color = vec4(1.0,0.5,0.5,1.0);
	else if (intensity > 0.5)
		color = vec4(0.6,0.3,0.3,1.0);
	else if (intensity > 0.25)
		color = vec4(0.4,0.2,0.2,1.0);
	else
		color = vec4(0.2,0.1,0.1,1.0);*/
	color = gl_Color * 0.6;

	if (intensity > 0.95)
		color = gl_Color;
	else if (intensity > 0.5)
		color = gl_Color * 0.8;
	else if (intensity > 0.25)
		color = gl_Color * 0.6;
	else
		color = gl_Color * 0.4;
	
	//gl_FragColor = color;
	//gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0);
	//gl_FragColor = vec4(n.x, n.y, n.z, 1.0);
	gl_FragColor = vec4(n.x, n.y, n.z, gl_FragCoord.z);
} 

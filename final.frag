uniform sampler2D tex;

void main()
{
	float to = 0.001;
	vec2 coords = gl_TexCoord[0].st;

	vec4 tempcolor;


	// normal outline
	
	tempcolor = texture2D(tex,vec2(coords.s, coords.t));
	vec3 P5n = tempcolor.rgb;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t-to));
	float P1n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t-to));
	float P2n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;		//(dot(P5n,tempcolor.rgb)+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t-to));
	float P3n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t));
	float P4n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t));
	float P6n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t+to));
	float P7n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t+to));
	float P8n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t+to));
	float P9n = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;

	float Gx = (P1n + 2.0*P2n + P3n) - (P7n + 2.0*P8n + P9n);
	float Gy = (P3n + 2.0*P6n + P9n) - (P1n + 2.0*P4n + P7n);

	float Gn = sqrt(Gx*Gx+Gy*Gy);
	Gn = Gn/4.0;	//max(0, min(1.0, Gn));


	// depth outline

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t-to));
	float P1d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t-to));
	float P2d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t-to));
	float P3d = tempcolor.a;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t));
	float P4d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t));
	float P5d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t));
	float P6d = tempcolor.a;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t+to));
	float P7d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t+to));
	float P8d = tempcolor.a;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t+to));
	float P9d = tempcolor.a;

	Gx = (P1d + 2.0*P2d + P3d) - (P7d + 2.0*P8d + P9d);
	Gy = (P3d + 2.0*P6d + P9d) - (P1d + 2.0*P4d + P7d);

	float Gd = sqrt(Gx*Gx+Gy*Gy);

	vec4 newcolor;
	newcolor = vec4(Gn+Gd, Gn+Gd, Gn+Gd, 1.0);
	if(Gn>0.01 || Gd>0.1)	newcolor = vec4(0.0, 0.0, 0.0, 1.0);
	else
	{
		newcolor = vec4(1.0, 1.0, 1.0, 1.0);
		float intensity;
		vec4 color;
		
		// normalizing the lights position to be on the safe side
		
		vec3 n = normalize(P5n);
		
		intensity = dot(lightDir,n);
		
		if (intensity > 0.95)
			color = vec4(1.0,0.5,0.5,1.0);
		else if (intensity > 0.5)
			color = vec4(0.6,0.3,0.3,1.0);
		else if (intensity > 0.25)
			color = vec4(0.4,0.2,0.2,1.0);
		else
			color = vec4(0.2,0.1,0.1,1.0);

	}
	gl_FragColor = newcolor;
}

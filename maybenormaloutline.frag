uniform sampler2D tex;

void main()
{
	float to = 0.001;
	vec2 coords = gl_TexCoord[0].st;

	vec4 tempcolor;
	
	tempcolor = texture2D(tex,vec2(coords.s, coords.t));
	vec3 P5n = tempcolor.rgb;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t-to));
	float P1d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t-to));
	float P2d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;		//(dot(P5n,tempcolor.rgb)+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t-to));
	float P3d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t));
	float P4d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t));
	float P6d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t+to));
	float P7d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t+to));
	float P8d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t+to));
	float P9d = ((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))-1.0)/2.0;	//((dot(P5n,tempcolor.rgb)/(length(P5n)*length(tempcolor.rgb)))+1.0)/2.0;


	vec4 first  = texture2D(tex,vec2(coords.s-to, coords.t-to));
	vec4 second = texture2D(tex,vec2(coords.s+to, coords.t-to));
	float A = (dot(second.rgb, first.rgb)+1.0)/2.0;	//(dot(second.rgb, first.rgb)/(length(second.rgb)*length(first.rgb))+1.0)/2.0;
	first  = texture2D(tex,vec2(coords.s-to, coords.t));
	second = texture2D(tex,vec2(coords.s+to, coords.t));
	float B = (dot(second.rgb, first.rgb)+1.0)/2.0;
	first  = texture2D(tex,vec2(coords.s-to, coords.t+to));
	second = texture2D(tex,vec2(coords.s+to, coords.t+to));
	float C = (dot(second.rgb, first.rgb)+1.0)/2.0;

	first  = texture2D(tex,vec2(coords.s-to, coords.t-to));
	second = texture2D(tex,vec2(coords.s-to, coords.t+to));
	float X = (dot(second.rgb, first.rgb)+1.0)/2.0;
	first  = texture2D(tex,vec2(coords.s, coords.t-to));
	second = texture2D(tex,vec2(coords.s, coords.t+to));
	float Y = (dot(second.rgb, first.rgb)+1.0)/2.0;
	first  = texture2D(tex,vec2(coords.s+to, coords.t-to));
	second = texture2D(tex,vec2(coords.s+to, coords.t+to));
	float Z = (dot(second.rgb, first.rgb)+1.0)/2.0;


	float Gx = (P1d + 2.0*P2d + P3d) - (P7d + 2.0*P8d + P9d);
	float Gy = (P3d + 2.0*P6d + P9d) - (P1d + 2.0*P4d + P7d);

	//float Gx = (A + 2.0*B + C);
	//float Gy = (X + 2.0*Y + Z);

	float G = sqrt(Gx*Gx+Gy*Gy);
	G = G/4.0;	//max(0, min(1.0, G));

	vec4 newcolor;
	newcolor = vec4(G, G, G, 1.0);
	if(G>0.01)	newcolor = vec4(0.0, 0.0, 0.0, 1.0);
	else	newcolor = vec4(1.0, 1.0, 1.0, 1.0);
	gl_FragColor = newcolor;
}

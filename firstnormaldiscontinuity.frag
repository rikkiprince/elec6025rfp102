uniform sampler2D tex;

void main()
{
	float to = 0.001;
	vec2 coords = gl_TexCoord[0].st;

	vec4 tempcolor;

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


	tempcolor = texture2D(tex,vec2(coords.s, coords.t));
	vec3 centre = tempcolor.rgb;

	// get up, down, left, right (convert to corners later?)
	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t));
	vec3 top = tempcolor.rgb;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t));
	vec3 bottom = tempcolor.rgb;
	
	tempcolor = texture2D(tex,vec2(coords.s, coords.t-to));
	vec3 left = tempcolor.rgb;
	tempcolor = texture2D(tex,vec2(coords.s, coords.t+to));
	vec3 right = tempcolor.rgb;

	float td = dot(centre, top);
	float bd = dot(centre, bottom);
	float ld = dot(centre, left);
	float rd = dot(centre, right);

	vec4 tc;
	vec4 bc;
	vec4 lc;
	vec4 rc;
	vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

	vec4 nc;
	if(td >= 0.85) nc = black;
	else if(bd >= 0.85) nc = black;
	else if(ld >= 0.85) nc = black;
	else if(rd >= 0.85) nc = black;
	else nc = white;

	/*if(td >= 0.85) tc = black;
	else tc = white;
	if(bd >= 0.85) bc = black;
	else bc = white;
	if(ld >= 0.85) lc = black;
	else lc = white;
	if(rd >= 0.85) rc = black;
	else rc = white;*/

	//vec4 nc = tc * bc * lc * rc;


	float Gx = (P1d + 2.0*P2d + P3d) - (P7d + 2.0*P8d + P9d);
	float Gy = (P3d + 2.0*P6d + P9d) - (P1d + 2.0*P4d + P7d);

	float G = sqrt(Gx*Gx+Gy*Gy);

	vec4 newcolor = vec4(G, G, G, 1.0);
	gl_FragColor = nc;
}

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
	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t-to));
	vec3 topleft = tempcolor.rgb;
	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t+to));
	vec3 topright = tempcolor.rgb;

	tempcolor = texture2D(tex,vec2(coords.s-to, coords.t+to));
	vec3 bottomleft = tempcolor.rgb;
	tempcolor = texture2D(tex,vec2(coords.s+to, coords.t+to));
	vec3 bottomright = tempcolor.rgb;

	float tld = dot(centre, topleft);
	float trd = dot(centre, topright);
	float bld = dot(centre, bottomleft);
	float brd = dot(centre, bottomright);

	vec4 tlc;
	vec4 trc;
	vec4 blc;
	vec4 brc;
	vec4 black = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 white = vec4(1.0, 1.0, 1.0, 1.0);

	float thresh = 0.5;

	/*vec4 nc;
	if(tld >= 0.85) nc = black;
	else if(trd >= thresh) nc = black;
	else if(bld >= thresh) nc = black;
	else if(brd >= thresh) nc = black;
	else nc = white;*/

	if(tld >= thresh) tlc = black;
	else tlc = white;
	if(trd >= thresh) trc = black;
	else trc = white;
	if(bld >= thresh) blc = black;
	else blc = white;
	if(brd >= thresh) brc = black;
	else brc = white;

	vec4 nc = tlc * trc * blc * brc;


	/*float Gx = (P1d + 2.0*P2d + P3d) - (P7d + 2.0*P8d + P9d);
	float Gy = (P3d + 2.0*P6d + P9d) - (P1d + 2.0*P4d + P7d);

	float G = sqrt(Gx*Gx+Gy*Gy);

	vec4 newcolor = vec4(G, G, G, 1.0);*/
	gl_FragColor = nc;
}

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

	float Gx = (P1d + 2.0*P2d + P3d) - (P7d + 2.0*P8d + P9d);
	float Gy = (P3d + 2.0*P6d + P9d) - (P1d + 2.0*P4d + P7d);

	float G = sqrt(Gx*Gx+Gy*Gy);

	vec4 newcolor = vec4(G, G, G, 1.0);
	gl_FragColor = newcolor;
}

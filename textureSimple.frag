uniform sampler2D tex;

void main()
{
	float to = 0.001;
	vec2 coords = gl_TexCoord[0].st;
	vec2 brcoords = vec2(coords.s+to, coords.t+to);
	vec2 blcoords = vec2(coords.s+to, coords.t+to);
	vec2 trcoords = vec2(coords.s+to, coords.t+to);
	vec2 tlcoords = vec2(coords.s+to, coords.t-to);
	vec4 color = texture2D(tex,coords);
	vec4 nextcolor = texture2D(tex,nextcoords);
	vec4 newcolor;
	if (color.a > nextcolor.a)
		newcolor = vec4(0.0, 0.0, 0.0, 1.0);
	else
		newcolor = vec4(1.0, 1.0, 1.0, 1.0);
	//vec4 newcolor = vec4(color.a, color.a, color.a, 1.0);
	vec4 bw = vec4(color.r, color.g, color.b, 1.0);
	gl_FragColor = newcolor;
}

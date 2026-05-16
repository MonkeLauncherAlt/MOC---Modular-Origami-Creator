// thanks to user Jaclav for preparing this shader
// follow on github: https://github.com/Jaclav

#version 120

uniform sampler2D texture;

void main() {
	//gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * vec4(0.1, 1, 1, 1);


	//gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * vec4(0.1, 1, 1, sin(gl_TexCoord[0].xy));
	//gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * vec4(0.1, 1, 1, sin(gl_TexCoord[0].y));
	gl_FragColor = texture2D(texture, gl_TexCoord[0].xy) * vec4(gl_TexCoord[0].x, gl_TexCoord[0].y, sin(gl_TexCoord[0].y), 1);

}
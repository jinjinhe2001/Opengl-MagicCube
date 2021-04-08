#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec4 ourcolor;
layout(location = 3) in float Index;

out vec2 v_TexCoord;
out vec4 eachcolor;
out float fIndex;
out	float If_HighLight;

uniform mat4 u_MVP[27];
uniform float HighLight[27];

void main()
{
	gl_Position = u_MVP[gl_InstanceID] * position;
	v_TexCoord = texCoord;
	eachcolor = ourcolor;
	fIndex = Index;
	If_HighLight = HighLight[gl_InstanceID];
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 eachcolor;
in float fIndex;
in float If_HighLight;

uniform vec4 u_Color;
uniform sampler2D u_Texture[6];

void main()
{
	vec4 texColor = eachcolor;
	if ((fIndex - 0) < 0.1f && (0 - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[0], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[0], v_TexCoord);
	else if ((fIndex - 0.2f) < 0.1f && (0.2f - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[1], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[1], v_TexCoord);
	else if ((fIndex - 0.4f) < 0.1f && (0.4f - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[2], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[2], v_TexCoord);
	else if ((fIndex - 0.6f) < 0.1f && (0.6f - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[3], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[3], v_TexCoord);
	else if ((fIndex - 0.8f) < 0.1f && (0.8f - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[4], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[4], v_TexCoord);
	else if ((fIndex - 1.0f) < 0.1f && (1.0f - fIndex) < 0.1f)
		if ((If_HighLight - 1) < 0.1f && (1 - If_HighLight) < 0.1f)
			color = texture(u_Texture[5], v_TexCoord) * vec4(0.6f, 0.7f, 0.6f, 1.0f);
		else
			color = texture(u_Texture[5], v_TexCoord);
};
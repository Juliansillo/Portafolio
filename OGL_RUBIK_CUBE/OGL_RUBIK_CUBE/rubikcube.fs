#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in float flagText;
// texture samplers
uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{   
    FragColor = texture(texture1, TexCoord);
}
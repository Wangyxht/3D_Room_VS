#version 330 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D Texture;

void main()
{
    color = texture(Texture, TexCoords);
}

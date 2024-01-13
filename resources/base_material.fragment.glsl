#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec4 color_modulate;

void main()
{    
    vec4 dif1 = texture(texture_diffuse1, TexCoords);
    vec4 dif2 = texture(texture_diffuse2, TexCoords);
    vec4 dif3 = texture(texture_diffuse3, TexCoords);
    vec4 stage1 = mix(dif2, dif1, dif1.a);
    vec4 stage2 = mix(dif3, stage1, stage1.a);
    vec4 stage3 = stage2 * color_modulate;
    FragColor = stage3;
}

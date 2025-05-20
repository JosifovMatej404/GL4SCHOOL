// bright_extract.frag
#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D scene;

void main() {
    vec3 color = texture(scene, TexCoords).rgb;
    float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722)); // luminance
    if (brightness > 1.0) // threshold
        FragColor = vec4(color, 1.0);
    else
        FragColor = vec4(0.0);
}

#version 330 core
layout (location = 0) in vec2 oPos;
layout (location = 1) in vec2 iPos;

uniform mat4 mat;

void main()
{
    gl_Position = mat * vec4(oPos * sqrt(iPos.x) * 0.5 + vec2(cos(iPos.y), sin(iPos.y)) * iPos.x, 0.0, 1.0);
}

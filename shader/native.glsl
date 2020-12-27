/*
* native
* Ô­Éú»­Ãæ
*/
#version 330 core
in vec2 iUV;
uniform sampler2D iFrame;
uniform vec2 iSize;
uniform float iTime;
uniform sampler2D iTexture0;
uniform sampler2D iTexture1;
uniform sampler2D iTexture2;
uniform sampler2D iTexture3;
out vec4 fragColor;
void main()
{
    fragColor = texture(iFrame, iUV);
}
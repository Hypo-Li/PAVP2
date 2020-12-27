/*
* sketch
* 素描效果
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

float edge()
{
    vec4 color = texture(iFrame, iUV);
    float offsetX = 1.0 / iSize.x;
    float offsetY = 1.0 / iSize.y;

    vec2 offsets[9] = vec2[](
        vec2(-offsetX,  offsetY),   // 左上
        vec2( 0.0f,     offsetY),   // 正上
        vec2( offsetX,  offsetY),   // 右上
        vec2(-offsetX,  0.0f),      // 左
        vec2( 0.0f,     0.0f),      // 中
        vec2( offsetX,  0.0f),      // 右
        vec2(-offsetX,  -offsetY),  // 左下
        vec2( 0.0f,     -offsetY),  // 正下
        vec2( offsetX,  -offsetY)   // 右下
    );
    
    float SobelGxKernel[9] = float[](
        -1, -2, -1,
        0,  0,  0,
        1,  2,  1
    );

    float SobelGyKernel[9] = float[](
        -1,  0,  1,
        -2,  0,  2,
        -1,  0,  1
    );

    float sampleTex;
    float colX = 0.0;
    float colY = 0.0;
    vec3 tempVec;
    for(int i = 0; i < 9; i++)
    {
        tempVec = texture(iFrame, iUV + offsets[i]).rgb;
        sampleTex = 0.2126 * tempVec.r + 0.7152 * tempVec.g + 0.0722 * tempVec.b;
        colX += sampleTex * SobelGxKernel[i];
        colY += sampleTex * SobelGyKernel[i];
    }
    return sqrt(colX * colX + colY * colY);
}

void main()
{
    fragColor = vec4(vec3(1.0 - edge()), 1.0);
}
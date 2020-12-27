/*
* cartoon
* 卡通效果
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

#define SMOOTH_HSV
#ifdef SMOOTH_HSV
vec3 hsv2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

	rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing	

	return c.z * mix( vec3(1.0), rgb, c.y);
}
#else
vec3 hsv2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

	return c.z * mix( vec3(1.0), rgb, c.y);
}
#endif

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

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
    vec3 color = texture(iFrame, iUV).rgb;
    color = rgb2hsv(color);
    color.b *= 6.0;
    color.b = ceil(color.b) / 6.0;
    color = hsv2rgb(color);
    fragColor = vec4(color - edge(), 1.0);
}
/*
* crt
* crtÏÔÊ¾Æ÷Ð§¹û
*/
//iTexture0 ../res/tex/noise0.png;
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

float onOff(float a, float b, float c)
{
	return step(c, sin(iTime + a*cos(iTime*b)));
}

vec2 screenDistort(vec2 uv)
{
	uv -= vec2(.5,.5);
	uv = uv*1.2*(1./1.2+2.*uv.x*uv.x*uv.y*uv.y);
	uv += vec2(.5,.5);
	return uv;
}

vec3 getVideo(vec2 uv)
{
	vec2 look = uv;
	float window = 1./(1.+20.*(look.y-mod(iTime/4.,1.))*(look.y-mod(iTime/4.,1.)));
	look.x = look.x + sin(look.y*10. + iTime)/50.*onOff(4.,4.,.3)*(1.+cos(iTime*80.))*window;
	float vShift = 0.4*onOff(2.,3.,.9)*(sin(iTime)*sin(iTime*20.) + 
										 (0.5 + 0.1*sin(iTime*200.)*cos(iTime)));
	look.y = mod(look.y + vShift, 1.);
	vec3 video = vec3(texture(iFrame,look));
	return video;
}

float noise(vec2 p)
{
	float s = texture(iTexture0,vec2(1.,2.*cos(iTime))*iTime*8. + p*1.).x;
	s *= s;
	return s;
}

float ramp(float y, float start, float end)
{
	float inside = step(start,y) - step(end,y);
	float fact = (y-start)/(end-start)*inside;
	return (1.-fact) * inside;
	
}

float stripes(vec2 uv)
{
	
	float noi = noise(uv*vec2(0.5,1.) + vec2(1.,3.));
	return ramp(mod(uv.y*4. + iTime/2.+sin(iTime + sin(iTime*0.63)),1.),0.5,0.6)*noi;
}

void main()
{
    vec2 uv = screenDistort(iUV);
    vec3 video = getVideo(uv);
    float vigAmt = 3.+.3*sin(iTime + 5.*cos(iTime*5.));
	float vignette = (1.-vigAmt*(uv.y-.5)*(uv.y-.5))*(1.-vigAmt*(uv.x-.5)*(uv.x-.5));
    video += stripes(uv);
	video += noise(uv*2.)/2.;
	video *= vignette;
	video *= (12.+mod(uv.y*30.+iTime,1.))/13.;
	
	fragColor = vec4(video,1.0);
}
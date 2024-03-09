//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul XI - 11_01_Shader.vert |
// ======================================
// 
//  Shaderul de varfuri / Vertex shader - afecteaza geometria scenei; 

#version 330 core

layout(location=0) in vec4 in_Position;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec3 in_Normal;
 
out vec3 FragPos;
out vec3 Normal;
out vec3 inLightPos;
out vec3 inViewPos;
out vec3 inViewPosFog;
out vec3 ex_Color;
out vec3 dir;
 
uniform mat4 matrUmbra;
uniform mat4 myMatrix;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform int codCol;

void main(void)
  {
   	if (codCol != 1)
    {
		gl_Position = projection*view*myMatrix*in_Position;
        Normal =mat3(view*myMatrix)*in_Normal; 
        inLightPos = vec3(view* vec4(lightPos, 1.0f));
        inViewPos =vec3(view*vec4(viewPos, 1.0f));
        inViewPosFog =vec3(view*myMatrix*vec4(viewPos, 1.0f));
        dir = mat3(view*myMatrix) * vec3(0.0,100.0,200.0); // pentru sursa directionala
        FragPos = vec3(view*myMatrix*in_Position);
        ex_Color=in_Color;
        if(codCol == 2) ex_Color = vec3(1.f,0.f,0.f);
        if(codCol == 3) ex_Color = vec3(1.f,1.f,0.f);
        if(codCol == 4) ex_Color = vec3(0.3f,0.3f,1.f);
        if(codCol == 5) ex_Color = vec3(1.f,0.f,1.f);
        if(codCol == 6) ex_Color = vec3(0.f,1.f,1.f);
        if(codCol == 7) ex_Color = vec3(1.f,0.5f,0.f);
        if(codCol == 8) ex_Color = vec3(0.5f,0.5f,1.f);
        if(codCol == 9) ex_Color = vec3(0.8f,0.f,0.8f);
        if(codCol == 10) ex_Color = vec3(1.f,0.f,0.f);
        if(codCol == 11) ex_Color = vec3(1.f,1.f,0.f);
        if(codCol == 12) ex_Color = vec3(0.3f,0.3f,1.f);
    }
	if (codCol == 1)
    {
        ex_Color=in_Color;
		gl_Position = projection*view*matrUmbra*myMatrix*in_Position;
        inViewPosFog =vec3(view*myMatrix*vec4(viewPos, 1.0f));
        FragPos = vec3(view*matrUmbra*myMatrix*in_Position);
    }
  } 
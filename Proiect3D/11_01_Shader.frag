//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul XI - 11_01_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core
 
in vec3 FragPos;  
in vec3 Normal; 
in vec3 inLightPos;
in vec3 inViewPos;
in vec3 inViewPosFog;
in vec3 dir;
in vec3 ex_Color; 

 
out vec4 out_Color;
 
uniform vec3 lightColor;
uniform int codCol; 
uniform vec3 fogColor;
uniform float fogFactor;

vec3 diffuse;
vec3 specular;
 
void main(void)
  {
    float dist = length(inViewPosFog - FragPos);
    float fogFactor = exp(-0.0003f * dist);
    if (codCol != 1) // pentru codCol==0 este aplicata iluminarea
    {
  	    // Ambient
        float ambientStrength = 0.2f;
        vec3 ambient = ambientStrength * lightColor;
  	
        // Diffuse 
        vec3 normala = normalize(Normal);
        vec3 lightDir = normalize(inLightPos - FragPos);
        if(codCol != -1)
        {
            //vec3 lightDir = normalize(dir); // cazul unei surse directionale
            float diff = max(dot(normala, lightDir), 0.0);
            diffuse = diff * lightColor;
        }
        
        if(codCol == -1)
        {
            vec3 lightDir = normalize(vec3(0.f,0.f,0.f));
            diffuse = 1 * lightColor;
        }

        // Specular
        float specularStrength = 1.0f;
        vec3 viewDir = normalize(inViewPos - FragPos);//vector catre observator normalizat (V)
        vec3 reflectDir = reflect(-lightDir, normala); // reflexia razei de lumina (R)
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 1);
        if(codCol > 4)
        {
            specular = specularStrength * spec * lightColor; 
        } 
        else
        {
            specular = vec3(0.0, 0.0, 0.0);
        }
        vec3 emission=vec3(0.0, 0.0, 0.0);
        vec3 result = emission+(ambient + diffuse + specular) * ex_Color;
        if(codCol != -2)
	        out_Color = vec4(mix(fogColor,result,fogFactor),1.0f);
        else
            out_Color = vec4(result,1.0f);

    }

    if (codCol==1) // pentru codCol==1 este desenata umbra
    {
        vec3 black = vec3 (0.0, 0.0, 0.0);
		out_Color = vec4(black,1.0f);
    }
  }

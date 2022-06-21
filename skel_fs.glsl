#version 130

struct Material {
  sampler2D diffuse;
  sampler2D specular;
  float shininess;
};

struct Light {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_POINT_LIGHTS 2

out vec4 frag_col;

in vec3 frag_3Dpos;
in vec3 vs_normal;
in vec2 vs_tex_coord;

uniform Material material;
uniform Light lights[NR_POINT_LIGHTS];
uniform vec3 view_pos;

// function prototypes
vec3 CalcPointLight(Light lights, vec3 normal, vec3 frag_3Dpos, vec3 viewDir);


void main() {
  vec3 normal = vs_normal;
  vec3 viewDir = normalize(view_pos - frag_3Dpos);
  vec3 result;
  // point lights
  for(int i = 0; i < NR_POINT_LIGHTS; i++)
      result += CalcPointLight(lights[i], normal, frag_3Dpos, viewDir);

  frag_col = vec4(result, 1.0);
}


// calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 frag_3Dpos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - frag_3Dpos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - frag_3Dpos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, vs_tex_coord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, vs_tex_coord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular, vs_tex_coord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}
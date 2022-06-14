#version 130

struct Material {
  //vec3 ambient;
  sampler2D diffuse;
  vec3 specular;
  float shininess;
};

struct Light {
  vec3 position;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 frag_col;

in vec3 frag_3Dpos;
in vec3 vs_normal;
in vec2 vs_tex_coord;

uniform Material material;
uniform Light light;
uniform vec3 view_pos;

void main() {
  // Ambient
  vec3 ambient = light.ambient * texture(material.diffuse, vs_tex_coord).rgb;

  // Diffuse
  vec3 normal = vs_normal;
  vec3 lightDir = normalize(light.position - frag_3Dpos);
  float diff = max(dot(normal, lightDir), 0.0);
  vec3 diffuse = light.diffuse * diff * texture(material.diffuse, vs_tex_coord).rgb;

  // Specular
  vec3 viewDir = normalize(view_pos - frag_3Dpos);
  vec3 reflectDir = reflect(-lightDir, normal);  
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
  vec3 specular = light.specular * spec * material.specular;  

  vec3 result = ambient + diffuse + specular;
  frag_col = vec4(result, 1.0);
}
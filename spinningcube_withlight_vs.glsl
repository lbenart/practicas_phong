// #version 130

// in vec3 v_pos;
// in vec3 v_normal;
// //in vec2 v_tex;

// out vec3 frag_3Dpos;
// out vec3 vs_normal;
// //out vec2 vs_tex_coord;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;
// uniform mat3 normal_to_world;

// void main() {
//   gl_Position =
//   frag_3Dpos =
//   vs_normal =
//   //vs_tex_coord =
// }

#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view *  model * vec4(position, 1.0f);
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal;
    TexCoords = texCoords;
}
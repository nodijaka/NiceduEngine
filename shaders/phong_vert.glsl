#version 410 core
const int MaxBones = 128;

layout (location = 0) in vec3 attr_Position;
layout (location = 1) in vec2 attr_Texcoord;
layout (location = 2) in vec3 attr_Normal;
layout (location = 3) in vec3 attr_Tangent;
layout (location = 4) in vec3 attr_Binormal;
layout (location = 5) in ivec4 BoneIDs;
layout (location = 6) in vec4 BoneWeights;

uniform mat4 PROJ_VIEW;
uniform mat4 WORLD;
uniform mat4 BoneTfms[MaxBones];
uniform int u_is_skinned;

out vec3 wpos;
out vec2 texcoord;
out vec3 normal;
out vec3 tangent;
out vec3 binormal;
out vec3 color;

void main()
{
   mat4 B = mat4(1.0);
   if (u_is_skinned > 0)
   {
       B *=    BoneTfms[BoneIDs.x] * BoneWeights.x + 
               BoneTfms[BoneIDs.y] * BoneWeights.y + 
               BoneTfms[BoneIDs.z] * BoneWeights.z + 
               BoneTfms[BoneIDs.w] * BoneWeights.w;
       /* Fallback when bone weights are zero */
       if (BoneWeights.x+BoneWeights.y+BoneWeights.z+BoneWeights.w < 0.01)
           B = BoneTfms[0];
   } 

   wpos = (WORLD * B * vec4(attr_Position, 1)).xyz;
   texcoord = attr_Texcoord;
   normal = normalize( (WORLD * B * vec4(attr_Normal, 0)).xyz );
   tangent = normalize( (WORLD * B * vec4(attr_Tangent, 0)).xyz );
   binormal = normalize( (WORLD * B * vec4(attr_Binormal, 0)).xyz );

   gl_Position = PROJ_VIEW * WORLD * B * vec4(attr_Position, 1);
}
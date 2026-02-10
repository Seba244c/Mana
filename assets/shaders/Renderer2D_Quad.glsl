// stages,vertex,fragment

varying(0, vec4, v_Color, smooth)
varying(1, vec2, v_TexCoord, smooth)
varying(2, uint, v_TexIndex, flat)

#if defined(vertex)
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in uint a_TexIndex;
layout (location = 3) in vec2 a_TexCoord;
uniform(0, mat4, u_ViewProjMatrix)

void main()
{
    v_Color = a_Color;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjMatrix * vec4(a_Pos, 1.0, 1.0);
}
#endif


#if defined(fragment)
layout (location = 0) out vec4 o_Out;

uniform(1, sampler2D, u_Textures[MAX_TEXTURES])

void main()
{
    vec4 texColor = v_Color;
    texColor *= texture(u_Textures[v_TexIndex], v_TexCoord);

    if (texColor.a == 0.0)
        discard; 

    o_Out = texColor;
}
#endif

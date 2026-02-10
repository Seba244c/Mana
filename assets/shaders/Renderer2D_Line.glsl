// stages,vertex,fragment

varying(0, vec4, v_Color, smooth)

#if defined(vertex)
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec4 a_Color;
uniform(0, mat4, u_ViewProjMatrix)

void main()
{
    v_Color = a_Color;
    gl_Position = u_ViewProjMatrix * vec4(a_Pos, 1.0, 1.0);
}
#endif


#if defined(fragment)
layout (location = 0) out vec4 o_Out;

void main()
{
    vec4 texColor = v_Color;

    o_Out = texColor;
}
#endif

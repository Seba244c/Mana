// stages,vertex,fragment

varying(0, vec4, v_FgColor, smooth)
varying(1, vec2, v_TexCoord, smooth)
varying(2, uint, v_TexIndex, flat)

#if defined(vertex)
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec4 a_FgColor;
layout (location = 2) in uint a_TexIndex;
layout (location = 3) in vec2 a_TexCoord;
uniform(0, mat4, u_ViewProjMatrix)

void main()
{
    v_FgColor = a_FgColor;
    v_TexCoord = a_TexCoord;
    v_TexIndex = a_TexIndex;
    gl_Position = u_ViewProjMatrix * vec4(a_Pos, 1.0, 1.0);
}
#endif


#if defined(fragment)
layout (location = 0) out vec4 o_Out;

uniform(1, sampler2D, u_Textures[MAX_TEXTURES])

float medianColor(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float screenPxRange() {
	const float pxRange = 4; // set to distance field's pixel range
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_Textures[v_TexIndex], 0));
    vec2 screenTexSize = vec2(1.0)/fwidth(v_TexCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

void main()
{
    vec4 msd = texture(u_Textures[v_TexIndex], v_TexCoord);
    float sd = medianColor(msd.r, msd.g, msd.b);
    float screenPxDistance = screenPxRange()*(sd - 0.5);
    float opacity = clamp(screenPxDistance + 0.5, 0.0, 1.0);

    o_Out = mix(vec4(0.0), v_FgColor, opacity);
}
#endif

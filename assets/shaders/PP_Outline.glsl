// stages,vertex,fragment

varying(0, vec2, v_TexCoord, smooth)

#if defined(vertex)
layout (location = 0) in vec2 a_Pos;
layout (location = 1) in vec2 a_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Pos, 1.0, 1.0);
}
#endif


#if defined(fragment)
layout (location = 0) out vec4 o_Out;

uniform(0, sampler2D, u_MaskTexture)
uniform(1, vec4, u_OutlineColor)
uniform(2, vec2, u_TexelSize)
uniform(3, float, u_Thickness)

void main() {
    float center = texture(u_MaskTexture, v_TexCoord).r;
    if (center > 0.5) {
        discard;
    }

    float outline = 0.0;
    float thickness = u_Thickness * max(u_TexelSize.x, u_TexelSize.y);
    vec2 pixelOffset = u_TexelSize * u_Thickness;
    
    // Sample 8 directions at the outline distance
    vec2 offsets[8] = vec2[](
        vec2(-1, -1), vec2(0, -1), vec2(1, -1),
        vec2(-1,  0),              vec2(1,  0),
        vec2(-1,  1), vec2(0,  1), vec2(1,  1)
    );
    
    for (int i = 0; i < 8; i++) {
        vec2 samplePos = v_TexCoord + offsets[i] * pixelOffset;
        float neighbor = texture(u_MaskTexture, samplePos).r;
        
        if (neighbor > 0.5) {
            outline = 1.0;
            break;
        }
    }

    if(outline > 0.0) {
        o_Out = u_OutlineColor;
    } else {
        discard;
    }
}
#endif

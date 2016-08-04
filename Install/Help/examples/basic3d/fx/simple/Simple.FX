//
// Simple
//

string XFile = "sphere.x";   // model
int    BCLR = 0xff202080;   // background

// light direction (view space)
float3 lightDir <  string UIDirectional = "Light Direction"; > = {0.577, -0.577, 0.577};

// light intensity
float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

// material reflectivity
float4 k_a : MATERIALAMBIENT = { 1.0f, 1.0f, 1.0f, 1.0f };    // ambient
float4 k_d : MATERIALDIFFUSE = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 k_s : MATERIALSPECULAR= { 1.0f, 1.0f, 1.0f, 1.0f };    // specular
int    n   : MATERIALPOWER = 32;                            // power

// texture
texture Tex0 < string name = "base.tga"; >;

// transformations
float4x4 World      : WORLD;
float4x4 View       : VIEW;
float4x4 Projection : PROJECTION;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 Diff : COLOR0;
    float4 Spec : COLOR1;
    float2 Tex  : TEXCOORD0;
};

VS_OUTPUT VS(
    float3 Pos  : POSITION, 
    float3 Norm : NORMAL, 
    float2 Tex  : TEXCOORD0)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float3 L = -lightDir;

    float4x4 WorldView = mul(World, View);

    float3 P = mul(float4(Pos, 1), (float4x3)WorldView);  // position (view space)
    float3 N = normalize(mul(Norm, (float3x3)WorldView)); // normal (view space)

    float3 R = normalize(2 * dot(N, L) * N - L);          // reflection vector (view space)
    float3 V = -normalize(P);                             // view direction (view space)

    Out.Pos  = mul(float4(P, 1), Projection);             // position (projected)
    Out.Diff = I_a * k_a + I_d * k_d * max(0, dot(N, L)); // diffuse + ambient
    Out.Spec = I_s * k_s * pow(max(0, dot(R, V)), n/4);   // specular
    Out.Tex  = Tex;                                       

    return Out;
}

sampler Sampler = sampler_state
{
    Texture   = (Tex0);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4 PS(
    float4 Diff : COLOR0,
    float4 Spec : COLOR1,
    float2 Tex  : TEXCOORD0) : COLOR
{
    return tex2D(Sampler, Tex) * Diff + Spec;
}

technique TNoShader
{
    pass P0
    {
        // transforms
        WorldTransform[0]   = (World);
        ViewTransform       = (View);
        ProjectionTransform = (Projection);

        // material
        MaterialAmbient  = (k_a); 
        MaterialDiffuse  = (k_d); 
        MaterialSpecular = (k_s); 
        MaterialPower    = (n);
        
        // lighting
        LightType[0]      = DIRECTIONAL;
        LightAmbient[0]   = (I_a);
        LightDiffuse[0]   = (I_d);
        LightSpecular[0]  = (I_s); 
        LightDirection[0] = (lightDir);
        LightRange[0]     = 100000.0f;

        LightEnable[0] = TRUE;
        Lighting       = TRUE;
        SpecularEnable = TRUE;
        
        // samplers
        Sampler[0] = (Sampler);
        
        // texture stages
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0]   = MODULATE;
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = DIFFUSE;

        ColorOp[1]   = DISABLE;
        AlphaOp[1]   = DISABLE;

        // shaders
        VertexShader = NULL;
        PixelShader  = NULL;
    }
}

technique TVertexShaderOnly
{
    pass P0
    {
        // lighting
        Lighting       = FALSE;
        SpecularEnable = TRUE;

        // samplers
        Sampler[0] = (Sampler);

        // texture stages
        ColorOp[0]   = MODULATE;
        ColorArg1[0] = TEXTURE;
        ColorArg2[0] = DIFFUSE;
        AlphaOp[0]   = MODULATE;
        AlphaArg1[0] = TEXTURE;
        AlphaArg2[0] = DIFFUSE;

        ColorOp[1]   = DISABLE;
        AlphaOp[1]   = DISABLE;

        // shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = NULL;
    }
}

technique TVertexAndPixelShader
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }  
}

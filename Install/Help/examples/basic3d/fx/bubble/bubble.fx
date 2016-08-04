//
// Bubble
//

matrix worldi : WorldIT;
matrix wvp : WorldViewProjection;
matrix mworld : World;
matrix viewInv : ViewIT;
float4 lightPos : Position < string UIPosition = "Light Position"; >;
float ticks : Time;

string XFile = "sphere.x";
texture colorTexture < string Name = "blue.tga"; >;

pixelshader pNIL;

struct VS_INPUT
{
    float3 Position     : POSITION;
    float4 UV           : TEXCOORD0;
    float3 Normal       : NORMAL;
};

struct VS_OUTPUT
{
    float4 HPosition    : POSITION;
    float4 TexCoord0    : TEXCOORD0;
    float4 diffCol      : COLOR0;
    float4 specCol      : COLOR1;
};

float4 ambiColor={0.2f, 0.2f, 0.2f, 1.0f};
float4 surfColor={0.8f, 0.8f, 0.8f, 1.0f};
float4 liteColor={1.0f, 1.0f, 1.0f, 1.0f};
float specExpon=22.0;
float horizontal=0.2;
float vertical=3.0;
float timeScale=0.005;

VS_OUTPUT VShade(VS_INPUT IN)
{
    VS_OUTPUT   OUT;

    // world normal
    float3 worldNormal = mul(IN.Normal, mworld).xyz;
    worldNormal = normalize(worldNormal);
    float timeNow = ((ticks/100.0)/timeScale);

    //build float4
    float4 tempPos;
    tempPos.xyz = IN.Position.xyz;
    tempPos.w   = 1.0;

    float iny = tempPos.y * vertical + timeNow;
    float wiggleX = sin(iny) * horizontal;
    float wiggleY = cos(iny) * horizontal; // deriv
    worldNormal.y = worldNormal.y + wiggleY;
    worldNormal = normalize(worldNormal);
    tempPos.x = tempPos.x + wiggleX;

    //compute worldspace position
    float3 worldSpacePos = mul(tempPos,mworld).xyz;
    float3 LightVec = normalize(lightPos - worldSpacePos);
    float ldn = dot(LightVec,worldNormal);

    float diffComp = max(0,ldn);
    float4 diffContrib = surfColor * ( diffComp * liteColor + ambiColor);
    OUT.diffCol = diffContrib;
    OUT.diffCol.w = 1.0;
    OUT.TexCoord0 = IN.UV;

    float3 EyePos = viewInv[3].xyz;
    float3 vertToEye = normalize(EyePos - worldSpacePos);
    float3 halfAngle = normalize(vertToEye + LightVec);

    float hdn = pow(max(0,dot(halfAngle,worldNormal)),specExpon);
    OUT.specCol = hdn * liteColor;

    // transform into homogeneous-clip space
    OUT.HPosition = mul(tempPos, wvp);

    return OUT;
}

uniform sampler   sampler0 = 
sampler_state 
{
    texture = <colorTexture>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

struct PS_INPUT
{
    float4 TexCoord0    : TEXCOORD0;
    float4 diffCol      : COLOR0;
    float4 specCol      : COLOR1;
};

struct pixelOutput
{
  float4 col : COLOR;
};

pixelOutput MrWigglePS_t(PS_INPUT IN)
{
    // Lookup the colorMap texture
    pixelOutput OUT; 
    float4 result = IN.diffCol * tex2D( sampler0, IN.TexCoord0) + IN.specCol;
    OUT.col = result;
    return OUT;
}

//////////////////////////////////////
// Techniques specs follow
//////////////////////////////////////
technique t0
{
    pass p0
    {
        VertexShader = compile vs_2_0 VShade();
        PixelShader = compile ps_1_1 MrWigglePS_t();

        // Z-buffering not to be used
        ZEnable      = false;
        ZWriteEnable = false;
        CullMode     = CW;

        // enable alpha blending
        AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = INVSRCALPHA;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;
    }
    pass p1
    {
        VertexShader = compile vs_2_0 VShade();
        PixelShader = compile ps_1_1 MrWigglePS_t();

        // Z-buffering not to be used
        ZEnable      = false;
        ZWriteEnable = false;
        CullMode     = CCW;

        // enable alpha blending
        AlphaBlendEnable = TRUE;
        SrcBlend         = SRCALPHA;
        DestBlend        = INVSRCALPHA;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;
    }
}

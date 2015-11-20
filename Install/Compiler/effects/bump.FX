//
// Bump2
//

// Constants
float4 lhtDir < string UIDirectional = "Light Direction"; >; 
float4 lhtPos < string UIPosition = "Light Position"; >;
vector vCPS : CameraPosition;  
matrix mTot : WorldViewProjection;
matrix mWldInv : WorldIT;
matrix mWld :  World; 

// Model and Texture Names
string XFile = "default.x";
texture tBase < string name = "base.tga"; >;
texture tBump < string name = "bump.dds"; >;

struct VS_INPUT
{
    float4  Pos     : POSITION;
    float3  Normal  : NORMAL;
    float3  Tex0    : TEXCOORD0;
    float3  Tangent : TANGENT;    
};

struct VS_OUTPUT
{
    float4  Pos           : POSITION;
    float3  tLight        : COLOR;
    float3  tNormal       : TEXCOORD0;   
    float3  depRead       : TEXCOORD1;    
    float3  tH            : TEXCOORD2; 
    float3  Tex0          : TEXCOORD3;    
};

VS_OUTPUT VShade(VS_INPUT i, uniform int ShadeMethod)
{
    VS_OUTPUT   o;
    float3      wNormal, wTangent, wBinorm, tLight;
    
    //tranform position    
    o.Pos = mul( i.Pos, mTot); 
           
    //transform normal
    //and tangent
    wNormal = mul( i.Normal, mWld );
    wTangent = mul( i.Tangent, mWld );

    //lee, normalize to defeat scaled world matrices
    wNormal = normalize(wNormal);
    wTangent = normalize(wTangent);

    //Cross product to create BiNormal
    wBinorm = cross( wTangent, wNormal );

    // The first method is diffuse dot3 bumpmap
    // transform the light vector into Tangent Space   
    float3x3 tangentMatrix = {wTangent, wBinorm, wNormal};
    tangentMatrix = transpose( tangentMatrix);
    tLight = mul(-lhtDir, tangentMatrix);
   
    // all other methods are specular dot3 bump 
    // and therefore use the half angle instead of the light       
    // compute the half angle vector    
    float3 wPos, H, tH;
        
    wPos = mul( i.Pos, mWld); 
       
    H = vCPS - wPos;
    H = normalize(H);
      
    H = H -lhtDir.xyz;
    H = normalize(H);

    //transform the half angle vector into tangent space
    tH = mul(H, tangentMatrix);
    o.tH = tH;
    o.depRead = tH;    

    if (ShadeMethod==1)
       tLight = tH;

    //mutiply by a half to bias, then add half
    o.tLight = 0.5f + tLight * 0.5f;
   
    // Copy UVs
    o.Tex0 = i.Tex0;                  
    o.tNormal = i.Tex0;                  
    
    return o;
}    

uniform sampler   sampler0 = 
sampler_state 
{
    texture = <tBase>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

uniform sampler   sampler1 = 
sampler_state 
{
    texture = <tBump>;
    MinFilter = Linear;
    MagFilter = Linear;
    MipFilter = Linear;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
};

struct PS_INPUT
{
    float3  tLight        : COLOR;
    float3  tNormal       : TEXCOORD0;    
    float3  depRead       : TEXCOORD1;    
    float3  tH            : TEXCOORD2;    
    float3  Tex0          : TEXCOORD3;    
};

struct PS_OUTPUT
{
    float4  Col        : COLOR;
};
              
PS_OUTPUT PShade(PS_INPUT i, uniform int ShadeMethod)
{
    PS_OUTPUT o;
    float4    cosang, t0, t1;
    float3    tLight;
    
    // Sample diffuse texture and Normal map    
    t0 = tex2D( sampler0, i.Tex0);
    // _bx2 = 2 * source - 1
    t1 = 2 * tex2D( sampler1, i.tNormal) - 1;
    tLight = 2 * i.tLight - 1;
       
    // DP Lighting in tangent space (where normal map is based)
    cosang = dot( t1, tLight );    
    if (ShadeMethod==1) 
    {
          cosang = cosang * cosang;
          cosang = cosang * cosang;
          cosang = cosang * cosang;
          cosang = cosang * cosang;                              
    }        

    // Modulate with diffuse texture    
    o.Col = t0 * cosang;
   
    return o;
}

PS_OUTPUT PShade2(PS_INPUT i)
{
    PS_OUTPUT o;
    float4    cosang, tDiffuse, tNormal, col;
    float3    tLight;
    
    // Sample diffuse texture and Normal map    
    tDiffuse = tex2D( sampler0, i.Tex0 );    

    // sample tLight
    // _bx2 = 2 * source - 1
    tNormal = 2 * tex2D( sampler1, i.tNormal) - 1;
    tLight = 2 * i.tLight - 1;
      
    // DP Lighting in tangent space (where normal map is based)
    col = dot( tNormal, tLight ) * tDiffuse;    
    
    cosang = dot( tNormal,i.tH );    
    cosang = pow( cosang, 32);    

    // Modulate with diffuse texture    
    col = col  + cosang;

    o.Col = col;
   
    return o;
}

technique tec0
{ 
    pass p0
    {
        VertexShaderConstant[0] = <mTot>; 
        VertexShaderConstant[4] = <mWldInv>;
        VertexShaderConstant[8] = <lhtPos>;
        VertexShaderConstant[9] = <vCPS>;
        VertexShaderConstant[10] = { 0, 0.5, 1, 2 };

        PixelShaderConstant[0] = { 0, 0.5, 1, -0.2 };
        PixelShaderConstant[1] = { 66, 66, 66, 66 };
        PixelShaderConstant[2] = { 0.985, 1, 0.945, 1 };
        PixelShaderConstant[3] = { 1, 1, 1, 1 };
        PixelShaderConstant[4] = { 2, 0.5, 4, 8 };

        Texture[0]   = <tBase>;
        Texture[1]   = <tBump>;

        AddressU[0] = wrap;
        AddressU[1] = wrap;
        AddressV[0] = wrap;
        AddressV[1] = wrap;
        AddressW[0] = wrap;
        AddressW[1] = wrap;

        MinFilter[0] = Linear;
        MagFilter[0] = Linear;
        MipFilter[0] = Linear;
        MinFilter[1] = Linear;
        MagFilter[1] = Linear;
        MipFilter[1] = Linear;

        VertexShader = 
        asm
        {
    vs.1.1

    dcl_position   v0
    dcl_normal     v1
    dcl_texcoord   v2
    dcl_tangent    v3
    dcl_binormal   v4

    ; xform and output vertex
    m4x4 oPos, v0, c0

    ; base map and bump map texture coordinates
    mov  oT0, v2
    mov  oT1, v2

    ; find light position in object space 
    mov r1, c8
    m4x4 r0, r1, c4

    ; find light vector in object space
    sub r0, r0, v0

    ; normalize light vector
    dp3 r1.x, r0, r0
    rsq r1.x, r1.x
    mul r0, r0.xyz, r1.xxx

    ; move light vector into tangent space
    dp3 r1.x, r0, v3  ; tangent
    dp3 r1.y, r0, v4  ; binormal
    dp3 r1.z, r0, v1  ; normal

    mov oT2, r1  ; light vector into tex coord2 (to index normalizer cubemap)
    mov r5, r1   ; cache light vector in r5 for later use

    ; find eye position in object space 
    mov r1, c9
    m4x4 r0, r1, c4

    ; find view vector in object space
    sub r0, r0, v0

    ; normalize view vector
    dp3 r1.x, r0, r0
    rsq r1.x, r1.x
    mul r0, r0.xyz, r1.xxx

    ; move view vector into tangent space
    dp3 r1.x, r0, v3  ; tangent
    dp3 r1.y, r0, v4  ; binormal
    dp3 r1.z, r0, v1  ; normal

    ; find half angle vector (L + V) / 2
    add r2, r1, r5

    ; normalize half angle vector
    dp3 r1.x, r2, r2
    rsq r1.x, r1.x
    mul r2, r2.xyz, r1.xxx

    mov oT3, r2  ; half angle vector into tex coord3 (to index normalizer cubemap)
        };


        PixelShader = 
        asm
        {
       ps.2.0

       dcl t0
       dcl t1
       dcl t2
       dcl t3

       dcl_2d   s0
       dcl_2d   s1

       texld r0, t0, s0 ; base map
       texld r1, t1, s1 ; bump map

       ; normalize L
       dp3 r2.x, t2, t2
       rsq r2.x, r2.x
       mul r2, t2, r2.x

       ; normalize H
       dp3 r3.x, t3, t3
       rsq r3.x, r3.x
       mul r3, t3, r3.x

       ; scale and bias N
       sub r1, r1, c4.y
       mul r1, r1, c4.x

       ; N.L
       dp3_sat r2, r1, r2

       ; N.L * diffuse_light_color
       mul r2, r2, c2
   
       ; (N.H)
       dp3_sat r1, r1, r3

       ; (N.H)^k
       pow r1, r1.x, c1.x

       ; (N.H)^k * specular_light_color
        mul r1, r1, c3

       ; [(N.L) * base] + (N.H)^k
       mad_sat r0, r0, r0, r1
   
       mov oC0, r0
        };
    }
}

technique tec1
{ 
    pass p0
    {      
        VertexShader = compile vs_1_1 VShade(0);
        PixelShader = compile ps_1_1 PShade(0);
    }
}

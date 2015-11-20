//--------------------------------------------------------------------------------------
// Origial File: ShadowVolume.fx
// Modified: StencilShadow.fx
//--------------------------------------------------------------------------------------

// DBPro Specific until DXSAS (U6)
float3   g_vLightView : POINTLIGHT;
float4x4 g_mWorld : WORLD;
float4x4 g_mViewProjection : VIEWPROJECTION;
float4x4 g_mWorldViewProjection : WORLDVIEWPROJECTION;
float    g_mShadowRange : MESHRADIUS;

/*********** DBPRO UNTWEAKABLES **********/
float4x4 boneMatrix[60] : BoneMatrixPalette;

// Shader Code
void VertShadowVolume( 
		       float3 vPos : POSITION,
                       float3 vNormal : NORMAL,
		       float4 Blendweight : TEXCOORD1,
		       float4 Blendindices : TEXCOORD2,
                       out float4 oPos : POSITION
                       )
{
    // bone animate the shadow mesh
    float3 netPosition = 0, netNormal = 0;
    for (int i = 0; i < 4; i++)
    {
     float index = Blendindices[i];
     float3x4 model = float3x4(boneMatrix[index][0], boneMatrix[index][1], boneMatrix[index][2]);
     float3 vec3 = mul(model, float4(vPos, 1));
     vec3 = vec3 + boneMatrix[index][3].xyz;
     float3x3 rotate = float3x3(model[0].xyz, model[1].xyz, model[2].xyz); 
     float3 norm3 = mul(rotate, vNormal);
     netPosition += vec3.xyz * Blendweight[i];
     netNormal += norm3.xyz * Blendweight[i];
    }
    float4 tempPos = float4(netPosition,1.0);
    float4 PosView = mul( tempPos, g_mWorld );
    float3 N = mul( netNormal, (float3x3)g_mWorld );
    float3 LightVec = PosView.xyz - g_vLightView;

    // Extrude quad vertex if normal faces away from light
    LightVec = normalize ( LightVec );
    if( dot( N, -LightVec  ) < 0.0f )
    {
      // stretch out
      PosView.x += LightVec.x * g_mShadowRange;
      PosView.y += LightVec.y * g_mShadowRange;
      PosView.z += LightVec.z * g_mShadowRange;
    }
    else
    {
      // defeat self-shadow zclash
      PosView.x += LightVec.x*0.1f;
      PosView.y += LightVec.y*0.1f;
      PosView.z += LightVec.z*0.1f;
    }
    oPos = mul( PosView, g_mViewProjection );
}

//--------------------------------------------------------------------------------------
// Techniques
//--------------------------------------------------------------------------------------

technique RenderShadowVolume
{
    pass P0
    {
        VertexShader = compile vs_2_0 VertShadowVolume();
        StencilPass = Keep;
        StencilZFail = Incr;
        CullMode     = CCW;
    }
    pass P1
    {
        VertexShader = compile vs_2_0 VertShadowVolume();
        CullMode = Cw;
        StencilZFail = Decr;
    }
}

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

// Shader Code
void VertShadowVolume( float4 vPos : POSITION,
                       float3 vNormal : NORMAL,
                       out float4 oPos : POSITION )
{
    float4 PosView = mul( vPos, g_mWorld );
    float3 LightVec = PosView.xyz - g_vLightView;
    float3 N = mul( vNormal, (float3x3)g_mWorld );

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
        VertexShader = compile vs_1_1 VertShadowVolume();
        StencilPass = Keep;
        StencilZFail = Incr;
        CullMode     = CCW;
    }
    pass P1
    {
        VertexShader = compile vs_1_1 VertShadowVolume();
        CullMode = Cw;
        StencilZFail = Decr;
    }
}

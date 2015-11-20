//
// FX Effect file - QUAD
//

float2 ViewSize : ViewSize = { 640, 480 };

texture background < string Name=""; >;	

sampler2D background_smp = sampler_state 
{
	Texture = <background>;
	AddressU = Clamp; AddressV = Clamp;
	MinFilter = Point; MagFilter = Point; MipFilter = None;
};

struct output
{
	float4 opos    : POSITION;  
 	float2 uv      : TEXCOORD0;  
};

output VS( float4 pos : POSITION )
{
	output OUT;
	OUT.uv = pos.xy * float2(0.5,-0.5) + 0.5;

	// Go here for more info: http://www.sjbrown.co.uk/?article=directx_texels
	float4 oPos = float4( pos.xy + float2( -1.0f/ViewSize.x, 1.0f/ViewSize.y ),0.0,1.0 );
	OUT.opos = oPos;

	return OUT;	
}

float4 PS(output IN) : COLOR
{
	return tex2D(background_smp,IN.uv);
}
  
technique screenquad
{
	pass p0
	{		
		VertexShader = compile vs_1_0 VS();
		PixelShader  = compile ps_1_0 PS();	
	}
}


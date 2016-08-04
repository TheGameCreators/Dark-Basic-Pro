// vertex shader version
vs.1.0

// take the vertex position data ( v0 ) and multiply it by the matrix which we have put in 
// register 0, the result is stored in oPos which is the position output register
m4x4 oPos, v0, c0 

// calculate the amount of light falling on a vertex by taking the normal and dot it with the // light vector
dp3 r0, v3, c4

// multiply the result of the dot product with the diffuse colour and place it in the oD0 
// register
mul oD0, r0.x, v5

// pass the texture coordinates from the input register ( v7 ) into the output register ( oT0 )
mov oT0.xy, v7

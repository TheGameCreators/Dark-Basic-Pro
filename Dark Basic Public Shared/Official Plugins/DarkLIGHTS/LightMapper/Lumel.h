#ifndef LUMEL_H
#define LUMEL_H

struct D3DXVECTOR3;
class LMPolyGroup;

class Lumel
{

private:

	//float posX, posY, posZ;
	float colR, colG, colB;

public:

	//float fShadow;
	//LMPolyGroup *pRefPoly;

	Lumel( );
	~Lumel( );

	//float GetPosX( );
	//float GetPosY( );
	//float GetPosZ( );
	//D3DXVECTOR3 GetPos( );

	float GetColR( );
	float GetColG( );
	float GetColB( );

	int GetFinalColR( );
	int GetFinalColG( );
	int GetFinalColB( );

	DWORD GetColor( );

	void SetCol( float red, float green, float blue );
	void AddCol( float red, float green, float blue );
	void AddColR( float red );
	void AddColG( float green );
	void AddColB( float blue );

	void Ambient( float red, float green, float blue );
};

inline void Lumel::SetCol( float red, float green, float blue )
{
	colR = red;	
	colG = green;
	colB = blue;
}

inline void Lumel::AddCol( float red, float green, float blue )
{
	AddColR( red );	
	AddColG( green );
	AddColB( blue );
}

inline void Lumel::AddColR( float red ) 
{
	//if ( red < 0 ) red = 0;
	colR += red;
}

inline void Lumel::AddColG( float green )
{
	//if ( green < 0 ) green = 0;
	colG += green;
}

inline void Lumel::AddColB( float blue )
{
	//if ( blue < 0 ) blue = 0;
	colB += blue;
}

inline void Lumel::Ambient( float red, float green, float blue )
{
	//ambient light to fake radiosity

	//limiting ambient
	//if ( colR < red ) colR = red;
	//if ( colG < green ) colG = green;
	//if ( colB < blue ) colB = blue;

	//adding ambient
	colR += red;
	colG += green;
	colB += blue;
}

#endif


#ifndef _CVECTOR_H_
#define _CVECTOR_H_

class cPlane3;

class cVector4
{
	public:
		cVector4 ( );
		cVector4 ( const cVector4 & vec );
		cVector4 ( float _x, float _y, float _z, float _w );
    
		union
		{
			struct
			{
				float    x;
				float    y;
				float    z;
				float    w;
			};

			float v [ 4 ];
		};
		
		bool                IsEmpty        ( void ) const;
		bool                SetBaryCentric ( const cVector4& V1, const cVector4& V2, const cVector4& V3, const float& f, const float& g );
		bool                SetCatmullRom  ( const cVector4& V1, const cVector4& V2, const cVector4& V3, const cVector4& V4, const float& s );
		bool                SetHermite     ( const cVector4& V1, const cVector4& T1, const cVector4& V2, const cVector4& T2, const float& s );
		cVector4            Lerp           ( const cVector4& V1, const float& s ) const;
		cVector4            Maximize       ( const cVector4& V1 ) const;
		cVector4            Minimize       ( const cVector4& V1 ) const;
		cVector4&           Scale          ( const float &Scale );
		cVector4            Cross          ( const cVector4& V1, const cVector4& V2 ) const;
		float               Dot            ( const cVector4& vec ) const;
		float               Length         ( void ) const;
		float               SquareLength   ( void ) const;
		bool                FuzzyCompare   ( const cVector4& vecCompare,  const float& Tolerance ) const;
		cVector4&           Normalize      ( void );

		cVector4  operator +  ( const cVector4& vec ) const;
		cVector4  operator -  ( const cVector4& vec ) const;
		cVector4  operator *  ( const float& Value  ) const;
		cVector4  operator /  ( const float& Value  ) const;
    
		cVector4& operator += ( const cVector4& vec );
		cVector4& operator -= ( const cVector4& vec );
		cVector4& operator /= ( const float& Value  );
		cVector4& operator *= ( const float& Value  );

		cVector4  operator +  ( ) const;
		cVector4  operator -  ( ) const;
		cVector4& operator =  ( const cVector4& vec );
		bool      operator == ( const cVector4& vec ) const;
		bool      operator != ( const cVector4& vec ) const;

		operator float * ( );
		operator const float * ( ) const;

		friend cVector4 operator * ( float Value, const cVector4& vec );
};

class cVector3
{
	public:

		cVector3 ( );
		cVector3 ( const cVector3 & vec );
		cVector3 ( float _x, float _y, float _z );
    
		union
		{
			struct
			{
				float    x;
				float    y;
				float    z;
			};

			float v [ 3 ];
		};
		
		bool                IsEmpty         ( void ) const;
		bool                SetBaryCentric  ( const cVector3& V1, const cVector3& V2, const cVector3& V3, const float& f, const float& g );
		bool                SetCatmullRom   ( const cVector3& V1, const cVector3& V2, const cVector3& V3, const cVector3& V4, const float& s );
		bool                SetHermite      ( const cVector3& V1, const cVector3& T1, const cVector3& V2, const cVector3& T2, const float& s );
		cVector3            Lerp            ( const cVector3& V1, const float& s) const;
		cVector3            Maximize        ( const cVector3& V1 ) const;
		cVector3            Minimize        ( const cVector3& V1 ) const;
		cVector3&           Scale           ( const float &Scale );
		cVector3            Cross           ( const cVector3& V1 ) const;
		float               Dot             ( const cVector3& vec ) const;
		float               Length          ( void ) const;
		float               SquareLength    ( void ) const;
		bool                FuzzyCompare    ( const cVector3& vecCompare,  const float& Tolerance ) const;
		cVector3&           Normalize       ( void );
		float               DistanceToPlane ( const cPlane3& Plane ) const;
		float               DistanceToPlane ( const cPlane3& Plane, const cVector3& Direction ) const;
		float               DistanceToLine  ( const cVector3 &vecStart, const cVector3& vecEnd ) const;

		cVector3  operator +  ( const cVector3& vec ) const;
		cVector3  operator -  ( const cVector3& vec ) const;
		cVector3  operator *  ( const cVector3& vec ) const;
		cVector3  operator *  ( const float& Value  ) const;
		cVector3  operator /  ( const float& Value  ) const;
    
		cVector3& operator += ( const cVector3& vec );
		cVector3& operator -= ( const cVector3& vec );
		cVector3& operator /= ( const float& Value  );
		cVector3& operator *= ( const float& Value  );

		cVector3  operator +  ( ) const;
		cVector3  operator -  ( ) const;
		cVector3& operator =  ( const cVector3& vec );
		bool      operator == ( const cVector3& vec ) const;
		bool      operator != ( const cVector3& vec ) const;

		operator float * ( );
		operator const float * ( ) const;

		friend cVector3 operator * ( float Value, const cVector3& vec );
};

class cVector2
{
	public:
		cVector2 ( ) { }
		cVector2 ( const cVector2 & vec ) { x = vec.x; y = vec.y; }
		cVector2 ( float _x, float _y ) { x = _x; y = _y; }
    
		union
		{
			struct
			{
				float    x;
				float    y;
			};

			float v [ 2 ];
		};
};

#endif

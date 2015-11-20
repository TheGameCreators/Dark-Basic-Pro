#include "sAnimation.h"

sAnimation::sAnimation ( )
{
	m_Name            = NULL;
	m_FrameName       = NULL;
	m_Frame           = NULL;
	m_Loop            = FALSE;
	m_Linear          = TRUE;
	m_NumPositionKeys = 0;
	m_PositionKeys    = NULL;
	m_NumRotateKeys   = 0;
	m_RotateKeys      = NULL;
	m_NumScaleKeys    = 0;
	m_ScaleKeys       = NULL;
	m_NumMatrixKeys   = 0;
	m_MatrixKeys      = NULL;
	m_Next            = NULL;
}

sAnimation::~sAnimation ( )
{
	SAFE_DELETE_ARRAY ( m_Name );
    SAFE_DELETE_ARRAY ( m_FrameName );
    SAFE_DELETE_ARRAY ( m_PositionKeys );
    SAFE_DELETE_ARRAY ( m_RotateKeys );
    SAFE_DELETE_ARRAY ( m_ScaleKeys );
    SAFE_DELETE_ARRAY ( m_MatrixKeys );
    SAFE_DELETE ( m_Next );
}

void sAnimation::Update ( float Time, BOOL Smooth )
{
	unsigned long	i,
					Key;
	float			TimeDiff,
					IntTime;
	D3DXMATRIX		Matrix,
					MatTemp;
	D3DXVECTOR3		Vector;
	D3DXQUATERNION	Quat;

	if ( m_Frame == NULL )
		return;

    // update rotation, scale, and position keys
    if ( m_NumRotateKeys || m_NumScaleKeys || m_NumPositionKeys )
	{
		D3DXMatrixIdentity ( &Matrix );

		if ( m_NumRotateKeys && m_RotateKeys != NULL )
		{
			// find the key that fits this time
			Key = 0;

			for ( i = 0; i < m_NumRotateKeys; i++ )
			{
				if ( m_RotateKeys [ i ].Time <= Time )
					Key = i;
				else
					break;
			}

			// if it's the last key or non-smooth animation, then just set the key value.
			if ( Key == ( m_NumRotateKeys - 1 ) || Smooth == FALSE )
			{
				Quat = m_RotateKeys [ Key ].Quaternion;
			}
			else 
			{
				// calculate the time difference and interpolate time
				TimeDiff = (float)m_RotateKeys [ Key + 1 ].Time - (float)m_RotateKeys [ Key ].Time;
				IntTime  = Time - m_RotateKeys [ Key ].Time;

				// get the quaternion value
				D3DXQuaternionSlerp ( &Quat, &m_RotateKeys [ Key ].Quaternion, &m_RotateKeys [ Key + 1 ].Quaternion, ( ( float ) IntTime / ( float ) TimeDiff ) );
			}

			// combine with the new matrix
			D3DXMatrixRotationQuaternion ( &MatTemp, &Quat );
			D3DXMatrixMultiply           ( &Matrix, &Matrix, &MatTemp );
		}

		if ( m_NumScaleKeys && m_ScaleKeys != NULL )
		{
			// find the key that fits this time
			Key = 0;
			
			for ( i = 0; i < m_NumScaleKeys; i++ )
			{
				if ( m_ScaleKeys [ i ].Time <= Time )
					Key = i;
				else
					break;
			}

			// if it's the last key or non-smooth animation, then just set the key value
			if ( Key == ( m_NumScaleKeys - 1 ) || Smooth == FALSE )
			{
				Vector = m_ScaleKeys [ Key ].Scale;
			} 
			else 
			{
				// calculate the time difference and interpolate time
				IntTime  = Time - m_ScaleKeys [ Key ].Time;

				// get the interpolated vector value
				Vector = m_ScaleKeys [ Key ].Scale + m_ScaleKeys [ Key ].ScaleInterpolation * ( float ) IntTime;
			}

			// combine with the new matrix
			D3DXMatrixScaling  ( &MatTemp, Vector.x, Vector.y, Vector.z );
			D3DXMatrixMultiply ( &Matrix, &Matrix, &MatTemp );
		}

		if ( m_NumPositionKeys && m_PositionKeys != NULL )
		{
			// find the key that fits this time
			Key = 0;

			for ( i = 0; i < m_NumPositionKeys; i++ )
			{
				if ( m_PositionKeys [ i ].Time <= Time )
					Key = i;
				else
					break;
			}

			// if it's the last key or non-smooth animation, then just set the key value.
			if ( Key == ( m_NumPositionKeys - 1 ) || Smooth == FALSE )
			{
				Vector = m_PositionKeys [ Key ].Pos;
			} 
			else 
			{
				// calculate the time difference and interpolate time
				IntTime  = Time - m_PositionKeys [ Key ].Time;

				// get the interpolated vector value
				Vector = m_PositionKeys [ Key ].Pos + m_PositionKeys [ Key ].PosInterpolation * ( float ) IntTime;
			}

			// combine with the new matrix
			D3DXMatrixTranslation ( &MatTemp, Vector.x, Vector.y, Vector.z );
			D3DXMatrixMultiply    ( &Matrix, &Matrix, &MatTemp );
		}
	
		// set the new matrix
		m_Frame->m_matTransformed = Matrix;
	}

	// update matrix keys
	if ( m_NumMatrixKeys && m_MatrixKeys != NULL )
	{
		// find the key that fits this time
		Key = 0;
		
		for ( i = 0; i < m_NumMatrixKeys; i++ )
		{
			if ( m_MatrixKeys [ i ].Time <= Time )
				Key = i;
			else
				break;
		}

		// if it's the last key or non-smooth animation, then just set the matrix.
		if ( Key == ( m_NumMatrixKeys - 1 ) || Smooth == FALSE )
		{
			m_Frame->m_matTransformed = m_MatrixKeys [ Key ].Matrix;
		}
		else 
		{
			// calculate the time difference and interpolate time
			IntTime  = Time - m_MatrixKeys [ Key ].Time;

			// set the new interpolation matrix
			Matrix = m_MatrixKeys [ Key ].MatInterpolation * IntTime;
			m_Frame->m_matTransformed = Matrix + m_MatrixKeys [ Key ].Matrix;
		}
	}
}
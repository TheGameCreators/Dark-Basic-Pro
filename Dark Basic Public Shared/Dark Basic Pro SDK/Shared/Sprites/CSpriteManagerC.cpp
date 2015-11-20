#include "cspritemanagerc.h"
#include <algorithm>    // for std::sort

#define DARKSDK __declspec ( dllexport )
#define SAFE_DELETE( p )       { if ( p ) { delete ( p );       ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release ( ); ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if ( p ) { delete [ ] ( p );   ( p ) = NULL; } }

// External types (duplicaed)
typedef int 							( *RetIntFunctionPointerPFN  )     ( ... );
typedef float 							( *RetFloatFunctionPointerPFN  )   ( ... );
typedef LPDIRECT3DTEXTURE9				( *IMAGE_RetLPD3DTEX9ParamIntPFN ) ( int );

// External Data from rest of code
extern LPDIRECT3DDEVICE9				m_pD3D;
extern IMAGE_RetLPD3DTEX9ParamIntPFN	g_Image_GetPointer;
extern RetIntFunctionPointerPFN			g_Image_GetWidth;
extern RetIntFunctionPointerPFN			g_Image_GetHeight;
extern RetFloatFunctionPointerPFN		g_Image_GetUMax;
extern RetFloatFunctionPointerPFN		g_Image_GetVMax;

extern int g_iFilterMode;



//////////////////////////
// Class static storage //
//////////////////////////



D3DXVECTOR3                     CSpriteManager::vecDisplayMin;
D3DXVECTOR3                     CSpriteManager::vecDisplayMax;
std::vector<tagSpriteData*>     CSpriteManager::m_SpriteDrawList;
CSpriteManager::t_SpriteList    CSpriteManager::m_SortedSpriteList;



//////////////////////////
// Class member classes //
//////////////////////////



struct CSpriteManager::PrioritiseSprite
{
    int m_id;
    tagSpriteData* m_ptr;

    PrioritiseSprite ( SpritePtr pItem ) 
        : m_id( pItem->first), m_ptr( pItem->second )
    {}

    PrioritiseSprite ( SpritePtrConst pItem ) 
        : m_id( pItem->first), m_ptr( pItem->second )
    {}

    bool operator < (const PrioritiseSprite& Other) const
    {
        // Sort by Priority
        if (m_ptr->iPriority < Other.m_ptr->iPriority)        return true;
        if (m_ptr->iPriority > Other.m_ptr->iPriority)        return false;
        
        // For matching priorities, sort by the image id
        // to try and avoid texture lookups and changes.
        if (m_ptr->iImage < Other.m_ptr->iImage)              return true;
        if (m_ptr->iImage > Other.m_ptr->iImage)              return false;

        // For matching image ids, sort by transparency
        // to try and avoid transparency mode changes.
        if (m_ptr->bTransparent < Other.m_ptr->bTransparent)  return true;
        if (m_ptr->bTransparent > Other.m_ptr->bTransparent)  return false;

        // Where everything else matches, sort by the sprite id
        // to give a unique order to all sprites
        return (m_id < Other.m_id);
    }
};



////////////////////////////
// Class member functions //
////////////////////////////



CSpriteManager::CSpriteManager ( void )
{
    m_CurrentId = 0;
    m_CurrentPtr = 0;
    m_FilterMode = 0;
}


CSpriteManager::~CSpriteManager ( void )
{
}


bool CSpriteManager::Add ( tagSpriteData* pData, int iID )
{
    tagSpriteData* ptr = 0;

    SpritePtr p = m_List.find( iID );
    if (p == m_List.end())
    {
        // If doesn't exist, then create one and add to the list
	    ptr = new tagSpriteData;
        m_List.insert( std::make_pair(iID, ptr) );
    }
    else
    {
        ptr = p->second;
    }

    // Update the sprite data
    *ptr = *pData;

    m_CurrentId = iID;
    m_CurrentPtr = ptr;

	return false;
}


bool CSpriteManager::Delete ( int iID )
{
    SpritePtr p = m_List.find( iID );
    if (p == m_List.end())
		return false;

    DeleteJustOne( p->second );
    m_List.erase( p );
	
    if (m_CurrentId == iID)
    {
        m_CurrentId = 0;
        m_CurrentPtr = 0;
    }

	return true;
}


void CSpriteManager::DrawImmediate ( tagSpriteData* pData )
{
    // No culling for a single sprite - it's not cost-effective.

    // Build a render list containing just one sprite
    tagSpriteData* pSpriteList[1] = { pData };

    // Render it
    RenderDrawList ( pSpriteList, 1, m_FilterMode );
}


tagSpriteData* CSpriteManager::GetData ( int iID ) const
{
    if (iID != m_CurrentId)
    {
        SpritePtrConst p = m_List.find( iID );
        if (p == m_List.end())
            return 0;

        m_CurrentId = iID;
        m_CurrentPtr = p->second;
    }
    return m_CurrentPtr;
}


int CSpriteManager::Update ( void ) const
{
    // Clear the old list and resize to hold up to the actual number of sprites.
    m_SortedSpriteList.clear();
    m_SortedSpriteList.reserve(m_List.size());

    // Update our copy of the display size, for culling purposes
    GetDisplaySize ();

    // Build the sorted list of sprites
    // Dump all visible sprites into a vector
    for (SpritePtrConst pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
    {
        if (pCheck->second)
        {
            PrioritiseSprite s(pCheck);
            if (s.m_ptr->iImage > 0 && s.m_ptr->bVisible && IsSpriteInDisplay ( s.m_ptr ))
            {
                m_SortedSpriteList.push_back( s );
            }
        }
    }

    if ( ! m_SortedSpriteList.empty() )
    {
        // Sort the vector
        std::sort( m_SortedSpriteList.begin(), m_SortedSpriteList.end() );

        // Prepare the draw list
        m_SpriteDrawList.clear();
        m_SpriteDrawList.reserve( m_SortedSpriteList.size() );

        // Build the draw list from the sorted list
        t_SpriteListPtr Last = m_SortedSpriteList.end();
        for (t_SpriteListPtr Current = m_SortedSpriteList.begin(); Current != Last; ++Current)
        {
            m_SpriteDrawList.push_back( Current->m_ptr );
        }

        // Render the draw list
        RenderDrawList ( &m_SpriteDrawList[0], m_SpriteDrawList.size(), m_FilterMode );
    }

    return m_SortedSpriteList.size();
}


void CSpriteManager::DeleteAll ( void )
{
    for (SpritePtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
	{
		tagSpriteData* ptr = pCheck->second;

	    // release the sprite
		if ( ptr )
		    DeleteJustOne(ptr);
    }

    m_List.clear();

    m_CurrentId = 0;
    m_CurrentPtr = 0;
}


void CSpriteManager::HideAll ( void )
{
    for (SpritePtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
	{
		tagSpriteData* ptr = pCheck->second;

		// if the data doesn't exist then skip it
		if ( ptr )
        {
		    // run through all sprites and set their visible property
		    ptr->bVisible = false;
        }
    }
}


void CSpriteManager::ShowAll ( void )
{
    for (SpritePtr pCheck = m_List.begin(); pCheck != m_List.end(); ++pCheck)
	{
		tagSpriteData* ptr = pCheck->second;

		// if the data doesn't exist then skip it
		if ( ptr )
        {
		    // run through all sprites and set their visible property
		    ptr->bVisible = true;
        }
    }
}



///////////////////////////////////
// Class static member functions //
///////////////////////////////////



void CSpriteManager::DeleteJustOne(tagSpriteData* ptr)
{
	// remove reference to image (do not delete image!)
	ptr->lpTexture = NULL;

	// free up the allocated memory block
	delete ptr;
}


void CSpriteManager::RenderDrawList ( tagSpriteData** pList, int iListSize, int iFilterMode )
{
    // Set up the rendering pipeline for all sprites

	// set the vertex shader
	m_pD3D->SetVertexShader ( NULL );
	m_pD3D->SetFVF ( D3DFVF_VERTEX2D );

	// mike - 280305 - ensure alpha test enable is off
	m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,		false );

	// set the cull and fill mode
	m_pD3D->SetRenderState  ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pD3D->SetRenderState  ( D3DRS_DITHERENABLE, FALSE );
	m_pD3D->SetRenderState  ( D3DRS_LIGHTING, FALSE);
	m_pD3D->SetRenderState  ( D3DRS_CULLMODE, D3DCULL_NONE );
	m_pD3D->SetRenderState  ( D3DRS_FOGENABLE, FALSE );
	m_pD3D->SetRenderState  ( D3DRS_ZWRITEENABLE, FALSE );
	m_pD3D->SetRenderState  ( D3DRS_ZENABLE, FALSE );

	m_pD3D->SetRenderState  ( D3DRS_COLORVERTEX, TRUE );
	m_pD3D->SetRenderState  ( D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1 );
	m_pD3D->SetRenderState  ( D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL );
	m_pD3D->SetRenderState  ( D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL );
	m_pD3D->SetRenderState  ( D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL );
	m_pD3D->SetRenderState  ( D3DRS_SPECULARENABLE, FALSE );
	
    // Get the state of antialiasing, and disable if set
    DWORD AAEnabled = FALSE;
    m_pD3D->GetRenderState  ( D3DRS_MULTISAMPLEANTIALIAS, &AAEnabled );
    if (AAEnabled)
    	m_pD3D->SetRenderState  ( D3DRS_MULTISAMPLEANTIALIAS, FALSE );

	// turn alpha blending on
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pD3D->SetRenderState ( D3DRS_SRCBLEND,         D3DBLEND_SRCALPHA );
	m_pD3D->SetRenderState ( D3DRS_DESTBLEND,        D3DBLEND_INVSRCALPHA );

	if ( iFilterMode == 0 )
	{
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER,       D3DTEXF_POINT  );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER,       D3DTEXF_POINT  );

		// Enable both vertical and horizontal scrolling of the texture
		m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,        D3DTADDRESS_WRAP );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,        D3DTADDRESS_WRAP );
	}
	else
	{
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER,       D3DTEXF_LINEAR  );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER,       D3DTEXF_LINEAR  );

		// lee - 090910 - need this for sprites that have the wrap artefeact (non scrolling more common)
		m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,        D3DTADDRESS_CLAMP );
		m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,        D3DTADDRESS_CLAMP );
	}

	m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER,       D3DTEXF_POINT );//D3DTEXF_NONE  );

	// setup texture states to allow for diffuse colours
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

    // Detection of state changes between sprites
    int iLastImage = -1;
    LPDIRECT3DTEXTURE9 pLastTexture = NULL;
	bool bLastTransparent=true;

	// display all sprites in the list
    for ( int iTemp = 0; iTemp < iListSize; iTemp++ )
	{
        tagSpriteData* pCurrent = pList[iTemp];

        // No need for a visibility check, as if we got here then we are either:
        // 1 - doing a full render, with visibility already checked, or
        // 2 - pasting a sprite, in which case visibility is being overridden anyway

        // If this sprite has a different image to the previous, update the texture pointer
        if ( pCurrent->iImage != iLastImage)
        {
            iLastImage = pCurrent->iImage;

            // If this sprite is based on an image, go get the latest texture for it
            if ( pCurrent->iImage )
		    {
			    pCurrent->lpTexture = g_Image_GetPointer ( pCurrent->iImage );		// get image data
		    }

            // Set the texture only when it changes
            if (pCurrent->lpTexture != pLastTexture)
            {
		        m_pD3D->SetTexture ( 0, pCurrent->lpTexture );
                pLastTexture = pCurrent->lpTexture;
            }
        }
        else
        {
            // Same image means same texture
            pCurrent->lpTexture = pLastTexture;
        }

		// toggle if sprite is not transparent (alpha-enabled)
        if (bLastTransparent != pCurrent->bTransparent)
        {
            bLastTransparent = pCurrent->bTransparent;
            m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, bLastTransparent ? TRUE : FALSE );
        }

		// U76 - 070710 - special ghost mode render state (simulates GHOST OBJECT ON)
		if ( pCurrent->bGhostMode )
		{
			m_pD3D->SetRenderState( D3DRS_SRCBLEND,			D3DBLEND_SRCCOLOR );
			m_pD3D->SetRenderState( D3DRS_DESTBLEND,		D3DBLEND_ONE );
		}
		else
		{
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,		D3DBLEND_SRCALPHA );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,		D3DBLEND_INVSRCALPHA );
		}

        m_pD3D->DrawPrimitiveUP ( D3DPT_TRIANGLESTRIP, 2, pCurrent->lpVertices, sizeof ( VERTEX2D ) );
	}

	// switch alpha blending off and re-enable z buffering
	m_pD3D->SetRenderState ( D3DRS_ALPHABLENDENABLE, FALSE );
    m_pD3D->SetRenderState ( D3DRS_ZENABLE, TRUE );

    // Re-enable Antialias if it was disabled earlier
	if (AAEnabled)
		m_pD3D->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, TRUE );
}


void CSpriteManager::GetDisplaySize()
{
    // Build display size from the viewport rather than the resolution...
    // 1. It's easier to get
    // 2. 'Someones' TPC DLL uses the viewport to restrict rendering of sprites
    D3DVIEWPORT9 SaveViewport;
    m_pD3D->GetViewport(&SaveViewport);

    vecDisplayMin = D3DXVECTOR3 ( 
        (float)SaveViewport.X, 
        (float)SaveViewport.Y,
        SaveViewport.MinZ );
    vecDisplayMax = D3DXVECTOR3 (
        (float)(SaveViewport.X + SaveViewport.Width),
        (float)(SaveViewport.Y + SaveViewport.Height),
        SaveViewport.MaxZ );
}


bool CSpriteManager::IsSpriteInDisplay(tagSpriteData* m_ptr)
{
    // Determine min/max of the sprite's draw area from its vertices
    D3DXVECTOR3 vecMin = D3DXVECTOR3 ( m_ptr->lpVertices[0].x, m_ptr->lpVertices[0].y, m_ptr->lpVertices[0].z );
    D3DXVECTOR3 vecMax = vecMin;

    for ( int iVertex = 1; iVertex < 4; iVertex++ ) 
    {
	    if ( m_ptr->lpVertices [ iVertex ].x < vecMin.x ) vecMin.x = m_ptr->lpVertices [ iVertex ].x;
	    if ( m_ptr->lpVertices [ iVertex ].y < vecMin.y ) vecMin.y = m_ptr->lpVertices [ iVertex ].y;
	    if ( m_ptr->lpVertices [ iVertex ].z < vecMin.z ) vecMin.z = m_ptr->lpVertices [ iVertex ].z;
	    if ( m_ptr->lpVertices [ iVertex ].x > vecMax.x ) vecMax.x = m_ptr->lpVertices [ iVertex ].x;
	    if ( m_ptr->lpVertices [ iVertex ].y > vecMax.y ) vecMax.y = m_ptr->lpVertices [ iVertex ].y;
	    if ( m_ptr->lpVertices [ iVertex ].z > vecMax.z ) vecMax.z = m_ptr->lpVertices [ iVertex ].z;
    }

    // Check box intersection
    return (vecDisplayMax.x >= vecMin.x && vecDisplayMin.x <= vecMax.x &&
		    vecDisplayMax.y >= vecMin.y && vecDisplayMin.y <= vecMax.y );
}


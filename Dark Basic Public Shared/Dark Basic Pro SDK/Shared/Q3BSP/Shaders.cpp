
#include "cBSPFile.h"
#include "loader.h"
#include "textures.h"

#include ".\..\core\globstruct.h"

struct _shaders shaders;

extern LPDIRECT3DDEVICE9		m_pD3D;
extern files_found				Q3A_Resources;
extern DWORD					g_dwCameraCullMode;

float OneSecTimer ( );
float Timer ( );

extern GlobStruct*						g_pGlob;

float Timer ( )
{
	return ( float ) timeGetTime ( ) / 1000;
}

float OneSecTimer ( )
{
	return ( float ) ( timeGetTime()%100000 ) / 1000;
}

BOOL strcpy2 ( LPSTR* dest, LPSTR src )
{

	if ( src == NULL )
		return FALSE;

	// ********** MJ - 4th September 2002 - CRASH FIX ******************
	// we can get an error here when trying to delete invalid memory
	// this try catch block simply checks if the length of
	// the string is greater than 0, on invalid pointers it will catch the error
	// and we don't attempt to delete the memory - no more crashes so far,
	// about the empty controlled statement, it's not important, all we need
	// to do is call the strlen function to check all is ok
	// *****************************************************************

	if(*dest!=NULL)
	{
		bool bSafeToDelete = true;
		try
		{
			if ( strlen ( *dest ) > 0 )
			{
				// no warning :)
			}
		}
		catch ( ... )
		{
			bSafeToDelete = false;
		}

		if ( bSafeToDelete )
		{
			if (dest) SAFE_DELETE(*dest);
		}
	}

	*dest = new char [ strlen ( src ) + 1 ];
	strcpy ( *dest, src );

	return TRUE;
}

BOOL InitShaders ( )
{

	shaders.Init ( );

	for ( int x = 0; x != Q3A_Resources.num_files; x++ )
	{
		LPSTR pk3_file = Q3A_Resources.files [ x ].fullname;
		
		files_found shader_files;

		file_loader::Find::Files_in_PK3 ( pk3_file, ".shader", &shader_files );

		byte* data;
		int length;

		for ( int y = 0; y != shader_files.num_files; y++ )
		{
			if ( file_loader::Load::From_PK3 ( pk3_file, shader_files.files [ y ].filename, &data, &length ) )
			{
				AddShaderFile ( data, length, pk3_file, shader_files.files [ y ].filename );
				SAFE_DELETE ( data );
			}
		}

		shader_files.Release ( );
	}

	return TRUE;
}

struct _dat
{
	char* text;
	int   pos;
	int   length;
	int   token;

	int GetToken ( )
	{
		while ( text [ pos ] != '{' && text [ pos ] != '}' && ( text [ pos ] == ' ' || text [ pos ] == '\t' || text [ pos ] == '\n' || text [ pos ] == 13 ) )
			pos++;

		if ( text [ pos ] == '{' )
		{
			pos++;
			token++;
			return 1;
		}

		if ( text [ pos ] == '}' )
		{
			pos++;
			token--;
			return -1;
		}

		return 0;
	}

	void GetTokens ( )
	{
		while ( GetToken ( ) != 0 )
			Comment ( );
	}

	BOOL IgnoreSpaces ( )
	{
		while ( text [ pos ] ==' ' || text [ pos ] =='\t' || text [ pos ] =='\n' || text [ pos ] == 13 )
		{
			pos++;
			
			if ( pos >= length )
				return TRUE;
		}
		
		return FALSE;
	}
	
	void GetText ( LPSTR* output )
	{
		if(output) SAFE_DELETE(*output);
		*output = new char [ 256 ];

		int x = 0;
		
		while ( text [ pos ] != ' ' && text [ pos ] != '\t' && text [ pos ] != '\n' && text [ pos ] != 13 && text [ pos ] != '{' )
		{
			( *output ) [ x ] = text [ pos ];
			x++;
			pos++;
		}

		( *output ) [ x ] = '\0';
	}

	float GetNumber ( )
	{
		int start = pos;

		int x = 0;

		for ( x = 0; ; x++, pos++ )
		{
			if ( text [ pos ] == '0' ) continue;
			if ( text [ pos ] == '1' ) continue;
			if ( text [ pos ] == '2' ) continue;
			if ( text [ pos ] == '3' ) continue;
			if ( text [ pos ] == '4' ) continue;
			if ( text [ pos ] == '5' ) continue;
			if ( text [ pos ] == '6' ) continue;
			if ( text [ pos ] == '7' ) continue;
			if ( text [ pos ] == '8' ) continue;
			if ( text [ pos ] == '9' ) continue;
			if ( text [ pos ] == '-' ) continue;
			if ( text [ pos ] == '.' ) continue;
			break;
		}

		char temp [ 256 ];

		strncpy ( temp, text + start, x );
		temp [ x ] = '\0';
		return ( float ) atof ( temp );
	}

	BOOL NextLine ( )
	{
		while ( text [ pos ] != 13 && text [ pos ] != '\n' )
		{
			pos++;
			
			if ( pos >= length )
				return TRUE;
		}

		pos++;

		if ( pos >= length )
			return TRUE;
		
		return FALSE;
	}

	BOOL Comment ( )
	{
		if ( IgnoreSpaces ( ) )
			return TRUE;

		if ( strncmp ( text + pos, "//", 2 ) == 0 )
		{
			if ( NextLine ( ) )
				return TRUE;

			return Comment ( );
		}
	
		return FALSE;
	}

	BOOL NextShader ( )
	{
		while ( token > 0 )
		{
			if ( Comment ( ) )
				return TRUE;

			if ( text [ pos ] == '{' )
				token++;

			if ( text [ pos ] == '}' ) 
				token--;
			
			pos++;

			if ( pos >= length )
				return TRUE;
		}

		if ( Comment ( ) )
			return TRUE;

		return FALSE;
	}
} dat;

void AddShaders ( LPSTR pk3name )
{
	files_found shader_files;
	file_loader::Find::Files_in_PK3 ( pk3name, ".shader", &shader_files );

	byte* data = NULL;
	int length;

	for ( int y = 0; y < shader_files.num_files; y++ )
	{
		if ( file_loader::Load::From_PK3 ( pk3name, shader_files.files [ y ].filename, &data, &length ) )
		{
			AddShaderFile ( data, length, pk3name, shader_files.files [ y ].fullname );
		}

		free ( data );
		data = NULL;
	}

	shader_files.Release ( );

}

BOOL AddShaderFile ( byte* data, int length, LPSTR pk3name, LPSTR filename )
{
	dat.text   = ( char* ) data;
	dat.pos    = 0;
	dat.length = length;
	dat.token  = 0;

back:
	if ( dat.pos >= dat.length )
		goto nextfile;

	dat.Comment ( );

	if ( dat.pos >= dat.length )
		goto nextfile;

	// it's a new shader
	if ( dat.token == 0 )
	{
back2:
		LPSTR name = NULL;
		dat.GetText ( &name );

		for ( int y = 0; y < shaders.entry_count; y++ )
		{
			if ( stricmp ( name, shaders.entry [ y ].name ) == 0 )
			{
				SAFE_DELETE_ARRAY ( name );
				dat.GetTokens ( );
				
				if ( dat.NextShader ( ) )
					goto nextfile;

				goto back2;
			}
		}

		int n = shaders.entry_count;

		shaders.Resize ( n + 1 );

		if ( pk3name != NULL )
		{
			shaders.entry [ n ].pk3name = NULL;
			strcpy2 ( &shaders.entry [ n ].pk3name, pk3name );
		}
		else
			shaders.entry [ n ].pk3name = NULL;

		// fix
		shaders.entry [ n ].stage = NULL;

		if ( filename != NULL )
		{
			shaders.entry [ n ].filename = NULL;
			strcpy2 ( &shaders.entry [ n ].filename, filename );
		}
		else 
			shaders.entry [ n ].filename = NULL;

		shaders.entry [ n ].name              = name;
		shaders.entry [ n ].stage_count       = 0;
		shaders.entry [ n ].params.sky        = FALSE;
		shaders.entry [ n ].params.nonsolid   = FALSE;
		shaders.entry [ n ].params.playerclip = FALSE;
		shaders.entry [ n ].params.cull       = D3DCULL_CCW;
		shaders.entry_count++;
	}
	else if ( dat.token == 1 )
	{
		LPSTR command = NULL;
		dat.GetText ( &command );

		if ( stricmp ( command, "surfaceparm" ) == 0 )
		{
			dat.Comment ( );
			LPSTR parm = NULL;
			dat.GetText ( &parm );
			
			int n = shaders.entry_count - 1;
	
			if ( stricmp ( parm, "sky" ) == 0 )
			{
				shaders.entry [ n ].params.sky = TRUE;

				/*
				{
					// mike - 240604 - WORKING ON GETTING SKIES IN

					shaders.entry [ n ].stage = ( shader::tex_stage* ) realloc ( shaders.entry [ n ].stage, sizeof ( shader::tex_stage ) * ( 1 ) );
					shaders.entry [ n ].stage [ 0 ].ani_freq         = 0;
					shaders.entry [ n ].stage [ 0 ].tex_count        = 1;
					shaders.entry [ n ].stage [ 0 ].blend.src        = D3DBLEND_ONE;
					shaders.entry [ n ].stage [ 0 ].blend.dest       = D3DBLEND_ZERO;
					shaders.entry [ n ].stage [ 0 ].zfunc            = D3DCMP_LESSEQUAL;
					shaders.entry [ n ].stage [ 0 ].zwrite           = TRUE;
					shaders.entry [ n ].stage [ 0 ].clamp            = TRUE;
					shaders.entry [ n ].stage [ 0 ].tcgen            = TCGEN_BASE;
					//shaders.entry [ n ].stage [ 0 ].tcgen            = -1;
					shaders.entry [ n ].stage [ 0 ].tcmod_count      = 0;
					shaders.entry [ n ].stage [ 0 ].tcmod            = NULL;
					shaders.entry [ n ].stage [ 0 ].alphafunc.enable = FALSE;
					shaders.entry [ n ].stage [ 0 ].alphafunc.ref    = 0;
					shaders.entry [ n ].stage [ 0 ].alphafunc.func   = D3DCMP_ALWAYS;
					shaders.entry [ n ].stage_count++;

					shaders.entry [ n ].stage [ 0 ].tex_name [ 0 ] = "env/ikzdm1_up";
					shaders.entry [ n ].stage [ 0 ].tex_count = 1;

					shaders.entry [ n ].stage [ 0 ].tcmod_count++;
					shaders.entry [ n ].stage [ 0 ].tcmod = 
						 ( shader::tex_stage::_tcmod* ) realloc ( shaders.entry [ n ].stage [ 0 ].tcmod, sizeof ( shader::tex_stage::_tcmod ) * shaders.entry [ n ].stage [ 0 ].tcmod_count );

					small m = shaders.entry [ n ].stage [ 0 ].tcmod_count - 1;

					shaders.entry [ n ].stage [ 0 ].tcmod [ 0 ].rotate   = 0;
					shaders.entry [ n ].stage [ 0 ].tcmod [ 0 ].scale_u  = 0.05f;
					shaders.entry [ n ].stage [ 0 ].tcmod [ 0 ].scale_v  = 0.05f;
					shaders.entry [ n ].stage [ 0 ].tcmod [ 0 ].scroll_u = 0;
					shaders.entry [ n ].stage [ 0 ].tcmod [ 0 ].scroll_v = 0;
				}
				*/
			}
			else if ( stricmp ( parm, "nonsolid" ) == 0 )
			{
				shaders.entry [ n ].params.nonsolid = TRUE;
			}
			else if ( stricmp ( parm, "playerclip" ) == 0 )
			{
				shaders.entry [ n ].params.playerclip = TRUE;
			}

			SAFE_DELETE_ARRAY ( parm );
		}
		else if ( stricmp ( command, "cull" ) == 0 )
		{
			dat.Comment ( );

			LPSTR parm = NULL;
			dat.GetText ( &parm );
				
			int n = shaders.entry_count - 1;

			if ( stricmp ( parm, "front"   ) == 0 ) shaders.entry [ n ].params.cull = D3DCULL_CW;
			if ( stricmp ( parm, "back"    ) == 0 ) shaders.entry [ n ].params.cull = D3DCULL_CCW;
			if ( stricmp ( parm, "disable" ) == 0 ) shaders.entry [ n ].params.cull = D3DCULL_NONE;
			if ( stricmp ( parm, "none"    ) == 0 ) shaders.entry [ n ].params.cull = D3DCULL_NONE;

			SAFE_DELETE_ARRAY ( parm );
		}

		SAFE_DELETE_ARRAY ( command );
	}
	else if ( dat.token == 2 )
	{
		int n = shaders.entry_count - 1;
		int s = shaders.entry [ n ].stage_count;

		int sc = shaders.entry [ n ].stage_count;

		if ( sc == 0 )
			shaders.entry [ n ].stage = NULL;

		shaders.entry [ n ].stage = ( shader::tex_stage* ) realloc ( shaders.entry [ n ].stage, sizeof ( shader::tex_stage ) * ( sc + 1 ) );
		shaders.entry [ n ].stage [ s ].ani_freq         = 0;
		shaders.entry [ n ].stage [ s ].tex_count        = 0;
		shaders.entry [ n ].stage [ s ].blend.src        = D3DBLEND_ONE;
		shaders.entry [ n ].stage [ s ].blend.dest       = D3DBLEND_ZERO;
		shaders.entry [ n ].stage [ s ].zfunc            = D3DCMP_LESSEQUAL;
		shaders.entry [ n ].stage [ s ].zwrite           = TRUE;
		shaders.entry [ n ].stage [ s ].clamp            = TRUE;
		shaders.entry [ n ].stage [ s ].tcgen            = TCGEN_BASE;
		shaders.entry [ n ].stage [ s ].tcmod_count      = 0;
		shaders.entry [ n ].stage [ s ].tcmod            = NULL;
		shaders.entry [ n ].stage [ s ].alphafunc.enable = FALSE;
		shaders.entry [ n ].stage [ s ].alphafunc.ref    = 0;
		shaders.entry [ n ].stage [ s ].alphafunc.func   = D3DCMP_ALWAYS;
		shaders.entry [ n ].stage_count++;

		for ( ;; )
		{
			dat.Comment ( );
			
			if ( dat.token != 2 )
				goto back;

			LPSTR command = NULL;
			dat.GetText ( &command );

			if ( stricmp ( command, "map" ) == 0 )
			{
				dat.Comment ( );
				
				if ( dat.token != 2 )
					goto back;

				LPSTR file = NULL;
				dat.GetText ( &file );

				shaders.entry [ n ].stage [ s ].tex_name [ 0 ] = file;
				shaders.entry [ n ].stage [ s ].tex_count = 1;
				
				if ( stricmp ( file, "$lightmap" ) == 0 )
					shaders.entry [ n ].stage [ s ].tcgen = TCGEN_LIGHTMAP;
			}

			if ( stricmp ( command, "clampmap" ) == 0 )
			{
				dat.Comment ( );
				
				if ( dat.token != 2 )
					goto back;

				LPSTR file = NULL;
				dat.GetText ( &file );

				shaders.entry [ n ].stage [ s ].tex_name [ 0 ] = file;
				shaders.entry [ n ].stage [ s ].tex_count      = 1;
				shaders.entry [ n ].stage [ s ].clamp          = TRUE;
			}
			else if ( stricmp ( command, "blendfunc" ) == 0 )
			{
				LPSTR src, dest;

				src = NULL;
				dest = NULL;

				dat.Comment ( );
				
				if ( dat.token != 2 )
					goto back;

				dat.GetText ( &src );
				
				if ( stricmp ( src, "add" ) == 0 )
				{
					shaders.entry [ n ].stage [ s ].blend.src  = D3DBLEND_ONE;
					shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_ONE;
					goto jump;
				}
				else if ( stricmp ( src, "filter" ) == 0 )
				{
					shaders.entry [ n ].stage [ s ].blend.src  = D3DBLEND_DESTCOLOR;
					shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_ZERO;
					goto jump;
				}
				else if ( stricmp ( src, "blend" ) == 0 )
				{
					shaders.entry [ n ].stage [ s ].blend.src  = D3DBLEND_SRCALPHA;
					shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_INVSRCALPHA;
					goto jump;
				}
				
				if      ( stricmp ( src, "GL_ONE"                 ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_ONE;
				else if ( stricmp ( src, "GL_ZERO"                ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_ZERO;
				else if ( stricmp ( src, "GL_DST_COLOR"           ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_DESTCOLOR;
				else if ( stricmp ( src, "GL_ONE_MINUS_DST_COLOR" ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_INVDESTCOLOR;
				else if ( stricmp ( src, "GL_SRC_ALPHA"           ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_SRCALPHA;
				else if ( stricmp ( src, "GL_ONE_MINUS_SRC_ALPHA" ) == 0 ) shaders.entry [ n ].stage [ s ].blend.src = D3DBLEND_INVSRCALPHA;
				else
				{
					goto jump;
				}

				dat.Comment ( );
				
				if ( dat.token != 2 )
					goto back;

				dat.GetText ( &dest );
	
				if      ( stricmp ( dest, "GL_ONE"                 ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_ONE;
				else if ( stricmp ( dest, "GL_ZERO"                ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_ZERO;
				else if ( stricmp ( dest, "GL_SRC_COLOR"           ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_SRCCOLOR;
				else if ( stricmp ( dest, "GL_ONE_MINUS_SRC_COLOR" ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_INVSRCCOLOR;
				else if ( stricmp ( dest, "GL_SRC_ALPHA"           ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_SRCALPHA;
				else if ( stricmp ( dest, "GL_ONE_MINUS_SRC_ALPHA" ) == 0 ) shaders.entry [ n ].stage [ s ].blend.dest = D3DBLEND_INVSRCALPHA;
			
				SAFE_DELETE ( dest );
	
		jump:
			
			if ( shaders.entry [ n ].stage [ s ].blend.src != D3DBLEND_ONE || shaders.entry [ n ].stage [ s ].blend.dest != D3DBLEND_ZERO )
				shaders.entry [ n ].stage [ s ].zwrite = FALSE;

			SAFE_DELETE ( src );
		}
		else if ( stricmp ( command, "animmap" ) == 0 )
		{
			dat.Comment ( );
			
			if ( dat.token != 2 )
				goto back;

			shaders.entry [ n ].stage [ s ].ani_freq = dat.GetNumber ( );

			int y = 0;

			for ( y = 0; ; y++ )
			{
				while ( dat.text [ dat.pos ] == ' ' || dat.text [ dat.pos ] == '\t' ) 
					dat.pos++;

				if ( dat.text [ dat.pos ] == '\n' || dat.text [ dat.pos ] == 13 )
					break;

				shaders.entry [ n ].stage [ s ].tex_name [ y ] = NULL;
				dat.GetText ( &shaders.entry [ n ].stage [ s ].tex_name [ y ] );
			}

			shaders.entry [ n ].stage [ s ].tex_count = y;
		}
		else if ( stricmp ( command, "depthfunc" ) == 0 )
		{
			LPSTR func = NULL;
			dat.Comment ( );
			
			if ( dat.token != 2 )
				goto back;

			dat.GetText ( &func );

			if ( stricmp ( func, "lequal" ) == 0 )
			{
				shaders.entry [ n ].stage [ s ].zfunc = D3DCMP_LESSEQUAL;
			}
			else if ( stricmp ( func, "equal" ) == 0 )
			{
				shaders.entry [ n ].stage [ s ].zfunc = D3DCMP_EQUAL;
			}
			
			SAFE_DELETE ( func );
		}
		else if ( stricmp ( command, "depthwrite" ) == 0 )
		{
			shaders.entry [ n ].stage [ s ].zwrite = TRUE;
		}
		else if ( stricmp ( command, "tcgen" ) == 0 )
		{
			LPSTR func = NULL;
			dat.Comment(); 
			
			if ( dat.token !=2 )
				goto back;

			dat.GetText ( &func );

			if      ( stricmp ( func, "base"        ) == 0 ) shaders.entry [ n ].stage [ s ].tcgen = TCGEN_BASE;
			else if ( stricmp ( func, "lightmap"    ) == 0 ) shaders.entry [ n ].stage [ s ].tcgen = TCGEN_LIGHTMAP;
			else if ( stricmp ( func, "environment" ) == 0 ) shaders.entry [ n ].stage [ s ].tcgen = TCGEN_ENVIRONMENT;
		}
		else if ( stricmp ( command, "tcmod" ) == 0 )
		{
			LPSTR func = NULL;
			dat.Comment ( );
			
			if ( dat.token != 2 )
				goto back;

			dat.GetText ( &func );

			shaders.entry [ n ].stage [ s ].tcmod_count++;
			shaders.entry [ n ].stage [ s ].tcmod = 
				 ( shader::tex_stage::_tcmod* ) realloc ( shaders.entry [ n ].stage [ s ].tcmod, sizeof ( shader::tex_stage::_tcmod ) * shaders.entry [ n ].stage [ s ].tcmod_count );

			small m = shaders.entry [ n ].stage [ s ].tcmod_count - 1;

			shaders.entry [ n ].stage [ s ].tcmod [ m ].rotate   = 0;
			shaders.entry [ n ].stage [ s ].tcmod [ m ].scale_u  = 0;
			shaders.entry [ n ].stage [ s ].tcmod [ m ].scale_v  = 0;
			shaders.entry [ n ].stage [ s ].tcmod [ m ].scroll_u = 0;
			shaders.entry [ n ].stage [ s ].tcmod [ m ].scroll_v = 0;

			if ( stricmp ( func, "rotate" ) == 0 )
			{
				dat.Comment ( );
				shaders.entry [ n ].stage [ s ].tcmod [ m ].rotate = dat.GetNumber ( );
			}
			else if ( stricmp ( func, "scale" ) == 0 )
			{
				dat.Comment ( );
				float su = dat.GetNumber ( );
				dat.Comment ( );
				float sv = dat.GetNumber ( );

				shaders.entry [ n ].stage [ s ].tcmod [ m ].scale_u = su;
				shaders.entry [ n ].stage [ s ].tcmod [ m ].scale_v = sv;
			}
			else if ( stricmp ( func, "scroll" ) == 0 )
			{
				dat.Comment ( );
				float su = dat.GetNumber ( );
				dat.Comment ( ); 
				float sv = dat.GetNumber ( );
				
				while ( su >  100 ) su -= 100;
				while ( su < -100 ) su += 100;
				while ( sv >  100 ) sv -= 100;
				while ( sv < -100 ) sv += 100;

				shaders.entry [ n ].stage [ s ].tcmod [ m ].scroll_u = su;
				shaders.entry [ n ].stage [ s ].tcmod [ m ].scroll_v = sv;
			}
		}
		else if ( stricmp ( command, "alphafunc" ) == 0 )
		{
			LPSTR func = NULL;
			dat.Comment ( );
			
			if ( dat.token != 2 )
				goto back;

			dat.GetText ( &func );
			
			if ( stricmp ( func, "GT0" ) == 0 )
			{
				shaders.entry [ n ].stage [ s ].alphafunc.enable = TRUE;
				shaders.entry [ n ].stage [ s ].alphafunc.ref    = 0;
				shaders.entry [ n ].stage [ s ].alphafunc.func    = D3DCMP_GREATER;
			}
			else if ( stricmp ( func, "LT128" ) == 0 )
			{
				shaders.entry [ n ].stage [ s ].alphafunc.enable = TRUE;
				shaders.entry [ n ].stage [ s ].alphafunc.ref    = 128;
				shaders.entry [ n ].stage [ s ].alphafunc.func   = D3DCMP_LESS;
			}
			else if ( stricmp ( func, "GE128" ) == 0 )
			{
				shaders.entry [ n ].stage [ s ].alphafunc.enable = TRUE;
				shaders.entry [ n ].stage [ s ].alphafunc.ref    = 128;
				shaders.entry [ n ].stage [ s ].alphafunc.func   = D3DCMP_GREATEREQUAL;
			}

			SAFE_DELETE ( func );
		}

			SAFE_DELETE_ARRAY ( command );
			dat.NextLine ( );
			dat.Comment ( );

			if ( dat.text [ dat.pos ] == '{' || dat.text [ dat.pos ] == '}' ) 
			{
				dat.GetTokens ( );
				goto back;
			}
		}

	}

	// unknown command
	if ( dat.NextLine ( ) )
		goto nextfile;

	if ( dat.Comment ( ) )
		goto nextfile;

	if ( dat.text [ dat.pos ] == '{' || dat.text [ dat.pos ] == '}') 
		dat.GetTokens ( );

	goto back;
nextfile:;


	return TRUE;
}


BOOL tc_changed;
float tu [ 2048 ],
		tv [ 2048 ];

void save_tc ( int face_idx )
{
	const _Q3MAP::face* f = &Q3MAP.faces [ face_idx ];

	switch ( f->type )
	{
		case FACETYPE_PATCH:
		{
			const mesh_t3* mesh = &Q3MAP.meshes [ f->mesh ];
			
			for ( int i = 0; i < mesh->verts_count; i++ )
			{
				tu [ i ] = mesh->verts [ i ].tu;
				tv [ i ] = mesh->verts [ i ].tv;
			}
	
			break;
		}

		case FACETYPE_NORMAL:
		case FACETYPE_MESH:
		break;
	}
}

void set_tc ( int face_idx, const shader* s, int st )
{
	tc_changed = FALSE;

	const _Q3MAP::face* f = &Q3MAP.faces [ face_idx ];
	
	switch ( f->type )
	{
		case FACETYPE_PATCH:
		{
			if ( f->mesh < 0 )
				break;
		
			const mesh_t3* mesh = &Q3MAP.meshes [ f->mesh ];

			// compute new texture coordinates for environment mapped textures
			if ( s->stage [ st ].tcgen == TCGEN_ENVIRONMENT )
			{
				if ( !tc_changed )
					save_tc ( face_idx );

				for ( int i = 0;i < mesh->verts_count; i++ )
				{
					D3DXVECTOR3 test = D3DXVECTOR3 ( g_Camera_GetXPosition ( 1 ), g_Camera_GetYPosition ( 1 ), g_Camera_GetZPosition ( 1 ) );
					D3DXVECTOR3 dir;
					D3DXVec3Normalize ( &dir, &( test-mesh->verts [ i ].v ) );

					mesh->verts [ i ].tu = dir.x+mesh->verts [ i ].v.x;
					mesh->verts [ i ].tv = dir.x+mesh->verts [ i ].v.y;
				}
			
				tc_changed = TRUE;
			}

			// texture modifications
			for ( int m = 0; m < s->stage [ st ].tcmod_count; m++ )
			{
				// scale
				float scale_u = s->stage [ st ].tcmod [ m ].scale_u;
				float scale_v = s->stage [ st ].tcmod [ m ].scale_v;

				if ( scale_u != 0 || scale_v != 0 )
				{
					if ( !tc_changed )
						save_tc ( face_idx );

					for ( int i = 0; i < mesh->verts_count; i++ )
					{
						mesh->verts [ i ].tu *= scale_u;
						mesh->verts [ i ].tv *= scale_v;
					}

					tc_changed = TRUE;
				}

				// scroll
				float scroll_u = s->stage [ st ].tcmod [ m ].scroll_u * OneSecTimer ( );
				float scroll_v = s->stage [ st ].tcmod [ m ].scroll_v * OneSecTimer ( );

				if ( scroll_u != 0 || scroll_v != 0 )
				{
					if ( !tc_changed )
						save_tc ( face_idx );

					for ( int i = 0; i < mesh->verts_count; i++ )
					{
						mesh->verts [ i ].tu += scroll_u;
						mesh->verts [ i ].tv += scroll_v;
					}
				
					tc_changed = TRUE;
				}
			}
			
			break;
		}
	
		case FACETYPE_NORMAL:
		case FACETYPE_MESH:
			// compute new texture coordinates for environment mapped textures
			if ( s->stage [ st ].tcgen == TCGEN_ENVIRONMENT )
			{
				if ( !tc_changed )
					save_tc ( face_idx );

				for ( int i = 0; i < f->vert_count; i++ )
				{
					D3DXVECTOR3 test = D3DXVECTOR3 ( g_Camera_GetXPosition ( 1 ), g_Camera_GetYPosition ( 1 ), g_Camera_GetZPosition ( 1 ) );
					D3DXVECTOR3 dir;
					D3DXVec3Normalize( &dir,&( test-Q3MAP.verts [ f->vert_start + i ].v ) );

					Q3MAP.verts [ f->vert_start + i ].tu = dir.x + Q3MAP.verts [ f->vert_start + i ].n.x;
					Q3MAP.verts [ f->vert_start + i ].tv = dir.y + Q3MAP.verts [ f->vert_start + i ].n.y;
				}

				tc_changed = TRUE;
			}
	
			// texture modifications
			for ( int m = 0; m < s->stage [ st ].tcmod_count; m++ )
			{
				// rotate
				float rotate = s->stage [ st ].tcmod [ m ].rotate * Timer ( );

				rotate = rotate / 360 * ( 2 * D3DX_PI );

				if ( rotate != 0 )
				{
					for ( int i = 0; i < f->vert_count; i++ )
					{
						D3DXVECTOR4 vecTest;
						D3DXVECTOR2 vecCoordinates = D3DXVECTOR2 ( Q3MAP.verts [ f->vert_start + i ].tu, Q3MAP.verts [ f->vert_start + i ].tv );

						D3DXMATRIX	matTranslation;
						D3DXMATRIX	matRotation, matRotateX, matRotateY, matRotateZ;
						D3DXMATRIX	matScale;
						D3DXMATRIX	matFinal;

						D3DXMatrixScaling ( &matScale, 1.0f, 1.0f, 1.0f );
				
						D3DXMatrixTranslation ( &matTranslation, 0.5f, 0.5f, 0.0f );

						D3DXMatrixRotationX ( &matRotateX, D3DXToRadian ( s->stage [ st ].tcmod [ m ].rotate * Timer ( ) ) );
						D3DXMatrixRotationY ( &matRotateY, 0.0f );
						D3DXMatrixRotationZ ( &matRotateZ, 1.0f );

						matRotation = matRotateX * matRotateY * matRotateZ;
						matFinal = matRotation * matScale * matTranslation;

						D3DXVec2Transform ( &vecTest, &vecCoordinates, &matFinal ); 

						Q3MAP.verts [ f->vert_start + i ].tu += vecTest.x * Q3MAP.verts [ f->vert_start + i ].tu;
						Q3MAP.verts [ f->vert_start + i ].tv += vecTest.y * Q3MAP.verts [ f->vert_start + i ].tv;

						//Q3MAP.verts [ f->vert_start + i].tu += Q3MAP.verts [ f->vert_start + i ].tu * sin ( rotate );
						//Q3MAP.verts [ f->vert_start + i].tv += Q3MAP.verts [ f->vert_start + i ].tv * sin ( rotate );
					}
				
					tc_changed = TRUE;
				}
				
				// scale
				float scale_u = s->stage [ st ].tcmod [ m ].scale_u;
				float scale_v = s->stage [ st ].tcmod [ m ].scale_v;

				if ( scale_u != 0 || scale_v != 0 )
				{
					if ( !tc_changed )
						save_tc ( face_idx );

					for ( int i = 0; i < f->vert_count; i++ )
					{
						Q3MAP.verts [ f->vert_start + i ].tu *= scale_u;
						Q3MAP.verts [ f->vert_start + i ].tv *= scale_v;
					}

					tc_changed = TRUE;
				}

				// scroll
				float scroll_u = s->stage [ st ].tcmod [ m ].scroll_u * OneSecTimer ( );
				float scroll_v = s->stage [ st ].tcmod [ m ].scroll_v * OneSecTimer ( );

				while ( scroll_u > 1 )
					scroll_u--;

				while ( scroll_v > 1 )
					scroll_v--;

				if ( scroll_u != 0 || scroll_v != 0 )
				{
					if ( !tc_changed ) 
						save_tc ( face_idx );

					for ( int i = 0; i < f->vert_count; i++ )
					{
						Q3MAP.verts [ f->vert_start + i ].tu += scroll_u;
						Q3MAP.verts [ f->vert_start + i ].tv += scroll_v;
					}
				
					tc_changed = TRUE;
				}
			}
			break;
	}
}

void restore_tc ( int face_idx )
{

	if ( tc_changed )
	{
		const _Q3MAP::face* f = &Q3MAP.faces [ face_idx ];

		switch ( f->type )
		{
			case FACETYPE_PATCH:
			{
				if ( f->mesh < 0 )
					break;

				const mesh_t3* mesh = &Q3MAP.meshes [ f->mesh ];
				
				for ( int i = 0; i < mesh->verts_count; i++ )
				{
					mesh->verts [ i ].tu = tu [ i ];
					mesh->verts [ i ].tv = tv [ i ];
				}
			
				break;
			}

			case FACETYPE_NORMAL:
			case FACETYPE_MESH:
				for ( int i = 0; i < f->vert_count; i++ )
				{
					Q3MAP.verts [ f->vert_start + i ].tu = vertices3 [ f->vert_start + i ].texture [ 0 ];
					Q3MAP.verts [ f->vert_start + i ].tv = vertices3 [ f->vert_start + i ].texture [ 1 ];
				}
				break;
		}
	}
}

#define NORMAL_TEXTURE 0
#define SHADER_TEXTURE 1

BOOL Q3ARenderTextureFaces ( int stex )
{
	int tex = Q3MAP.sort.tex [ stex ].tex_ref;

	int type;

	if ( tex < -1 )
		type = SHADER_TEXTURE;
	else
		type = NORMAL_TEXTURE;

	switch ( type )
	{
		case NORMAL_TEXTURE:
		{
			if ( tex >= 0 )
				m_pD3D->SetTexture ( 0, Textures.Tex [ tex ].lpD3DTex );
			else
				m_pD3D->SetTexture ( 0, NULL );

			// set blending states
			m_pD3D->SetRenderState ( D3DRS_SRCBLEND,		D3DBLEND_ONE );
			m_pD3D->SetRenderState ( D3DRS_DESTBLEND,		D3DBLEND_ZERO );

			// set z buffer settings
			m_pD3D->SetRenderState ( D3DRS_ZFUNC,			D3DCMP_LESSEQUAL );
			m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE,	TRUE );

			// set cull mode
			m_pD3D->SetRenderState ( D3DRS_CULLMODE,		g_dwCameraCullMode );

			// 250604
			m_pD3D->SetRenderState ( D3DRS_CULLMODE,		D3DCULL_NONE );

			// alphatest
			m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE,	FALSE );

			// render the faces
			for ( int x = 0; x < Q3MAP.sort.tex [ stex ].face_count; x++ )
			{
				int face_idx = Q3MAP.sort.tex [ stex ].face [ x ];

				if ( !Q3MAP.mark_faces [ face_idx ] )
					continue;

				const _Q3MAP::face* f = &Q3MAP.faces [ face_idx ];

				// MIKE 250303 - ignore unloaded textures and no lightmaps
				if ( f->tex_ref == -1 && f->lmap <= 0 )
					continue;

				// mike - 120405 - comment out becuase it causes patches to be skipped
				//if ( f->vert_count < 3 )
				//	continue;

				// mike - 230604 - don't show unloaded textures
				if ( f->tex_ref < 0 )
					continue;

				if ( f->lmap >= 0 )
					m_pD3D->SetTexture ( 1, Textures.LightMap [ f->lmap ].lpD3DTex );
				else
					m_pD3D->SetTexture ( 1, NULL );

				switch ( f->type )
				{
					case FACETYPE_PATCH:
					{
						if ( f->mesh < 0 )
							break;

						const mesh_t3* mesh = &Q3MAP.meshes [ f->mesh ];
						
						m_pD3D->DrawIndexedPrimitiveUP (
															D3DPT_TRIANGLELIST,
															0,
															mesh->verts_count,
															mesh->elems_count / 3,
															mesh->elems,
															D3DFMT_INDEX16,
															mesh->verts,
															sizeof ( CUSTOMVERTEX ) 
													   );

						g_pGlob->dwNumberOfPolygonsDrawn += mesh->elems_count / 3;

						break;
					}
					
					case FACETYPE_NORMAL:
					{
						m_pD3D->DrawPrimitiveUP ( 
													D3DPT_TRIANGLEFAN,
													f->vert_count - 2,
													( void** ) &Q3MAP.verts [ f->vert_start ],
													sizeof ( CUSTOMVERTEX ) 
												);

						g_pGlob->dwNumberOfPolygonsDrawn += f->vert_count - 2;
						
						break;
					}

					case FACETYPE_MESH:
					{
						// mj change - don't draw meshes
						//continue;

							// stage 1
							m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
							m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE   );
							m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE     );
							m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE     );
							m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

							// set texture mode
							m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
							m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

							// filter
							m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
							m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
							m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

							// stage 2
							m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
							m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

							// stage 3
							m_pD3D->SetTextureStageState ( 2, D3DTSS_COLOROP,   D3DTOP_DISABLE );
							m_pD3D->SetTextureStageState ( 2, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

						m_pD3D->DrawIndexedPrimitiveUP ( 
															D3DPT_TRIANGLELIST,
															0,
															f->vert_count,
															f->elem_count / 3,
															( void** ) &Q3MAP.elems [ f->elem_start ],
															D3DFMT_INDEX16,
															( void** ) &Q3MAP.verts [ f->vert_start ],
															sizeof ( CUSTOMVERTEX ) );

						g_pGlob->dwNumberOfPolygonsDrawn += f->elem_count / 3;

							// stage 1
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
			
			// set texture mode
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

			// filter
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
			
			// stage 2
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG1, D3DTA_TEXTURE     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLOROP,   D3DTOP_MODULATE   );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_COLORARG2, D3DTA_CURRENT     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAARG1, D3DTA_CURRENT     );
			m_pD3D->SetTextureStageState ( 1, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );

			// set texture mode
			m_pD3D->SetSamplerState ( 1, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );

			// filter
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pD3D->SetSamplerState ( 1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

					
						break;
					}
					
					case FACETYPE_FLARE:
					{
						break;
					}
				}

			}
		
			return TRUE;
		}

		case SHADER_TEXTURE:
		{
			const shader* s = &shaders.entry [ -( tex + 2 )];
			
			m_pD3D->SetRenderState ( D3DRS_CULLMODE, s->params.cull );
			
			m_pD3D->SetTexture ( 1, NULL );

			int rendered = 0;
			
			// process all stages - this is done by overdrawing, because there might be too many stages ( no hardware support )
			for ( int st = 0; st < s->stage_count; st++ )
			{
				if ( s->stage [ st ].tex_count == 0 ) 
					continue;
				
				int a = 0;
				
				if ( s->stage [ st ].ani_freq > 0 )
				{
					int texs = s->stage [ st ].tex_count;
					int pic  = (int) (s->stage [ st ].ani_freq * texs * Timer ( ) / 10);
					
					pic %= texs;
					a = pic;
				}
				
				// texture processing
				BOOL lightmap = FALSE;

				if ( s->stage [ st ].tex_ref [ a ] < 0 )
				{
					if ( stricmp ( s->stage [ st ].tex_name [ a ], "$whiteimage" ) == 0 )
					{
						// mj - stop white textures
						//m_pD3D->SetTexture ( 0, NULL );
					}
					else if ( stricmp ( s->stage [ st ].tex_name [ a ], "$lightmap" ) == 0 )
					{
						lightmap = TRUE;
					}
				}
				else
					m_pD3D->SetTexture ( 0, Textures.Tex [ s->stage [ st ].tex_ref [ a ] ].lpD3DTex );
	
				// set blending states
				m_pD3D->SetRenderState ( D3DRS_SRCBLEND,  s->stage [ st ].blend.src );
				m_pD3D->SetRenderState ( D3DRS_DESTBLEND, s->stage [ st ].blend.dest );

				// set z buffer settings
				m_pD3D->SetRenderState ( D3DRS_ZFUNC,        s->stage [ st ].zfunc );
				m_pD3D->SetRenderState ( D3DRS_ZWRITEENABLE, s->stage [ st ].zwrite );

				// alphatest
				m_pD3D->SetRenderState ( D3DRS_ALPHATESTENABLE, s->stage [ st ].alphafunc.enable );
				m_pD3D->SetRenderState ( D3DRS_ALPHAREF,        s->stage [ st ].alphafunc.ref );
				m_pD3D->SetRenderState ( D3DRS_ALPHAFUNC,       s->stage [ st ].alphafunc.func );

				// texture coordinates  ( may change when lightmap coordinates are required )
				if ( s->stage [ st ].tcgen == TCGEN_LIGHTMAP )
				{
					m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 1 );
				}
				else
				{
					m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				}

				m_pD3D->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 1 );

				// render the faces
				for ( int x = 0; x < Q3MAP.sort.tex [ stex ].face_count; x++ )
				{
					int face_idx = Q3MAP.sort.tex [ stex ].face [ x ];

					if ( !Q3MAP.mark_faces [ face_idx ] )
						continue;

					set_tc ( face_idx, s, st );

					const _Q3MAP::face* f = &Q3MAP.faces [ Q3MAP.sort.tex [ stex ].face [ x ] ];

					if ( lightmap )
					{
						if ( f->lmap >= 0 )
							m_pD3D->SetTexture ( 0, Textures.LightMap [ f->lmap ].lpD3DTex );
						else
							m_pD3D->SetTexture ( 0, NULL );
						
					}
								
					// render the face
					switch ( f->type )
					{
						case FACETYPE_PATCH:
						{
							// render the patch	

							if ( f->mesh < 0 )
								break;

							//m_pD3D->SetTexture ( 0, NULL );
							//m_pD3D->SetTexture ( 1, NULL );
							
							const mesh_t3* mesh = &Q3MAP.meshes [ f->mesh ];

							m_pD3D->DrawIndexedPrimitiveUP (
																D3DPT_TRIANGLELIST,
																0, 
																mesh->verts_count, 
																mesh->elems_count / 3,
																mesh->elems,
																D3DFMT_INDEX16,
																mesh->verts,
																sizeof ( CUSTOMVERTEX )
														   );

							g_pGlob->dwNumberOfPolygonsDrawn += mesh->elems_count / 3;
							break;
						}

						case FACETYPE_NORMAL:
						{
							// render the polygon
							m_pD3D->DrawPrimitiveUP (
														D3DPT_TRIANGLEFAN, 
														f->vert_count - 2,
														( void** ) &Q3MAP.verts [ f->vert_start ],
														sizeof ( CUSTOMVERTEX ) 
													);

							g_pGlob->dwNumberOfPolygonsDrawn += f->vert_count - 2;
							
							break;
						}

						case FACETYPE_MESH:
						{
							// mj change - don't render meshes
							//continue;

							// render the mesh
							m_pD3D->DrawIndexedPrimitiveUP ( 
																D3DPT_TRIANGLELIST,
																0,
																f->vert_count,
																f->elem_count / 3,
																( void** ) &Q3MAP.elems [ f->elem_start ],
																D3DFMT_INDEX16,
																( void** ) &Q3MAP.verts [ f->vert_start ],
																sizeof ( CUSTOMVERTEX )
														  );

							g_pGlob->dwNumberOfPolygonsDrawn += f->elem_count / 3;
						
							break;
						}
						
						case FACETYPE_FLARE:
						{
							break;
						}
					}

					restore_tc ( face_idx );

					rendered++;
				}

				m_pD3D->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
				m_pD3D->SetTextureStageState ( 1, D3DTSS_TEXCOORDINDEX, 1 );

				if ( rendered == 0 )
					break;
			}

			return TRUE;
		}
	}

	return FALSE;
}
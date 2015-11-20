#ifndef _STEAMCHECKFORWORKSHOP_H_
#define _STEAMCHECKFORWORKSHOP_H_

#include <D3D9.h>
#include <D3DX9.h>

typedef void	(*t_SteamGetWorkshopItemPathDLL)(LPSTR);
typedef int		(*t_SteamIsWorkshopLoadingOnDLL)();

bool CheckForWorkshopFile ( LPSTR VirtualFilename);
bool Steam_CanIUse_W_();

#endif
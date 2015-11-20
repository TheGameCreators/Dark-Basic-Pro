#include "RenderList.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace
{
    class RenderListItem;
    typedef std::vector<RenderListItem>     RenderList;
    typedef RenderList::iterator            RenderListIter;
    typedef void (*FunctionPtr)(...);

    // This used to hold a function pointer to be called during rendering of each camera
    class RenderListItem
    {
        int         m_Priority;
        FunctionPtr m_Pointer;

    public:
        explicit RenderListItem(int iPriority, FunctionPtr pPointer)
            : m_Priority(iPriority), m_Pointer(pPointer) { }

        // Less-than operator, for sorting
        bool operator<(const RenderListItem& Other) const
        {
            if (m_Priority < Other.m_Priority) return true;
            if (m_Priority > Other.m_Priority) return false;
            return m_Pointer < Other.m_Pointer;
        }

        // Equal-to operator, for identifying functions to remove
        bool operator==(const FunctionPtr pPointer) const
        {
            return pPointer == m_Pointer;
        }

        void Execute() const
        {
            m_Pointer();
        }
    };


    RenderList                          g_vRenderList;
    bool                                g_bRenderListDirty;
}


DARKSDK void AddToRenderList(LPVOID pFunction, int iPriority)
{
    if (pFunction)
    {
        // Remove any existing copy of this function
        // Resubmitting the function is therefore equivalent to changing its priority.
        RemoveFromRenderList( pFunction );

        // Mark the stack as dirty
        g_bRenderListDirty = true;

        // Add the item
        g_vRenderList.push_back( RenderListItem( iPriority, (FunctionPtr)pFunction ) );
    }
}


DARKSDK void RemoveFromRenderList(LPVOID pFunction)
{
    // std::remove - Moves items matching the function pointer to the end of the vector,
    //               returning the first of the matching ones.
    // std::vector::erase - Erase from the first removed item to the end of the vector.
    g_vRenderList.erase(
        std::remove( g_vRenderList.begin(), g_vRenderList.end(), (FunctionPtr)pFunction ),
        g_vRenderList.end()
    );
}

#include "DBDLLCore.h"
extern DWORD g_dwSyncMask;
extern GlobStruct g_Glob;
extern RetVoidFunctionPointerPFN g_Camera3D_Update;
extern RetVoidFunctionPointerPFN g_Basic3D_UpdateOnce;
extern RetVoidFunctionPointerPFN g_Basic3D_StencilRenderStart;
extern RetVoidFunctionPointerPFN g_World3D_Update;
extern RetVoidFunctionPointerPFN g_Light3D_Update;
extern RetVoidFunctionPointerPFN g_LODTerrain_Update;
extern RetVoidFunctionPointerPFN g_Matrix3D_Update;
extern RetVoidFunctionPointerPFN g_Basic3D_Update;
extern RetVoidFunctionPointerPFN g_Particles_Update;
extern RetVoidFunctionPointerPFN g_Basic3D_UpdateGhostLayer;
extern RetVoidFunctionPointerPFN g_Basic3D_StencilRenderEnd;
extern RetVoidFunctionPointerPFN g_Matrix3D_LastUpdate;
extern RetVoidFunctionPointerPFN g_Basic3D_UpdateNoZDepth;

void ExecuteRenderList()
{
    // If the render list has been added to, re-sort it
    if (g_bRenderListDirty)
    {
        g_bRenderListDirty = false;
        std::sort( g_vRenderList.begin(), g_vRenderList.end() );
    }

    // Make a copy, in case someone manipulates the list from their own
    // function while it's being actioned.
    std::vector<RenderListItem> RenderList( g_vRenderList );

    // Execute each item in the list, by calling the Execute method of every item in the vector
    std::for_each(
        RenderList.begin(),
        RenderList.end(),
        std::mem_fun_ref( &RenderListItem::Execute )
    );
}

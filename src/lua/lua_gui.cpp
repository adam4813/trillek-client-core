#include "systems/lua-system.hpp"

#include <luawrapper/luawrapper.hpp>
#include <luawrapper/luawrapperutil.hpp>

#include <Rocket/Core.h>

#include "systems/gui.hpp"
#include "trillek-game.hpp"


#define GUI_BINDFN(a)	{ #a, Gui_##a },
#define GUI_BINDFNA(a,f)	{ #a, Gui_##f },

// lua GuiSystem static function
#define GUI_MAKEFN(a) static int Gui_##a(lua_State* L)

// lua GuiSystem member function
#define GUI_MAKEMFN(a) \
    static int Gui_##a(lua_State* L) { \
    if(!luaW_is<gui::GuiSystem>(L, 1)) { return 0; } \
    gui::GuiSystem *gui = luaW_to<gui::GuiSystem>(L, 1);

// lua GuiSystem member function, returning nil on error
#define GUI_MAKEMRFN(a) \
    static int Gui_##a(lua_State* L) { \
    if(!luaW_is<gui::GuiSystem>(L, 1)) { lua_pushnil(L); return 1; } \
    gui::GuiSystem *gui = luaW_to<gui::GuiSystem>(L, 1);

using namespace trillek;
using namespace trillek::script;

// GuiSystem get
GUI_MAKEFN(Get) {
    luaW_push<gui::GuiSystem>(L, &game.GetGUISystem());
    return 1;
}

// GuiSystem calls
GUI_MAKEMFN(load_doc)
    auto doc = lua_tostdstring(L, 2);
    uint32_t docid = gui->AsyncLoadDocument(doc);
    lua_pushunsigned(L, docid);
    return 1;
}
GUI_MAKEMFN(sync_load_doc)
    auto doc = lua_tostdstring(L, 2);
    uint32_t docid = gui->LoadDocument(doc);
    lua_pushunsigned(L, docid);
    return 1;
}
GUI_MAKEMFN(close_doc)
    int isnum;
    uint32_t doc = lua_tounsignedx(L, 2, &isnum);
    if(isnum) {
        gui->AsyncCloseDocument(doc);
    }
    return 0;
}
GUI_MAKEMFN(show_doc)
    int isnum;
    uint32_t doc = lua_tounsignedx(L, 2, &isnum);
    if(isnum) {
        gui->ShowDocument(doc);
    }
    return 0;
}
GUI_MAKEMFN(hide_doc)
    int isnum;
    uint32_t doc = lua_tounsignedx(L, 2, &isnum);
    if(isnum) {
        gui->HideDocument(doc);
    }
    return 0;
}
GUI_MAKEMFN(doc_visible)
    int isnum;
    uint32_t doc = lua_tounsignedx(L, 2, &isnum);
    if(isnum) {
        lua_pushboolean(L, (gui->IsDocumentVisible(doc) ? 1 : 0));
        return 1;
    }
    lua_pushboolean(L, 0);
    return 1;
}
GUI_MAKEMFN(get_doc)
    int isnum;
    uint32_t doc = lua_tounsignedx(L, 2, &isnum);
    if(!isnum) {
        lua_pushnil(L); return 1;
    }
    auto docp = gui->GetDocument(doc);
    if(docp == nullptr) {
        lua_pushnil(L); return 1;
    }
    luaW_push(L, docp);
    return 1;
}
GUI_MAKEMFN(load_font)
    auto font = lua_tostdstring(L, 2);
    gui->LoadFont(font);
    return 0;
}
GUI_MAKEMFN(subscribe)
    return 0;
}

// Rocket Element

#define ELEM_MAKEFN(a) \
    static int UIElem_##a(lua_State* L) { \
    if(!luaW_is<Rocket::Core::Element>(L, 1)) { return 0; } \
    Rocket::Core::Element* elem = luaW_to<Rocket::Core::Element>(L, 1);
#define ELEM_MAKERFN(a) \
    static int UIElem_##a(lua_State* L) { \
    if(!luaW_is<Rocket::Core::Element>(L, 1)) { lua_pushnil(L); return 1; } \
    Rocket::Core::Element* elem = luaW_to<Rocket::Core::Element>(L, 1);

using Rocket::Core::String;

template<>
struct luaU_Impl<Rocket::Core::String> {
    static Rocket::Core::String luaU_check(lua_State* L, int index) {
        unsigned x;
        const char * cs = lua_tolstring(L, index, &x);
        return String(cs, cs+x);
    }
    static Rocket::Core::String luaU_to(lua_State* L, int index) {
        unsigned x;
        const char * cs = lua_tolstring(L, index, &x);
        return String(cs, cs+x);
    }
    static void luaU_push(lua_State* L, const Rocket::Core::String& val) {
        lua_pushlstring(L, val.CString(), val.Length());
    }
    static void luaU_push(lua_State* L, Rocket::Core::String& val) {
        lua_pushlstring(L, val.CString(), val.Length());
    }
};

ELEM_MAKEFN(addClass)
    elem->SetClass(luaU_to<String>(L, 2), true);
    return 0;
}
ELEM_MAKEFN(removeClass)
    elem->SetClass(luaU_to<String>(L, 2), false);
    return 0;
}
ELEM_MAKEFN(setClassNames)
    elem->SetClassNames(luaU_to<String>(L, 2));
    return 0;
}
ELEM_MAKERFN(classNames)
    luaU_push(L, elem->GetClassNames());
    return 1;
}
ELEM_MAKERFN(isClassSet)
    lua_pushbool(L, elem->IsClassSet(luaU_to<String>(L, 2)));
    return 1;
}
ELEM_MAKERFN(visible)
    lua_pushbool(L, elem->IsVisible());
    return 1;
}
ELEM_MAKERFN(zIndex)
    lua_pushnumber(L, elem->GetZIndex());
    return 1;
}
ELEM_MAKEFN(addPseudoClass)
    elem->SetPseudoClass(luaU_to<String>(L, 2), true);
    return 0;
}
ELEM_MAKEFN(removePseudoClass)
    elem->SetPseudoClass(luaU_to<String>(L, 2), false);
    return 0;
}
ELEM_MAKERFN(isPseudoClassSet)
    lua_pushbool(L, elem->IsPseudoClassSet(luaU_to<String>(L, 2)));
    return 1;
}
ELEM_MAKERFN(tagName)
    luaU_push(L, elem->GetTagName());
    return 1;
}
ELEM_MAKERFN(id)
    luaU_push(L, elem->GetId());
    return 1;
}
ELEM_MAKEFN(setId)
    elem->SetId(luaU_to<String>(L, 2));
    return 0;
}
ELEM_MAKERFN(absoluteLeft)
    lua_pushnumber(L, elem->GetAbsoluteLeft());
    return 1;
}
ELEM_MAKERFN(absoluteTop)
    lua_pushnumber(L, elem->GetAbsoluteTop());
    return 1;
}
ELEM_MAKERFN(clientLeft)
    lua_pushnumber(L, elem->GetClientLeft());
    return 1;
}
ELEM_MAKERFN(clientTop)
    lua_pushnumber(L, elem->GetClientTop());
    return 1;
}
ELEM_MAKERFN(clientWidth)
    lua_pushnumber(L, elem->GetClientWidth());
    return 1;
}
ELEM_MAKERFN(clientHeight)
    lua_pushnumber(L, elem->GetClientHeight());
    return 1;
}
ELEM_MAKERFN(offsetParent)
    luaW_push(L, elem->GetOffsetParent());
    return 1;
}
ELEM_MAKERFN(offsetLeft)
    lua_pushnumber(L, elem->GetOffsetLeft());
    return 1;
}
ELEM_MAKERFN(offsetTop)
    lua_pushnumber(L, elem->GetOffsetTop());
    return 1;
}
ELEM_MAKERFN(offsetWidth)
    lua_pushnumber(L, elem->GetOffsetWidth());
    return 1;
}
ELEM_MAKERFN(offsetHeight)
    lua_pushnumber(L, elem->GetOffsetHeight());
    return 1;
}
ELEM_MAKERFN(scrollLeft)
    lua_pushnumber(L, elem->GetScrollLeft());
    return 1;
}
ELEM_MAKEFN(setScrollLeft)
    elem->SetScrollLeft(lua_tonumber(L, 2));
    return 0;
}
ELEM_MAKERFN(scrollTop)
    lua_pushnumber(L, elem->GetScrollTop());
    return 1;
}
ELEM_MAKEFN(setScrollTop)
    elem->SetScrollTop(lua_tonumber(L, 2));
    return 0;
}
ELEM_MAKERFN(scrollWidth)
    lua_pushnumber(L, elem->GetScrollWidth());
    return 1;
}
ELEM_MAKERFN(scrollHeight)
    lua_pushnumber(L, elem->GetScrollHeight());
    return 1;
}
ELEM_MAKERFN(parent)
    luaW_push(L, elem->GetParentNode());
    return 1;
}
ELEM_MAKERFN(nextSibling)
    luaW_push(L, elem->GetNextSibling());
    return 1;
}
ELEM_MAKERFN(previousSibling)
    luaW_push(L, elem->GetPreviousSibling());
    return 1;
}
ELEM_MAKERFN(firstChild)
    luaW_push(L, elem->GetFirstChild());
    return 1;
}
ELEM_MAKERFN(lastChild)
    luaW_push(L, elem->GetLastChild());
    return 1;
}
ELEM_MAKERFN(child)
    luaW_push(L, elem->GetChild(lua_tointeger(L, 2)));
    return 1;
}
ELEM_MAKERFN(numChildren)
    lua_pushinteger(L, elem->GetNumChildren(false));
    return 1;
}
ELEM_MAKERFN(innerRML)
    luaU_push(L, elem->GetInnerRML());
    return 1;
}
ELEM_MAKEFN(setInnerRML)
    elem->SetInnerRML(luaU_to<String>(L, 2));
    return 0;
}
ELEM_MAKERFN(getElementById)
    luaW_push(L, elem->GetElementById(luaU_to<String>(L, 2)));
    return 1;
}
ELEM_MAKERFN(getElementsByTagName)
    Rocket::Core::ElementList el;
    elem->GetElementsByTagName(el, luaU_to<String>(L, 2));
    int ct = el.size();
    lua_createtable(L, ct, 0);
    int t = lua_gettop(L);
    for(int i = 0; i < ct; i++) {
        lua_pushinteger(L, i);
        luaW_push(L, el.at(i));
        lua_settable(L, t);
    }
    return 1;
}
ELEM_MAKERFN(getElementsByClassName)
    Rocket::Core::ElementList el;
    elem->GetElementsByClassName(el, luaU_to<String>(L, 2));
    int ct = el.size();
    lua_createtable(L, ct, 0);
    int t = lua_gettop(L);
    for(int i = 0; i < ct; i++) {
        lua_pushinteger(L, i);
        luaW_push(L, el.at(i));
        lua_settable(L, t);
    }
    return 1;
}

/*
ELEM_MAKERFN()
    return 1;
}
ELEM_MAKEFN()
    return 0;
}
*/

static luaL_Reg GuiSys_table[] =
{
    GUI_BINDFN(Get)
    { nullptr, nullptr } // table end marker
};

static luaL_Reg GuiSys_metatable[] =
{
    GUI_BINDFNA(load_document, load_doc)
    GUI_BINDFN(load_doc)
    GUI_BINDFN(sync_load_doc)
    GUI_BINDFN(close_doc)
    GUI_BINDFN(show_doc)
    GUI_BINDFN(hide_doc)
    GUI_BINDFN(doc_visible)
    GUI_BINDFN(get_doc)
    GUI_BINDFN(load_font)
    GUI_BINDFN(subscribe)
    { nullptr, nullptr } // table end marker
};

template<typename T>
static void luaW_Rocketidentifier(lua_State* L, T* obj)
{
    obj->AddReference();
    lua_pushlightuserdata(L, ((char*)obj) + obj->GetReferenceCount());
}
template<typename T>
static void luaW_Rocketremover(lua_State* L, T* obj)
{
    obj->RemoveReference();
}

#define ELEM_BINDFN(a) { #a, UIElem_##a },
static luaL_Reg UIElem_table[] = {
    { nullptr, nullptr } // end marker
};
static luaL_Reg UIElem_metatable[] = {
    ELEM_BINDFN(addClass)
    ELEM_BINDFN(removeClass)
    ELEM_BINDFN(setClassNames)
    ELEM_BINDFN(classNames)
    ELEM_BINDFN(isClassSet)
    ELEM_BINDFN(visible)
    ELEM_BINDFN(zIndex)
    ELEM_BINDFN(addPseudoClass)
    ELEM_BINDFN(removePseudoClass)
    ELEM_BINDFN(isPseudoClassSet)
    ELEM_BINDFN(tagName)
    ELEM_BINDFN(id)
    ELEM_BINDFN(setId)
    ELEM_BINDFN(absoluteLeft)
    ELEM_BINDFN(absoluteTop)
    ELEM_BINDFN(clientLeft)
    ELEM_BINDFN(clientTop)
    ELEM_BINDFN(clientWidth)
    ELEM_BINDFN(clientHeight)
    ELEM_BINDFN(offsetParent)
    ELEM_BINDFN(offsetLeft)
    ELEM_BINDFN(offsetTop)
    ELEM_BINDFN(offsetWidth)
    ELEM_BINDFN(offsetHeight)
    ELEM_BINDFN(scrollLeft)
    ELEM_BINDFN(setScrollLeft)
    ELEM_BINDFN(scrollTop)
    ELEM_BINDFN(setScrollTop)
    ELEM_BINDFN(scrollWidth)
    ELEM_BINDFN(scrollHeight)
    ELEM_BINDFN(parent)
    ELEM_BINDFN(nextSibling)
    ELEM_BINDFN(previousSibling)
    ELEM_BINDFN(firstChild)
    ELEM_BINDFN(lastChild)
    ELEM_BINDFN(child)
    ELEM_BINDFN(numChildren)
    ELEM_BINDFN(innerRML)
    ELEM_BINDFN(setInnerRML)
    ELEM_BINDFN(getElementById)
    ELEM_BINDFN(getElementsByTagName)
    ELEM_BINDFN(getElementsByClassName)
    { nullptr, nullptr } // end marker
};

static luaL_Reg UIElemDoc_table[] = {
    { nullptr, nullptr } // end marker
};
static luaL_Reg UIElemDoc_metatable[] = {
    { nullptr, nullptr } // end marker
};

namespace trillek {
namespace script {

int luaopen_GuiSys(lua_State* L) {
    luaW_register<gui::GuiSystem>(L,
        "GUI",
        GuiSys_table,
        GuiSys_metatable,
        nullptr // no constructor
        );
    luaW_register<Rocket::Core::Element>(L,
        "Element",
        UIElem_table, UIElem_metatable,
        nullptr, // no constructor
        luaW_Rocketremover,
        luaW_Rocketidentifier
        );
    luaW_register<Rocket::Core::ElementDocument>(L,
        "ElementDoc",
        UIElemDoc_table, UIElemDoc_metatable,
        nullptr, // no constructor
        luaW_Rocketremover,
        luaW_Rocketidentifier
        );
    luaW_extend<Rocket::Core::ElementDocument, Rocket::Core::Element>(L);
    lua_newtable(L);
    lua_setglobal(L, "_UI");
    return 1;
}

} // script
} // trillek

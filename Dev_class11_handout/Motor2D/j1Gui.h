#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "UIelement.h"

struct _TTF_Font;

// Structure of classes

// ---------------------------------------------------
enum EVENTS
{
	MOUSE_IN,
	MOUSE_OUT,
	MOUSE_CLICK,
	MOUSE_RELEASE
};

// ---------------------------------------------------
class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	const SDL_Texture* GetAtlas() const;
	
	// Gui creation functions
	Label*		CreateLabel(const int& x, const int& y, const char* s, _TTF_Font* f, j1Module* l, UIelement* p);
	Image*		CreateImage(const int& x, const int& y, const SDL_Rect& s, SDL_Texture* t, bool d, j1Module* l, UIelement* p);
	Image*		CreateImage(const int& x, const int& y, const SDL_Rect* s, SDL_Texture* t, bool d, j1Module* l, UIelement* p);
	Button*		CreateButton(const int& x, const int& y, const SDL_Rect& idle, const SDL_Rect& hover, const SDL_Rect& click, SDL_Texture* t, bool d, j1Module* l, UIelement* p);
	InputText*	CreateInputText(const int& x, const int& y, const SDL_Rect& sect, SDL_Texture* t, const char* str, _TTF_Font* f, bool d, j1Module* l, UIelement* p);
	//EXERCISE 1
	ScrollBar*	CreateScrollBar(const int& x, const int& y, const SDL_Rect& s_b, const SDL_Rect& s_t, SDL_Texture* t_b, SDL_Texture* t_t, bool d, j1Module* l, UIelement* p);

	p2List<UIelement*>	UI_elements;
	Cursor*				cursor;
	 
private:
	p2SString		atlas_file_name;
	SDL_Texture*	atlas;
	bool			dragging;
	UIelement*		drag_element;
	UIelement*		focus;

	// Principal functions of the Update
	void			DrawAllUI();
	void			OnGuiCaller();
	void			Drag();
	UIelement*		FindFocus();

	// Secundary functions
	bool			IsCursorHover(const UIelement* box);
	void			UpdateDragElementPos();
	bool			IsCursorHoverOtherElement();
};

#endif // __j1GUI_H__
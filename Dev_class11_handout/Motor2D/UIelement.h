#ifndef UIELEMENT_H
#define UIELEMENT_H

#include "SDL/include/SDL.h"
#include "j1Module.h"

struct _TTF_Font;

enum UI_TYPE
{
	LABEL,
	IMAGE,
	BUTTON,
	INPUT_TEXT,
	SCROLL_BAR,
	CURSOR
};

// ---------------------------------------------------
enum BUTTON_STATE
{
	IDLE,
	HOVER,
	CLICK,
};

class UIelement
{
	friend class j1Gui;
public:
	UIelement();
	UI_TYPE type;

protected:
	SDL_Rect pos;
	SDL_Rect section;
	SDL_Texture* texture;
	j1Module* listener;
	UIelement* parent;
	bool was_cursor_inside;
	bool dragable;
	bool focusable;

public:
	virtual void Draw();
	virtual void Update();
	virtual const int GetScreenX() const;
	virtual const int GetScreenY() const;
};

// ---------------------------------------------------
class Label : public UIelement
{
public:
	Label(const int& x, const int& y, const char* t, _TTF_Font* f, j1Module* l, UIelement* p);

private:
	p2SString text;
	_TTF_Font* font;

public:
	void Draw();
};

// ---------------------------------------------------
class Image : public UIelement
{
public:
	Image(const int& x, const int& y, SDL_Texture* t, bool d, j1Module* l, UIelement* p, const SDL_Rect* s = NULL);

public:
	void Draw();
};

// ---------------------------------------------------
class Button : public UIelement
{
public:
	Button(const int& x, const int& y, const SDL_Rect& idle, const SDL_Rect& hover, const SDL_Rect& click, SDL_Texture* t, bool d, j1Module* l, UIelement* p);

	BUTTON_STATE state;

private:
	SDL_Rect draw_idle;
	SDL_Rect draw_hover;
	SDL_Rect draw_click;

public:
	void Draw();
};

// ---------------------------------------------------
class InputText : public UIelement
{
public:
	InputText(const int& x, const int& y, const SDL_Rect* sect, SDL_Texture* t, const char* str, _TTF_Font* f, bool d, j1Module* l, UIelement* p);
	bool empty;

private:
	p2SString	text;
	_TTF_Font*	font;
	Image*		image;

public:
	//void Update();
	void Draw();
};

// EXERCISE 1---------------------------------------------------
class ScrollBar : public UIelement
{
public:
	ScrollBar(const int& x, const int& y, const SDL_Rect* s_b, const SDL_Rect* s_t, SDL_Texture* t_b, SDL_Texture* t_t, bool d, j1Module* l, UIelement* p);

	Image*			thumb;
	bool			moving_thumb;

private:
	float			current_value;
	_TTF_Font*		font;
	SDL_Texture*	value_texture;

public:
	void Draw();
	void Update();
};

// ---------------------------------------------------
class Cursor : public UIelement
{
public:
	Cursor(const int& x, const int& y, const SDL_Rect& s, SDL_Texture* t, j1Module* l);

	bool hover;

public:
	void Update();
	void Draw();
};

#endif // __UIelement_H__
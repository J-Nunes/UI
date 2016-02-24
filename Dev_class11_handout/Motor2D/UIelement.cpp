#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "UIelement.h"

// class UIelement ---------------------------------------------------
UIelement::UIelement()
{}
void UIelement::Draw()
{}

void UIelement::Update()
{}

const int UIelement::GetScreenX() const
{
	int ret = pos.x;

	UIelement* tmp = parent;
	while (tmp != NULL)
	{
		ret += tmp->pos.x;
		tmp = tmp->parent;
	}

	return ret;
}

const int UIelement::GetScreenY() const
{
	int ret = pos.y;

	UIelement* tmp = parent;
	while (tmp != NULL)
	{
		ret += tmp->pos.y;
		tmp = tmp->parent;
	}

	return ret;
}

// class Image ---------------------------------------------------
Image::Image(const int& x, const int& y, SDL_Texture* t, bool d, j1Module* l, UIelement* p, const SDL_Rect* s)
{
	texture = t;
	if (s != NULL)
	{
		section = *s;
		pos.w = section.w;
		pos.h = section.h;
	}
	else
	{
		section = { 0, 0, 0, 0 };
		SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	}
	type = IMAGE;
	pos.x = x;
	pos.y = y;
	listener = l;
	parent = p;
	was_cursor_inside = false;
	dragable = d;
}

void Image::Draw()
{
	if (section.x == 0 && section.y == 0 && section.w == 0 && section.h == 0)
		App->render->Blit(texture, GetScreenX(), GetScreenY(), NULL, 0.0f);
	else
		App->render->Blit(texture, GetScreenX(), GetScreenY(), &section, 0.0f);
}

// class Label ---------------------------------------------------
Label::Label(const int& x, const int& y, const char* t, _TTF_Font* f, j1Module* l, UIelement* p)
{
	font = f;
	text.create(t);
	texture = App->font->Print(text.GetString(), font);
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	type = LABEL;
	listener = l;
	parent = p;
	was_cursor_inside = false;
	dragable = false;
}

void Label::Draw()
{
	App->render->Blit(texture, GetScreenX(), GetScreenY(), NULL, 0.0f);
}

// class Button ---------------------------------------------------
Button::Button(const int& x, const int& y, const SDL_Rect& idle, const SDL_Rect& hover, const SDL_Rect& click, SDL_Texture* t, bool d, j1Module* l, UIelement* p)
{
	pos.x = x;
	pos.y = y;
	pos.w = idle.w;
	pos.h = idle.h;
	texture = t;
	draw_idle = idle;
	draw_hover = hover;
	draw_click = click;
	type = BUTTON;
	state = IDLE;
	listener = l;
	parent = p;
	was_cursor_inside = false;
	dragable = d;
}

void Button::Draw()
{
	switch (state)
	{
	case IDLE:
		App->render->Blit(texture, GetScreenX(), GetScreenY(), &draw_idle, 0.0f);
		break;

	case HOVER:
		App->render->Blit(texture, GetScreenX(), GetScreenY(), &draw_hover, 0.0f);
		break;

	case CLICK:
		App->render->Blit(texture, GetScreenX(), GetScreenY(), &draw_click, 0.0f);
		break;
	}
}

// class InputText ---------------------------------------------------
InputText::InputText(const int& x, const int& y, const SDL_Rect* sect, SDL_Texture* t, const char* str, _TTF_Font* f, bool d, j1Module* l, UIelement* p)
{
	image = new Image(x, y, t, d, l, p, sect);
	App->gui->UI_elements.add(image);
	pos.x = 10;
	pos.y = 10;
	App->font->CalcSize(str, pos.w, pos.h, font);
	font = f;
	text.create(str);
	texture = App->font->Print(text.GetString(), font);
	type = INPUT_TEXT;
	listener = l;
	parent = image;
	was_cursor_inside = false;
	dragable = false;
	empty = true;
}

void InputText::Draw()
{
	App->render->Blit(texture, GetScreenX(), GetScreenY(), NULL, 0.0f);
}

// //EXERCISE 1 ---------------------------------------------------
ScrollBar::ScrollBar(const int& x, const int& y, const SDL_Rect* s_b, const SDL_Rect* s_t, SDL_Texture* t_b, SDL_Texture* t_t, bool d, j1Module* l, UIelement* p)
{
	thumb = App->gui->CreateImage(50, 0, *s_t, t_t, true, l, this);
	texture = t_b;
	if (s_b != NULL)
	{
		section = *s_b;
		pos.w = section.w;
		pos.h = section.h;
	}
	else
	{
		section = { 0, 0, 0, 0 };
		SDL_QueryTexture(texture, NULL, NULL, &pos.w, &pos.h);
	}
	pos.x = x;
	pos.y = y;
	type = SCROLL_BAR;
	listener = l;
	parent = p;
	was_cursor_inside = false;
	dragable = false;
	moving_thumb = false;
}

void ScrollBar::Draw()
{
	if (section.x == 0 && section.y == 0 && section.w == 0 && section.h == 0)
		App->render->Blit(texture, GetScreenX(), GetScreenY(), NULL, 0.0f);
	else
		App->render->Blit(texture, GetScreenX(), GetScreenY(), &section, 0.0f);
}

void ScrollBar::Update()
{
	current_value = thumb->GetScreenX() / (GetScreenX() + pos.w);
}

// END OF EXERCISE 1-----------------------------------------------

// class Cursor ---------------------------------------------------
Cursor::Cursor(const int& x, const int& y, const SDL_Rect& s, SDL_Texture* t, j1Module* l)
{
	texture = t;
	section = s;
	pos.x = x;
	pos.y = y;
	pos.w = pos.h = 0;
	type = CURSOR;
	hover = false;
	listener = l;
}

void Cursor::Update()
{
	App->input->GetMousePosition(pos.x, pos.y);
}

void Cursor::Draw()
{
	if (hover == true)
	{
		section.x = 32;
		App->render->Blit(texture, pos.x, pos.y, &section, 0.0f);
	}
	else
	{
		section.x = 0;
		App->render->Blit(texture, pos.x, pos.y, &section, 0.0f);
	}
}
#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Gui.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;
	
	dragging = false;

	SDL_ShowCursor(SDL_DISABLE);

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	cursor->Update();
	OnGuiCaller();
	Drag();
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{
	DrawAllUI();

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	p2List_item<UIelement*>* ui_item = UI_elements.start;
	while (ui_item)
	{
		delete ui_item->data;
		ui_item = ui_item->next;
	}
	UI_elements.clear();

	return true;
}

// Const getter for atlas
const SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

//Factories
Image* j1Gui::CreateImage(const int& x, const int& y, const SDL_Rect& s, SDL_Texture* t, bool d, j1Module* l, UIelement* p)
{
	Image* ret;

	if (t != NULL)
		ret = new Image(x, y, t, d, l, p, &s);
	else
		ret = new Image(x, y, atlas, d, l, p, &s);

	return ret;
}

Image* j1Gui::CreateImage(const int& x, const int& y, const SDL_Rect* s, SDL_Texture* t, bool d, j1Module* l, UIelement* p)
{
	Image* ret;

	if (t != NULL)
		ret = new Image(x, y, t, d, l, p);
	else
		ret = new Image(x, y, atlas, d, l, p);

	return ret;
}

Button* j1Gui::CreateButton(const int& x, const int& y, const SDL_Rect& idle, const SDL_Rect& hover, const SDL_Rect& click, SDL_Texture* t, bool d, j1Module* l, UIelement* p)
{
	Button* ret;
	if (t != NULL)
		ret = new Button(x, y, idle, hover, click, t, d, l, p);
	else
		ret = new Button(x, y, idle, hover, click, atlas, d, l, p);

	return ret;
}

Label* j1Gui::CreateLabel(const int& x, const int& y, const char* s, _TTF_Font* f, j1Module* l, UIelement* p)
{
	Label* ret = new Label(x, y, s, f, l, p);

	return ret;
}

InputText* j1Gui::CreateInputText(const int& x, const int& y, const SDL_Rect& sect, SDL_Texture* t, const char* str, _TTF_Font* f, bool d, j1Module* l, UIelement* p)
{
	InputText* ret;
	if (t != NULL)
		ret = new InputText(x, y, &sect, t, str, f, d, l, p);

	else
		ret = new InputText(x, y, &sect, atlas, str, f, d, l, p);

	return ret;
}

ScrollBar* j1Gui::CreateScrollBar(const int& x, const int& y, const SDL_Rect& s_b, const SDL_Rect& s_t, SDL_Texture* t_b, SDL_Texture* t_t, bool d, j1Module* l, UIelement* p)
{
	ScrollBar* ret;
	if (t_b != NULL)
		ret = new ScrollBar(x, y, &s_b, &s_t, t_b, t_t, d, l, p);

	else
		ret = new ScrollBar(x, y, &s_b, &s_t, atlas, t_t, d, l, p);

	return ret;
}

// Update Methods
void j1Gui::DrawAllUI()
{
	p2List_item<UIelement*>* ui_item = UI_elements.start;
	while (ui_item)
	{
		ui_item->data->Draw();
		ui_item = ui_item->next;
	}

	cursor->Draw();
}

void j1Gui::OnGuiCaller()
{
	p2List_item<UIelement*>* ui_item = UI_elements.end;
	while (ui_item != NULL)
	{
		if (IsCursorHover(ui_item->data))
		{
			if (ui_item->data->was_cursor_inside == false && dragging == false)
			{
				cursor->listener->OnGui(MOUSE_IN, cursor);
				ui_item->data->listener->OnGui(MOUSE_IN, ui_item->data);
				ui_item->data->was_cursor_inside = true;
			}

			else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT && dragging == false)
			{
				cursor->listener->OnGui(MOUSE_CLICK, cursor);
				ui_item->data->listener->OnGui(MOUSE_CLICK, ui_item->data);
			}

			else if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP)
			{
				cursor->listener->OnGui(MOUSE_RELEASE, cursor);
				ui_item->data->listener->OnGui(MOUSE_RELEASE, ui_item->data);
			}
		}

		else if (ui_item->data->was_cursor_inside == true)
		{
			ui_item->data->was_cursor_inside = false;
			if (IsCursorHoverOtherElement() != true)
				cursor->listener->OnGui(MOUSE_OUT, cursor);
			if (dragging == false)
				ui_item->data->listener->OnGui(MOUSE_OUT, ui_item->data);
		}
		
		ui_item = ui_item->prev;
	}
}


void j1Gui::Drag()
{
	if (dragging == false)
	{
		p2List_item<UIelement*>* ui_item = UI_elements.end;
		while (ui_item != NULL)
		{
			if (ui_item->data->dragable == true && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			{
				if (IsCursorHover(ui_item->data))
				{
					drag_element = ui_item->data;
					dragging = true;
					UpdateDragElementPos();
					break;
				}
			}
			ui_item = ui_item->prev;
		}
	}

	else
	{
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)
			UpdateDragElementPos();

		else
		{
			dragging = false;
			if (IsCursorHover(drag_element) != true)
				drag_element->listener->OnGui(MOUSE_OUT, drag_element);
		}
	}
}

UIelement* j1Gui::FindFocus()
{
	UIelement* ret = NULL;

	if (App->input->GetKey(SDL_SCANCODE_TAB) == KEY_DOWN)
	{
		p2List_item<UIelement*>* ui_item = UI_elements.start;
		while (ui_item != NULL)
		{
			if (ui_item->data->focusable == true)
				return ret;

			ui_item = ui_item->next;
		}
	}
	return ret;
}

// Secundary methods
bool j1Gui::IsCursorHover(const UIelement* box)
{
	bool ret = false;

	if (cursor->pos.x < box->GetScreenX() + box->pos.w &&
		cursor->pos.x > box->GetScreenX() &&
		cursor->pos.y < box->GetScreenY() + box->pos.h &&
		cursor->pos.y > box->GetScreenY())
	{
		ret = true;
	}

	return ret;
}

void j1Gui::UpdateDragElementPos()
{
	iPoint mouse;
	App->input->GetMouseMotion(mouse.x, mouse.y);
	//EXERCISE 5
	if (drag_element->parent != NULL && drag_element->parent->type == SCROLL_BAR)
	{
		Image* thumb = (Image*)drag_element;
		ScrollBar* bar = (ScrollBar*)drag_element->parent;

		if (thumb->GetScreenX() > bar->GetScreenX() &&
			thumb->GetScreenX() + thumb->pos.w < bar->GetScreenX() + bar->pos.w)
		{
			drag_element->pos.x += mouse.x;
		}

		else if (thumb->GetScreenX() <= bar->GetScreenX())
			drag_element->pos.x += 1;

		else 
			drag_element->pos.x -= 1;
	}
	//END OF EXERCISE 5
	else
	{
		drag_element->pos.x += mouse.x;
		drag_element->pos.y += mouse.y;
	}
}

bool j1Gui::IsCursorHoverOtherElement()
{
	p2List_item<UIelement*>* ui_item = UI_elements.start;
	while (ui_item != NULL)
	{
		if (ui_item->data->was_cursor_inside == true)
			return true;

		ui_item = ui_item->next;
	}

	return false;
}
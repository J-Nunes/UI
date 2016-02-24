#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Fonts.h"

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("iso_walk.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
	}

	cursor_text = App->tex->Load("gui/cursor.png");
	cursor_colliding_text = App->tex->Load("gui/cursor_sword.png");
	logo = App->tex->Load("gui/WOW_logo.png");

	debug_tex = App->tex->Load("maps/path2.png");

	_TTF_Font* input_font = App->font->Load("fonts/open_sans/OpenSans-Regular.ttf", 22);

	int x, y;
	App->input->GetMousePosition(x, y);
	App->gui->cursor = new Cursor(x, y, { 0, 0, 32, 32 }, cursor_text, this);

	App->gui->UI_elements.add(App->gui->CreateImage(500, 50, NULL, logo, true, this, NULL));

	UIelement* window = App->gui->CreateImage(50, 100, { 32, 542, 420, 452 }, NULL, true, this, NULL);
	App->gui->UI_elements.add(window);

	App->gui->UI_elements.add(App->gui->CreateInputText(50, 100, { 495, 573, 332, 52 }, NULL, "Password", input_font, true, this, window));

	UIelement* button = App->gui->CreateButton(100, 200, { 642, 169, 229, 69 }, { 0, 113, 229, 69 }, { 411, 169, 229, 69 }, NULL, true, this, window);
	App->gui->UI_elements.add(button);

	App->gui->UI_elements.add(App->gui->CreateLabel(50, 25, "El WoW lo peta maximo", NULL, this, button));
	
	//EXERCISE 1
	ScrollBar* scroll_bar = App->gui->CreateScrollBar(50, 300, { 0, 11, 307, 11 }, { 805, 318, 26, 15 }, NULL, NULL, false, this, window);
	App->gui->UI_elements.add(scroll_bar);
	App->gui->UI_elements.add(scroll_bar->thumb);

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{

	// debug pathfing ------------------
	static iPoint origin;
	static bool origin_selected = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if(App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		if(origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	// Gui ---
	
	// -------
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += floor(200.0f * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= floor(200.0f * dt);

	App->map->Draw();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint map_coordinates = App->map->WorldToMap(x - App->render->camera.x, y - App->render->camera.y);
	p2SString title("Map:%dx%d Tiles:%dx%d Tilesets:%d Tile:%d,%d",
					App->map->data.width, App->map->data.height,
					App->map->data.tile_width, App->map->data.tile_height,
					App->map->data.tilesets.count(),
					map_coordinates.x, map_coordinates.y);

	//App->win->SetTitle(title.GetString());

	// Debug pathfinding ------------------------------
	//int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	App->render->Blit(debug_tex, p.x, p.y);

	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::OnGui(EVENTS even, UIelement* gui_element)
{
	switch (even)
	{
		case MOUSE_IN:
			switch (gui_element->type)
			{
				case BUTTON:
				{
					Button* b = (Button*)gui_element;
					b->state = HOVER;
					break;
				}

				case CURSOR:
					App->gui->cursor->hover = true;
					break;
			}
		break;

		case MOUSE_CLICK:
			switch (gui_element->type)
			{
				case BUTTON:
				{
					Button* b = (Button*)gui_element;
					b->state = CLICK;
					break;
				}

				case CURSOR:
				{
					App->gui->cursor->hover = false;
					break;
				}

				case SCROLL_BAR:
				{
				}
			}
		break;

		case MOUSE_RELEASE:
			switch (gui_element->type)
			{
				case BUTTON:
				{
					Button* b = (Button*)gui_element;
					b->state = HOVER;
					break;
				}

				case CURSOR:
				{
					App->gui->cursor->hover = true;
					break;
				}

				case SCROLL_BAR:
				{
				}
			}
		break;

		case MOUSE_OUT:
			switch (gui_element->type)
			{
				case BUTTON:
				{
					Button* b = (Button*)gui_element;
					b->state = IDLE;
					break; 
				}

				case CURSOR:
					App->gui->cursor->hover = false;
					break;
			}
		break;
	}
}

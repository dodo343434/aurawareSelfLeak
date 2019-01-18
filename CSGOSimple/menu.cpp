#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "droid.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"

const char* weaponNamesForCombobox[] = {
	"Desert Eagle",
	"Dual Berettas",
	"Five-SeveN",
	"Glock-18",
	"AK-47",
	"AUG",
	"AWP",
	"FAMAS",
	"G3SG1",
	"Galil AR",
	"M249",
	"M4A4",
	"MAC-10",
	"P90",
	"UMP-45",
	"XM1014",
	"PP-Bizon",
	"MAG-7",
	"Negev",
	"Sawed.Off",
	"Tec-9",
	"P2000",
	"MP7",
	"MP9",
	"Nova",
	"P250",
	"SCAR-20",
	"SG 553",
	"SSG 08",
	"M4A1-S",
	"USP-S",
	"CZ75-Auto" ,
	"R8 Revolver",
	"Knife"
};

const char* typeoflag[] = {
	" Factor",
	" Adaptive",
	" Break"
};

const char* ChamBTtyp[] = {
	" All Ticks",
	" Last Tick",
	" 3 Ticks"
};
const char* knifeNames[] =
{
	" Default",
	" Bayonet",
	" Flip",
	" Gut",
	" Karambit",
	" M9 Bayonet",
	" Huntsman",
	" Falchion",
	" Bowie",
	" Butterfly",
	"But Plugs"
};

const char* TrigType[] =
{
	" Rifle",
	" Pistol",
	" Sniper"
};
const char* Hitsounds[] =
{
	" Off",
	" Skeet",
	" Normal",
	" Bubble"
};
const char* Hitboxx[] =
{
	"",
	"   Head",
	"   Neck",
	"   Chest",
	"   Stomach",
	"   Closest"
};
static char* SkyColor[] = {
	" Default" ,
	" Purple" ,
	" Red" ,
	" Blue" ,
	" Green"
};
const char* Ltype[] = {

	" Aim",
	" Backtrack",
	" Trigger"

};
const char* skyboxchanger[] =
{
	"Off",
	"Ymaja",
	"Himalaya",
	"Mijtm",
	"Jungle",
	"New Jok",
	"Light",
	"Night",
	"Storm Light"
};
const char* TypeOfVis[] =
{
	" ESP" ,
	" Glow",
	" Chams"
};
const char* GlowTypeARR[] = {
	" Normal"," Skinny"," Pulsating"
};

const char* BoxTypeARR[] = {
	" Normal"," Corner"
};

const char* ChamsTypeARR[] = {
	"tvisible",
	"ctvisible",
	"colort",
	"colorct"
};

static char* sidebar_tabs[] = {
	"ESP",
	"AIM",
	"MISC",
	"CONFIG"
};

static ConVar* cl_mouseenable = nullptr;

constexpr static float get_sidebar_item_width() { return 129.0f; }
constexpr static float get_sidebar_item_height() { return  40.0f; }

ImGuiID Colorpicker_Close = 0;
__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }

void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true)
{
	ImGui::SameLine();
	bool switch_entity_teams = false;
	bool switch_color_vis = false;
	bool open_popup = ImGui::ColorButtonFloat(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(13, 0));

	if (open_popup) {
		ImGui::OpenPopup(picker_idname);
		Colorpicker_Close = ImGui::GetID(picker_idname);
	}

	if (ImGui::BeginPopup(picker_idname))
	{
		const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
		if (ImGui::Button(button_name0, ImVec2(-1, 0)))
			switch_entity_teams = !switch_entity_teams;

		const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
		if (ImGui::Button(button_name1, ImVec2(-1, 0)))
			switch_color_vis = !switch_color_vis;

		std::string id_new = picker_idname;
		id_new += "##pickeritself_";

		ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
		ImGui::EndPopup();
	}
}

void ColorPickerBox(const char* picker_idname, float col_n[], bool alpha = true)
{
	ImGui::SameLine();
	bool open_popup = ImGui::ColorButtonFloat(picker_idname, col_n, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(36, 0));
	if (open_popup)
	{
		ImGui::OpenPopup(picker_idname);
		Colorpicker_Close = ImGui::GetID(picker_idname);
	}

	if (ImGui::BeginPopup(picker_idname))
	{
		std::string id_new = picker_idname;
		id_new += "##pickeritself_";

		ImGui::ColorPicker4(id_new.c_str(), col_n, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
		ImGui::EndPopup();
	}
}

namespace ImGuiEx
{
	inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
	{
		auto clr = ImVec4{
			v->r() / 255.0f,
			v->g() / 255.0f,
			v->b() / 255.0f,
			v->a() / 255.0f
		};

		if (ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
			v->SetColor(clr.x, clr.y, clr.z, clr.w);
			return true;
		}
		return false;
	}
	inline bool ColorEdit3(const char* label, Color* v)
	{
		return ColorEdit4(label, v, false);
	}
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
	bool values[N] = { false };

	values[activetab] = true;

	for (auto i = 0; i < N; ++i) {
		if (ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
			activetab = i;
		}
		if (sameline && i < N - 1)
			ImGui::SameLine();
	}
}

ImVec2 get_sidebar_size()
{

	constexpr float padding = 10.0f;

	constexpr auto size_w = 0.0f;
	constexpr auto size_h = 56.0f;


	return ImVec2{ size_w, size_h };
}

void RenderEspTab()
{
	static char* esp_tab_names[] = { "ESP", "CHAMS", "GLOW" };
	static int   active_esp_tab = 0;

	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	{
		render_tabs(esp_tab_names, active_esp_tab, group_w / _countof(esp_tab_names), 25.0f, true);
	}

	if (active_esp_tab == 0) {
		ImGui::BeginGroupBox("##espmain");
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 3));
			ImGui::Checkbox("Enabled", &g_Options.esp_enabled); //ImGui::SameLine(); ImGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
			ImGui::Checkbox("Team check", &g_Options.esp_enemies_only);
			ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes); ImGui::SameLine(); ImGuiEx::ColorEdit3("##vis", &g_Options.color_esp_Box);
			ImGui::Combo("Box Type ", &g_Options.BoxType, BoxTypeARR, ARRAYSIZE(BoxTypeARR));
			ImGui::Checkbox("Eye Pos ESP", &g_Options.EyePosESP);
			ImGui::Checkbox("Health", &g_Options.esp_player_health);
			ImGui::Checkbox("Armour", &g_Options.esp_player_armour);
			ImGui::Checkbox("Skeleton", &g_Options.esp_player_Skel); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Skeleton", &g_Options.color_Skel);
			ImGui::Checkbox("KevESP", &g_Options.KevESP);
			ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weapons", &g_Options.color_esp_weapons);
			ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weaponsplayer", &g_Options.color_esp_playerweapons);
			ImGui::Checkbox("Names", &g_Options.esp_player_names); ImGui::SameLine(); ImGuiEx::ColorEdit3("##playername", &g_Options.color_esp_playername);
		}ImGui::PopStyleVar();
		ImGui::EndChild();
	}
	if (active_esp_tab == 1) {
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 3));
		ImGui::BeginGroupBox("##espmain");
		{
			ImGui::BeginChild("Chams", ImVec2(500, 300));
			{
				//ImGui::BeginChild("Chams", ImVec2(166, 200));
				//{
				ImGui::Checkbox("Enabled", &g_Options.chams_player_enabled);
				ImGui::Checkbox("E", &g_Options.theirchams); ImGui::SameLine(); ColorPickerBox("##E", g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, false);
				ImGui::Checkbox("F", &g_Options.yourteamchams); ImGui::SameLine(); ColorPickerBox("##T", g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, false);
				ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
				//ImGui::Checkbox("Team Check", &g_Options.chams_player_enemies_only); ImGui::SameLine(); ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Reg", &g_Options.chams_player_regular);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat", &g_Options.chams_player_flat);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("RegZ", &g_Options.chams_player_regular_IgnoreZ);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("FlatZ", &g_Options.chams_player_flat_IgnoreZ);
				ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("WireZ", &g_Options.chams_player_wireframe_IgnoreZ);
				//}
				//ImGui::EndChild();
				//ImGui::SameLine();
				//ImGui::BeginChild("Chams2", ImVec2(166, 200));
				//{
				//	ImGui::Checkbox("Enabled Arms", &g_Options.chams_arms_enabled);
				//	ImGuiEx::ColorEdit3("##Arms", &g_Options.color_chams_arms_visible);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe Arms", &g_Options.chams_arms_wireframe); //ImGui::SameLine(); ImGuiEx::ColorEdit4("!Vis", &g_Options.color_chams_arms_occluded);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat Arms ", &g_Options.chams_arms_flat);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Ignore-Z Arms", &g_Options.chams_arms_ignorez);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass Arms", &g_Options.chams_arms_glass);
				//}
				//ImGui::EndChild();
				//ImGui::SameLine();
				//ImGui::BeginChild("Chams3", ImVec2(166, 200));
				//{
				//	ImGui::Checkbox("Chams Weapon", &g_Options.chams_wep_enabled);
				//	ImGuiEx::ColorEdit3("##Colour", &g_Options.color_chams_wep_visible);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe Weapon", &g_Options.chams_wep_wireframe);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat Weapon", &g_Options.chams_wep_flat);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Ignore-Z Weapon ", &g_Options.chams_wep_ignorez);
				//	ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass Weapon", &g_Options.chams_wep_glass);
				//}
				//ImGui::EndChild();
			}		//ImGui::Columns(1, nullptr, false);
			ImGui::EndChild();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
	}
	if (active_esp_tab == 2)
	{
		ImGui::BeginGroupBox("##espmain");
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 3));
			ImGui::Checkbox("Enabled", &g_Options.glow_enabled); ImGui::SameLine();
			ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
			ImGui::Combo("Glow Type", &g_Options.GlowType, GlowTypeARR, ARRAYSIZE(GlowTypeARR));
			ImGui::Checkbox("Players", &g_Options.glow_players);
			ImGuiEx::ColorEdit3("E", &g_Options.color_glow_enemy);
			ImGuiEx::ColorEdit3("T", &g_Options.color_glow_ally);
			ImGui::Checkbox("Chickens", &g_Options.glow_chickens); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Chickens", &g_Options.color_glow_chickens);
			ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4 Carrier", &g_Options.color_glow_c4_carrier);
			ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4", &g_Options.color_glow_planted_c4);
			ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Defuse Kit", &g_Options.color_glow_defuse);
			ImGui::Checkbox("Weapons", &g_Options.glow_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weapons", &g_Options.color_glow_weapons);
			ImGui::EndChild();
		}ImGui::PopStyleVar();
	}
}

void RenderMiscTab()
{
	bool placeholder_true = true;

	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::ToggleButton("MISC", &placeholder_true, ImVec2{ group_w, 25.0f });
	ImGui::PopStyleVar();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 3));
	ImGui::BeginGroupBox("##espmain");
	{
		ImGui::Checkbox("Bunny hop", &g_Options.misc_bhop);
		ImGui::Checkbox("Third Person", &g_Options.misc_thirdperson);
		ImGui::SliderFloat("FOV", &g_Options.misc_thirdperson_dist, 0.f, 150.f);
		ImGui::Checkbox("Knife Left", &g_Options.misc_KnifeLeft);
		ImGui::SliderInt("FOV ", &g_Options.viewmodel_fov, 68, 120);
		//ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
		ImGui::Checkbox(("NightMode"), &g_Options.NightMode);
		if (ImGui::Button("Unload", ImVec2{ 150, 25 })) {
			g_Unload = true;
		}
	}ImGui::EndChild();
	ImGui::PopStyleVar();
}

void RenderEmptyTab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	bool placeholder_true = true;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 3));
	ImGui::BeginGroupBox("##espmain");
	{
		ImGui::Text("Still in Progress");
		ImGui::Text("Knife Model Changer");
		ImGui::Separator();
		ImGui::Combo("Knifes", &g_Options.knifemodel, knifeNames, ARRAYSIZE(knifeNames));
	}
	ImGui::EndChild();
	ImGui::PopStyleVar();
}

void Menu::Render()
{
	if (!_visible)
		return;

	ImGui_ImplDX9_NewFrame();
	ImGui::GetIO().MouseDrawCursor = _visible;

	const auto sidebar_size = get_sidebar_size();
	static int active_sidebar_tab = 0;

	//ImGui::PushStyle(_style);

	ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
	ImGui::SetNextWindowSize(ImVec2{ 550, 400 }, ImGuiSetCond_Once);


	if (ImGui::Begin("CSGOSimple",
		&_visible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar)) {

		//auto& style = ImGui::GetStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
		{
			ImGui::BeginGroupBox("##sidebar", sidebar_size);
			{
				render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), true);
			}
			ImGui::EndGroupBox();
		}
		ImGui::PopStyleVar();

		auto size = ImVec2{ 0.0f, 314.0f };

		ImGui::BeginGroupBox("##body", size);
		if (active_sidebar_tab == 0) {
			RenderEspTab();
		}
		else if (active_sidebar_tab == 1) {
			RenderEmptyTab();
		}
		else if (active_sidebar_tab == 2) {
			RenderMiscTab();
		}
		else if (active_sidebar_tab == 3) {
			RenderEmptyTab();
		}
		ImGui::EndGroupBox();

		ImGui::SameLine(ImGui::GetWindowWidth() - 150 - ImGui::GetStyle().WindowPadding.x);

		ImGui::End();
	}

	//ImGui::PopStyle();

	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::Initialize()
{
	_visible = true;

	cl_mouseenable = g_CVar->FindVar("cl_mouseenable");

	ImGui::CreateContext();

	ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);
}

void Menu::Shutdown()
{
	ImGui_ImplDX9_Shutdown();
	cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
	ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Show()
{
	_visible = true;
	cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
	_visible = false;
	cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
	_visible = !_visible;
}

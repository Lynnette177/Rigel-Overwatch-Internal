#pragma once
#include <d3d11.h>
#include "Fucking Imgui/imgui.h"
#include "Fucking Imgui/imgui_impl_dx11.h"
#include "Fucking Imgui/imgui_impl_win32.h"
#include "detours.h"
#include"Config.hpp"
int a = 0;
ImFont* ico = nullptr;
ImFont* ico234 = nullptr;
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "detours.lib")

using namespace OW;

namespace window
{
	//BOOL Visible = TRUE;

	void Initialize(ID3D11Device* g_pd3dDevice, ID3D11DeviceContext* g_pd3dDeviceContext, HWND hWnd)
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	}
	
	float colora, colorb, colorc;
	float jj1 = 1, jj2 = 0, jj3 = 1;
	float colora2, colorb2, colorc2;
	float jj12 = 1, jj22 = 0, jj32 = 1;
	float colora3, colorb3, colorc3;
	float jj13 = 1, jj23 = 0, jj33 = 1;
	float colora4, colorb4, colorc4;
	float jj14 = 1, jj24 = 0, jj34 = 1;
	float colora5, colorb5, colorc5;
	float jj15 = 1, jj25 = 0, jj35 = 1;
	int tab_index = 5;
	int subindex = 1;
	void Render()
	{

		if (!Config::Menu)
			return;
		if (!a) {
			a = 1;
			colora, colorb, colorc;
			colora = 40.f;
			colorb = 255.f;
			colorc = 0.f;
			jj1 = 1, jj2 = 0, jj3 = 1;

			colora2, colorb2, colorc2;
			colora2 = 70.f;
			colorb2 = 225.f;
			colorc2 = 30.f;
			jj12 = 1, jj22 = 0, jj32 = 1;

			colora3, colorb3, colorc3;
			colora3 = 100.f;
			colorb3 = 195.f;
			colorc3 = 60.f;
			jj13 = 1, jj23 = 0, jj33 = 1;

			colora4, colorb4, colorc4;
			colora4 = 130.f;
			colorb4 = 165.f;
			colorc4 = 90.f;
			jj14 = 1, jj24 = 0, jj34 = 1;

			colora5, colorb5, colorc5;
			colora5 = 160.f;
			colorb5 = 135.f;
			colorc5 = 120.f;
			jj15 = 1, jj25 = 0, jj35 = 1;
		}
#pragma region Styling
		
#pragma endregion
		ImGui::SetNextWindowPos(ImVec2(OW::WX/2.f - 550.f, OW::WY/2.f - 425.f));
		ImGui::SetNextWindowSize(ImVec2(1100, 850));
		ImGui::Begin(skCrypt(u8"Rigel Internal"), NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		//ImGui::PushFont(ico);
		//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora / 255.f, colorb / 255.f, colorc / 255.f, 255.f / 255.f));
		//ImGui::Text(skCrypt("              T"));
		//ImGui::PopStyleColor();
		//ImGui::SameLine();
		//ImGui::PopFont();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora4 / 255.f, colorb4 / 255.f, colorc4 / 255.f, 255.f / 255.f));
		ImGui::PushFont(ico234);
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Text(skCrypt("                    Rigel Internal"));
		ImGui::PopStyleColor();
		ImGui::PopFont();
		ImGui::PushFont(ico);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora / 255.f, colorb / 255.f, colorc / 255.f, 255.f / 255.f));
		ImGui::Text(skCrypt("    Q"));
		ImGui::PopStyleColor();
		if (jj1 == 0) colora -= 0.7;
		else colora += 0.7;
		if (colora <= 0) jj1 = 1;
		else if (colora >= 255) jj1 = 0;

		if (jj2 == 0) colorb -= 0.7;
		else colorb += 0.7;
		if (colorb <= 0) jj2 = 1;
		else if (colorb >= 255) jj2 = 0;

		if (jj3 == 0) colorc -= 0.7;
		else colorc += 0.7;
		if (colorc <= 0) jj3 = 1;
		else if (colorc >= 255) jj3 = 0;
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora2 / 255.f, colorb2 / 255.f, colorc2 / 255.f, 255.f / 255.f));
		ImGui::Text(skCrypt("       I"));
		ImGui::PopStyleColor();
		if (jj12 == 0) colora2 -= 0.7;
		else colora2 += 0.7;
		if (colora2 <= 0) jj12 = 1;
		else if (colora2 >= 255) jj12 = 0;

		if (jj22 == 0) colorb2 -= 0.7;
		else colorb2 += 0.7;
		if (colorb2 <= 0) jj22 = 1;
		else if (colorb2 >= 255) jj22 = 0;

		if (jj32 == 0) colorc2 -= 0.7;
		else colorc2 += 0.7;
		if (colorc2 <= 0) jj32 = 1;
		else if (colorc2 >= 255) jj32 = 0;
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora3 / 255.f, colorb3 / 255.f, colorc3 / 255.f, 255.f / 255.f));
		ImGui::Text(skCrypt("        N"));
		ImGui::PopStyleColor();
		if (jj13 == 0) colora3 -= 0.7;
		else colora3 += 0.7;
		if (colora3 <= 0) jj13 = 1;
		else if (colora3 >= 255) jj13 = 0;

		if (jj23 == 0) colorb3 -= 0.7;
		else colorb3 += 0.7;
		if (colorb3 <= 0) jj23 = 1;
		else if (colorb3 >= 255) jj23 = 0;

		if (jj33 == 0) colorc3 -= 0.7;
		else colorc3 += 0.7;
		if (colorc3 <= 0) jj33 = 1;
		else if (colorc3 >= 255) jj33 = 0;
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora4 / 255.f, colorb4 / 255.f, colorc4 / 255.f, 255.f / 255.f));
		ImGui::Text(skCrypt("        O"));
		ImGui::PopStyleColor();
		if (jj14 == 0) colora4 -= 0.7;
		else colora4 += 0.7;
		if (colora4 <= 0) jj14 = 1;
		else if (colora4 >= 255) jj14 = 0;

		if (jj24 == 0) colorb4 -= 0.7;
		else colorb4 += 0.7;
		if (colorb4 <= 0) jj24 = 1;
		else if (colorb4 >= 255) jj24 = 0;

		if (jj34 == 0) colorc4 -= 0.7;
		else colorc4 += 0.7;
		if (colorc4 <= 0) jj34 = 1;
		else if (colorc4 >= 255) jj34 = 0;
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora5 / 255.f, colorb5 / 255.f, colorc5 / 255.f, 255.f / 255.f));
		ImGui::Text(skCrypt("       J"));
		ImGui::PopStyleColor();
		if (jj15 == 0) colora5 -= 0.7;
		else colora5 += 0.7;
		if (colora5 <= 0) jj15 = 1;
		else if (colora5 >= 255) jj15 = 0;

		if (jj25 == 0) colorb5 -= 0.7;
		else colorb5 += 0.7;
		if (colorb5 <= 0) jj25 = 1;
		else if (colorb5 >= 255) jj25 = 0;

		if (jj35 == 0) colorc5 -= 0.7;
		else colorc5 += 0.7;
		if (colorc5 <= 0) jj35 = 1;
		else if (colorc5 >= 255) jj35 = 0;
		ImGui::PopFont();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		if (ImGui::Button(skCrypt(u8"��׼ѡ��"), ImVec2(170, 50)))
			tab_index = 1;
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		if (ImGui::Button(skCrypt(u8"����ѡ��"), ImVec2(170, 50)))
			tab_index = 2;
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		if (ImGui::Button(skCrypt(u8"��ŭѡ��"), ImVec2(170, 50)))
			tab_index = 3;
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		if (ImGui::Button(skCrypt(u8"�Ϸ�ѡ��"), ImVec2(170, 50)))
			tab_index = 4;
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();
		if (ImGui::Button(skCrypt(u8"����ѡ��"), ImVec2(170, 50)))
			tab_index = 5;

		ImGui::Separator();

		if (tab_index == 1) {
			ImGui::Spacing();
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(153.f / 255.f, 153.f / 255.f, 255.f / 255.f, 255.f / 255.f));
			ImGui::BulletText(Config::nowhero.data());
			ImGui::PopStyleColor(1);
			ImGui::Spacing();
			if (ImGui::Button(skCrypt(u8"����׼ѡ��"), ImVec2(100, 30)))
				subindex = 1;
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			if (ImGui::Button(skCrypt(u8"����׼ѡ��"), ImVec2(100, 30)))
				subindex = 2;
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			if (ImGui::Button(skCrypt(u8"AIM MISC"), ImVec2(100, 30)))
				subindex = 3;
			if (subindex == 1) {
				ImGui::Spacing();

				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
				ImGui::Toggle(skCrypt(u8"�������"), &Config::triggerbot, ImGuiToggleFlags_Animated);
				//ImGui::Checkbox(skCrypt(u8"�Զ����"), &Config::triggerbot);
				if (Config::triggerbot) {
					Config::Flick = false;
					Config::hanzo_flick = false;
					Config::Tracking = false;
					Config::silent = false;
				}

				//ImGui::Checkbox(skCrypt(u8"��ǹ����"), &Config::Tracking);
				ImGui::Toggle(skCrypt(u8"��ǹ����"), &Config::Tracking, ImGuiToggleFlags_Animated);
				if (Config::Tracking) {
					Config::Flick = false;
					Config::hanzo_flick = false;
					Config::triggerbot = false;
					Config::silent = false;
				}
				//ImGui::Checkbox(skCrypt(u8"˦ǹ����"), &Config::Flick);
				ImGui::Toggle(skCrypt(u8"˦ǹ����"), &Config::Flick, ImGuiToggleFlags_Animated);
				if (Config::Flick) {
					Config::Tracking = false;
					Config::hanzo_flick = false;
					Config::triggerbot = false;
					Config::silent = false;
				}
				//ImGui::Checkbox(skCrypt(u8"ȫ�ֻ���"), &Config::trackback);

				//ImGui::Checkbox(skCrypt(u8"����Ԥ��"), &Config::Prediction);
				ImGui::Toggle(skCrypt(u8"����Ԥ��"), &Config::Prediction, ImGuiToggleFlags_Animated);
				if (Config::Prediction) ImGui::Toggle(skCrypt(u8"����Ԥ��"), &Config::Gravitypredit, ImGuiToggleFlags_Animated);
				//ImGui::Checkbox(skCrypt(u8"����Ԥ��"), &Config::Gravitypredit);
				else Config::Gravitypredit = false;
				if (local_entity.HeroID == eHero::HERO_HANJO) {
					ImGui::Toggle(skCrypt(u8"Hanzo˦ǹ"), &Config::hanzo_flick, ImGuiToggleFlags_Animated);
					ImGui::Toggle(skCrypt(u8"Auto Calculate Speed"), &Config::hanzoautospeed, ImGuiToggleFlags_Animated);
				}
				else Config::hanzoautospeed = false;
				if (Config::hanzo_flick) {
					Config::Flick = false;
					Config::triggerbot = false;
					Config::Tracking = false;
					Config::silent = false;
				}
				//ImGui::Checkbox(skCrypt(u8"���˦��"), &Config::hanzo_flick);
				else Config::hanzo_flick = false;
				if (Config::hanzo_flick || Config::Prediction) {
					ImGui::Separator();
					ImGui::BulletText(skCrypt(u8"Ԥ�в�����"));
					ImGui::SliderFloat(skCrypt(u8"�ӵ������ٶ�"), &Config::predit_level, 0.f, 200.f, skCrypt("%.2f"));
				}
				if (Config::hanzo_flick || Config::Flick || Config::silent) {
					//ImGui::Checkbox(skCrypt(u8"��ס�Զ���ǹ"), &Config::AutoShoot);
					ImGui::Toggle(skCrypt(u8"��ס�Զ���ǹ"), &Config::AutoShoot, ImGuiToggleFlags_Animated);
					if (Config::AutoShoot) {
						ImGui::SliderInt(skCrypt(u8"��ǹ���"), &Config::Shoottime, 0, 1500, skCrypt("%.2f"));
					}
				}
				ImGui::PopStyleColor(1);
				ImGui::BulletText(skCrypt(u8"�󶨰���"));
				if (ImGui::BeginCombo(skCrypt(u8"##Key"), keys))
				{
					for (int i = 0; i < 5; i++)
					{
						const bool type = keys == key_type[i];
						if (ImGui::Selectable(key_type[i], type))
						{
							keys = key_type[i];
							switch (i)
							{
							case 0:
								Config::aim_key = VK_LBUTTON;
								break;
							case 1:
								Config::aim_key = VK_RBUTTON;
								break;
							case 2:
								Config::aim_key = VK_MBUTTON;
								break;
							case 3:
								Config::aim_key = VK_XBUTTON1;
								break;
							case 4:
								Config::aim_key = VK_XBUTTON2;
								break;
							}
						}
					}
					ImGui::EndCombo();
				}

				//ImGui::BulletText(skCrypt(u8"Fov"));
				//ImGui::SliderFloat(skCrypt(u8"Fov��ֵ"), &Config::Fov, 0.f, 500.f, skCrypt("%.3f"));

				//ImGui::SliderFloat(skCrypt(u8"HitBox��С����"), &Config::hitbox, 0.12f, 0.28f, skCrypt("%.3f"));
				//ImGui::SliderFloat(skCrypt(u8"HitBox��С����"), &Config::hitbox, 0.05f, 0.28f, skCrypt("%.3f"));
				//ImGui::SameLine(); Render::Help(skCrypt(u8"������ǹ��Χ������0.12-0.28������������0.3���ϣ���׼"));
				//ImGui::BulletText(skCrypt(u8"��׼�ٶ�"));
				//ImGui::SliderFloat(skCrypt(u8"��ǹ�ٶ�"), &Config::Tracking_smooth, 0.f, 3.f, skCrypt("%.2f"));
				//ImGui::SliderFloat(skCrypt(u8"˦ǹ�ٶ�"), &Config::Flick_smooth, 0.f, 3.f, skCrypt("%.2f"));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.f, 1.0f));
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
				ImGuiKnobs::Knob(skCrypt(u8"FOV(MIN)"), &Config::minFov1, 1.f, 500.f, 1.f, "%.1f", ImGuiKnobVariant_Space, 100.f);
				if (!Config::autoscalefov)
					Config::Fov = Config::minFov1;
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGuiKnobs::Knob(skCrypt(u8"HitBox"), &Config::hitbox, 0.05f, 0.28f, 0.0001f, "%.2f", ImGuiKnobVariant_Stepped, 100.f);
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				if (local_entity.HeroID != eHero::HERO_GENJI) {
					ImGuiKnobs::Knob(skCrypt(u8"��ǹ�ٶ�"), &Config::Tracking_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
				}
				else {
					ImGuiKnobs::Knob(skCrypt(u8"ת���ٶ�"), &Config::Tracking_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGuiKnobs::Knob(skCrypt(u8"�����ٶ�"), &Config::bladespeed, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
				}
				ImGuiKnobs::Knob(skCrypt(u8"˦ǹ�ٶ�"), &Config::Flick_smooth, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGuiKnobs::Knob(skCrypt(u8"˦ǹ����"), &Config::accvalue, 0.01f, 1.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
				ImGui::Separator();
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::PopStyleColor(1);
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
				ImGui::Toggle(skCrypt(u8"�Զ���׼�����λ"), &Config::autobone, ImGuiToggleFlags_Animated);
				ImGui::Toggle(skCrypt(u8"��Ҫ��׼����"), &Config::switch_team, ImGuiToggleFlags_Animated);
				if (!Config::autobone) {
					if (Config::Bone == 1) {
						Config::BoneName = skCrypt(u8"ͷ��");
					}
					else if (Config::Bone == 2) {
						Config::BoneName = skCrypt(u8"����");
					}
					else {
						Config::BoneName = skCrypt(u8"�ز�");
					}
					ImGui::Spacing();
					ImGui::SliderInt(Config::BoneName, &Config::Bone, 1, 3);
				}
				ImGui::PopStyleColor(1);
			}

			if (subindex == 2) {
				ImGui::BulletText(u8"SecondAim�����ھ�Ĭ�ͺϷ�");
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
				ImGui::Toggle(skCrypt(u8"��������׼ѡ��"), &Config::secondaim, ImGuiToggleFlags_Animated);
				ImGui::PopStyleColor(1);
				if (Config::secondaim) {
					//ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(153.f / 255.f, 153.f / 255.f, 255.f / 255.f, 255.f / 255.f));
					//ImGui::BulletText(Config::nowhero.data());
					//ImGui::PopStyleColor(1);
					//ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
					ImGui::Toggle(skCrypt(u8"High priority"), &Config::highPriority, ImGuiToggleFlags_Animated);
					ImGui::Toggle(skCrypt(u8"˦ǹ2"), &Config::Flick2, ImGuiToggleFlags_Animated);
					if (Config::Flick2) Config::Tracking2 = false;
					ImGui::Toggle(skCrypt(u8"��ǹ2"), &Config::Tracking2, ImGuiToggleFlags_Animated);
					if (Config::Tracking2) Config::Flick2 = false;
					ImGui::Toggle(skCrypt(u8"����Ԥ��2"), &Config::Prediction2, ImGuiToggleFlags_Animated);
					ImGui::Toggle(skCrypt(u8"����׼����"), &Config::switch_team2, ImGuiToggleFlags_Animated);

					if (Config::Prediction2) ImGui::Toggle(skCrypt(u8"����Ԥ��2"), &Config::Gravitypredit2, ImGuiToggleFlags_Animated);
					else Config::Gravitypredit2 = false;

					if (Config::Prediction2) {
						ImGui::Separator();
						ImGui::BulletText(skCrypt(u8"Ԥ�в���2��"));
						ImGui::SliderFloat(skCrypt(u8"�ӵ������ٶ�2"), &Config::predit_level2, 0.f, 200.f, skCrypt("%.2f"));
					}

					ImGui::PopStyleColor(1);
					ImGui::BulletText(skCrypt(u8"�󶨰���2"));

					if (ImGui::BeginCombo(skCrypt(u8"##Key2"), keys2))
					{
						for (int i = 0; i < 5; i++)
						{
							const bool type2 = keys2 == key_type2[i];
							if (ImGui::Selectable(key_type2[i], type2))
							{
								keys2 = key_type2[i];
								switch (i)
								{
								case 0:
									Config::aim_key2 = VK_LBUTTON;
									break;
								case 1:
									Config::aim_key2 = VK_RBUTTON;
									break;
								case 2:
									Config::aim_key2 = VK_MBUTTON;
									break;
								case 3:
									Config::aim_key2 = VK_XBUTTON1;
									break;
								case 4:
									Config::aim_key2 = VK_XBUTTON2;
									break;
								}
							}
						}
						ImGui::EndCombo();
					}

					ImGui::BulletText(skCrypt(u8"��������2"));

					if (ImGui::BeginCombo(skCrypt(u8"##FIREKey2"), keys3))
					{
						for (int i = 0; i < 5; i++)
						{
							const bool type3 = keys3 == key_type3[i];
							if (ImGui::Selectable(key_type3[i], type3))
							{
								keys3 = key_type3[i];
								switch (i)
								{
								case 0:
									Config::togglekey = 0;
									break;
								case 1:
									Config::togglekey = 1;
									break;
								case 2:
									Config::togglekey = 2;
									break;
								case 3:
									Config::togglekey = 3;
									break;
								case 4:
									Config::togglekey = 4;
									break;
								}
							}
						}
						ImGui::EndCombo();
					}

					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
					ImGui::Toggle(skCrypt(u8"Closest"), &Config::autobone2, ImGuiToggleFlags_Animated);
					ImGui::PopStyleColor(1);
					if (!Config::autobone2) {
						ImGui::Spacing();
						if (Config::Bone2 == 1) {
							Config::BoneName2 = skCrypt(u8"Head");
						}
						else if (Config::Bone2 == 2) {
							Config::BoneName2 = skCrypt(u8"Neck");
						}
						else {
							Config::BoneName2 = skCrypt(u8"Chest");
						}
						ImGui::SliderInt(Config::BoneName2, &Config::Bone2, 1, 3);
					}

					ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.6f, 1.f, 1.0f));
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();
					ImGuiKnobs::Knob(skCrypt(u8"FOV2(MIN)"), &Config::minFov2, 1.f, 500.f, 1.f, "%.1f", ImGuiKnobVariant_Space, 100.f);
					if(!Config::autoscalefov)
						Config::Fov2 = Config::minFov2;
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGuiKnobs::Knob(skCrypt(u8"HitBox2"), &Config::hitbox2, 0.05f, 0.28f, 0.0001f, "%.2f", ImGuiKnobVariant_Stepped, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGuiKnobs::Knob(skCrypt(u8"��ǹ�ٶ�2"), &Config::Tracking_smooth2, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGuiKnobs::Knob(skCrypt(u8"˦ǹ�ٶ�2"), &Config::Flick_smooth2, 0.f, 3.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGuiKnobs::Knob(skCrypt(u8"˦ǹ����2"), &Config::accvalue2, 0.01f, 1.f, 0.001f, "%.2f", ImGuiKnobVariant_Wiper, 100.f);
					ImGui::Separator();
					ImGui::Spacing();
					ImGui::SameLine();
					ImGui::PopStyleColor(1);
				}
			}
			if (subindex == 3) {
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
				ImGui::Toggle(skCrypt(u8"AI AIM(Beta)"), &Config::aiaim, ImGuiToggleFlags_Animated);
				ImGui::Toggle(skCrypt(u8"�Զ�FOV"), &Config::autoscalefov, ImGuiToggleFlags_Animated);
				if (!Config::autoscalefov) {
					Config::Fov = Config::minFov1;
					Config::Fov2 = Config::minFov2;
				}
				ImGui::Toggle(skCrypt(u8"����ͣ��"), &Config::lockontarget, ImGuiToggleFlags_Animated);
				ImGui::Toggle(skCrypt(u8"���ٲ���"), &Config::trackcompensate, ImGuiToggleFlags_Animated);
				ImGui::SliderFloat(skCrypt(u8"������Χ"), &Config::comarea, 0.f, 0.1f, skCrypt("%.3f"));
				ImGui::SliderFloat(skCrypt(u8"�����ٶ�"), &Config::comspeed, 0.f, 1.f, skCrypt("%.2f"));
				ImGui::Toggle(skCrypt(u8"��ֱ��������"), &Config::norecoil, ImGuiToggleFlags_Animated);
				ImGui::SliderFloat(skCrypt(u8"��ֱ����������"), &Config::recoilnum, 0.f, 1.f, skCrypt("%.2f"));
				ImGui::Toggle(skCrypt(u8"ˮƽ��������"), &Config::horizonreco, ImGuiToggleFlags_Animated);
				ImGui::PopStyleColor(1);
				if (local_entity.HeroID == eHero::HERO_GENJI) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
					//ImGui::Checkbox(skCrypt(u8"Դ���뵶"), &Config::GenjiBlade);
					ImGui::Toggle(skCrypt(u8"Դ���뵶"), &Config::GenjiBlade, ImGuiToggleFlags_Animated);
					//ImGui::Checkbox(skCrypt(u8"Դ���Զ�S"), &Config::AutoShiftGenji);
					ImGui::Toggle(skCrypt(u8"Դ���Զ�Dash"), &Config::AutoShiftGenji, ImGuiToggleFlags_Animated);
					ImGui::PopStyleColor(1);
				}
				else {
					Config::GenjiBlade = false;
					Config::AutoShiftGenji = false;
				}
				if (local_entity.HeroID == eHero::HERO_WIDOWMAKER) {
					ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
					ImGui::Toggle(skCrypt(u8"Auto Unscope"), &Config::widowautounscope, ImGuiToggleFlags_Animated);
					ImGui::PopStyleColor(1);
				}
				else Config::widowautounscope = false;
			}
		}
		if (tab_index == 2) {
			/*ImGui::Checkbox(skCrypt(u8"�����Ϣ"), &Config::draw_info);
			ImGui::Checkbox(skCrypt(u8"����͸��"), &Config::draw_skel);
			ImGui::Checkbox(skCrypt(u8"������Ϣ"), &Config::skillinfo);*/
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::Toggle(skCrypt(u8"�����Ϣ"), &Config::draw_info, ImGuiToggleFlags_Animated);
			if (Config::draw_info) {
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"BattleTag"), &Config::drawbattletag, ImGuiToggleFlags_Animated);
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"Ѫ����ֵ"), &Config::drawhealth, ImGuiToggleFlags_Animated);
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"Ѫ������"), &Config::healthbar, ImGuiToggleFlags_Animated);
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"�������"), &Config::dist, ImGuiToggleFlags_Animated);
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"���ƻ���"), &Config::name, ImGuiToggleFlags_Animated);
				ImGui::Spacing();
				ImGui::SameLine();
				ImGui::Toggle(skCrypt(u8"�ռ�����"), &Config::ult, ImGuiToggleFlags_Animated);
			}
			ImGui::Toggle(skCrypt(u8"RigelAICrossHair"), &Config::crosscircle, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"��������"), &Config::draw_skel, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"�״�"), &Config::radar, ImGuiToggleFlags_Animated);
			if (Config::radar) {
				ImGui::Toggle(skCrypt(u8"�״� ����"), &Config::radarline, ImGuiToggleFlags_Animated);
			}
			else Config::radarline = false;
			ImGui::Toggle(skCrypt(u8"ʵ����Ϣ"), &Config::skillinfo, ImGuiToggleFlags_Animated);
			ImGui::PopStyleColor(1);
			/*ImGui::Checkbox(skCrypt(u8"����͸��"), &Config::outline);
			if (Config::outline) {
				TCHAR buf[100];
				_stprintf(buf, _T("%d"), 1);
				WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
			}
			else {
					TCHAR buf[100];
					_stprintf(buf, _T("%d"), 0);
					WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
			}*/
			ImGui::Spacing();
			/*ImGui::BulletText(skCrypt(u8"����͸��ѡ��"));
			if (ImGui::BeginCombo(skCrypt(u8"ѡ��"), espop))
			{
				for (int i = 0; i < 4; i++)
				{
					const bool type = espop == espop_type[i];
					if (ImGui::Selectable(espop_type[i], type))
					{
						TCHAR buf[100];
						espop = espop_type[i];
						switch (i)
						{
						case 0:
							_stprintf(buf, _T("%d"), 0);
							WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
							break;
						case 1:
							_stprintf(buf, _T("%d"), 1);
							WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
							break;
						case 2:
							_stprintf(buf, _T("%d"), 2);
							WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
							break;
						case 3:
							_stprintf(buf, _T("%d"), 3);
							WritePrivateProfileString(_T("Section1"), _T("esp"), buf, _T("D:\\KEY.ini"));
							break;
						}
					}
				}
				ImGui::EndCombo();
			}ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Spacing();
			ImGui::Checkbox(skCrypt(u8"�ⲿ����͸��"), &Config::externaloutline);
			ImGui::Checkbox(skCrypt(u8"����͸��"), &Config::draw_edge);
			ImGui::Checkbox(skCrypt(u8"3D����͸��"), &Config::drawbox3d);
			ImGui::Checkbox(skCrypt(u8"Ŀ������"), &Config::drawline);
			ImGui::ColorEdit3(skCrypt(u8"������ɫ"), (float*)&Config::EnemyCol);
			ImGui::Checkbox(skCrypt(u8"��ʾFovԲȦ"), &Config::draw_fov);*/
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::Toggle(skCrypt(u8"��Ӱ͸��"), &Config::outline, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"HeadShot Line"), &Config::eyeray, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"����͸�ӵ���"), &Config::externaloutline, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"����͸�Ӷ���"), &Config::teamoutline, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"����������ɫ"), &Config::healthoutline, ImGuiToggleFlags_Animated);
			if (Config::healthoutline) Config::rainbowoutline = 0;
			ImGui::Toggle(skCrypt(u8"�����ʺ���ɫ"), &Config::rainbowoutline, ImGuiToggleFlags_Animated);
			if (Config::rainbowoutline) Config::healthoutline = 0;
			ImGui::Toggle(skCrypt(u8"Health Pack"), &Config::draw_hp_pack, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"�������"), &Config::draw_edge, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"3D�������"), &Config::drawbox3d, ImGuiToggleFlags_Animated);
			ImGui::Toggle(skCrypt(u8"���߻���"), &Config::drawline, ImGuiToggleFlags_Animated);
			ImGui::PopStyleColor(1);
			ImGui::ColorEdit3(skCrypt(u8"������ɫ"), (float*)&Config::EnemyCol);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::Toggle(skCrypt(u8"Fov����"), &Config::draw_fov, ImGuiToggleFlags_Animated);
			ImGui::PopStyleColor(1);
			ImGui::ColorEdit3(skCrypt(u8"FOVȦ��ɫ"), (float*)&Config::fovcol);
			ImGui::ColorEdit3(skCrypt(u8"FOV2Ȧ��ɫ"), (float*)&Config::fovcol2);
			ImGui::BulletText(skCrypt(u8"������ɫ"));
			ImGui::ColorEdit3(skCrypt(u8"���ɼ�������ɫ"), (float*)&Config::invisenargb);
			ImGui::SliderFloat(skCrypt(u8"���ɼ���������"), &Config::invisenargb.w, 0.0f, 1.f, skCrypt("%.2f"));
			ImGui::ColorEdit3(skCrypt(u8"�ɼ�������ɫ"), (float*)&Config::enargb);
			ImGui::SliderFloat(skCrypt(u8"�ɼ���������"), &Config::enargb.w, 0.0f, 1.f, skCrypt("%.2f"));
			ImGui::ColorEdit3(skCrypt(u8"��׼Ŀ����ɫ"), (float*)&Config::targetargb);
			ImGui::SliderFloat(skCrypt(u8"��׼Ŀ����ɫ����"), &Config::targetargb.w, 0.0f, 1.f, skCrypt("%.2f"));
			ImGui::ColorEdit3(skCrypt(u8"��׼Ŀ��2��ɫ"), (float*)&Config::targetargb2);
			ImGui::SliderFloat(skCrypt(u8"��׼Ŀ��2��ɫ����"), &Config::targetargb2.w, 0.0f, 1.f, skCrypt("%.2f"));
			ImGui::ColorEdit3(skCrypt(u8"����������ɫ"), (float*)&Config::allyargb);
			ImGui::SliderFloat(skCrypt(u8"������������"), &Config::allyargb.w, 0.0f, 1.f, skCrypt("%.2f"));
			ImGui::Separator();
		}
		if (tab_index == 3) {
			//ImGui::Checkbox(skCrypt(u8"��û����ҪӮ����ֻ�ǲ�����"), &Config::Rage);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::Toggle(skCrypt(u8"����"), &Config::trackback, ImGuiToggleFlags_Animated);
			ImGui::SliderFloat(skCrypt(u8"����ʱ��"), &Config::backtracksecond, 0.1f, 4.f, skCrypt("%.2f"));
			ImGui::Toggle(skCrypt(u8"������׼"), &Config::Rage, ImGuiToggleFlags_Animated);
			ImGui::SameLine(); Render::Help(skCrypt(u8"Aim Instantly"));
			if (Config::Rage) {
				ImGui::Toggle(skCrypt(u8"�پ�Ĭ"), &Config::fakesilent, ImGuiToggleFlags_Animated);
			}
			//ImGui::Checkbox(skCrypt(u8"360����׼"), &Config::fov360);
			ImGui::Toggle(skCrypt(u8"��FOV����"), &Config::fov360, ImGuiToggleFlags_Animated);
			/*ImGui::SameLine(); Render::Help(skCrypt(u8"Set fov to 6000"));
			if (Config::fov360) {
				Config::Fov = 6000;
				Config::Fov2 = 6000;
			}*/
			if (!Config::fov360 && Config::Fov == 6000) Config::Fov = 200;
			if (!Config::fov360 && Config::Fov2 == 6000) Config::Fov2 = 200;
			ImGui::BulletText(skCrypt(u8"Risk using silent aim"));
			ImGui::Toggle(skCrypt(u8"��Ĭ����"), &Config::silent, ImGuiToggleFlags_Animated);
			//ImGui::Checkbox(skCrypt(u8"��Ĭ����"), &Config::silent);
			ImGui::SameLine(); Render::Help(skCrypt(u8"Disable trackback at first"));
			if (Config::silent) {
				Config::Tracking = false;
				Config::hanzo_flick = false;
				Config::Flick = false;
				Config::triggerbot = false;
				Config::trackback = false;
				//ImGui::Checkbox(skCrypt(u8"AntiAim������AA"), &Config::Antiaim);
				ImGui::Toggle(skCrypt(u8"��ǹ��Ĭ��׼"), &Config::silenttrace, ImGuiToggleFlags_Animated);
				ImGui::Toggle(skCrypt(u8"AntiAim AA"), &Config::Antiaim, ImGuiToggleFlags_Animated);
				if (Config::Antiaim) {
					ImGui::Toggle(skCrypt(u8"Spinbot"), &Config::spinbot, ImGuiToggleFlags_Animated);
					ImGui::SameLine(); Render::Help(skCrypt(u8"Auto Spinning when using AA"));
				}
			}
			else {
				Config::silenttrace = false;
				Config::Antiaim = false;
				/*TCHAR buf[100];
				_stprintf(buf, _T("%d"), 0);
				WritePrivateProfileString(_T("Section1"), _T("antiaim"), buf, _T("D:\\KEY.ini"));*/
			}
			ImGui::PopStyleColor(1);
		}
		if (tab_index == 4) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::BulletText(skCrypt(u8"Play more legit"));
			//ImGui::Checkbox(skCrypt(u8"�Զ���׼�����λ"), &Config::autobone);
			//ImGui::Checkbox(skCrypt(u8"�Զ���ǹ(˦ǹ���ã�"), &Config::dontshot);

			ImGui::Toggle(skCrypt(u8"�Զ���ǹFLICK ONLY"), &Config::dontshot, ImGuiToggleFlags_Animated);

			ImGui::SliderFloat(skCrypt(u8"��ǹhitbox(missbox)"), &Config::missbox, 0.f, 1.f, skCrypt("%.2f"));
			ImGui::SliderInt(skCrypt(u8"ÿ��?ǹ��һǹ"), &Config::shotmanydont, 0, 6);
			ImGui::Spacing();
			//ImGui::Checkbox(skCrypt(u8"�л�Ŀ���ӳٺ���"), &Config::targetdelay);
			ImGui::Toggle(skCrypt(u8"�л�Ŀ���ӳٺ���"), &Config::targetdelay, ImGuiToggleFlags_Animated);

			ImGui::SliderInt(skCrypt(u8"��׼�ӳ�"), &Config::targetdelaytime, 0, 1000, skCrypt("%.2f"));
			//ImGui::Checkbox(skCrypt(u8"��׼��ʱ��ǹ"), &Config::hitboxdelayshoot);
			ImGui::Toggle(skCrypt(u8"��׼��ʱ��ǹ"), &Config::hitboxdelayshoot, ImGuiToggleFlags_Animated);

			ImGui::SliderInt(skCrypt(u8"����ʱ"), &Config::hiboxdelaytime, 0, 1000, skCrypt("%.2f"));
			ImGui::PopStyleColor(1);

		}
		if (tab_index == 5) {
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			//ImGui::Checkbox(skCrypt(u8"��׼����"), &Config::switch_team);

			//ImGui::Checkbox(skCrypt(u8"�Զ���ս"), &Config::AutoMelee);
			ImGui::Toggle(skCrypt(u8"�ı���Ϸ���ӽ�"), &Config::enablechangefov, ImGuiToggleFlags_Animated);
			if(Config::enablechangefov)
				ImGui::SliderFloat(skCrypt(u8"�ӽǸ���Ϊ������"), &Config::CHANGEFOV, 1.f, 179.f, skCrypt("%.2f"));
			ImGui::Toggle(skCrypt(u8"Name Spoofer"), &Config::namespoofer, ImGuiToggleFlags_Animated);
			if (Config::namespoofer) {
				ImGui::InputText(u8"Spoof Name", Config::fakename, sizeof(Config::fakename));
			}
			ImGui::Toggle(skCrypt(u8"�Զ���ս"), &Config::AutoMelee, ImGuiToggleFlags_Animated);
			ImGui::PopStyleColor(1);
			ImGui::SliderFloat(skCrypt(u8"�Զ���սѪ��"), &Config::meleehealth, 0.f, 80.f, skCrypt("%.2f"));
			ImGui::SliderFloat(skCrypt(u8"�Զ���ս����"), &Config::meleedistance, 0.f, 10.f, skCrypt("%.2f"));
			//ImGui::Checkbox(skCrypt(u8"�Զ�����"), &Config::AutoSkill);
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			ImGui::Toggle(skCrypt(u8"�Զ���������"), &Config::AutoSkill, ImGuiToggleFlags_Animated);
			ImGui::SliderFloat(skCrypt(u8"�Զ��������� Trigger HP"), &Config::SkillHealth, 0.f, 80.f, skCrypt("%.2f"));
			ImGui::PopStyleColor(1);
			
			if (ImGui::Button(u8"�ֶ��������"))
			{
				Config::manualsave = true;
				//ImGui::PushFont(font1);
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, skCrypt(u8"�ֶ���������ɹ�"), "" });
				//ImGui::PopFont();
			}
			ImGui::BulletText(skCrypt(u8"Press when nothing is working"));
			if (ImGui::Button(u8"����")) {
				Config::doingentity = 0;
				Sleep(1000);
				abletotread = 0;
				Config::doingentity = 1;
				_beginthread((_beginthread_proc_type)entity_scan_thread, 0, 0);
				Sleep(51);
				_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
				Sleep(51);
				ImGui::InsertNotification({ ImGuiToastType_Success, 3000, skCrypt(u8"�����ɹ�"), "" });
			}

			//ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.f, 0.f, 0.f, 1.0f));
			//ImGui::Toggle(skCrypt(u8"��������"), &Config::musicplay, ImGuiToggleFlags_Animated);

			//ImGui::PopStyleColor(1);
			//nowtime = time(nullptr);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(colora5 / 255.f, colorb5 / 255.f, colorc5 / 255.f, 255.f / 255.f));
			//ImGui::Text(u8"ʣ��ʱ�䣺%d��%dСʱ%d����%d��", (expiretime - nowtime) / 60 / 60 / 24, ((expiretime - nowtime) / 60 / 60) % 24, ((expiretime - nowtime) / 60) % 60, (expiretime - nowtime) % 60);
			//if (Config::loginornot && (expiretime - nowtime) < 0) abort();
			ImGui::Text(u8"��ǰFPS: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::PopStyleColor();
			ImGui::End();
		}
	}
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace menu
{
	DWORD64 m_ulOldWndProc;
	LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	bool Initialize();
	void CreateRenderTarget(IDXGISwapChain* pSwapChain);
	void CleanupRenderTarget();


	typedef HRESULT(__stdcall* fn_D3D11CreateDeviceAndSwapChain_t)(IDXGIAdapter* pAdapter, D3D_DRIVER_TYPE, HMODULE, UINT, D3D_FEATURE_LEVEL*, UINT, UINT, DXGI_SWAP_CHAIN_DESC*, IDXGISwapChain**, ID3D11Device**, D3D_FEATURE_LEVEL*, ID3D11DeviceContext**);

	typedef HRESULT(__stdcall* D3D11PresentHook) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	typedef HRESULT(__stdcall* D3D11ResizeBuffersHook) (IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);

	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
	HRESULT __stdcall hookD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags);


	DWORD Width, Height;

	ID3D11Device* pDevice = NULL;
	ID3D11DeviceContext* pContext = NULL;

	bool firstTime = true;
	D3D11PresentHook phookD3D11Present = NULL;
	D3D11ResizeBuffersHook phookD3D11ResizeBuffer = NULL;
	ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	DWORD LastPressedKey = 0;

	HRESULT __stdcall hkD3D11Present(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
	{
		if (firstTime)
		{
			firstTime = false;

			pSwapChain->GetDevice(__uuidof(pDevice), (PVOID*)&pDevice);
			pDevice->GetImmediateContext(&pContext);

			HWND owWindow = FindWindowA("TankWindowClass", NULL);

			window::Initialize(pDevice, pContext, owWindow);

			m_ulOldWndProc = SetWindowLongPtrA(owWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

			CreateRenderTarget(pSwapChain);
		}

		pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.f);
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(30.f / 255.f, 102.f / 255.f, 150.f / 255.f, 70.f / 255.f));
		ImGui::RenderNotifications();
		ImGui::PopStyleVar(1); // Don't forget to Pop()
		ImGui::PopStyleColor(1);



		window::Render();

		viewMatrix = SDK->RPM<Matrix>(viewMatrixPtr);
		viewMatrix_xor = SDK->RPM<Matrix>(viewMatrix_xor_ptr);


		if (entities.size() > 0)
			esp();
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		return phookD3D11Present(pSwapChain, SyncInterval, Flags);

	}

	void CleanupRenderTarget()
	{
		if (nullptr != g_mainRenderTargetView)
		{
			g_mainRenderTargetView->Release();
			g_mainRenderTargetView = nullptr;
		}
	}

	HRESULT __stdcall hookD3D11ResizeBuffers(IDXGISwapChain* pSwapChain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags)
	{
		ImGui_ImplDX11_InvalidateDeviceObjects();
		CleanupRenderTarget();
		HRESULT toReturn = phookD3D11ResizeBuffer(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
		CreateRenderTarget(pSwapChain);
		ImGui_ImplDX11_CreateDeviceObjects();

		return toReturn;
	}

	void CreateRenderTarget(IDXGISwapChain* pSwapChain)
	{
		DXGI_SWAP_CHAIN_DESC sd;
		pSwapChain->GetDesc(&sd);
		// Create the render target
		ID3D11Texture2D* pBackBuffer;
		D3D11_RENDER_TARGET_VIEW_DESC render_target_view_desc;
		ZeroMemory(&render_target_view_desc, sizeof(render_target_view_desc));
		render_target_view_desc.Format = sd.BufferDesc.Format;
		render_target_view_desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&pBackBuffer);
		pDevice->CreateRenderTargetView(pBackBuffer, &render_target_view_desc, &g_mainRenderTargetView);
		pContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);

		// GetBufferSize
		D3D11_TEXTURE2D_DESC desc;
		pBackBuffer->GetDesc(&desc);
		Width = desc.Width;
		Height = desc.Height;

		pBackBuffer->Release();
	}

	bool Initialize()
	{
		ImGui::CreateContext();
		ImGuiStyle* style = &ImGui::GetStyle();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImFontConfig font_cfg;
		font_cfg.FontDataOwnedByAtlas = false;
		io.Fonts->AddFontFromMemoryTTF((void*)SimHei_data, sizeof(SimHei_data), 24.f, &font_cfg, io.Fonts->GetGlyphRangesChineseFull());
		ImGui::MergeIconsWithLatestFont(25.f, false);
		io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 18.f, &font_cfg);
		ImGui::MergeIconsWithLatestFont(25.f, false);
		if (ico == nullptr)
			ico = io.Fonts->AddFontFromMemoryTTF(&icon, sizeof icon, 65.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		if (ico234 == nullptr)
			ico234 = io.Fonts->AddFontFromMemoryTTF((void*)tahoma, sizeof(tahoma), 65.f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		ImCandy::Theme_Blender();
		//ImCandy::Theme_Nord();
		IDXGISwapChain* pSwapChain;
		HWND hWnd = FindWindowA("TankWindowClass", NULL);

		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = hWnd;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = TRUE; //((GetWindowLong(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		fn_D3D11CreateDeviceAndSwapChain_t D3D11CreateDeviceAndSwapChain_t = (fn_D3D11CreateDeviceAndSwapChain_t)(GetProcAddress(GetModuleHandleA("d3d11.dll"), "D3D11CreateDeviceAndSwapChain"));

		if (FAILED(D3D11CreateDeviceAndSwapChain_t(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, &featureLevel, 1
			, D3D11_SDK_VERSION, &swapChainDesc, &pSwapChain, &pDevice, NULL, &pContext)))
		{
			MessageBox(hWnd, "Failed to create D3D11 device and DXGISwapChain!", "Error", MB_ICONERROR);
			return false;
		}

		DWORD_PTR* pSwapChainVtable = (DWORD64*)pSwapChain;
		pSwapChainVtable = (DWORD64*)pSwapChainVtable[0];
		// ��ȡ���������麯����

		phookD3D11Present = (D3D11PresentHook)(DWORD64*)pSwapChainVtable[8];
		phookD3D11ResizeBuffer = (D3D11ResizeBuffersHook)(DWORD64*)pSwapChainVtable[13];
		// ���� D3D11Present �� D3D11ResizeBuffer ����
		pSwapChain->Release();
		pDevice->Release();
		pContext->Release();
		// �ͷŽ��������豸��������
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		// ʹ�� Detours ��ܽ��Զ���� Hook �������ӵ� D3D11Present �� D3D11ResizeBuffer
		if (DetourAttach(&(LPVOID&)phookD3D11Present, hkD3D11Present) != NO_ERROR)
			MessageBox(hWnd, "Failed to Detours", "Error", MB_ICONERROR);
		if (DetourAttach(&(LPVOID&)phookD3D11ResizeBuffer, hookD3D11ResizeBuffers) != NO_ERROR)
			MessageBox(hWnd, "Failed to Detours", "Error", MB_ICONERROR);
		DetourTransactionCommit();


		return true;
	}

	bool Uninitialize()
	{
		HWND hWnd = FindWindowA("TankWindowClass", NULL);

		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		if (DetourDetach(&(PVOID&)phookD3D11Present, (PBYTE)hkD3D11Present) != NO_ERROR)
			MessageBox(hWnd, "Failed to Detours", "Error", MB_ICONERROR);
		if (DetourDetach(&(PVOID&)phookD3D11ResizeBuffer, (PBYTE)hookD3D11ResizeBuffers) != NO_ERROR)
			MessageBox(hWnd, "Failed to Detours", "Error", MB_ICONERROR);
		DetourTransactionCommit();
		return true;
	}


	LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
		case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
		{
			switch (wParam)
			{
			case MK_MBUTTON:
				LastPressedKey = VK_MBUTTON;
				break;
			case MK_XBUTTON1:
				LastPressedKey = VK_XBUTTON1;
				break;
			case MK_XBUTTON2:
				LastPressedKey = VK_XBUTTON2;
				break;
			default:
				LastPressedKey = wParam;
				break;
			}
			break;
		}
		case WM_KEYDOWN:
		{
			LastPressedKey = wParam;
			break;
		}
		}


		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return TRUE;


		return CallWindowProcA((WNDPROC)m_ulOldWndProc, hWnd, msg, wParam, lParam);
	}
}

#pragma once
#include "Target.hpp"
#include"imgui_toggle.h"
#include"candy.h"
#include "imgui_notify.h"
#include "tahoma.h" // <-- Required font!
#include "simhei.h"
#include "imgui-knobs.h"
#include "USEVEH.h"
#include "icon.h"

#define M_PI        3.14159265358979323846f
#define M_RADPI        57.295779513082f
#define M_PI_F        ((float)(M_PI))
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define ANGLE2SHORT(x) ((int)((x)*65536/360) & 65535)
#define SHORT2ANGLE(x)    ((x)*(360.0/65536))
#include <windows.h>
#include <vfw.h>
#include<ctime>
#pragma comment(lib,"vfw32.lib")

time_t expiretime;
time_t nowtime;

float WX, WY;

std::mutex mutex;

namespace OW {


	inline void configsavenloadthread();
	inline void aimbot_thread();
	int abletotread = 0;
	int howbigentitysize;
	inline void entity_scan_thread() {
		while (Config::doingentity == 1) {
			if (abletotread == 0) {
				ow_entities_scan = get_ow_entities();
				abletotread = 1;
			}
			Sleep(5);
		}
	}

	int entitytime = 0;
	int jumpentity = 0;
	Vector3 lastpos = { 0, 0, 0 };
	inline void entity_thread() {
		while (Config::doingentity == 1) {
			if (entitytime == 0) entitytime = GetTickCount();
			if (GetTickCount() - entitytime >= 100 && abletotread)
			{
				mutex.lock();
				ow_entities = ow_entities_scan;
				abletotread = 0;
				entitytime = GetTickCount();
				mutex.unlock();
				//std::cout << ow_entities_scan.size();
			}
			if (!(ow_entities.size() > 0)) {
				mutex.lock();
				//abletotread = 0;
				entities = {};
				hp_dy_entities = {};
				mutex.unlock();
				//entities = {};
				//local_entity.AngleBase=0;
				//entitytime = GetTickCount();
				Sleep(1000);
				continue;
			}
			//mutex.unlock();
			std::vector<c_entity> tmp_entities{};
			std::vector<hpanddy> hpdy_entities{};
			//int howbigentitysize = ow_entities.size();
			//if (howbigentitysize > 30) howbigentitysize = 30;
			c_entity lastentity{};
			for (int i = 0; i < ow_entities.size(); i++) {
				c_entity entity{};
				if (!ow_entities[i].first || !ow_entities[i].second) continue;
				if (i > ow_entities.size()) continue;
				const auto& [ComponentParent, LinkParent] = ow_entities[i];//注意这里的&
				entity.address = ComponentParent;
				if (!entity.address) continue;
				if (!LinkParent) continue;

				uint64_t Ptr = SDK->RPM<uint64_t>(ComponentParent + 0x30) & 0xFFFFFFFFFFFFFFC0;
				if (Ptr < 0xFFFFFFFFFFFFFFEF) {
					uint64_t EntityID = SDK->RPM<uint64_t>(Ptr + 0x10);
					if (EntityID == 0x400000000000060 || EntityID == 0x40000000000480A || EntityID == 0x40000000000005F || EntityID == 0x400000000002533) {
						hpanddy hpdyentity{};
						hpdyentity.entityid = EntityID;
						hpdyentity.MeshBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
						hpdyentity.POS = SDK->RPM<XMFLOAT3>(hpdyentity.MeshBase + 0x380 + 0x50);
						hpdy_entities.push_back(hpdyentity);
						continue;
					}
				}



				entity.HealthBase = DecryptComponent(ComponentParent, TYPE_HEALTH);
				entity.LinkBase = DecryptComponent(LinkParent, TYPE_LINK);
				entity.TeamBase = DecryptComponent(ComponentParent, TYPE_TEAM);
				entity.VelocityBase = DecryptComponent(ComponentParent, TYPE_VELOCITY);
				entity.HeroBase = DecryptComponent(LinkParent, TYPE_P_HEROID);
				entity.OutlineBase = DecryptComponent(ComponentParent, TYPE_OUTLINE);
				entity.BoneBase = DecryptComponent(ComponentParent, TYPE_BONE);
				entity.RotationBase = DecryptComponent(ComponentParent, TYPE_ROTATION);
				entity.SkillBase = DecryptComponent(ComponentParent, TYPE_SKILL);
				entity.VisBase = DecryptComponent(LinkParent, TYPE_P_VISIBILITY);
				entity.AngleBase = DecryptComponent(LinkParent, TYPE_PLAYERCONTROLLER);
				entity.EnemyAngleBase = DecryptComponent(ComponentParent, TYPE_ANGLE);

				health_compo_t health_compo{};
				velocity_compo_t velo_compo{};
				hero_compo_t hero_compo{};
				vis_compo_t vis_compo{};

				//printf("%llx\n", SDK->RPM<uint16_t>(ComponentParent + 0x48));
				if (entity == lastentity) continue;
				else lastentity = entity;

				if (entity.HealthBase) {
					health_compo = SDK->RPM<health_compo_t>(entity.HealthBase);
					Vector2 healthext = SDK->RPM<Vector2>(entity.HealthBase + 0xF0);
					entity.PlayerHealth = health_compo.health + health_compo.armor + health_compo.barrier + healthext.Y;
					entity.PlayerHealthMax = health_compo.health_max + health_compo.armor_max + health_compo.barrier_max + healthext.X;
					entity.MinHealth = health_compo.health;
					entity.MaxHealth = health_compo.health_max;
					entity.MinArmorHealth = health_compo.armor;
					entity.MaxArmorHealth = health_compo.armor_max;
					entity.MinBarrierHealth = health_compo.barrier;
					entity.MaxBarrierHealth = health_compo.barrier_max;
					entity.Alive = (entity.PlayerHealth > 0.f) ? true : false;
					entity.imort = health_compo.isImmortal;
					entity.barrprot = health_compo.isBarrierProjected;
					//printf("health %f,health address:%llx\n", entity.PlayerHealth, entity.HealthBase);
				}
				else continue;
				if (entity.RotationBase) {
					entity.Rot = SDK->RPM<Vector3>(SDK->RPM<uint64_t>(entity.RotationBase + 0x7C0 + 0x10) + 0x8FC);
				}
				if (entity.VelocityBase) {
					velo_compo = SDK->RPM<velocity_compo_t>(entity.VelocityBase);
					entity.pos = Vector3(velo_compo.location.x, velo_compo.location.y - 1.f, velo_compo.location.z); // ㅋㅋ 
					entity.velocity = Vector3(velo_compo.velocity.x, velo_compo.velocity.y, velo_compo.velocity.z);
					int head_index = entity.GetSkel()[0];
					int neck_index = entity.GetSkel()[1];
					int chest_index = entity.GetSkel()[2];
					entity.head_pos = entity.GetBonePos(head_index);
					//这里弄高了0.02
					entity.head_pos.Y += 0.02;
					//if (entity.head_pos == lastpos) continue;
					//else lastpos = entity.head_pos;
					entity.neck_pos = entity.GetBonePos(neck_index);
					entity.chest_pos = entity.GetBonePos(chest_index);
				}
				if (entity.HeroBase) {
					hero_compo = SDK->RPM<hero_compo_t>(entity.HeroBase);
					entity.HeroID = hero_compo.heroid;
					if (entity.HeroID == eHero::HERO_WRECKINGBALL) {
						int head_index = entity.GetSkel()[0];
						int neck_index = entity.GetSkel()[1];
						int chest_index = entity.GetSkel()[2];
						entity.head_pos = entity.GetBonePos(head_index);
						//这里弄高了0.02
						entity.head_pos.Y += 0.02;
						//if (entity.head_pos == lastpos) continue;
						//else lastpos = entity.head_pos;
						entity.neck_pos = entity.GetBonePos(neck_index);
						entity.chest_pos = entity.GetBonePos(chest_index);
					}
					if (entity.HeroID == eHero::HERO_DVA && GetHeroEngNames(entity.HeroID, entity.LinkBase) != u8"Hana") {
						entity.imort = false;
						entity.head_pos.Y -= 0.1;
						entity.chest_pos = entity.neck_pos;
						entity.chest_pos.Y -= 0.3;
					}
					if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4 || entity.HeroID == eHero::HERO_TRAININGBOT5 || entity.HeroID == eHero::HERO_TRAININGBOT6 || entity.HeroID == eHero::HERO_TRAININGBOT7)
						entity.chest_pos = entity.GetBonePos(83);
				}
				else {
					if (entity.MaxHealth == 225) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y + 1, temppos.z);
						entity.HeroID = 0x16dd;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 30) {
						XMFLOAT3 temppos = SDK->RPM<XMFLOAT3>(entity.VelocityBase + 0x380 + 0x50);
						entity.head_pos = Vector3(temppos.x, temppos.y, temppos.z);
						entity.HeroID = 0x16ee;
						entity.neck_pos = entity.head_pos;
						entity.chest_pos = entity.head_pos;
						entity.pos = entity.neck_pos;
					}
					else if (entity.MaxHealth == 1000) {
						entity.HeroID = 0x16bb;
					}
					else continue;
				}
				if (Config::draw_info && Config::drawbattletag) {
					entity.statcombase = DecryptComponent(LinkParent, TYPE_STAT);
					if (entity.statcombase) {
						/*DWORD_PTR statcom = entity.statcombase;
						char result[MAX_PATH] = "";
						memcpy_s(result, MAX_PATH, (char*)SDK->RPM<uint64_t>(statcom + 0xe0), MAX_PATH);
						//SDK->read_buf(SDK->RPM<uint64_t>(statcom + 0xe0), result, MAX_PATH);
						std::stringstream ss;
						ss << result;
						entity.battletag = ss.str();*/
						//entity.battletag = result;
						////entity.battletag = std::string((const char*)SDK->RPM<uint64_t>(statcom + 0xe0));
						char buffer[64] = u8"";
						if (entity != local_entity) {
							uintptr_t off = SDK->RPM<uintptr_t>(entity.statcombase + 0xE0);
							SDK->read_buf(off, buffer, sizeof(char) * 64);
							entity.battletag = buffer;
						}
						//std::cout << buffer << std::endl;
						//std::cout << entity.battletag << std::endl;
					}
				}
				if (entity.TeamBase) {
					entity.Team = (entity.GetTeam() == eTeam::TEAM_DEATHMATCH || entity.GetTeam() != local_entity.GetTeam()) ? true : false;

				}
				if (entity.VisBase) {
					vis_compo = SDK->RPM<vis_compo_t>(entity.VisBase);
					entity.Vis = (DecryptVis(vis_compo.key1) ^ vis_compo.key2) ? true : false;
					//printf("%llx\n", SDK->RPM<uint64_t>(entity.VisBase + 0xa0));
				}
				if (entity.SkillBase) {
					entity.SkillBase += 0x50;
					entity.skill1act = IsSkillActive(entity.SkillBase, 0, 0x28E3);
					entity.skill2act = IsSkillActive(entity.SkillBase, 0, 0x28E9);
					entity.ultimate = readult(entity.SkillBase, 0, 0x1e32);
					if (entity.HeroID == eHero::HERO_SOMBRA && entity.Team && !Config::Rage && !Config::fov360 && !Config::silent && !Config::fakesilent) {
						entity.Vis = (entity.Vis && !IsSkillActivate1(entity.SkillBase, 0, 0x7C5));
					}
					//entity.skillcd1 = readskillcd(entity.SkillBase, 0, 0x189c);
					//entity.skillcd2 = readskillcd(entity.SkillBase, 0, 0x1f89);
					//std::cout << entity.skillcd1 << " " << entity.skillcd2 << std::endl;

					//if (entity.skill1act) {
					//	std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " shift on.\n";
						//ImGui::GetForegroundDrawList()->AddText(ImVec2(WX, WY/2), ImGui::ColorConvertFloat4ToU32(ImVec4(255 / 255.0, 255 / 255.0, 255 / 255.0, 255 / 255.0)), GetHeroNames(entity.HeroID, entity.LinkBase).c_str());

					//}
					//if (entity.skill2act) {
					//	std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " E on.\n";
					//}
					//if (entity.skill1act|| entity.skill2act) std::cout << 1 << std::endl;
					//if (IsSkillActive(entity.SkillBase, 0, 0x4BF))std::cout<<1<<std::endl;
					//else {} //std::cout << 0 << std::endl;
					//std::cout << ow_entities.size() << std::endl;
				}

				if (entity.OutlineBase)
				{
					if (Config::externaloutline && !entity.Vis)	SetBorderLine(0x2, entity.OutlineBase);
					else SetBorderLine(0x1, entity.OutlineBase);
					if (Config::externaloutline) {
						if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && !Config::rainbowoutline) {
							//printf("%llx\n",SDK->RPM<uint32_t>(entity.OutlineBase + 0x130) );
							if (entity.Vis) {
								Config::visenemy = convertToHex(Config::enargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::visenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::visenemy);
							}
							else if (i != Config::Targetenemyi) {
								Config::invisenemy = convertToHex(Config::invisenargb);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::invisenemy);
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::invisenemy);
							}
						}
						else if (entity.Team && i != Config::Targetenemyi && Config::healthoutline && !Config::rainbowoutline) {
							if (entity.PlayerHealth == entity.MaxHealth) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(0.2, 1, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth >= 0.3 && entity.PlayerHealth / entity.MaxHealth < 0.7) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0.8, 0.2, 1)));
							}
							else if (entity.PlayerHealth / entity.MaxHealth > 0 && entity.PlayerHealth / entity.MaxHealth < 0.3) {
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(ImVec4(1, 0, 0.2, 1)));
								SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(ImVec4(1, 0, 0.2, 1)));
							}
						}
						else if (entity.Team && i != Config::Targetenemyi && !Config::healthoutline && Config::rainbowoutline) {
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, convertToHex(Config::rainbowargb));
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, convertToHex(Config::rainbowargb));
							if (Config::cps1 == 0) Config::rainbowargb.x += 0.0002;
							else if (Config::cps1 == 1) Config::rainbowargb.x -= 0.0003;
							if (Config::cps2 == 0) Config::rainbowargb.y += 0.0005;
							else if (Config::cps2 == 1) Config::rainbowargb.y -= 0.0002;
							if (Config::cps3 == 0) Config::rainbowargb.z += 0.0003;
							else if (Config::cps3 == 1) Config::rainbowargb.z -= 0.0005;
							if (Config::rainbowargb.x <= 0) Config::cps1 = 0;
							else if (Config::rainbowargb.x >= 1) Config::cps1 = 1;
							if (Config::rainbowargb.y <= 0) Config::cps2 = 0;
							else if (Config::rainbowargb.y >= 1) Config::cps2 = 1;
							if (Config::rainbowargb.z <= 0) Config::cps3 = 0;
							else if (Config::rainbowargb.z >= 1) Config::cps3 = 1;
						}
						if (!entity.Team) {
							Config::Allycolor = convertToHex(Config::allyargb);
							//printf("%llx\n", Config::Allycolor);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
							SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);

						}
					}
					else if (Config::teamoutline && !entity.Team) {
						if (!entity.Vis) SetBorderLine(0x2, entity.OutlineBase);
						else SetBorderLine(0x1, entity.OutlineBase);
						Config::Allycolor = convertToHex(Config::allyargb);
						//printf("%llx\n", Config::Allycolor);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x130, Config::Allycolor);
						SDK->WPM<uint32_t>(entity.OutlineBase + 0x144, Config::Allycolor);
					}
				}


				//const auto angle_component = DecryptComponent(LinkParent, TYPE_PLAYERCONTROLLER);
				if (entity.AngleBase)
				{
					float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(entity.head_pos);
					if (dist <= 1 && GetHeroEngNames(entity.HeroID, entity.LinkBase) != skCrypt(u8"Unknown").decrypt()) {
						entity.skillcd1 = readskillcd(entity.SkillBase, 0, 0x189c);
						//std::cout << entity.skillcd1 << std::endl;
						//entity.skillready= cdready(entity.SkillBase, 0, 0x189c);
						//if (!readyornot) printf("yes\n");
						//else printf("no\n");
						entity.skillcd2 = readskillcd(entity.SkillBase, 0, 0x1f89);
						local_entity = entity;
						Config::reloading = IsSkillActivate1(local_entity.SkillBase, 0, 0x4BF);
						/*bool intshoot = IsSkillActivate1(local_entity.SkillBase, 11, 0xad);
						if (intshoot) std::cout << "1\n";
						else std::cout << "0\n";*/

						//char buffer[] = u8"猎户座用户";
						if (Config::namespoofer) {
							local_entity.statcombase = DecryptComponent(LinkParent, TYPE_STAT);
							uintptr_t off = SDK->RPM<uintptr_t>(local_entity.statcombase + 0xE0);
							SDK->write_buf(off, Config::fakename, sizeof(Config::fakename));
						}

						//std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " shift CD:" << SkillCD(entity.SkillBase, 0, 0x189C) << "\n";
						//std::cout << GetHeroNames(entity.HeroID, entity.LinkBase).c_str() << " E CD." << SkillCD(entity.SkillBase, 0, 0x1F89) << "\n";
						SDK->g_player_controller = entity.AngleBase;
						//printf("%llx\n", SDK->g_player_controller);
						if (local_entity.GetTeam() == eTeam::TEAM_DEATHMATCH) entity.Team = false;
						//	printf("%f\n", SDK->RPM<float>(SDK->g_player_controller + 0x1760));
						//float spectuurtu = SDK->RPM<float>(SDK->g_player_controller + 0x178C);
						//SDK->WPM<float>(SDK->g_player_controller + 0x178C, -100);
					}
					//printf("d4a %d\n", SDK->RPM<uint16>(SDK->g_player_controller + 0xd4a));
					//printf("d4c %d\n", SDK->RPM<uint16>(SDK->g_player_controller + 0xd4c));
				}
				if (ComponentParent && LinkParent && GetHeroEngNames(entity.HeroID, entity.LinkBase) != skCrypt(u8"Unknown").decrypt())
					tmp_entities.push_back(entity);
			}
			mutex.lock();
			entities = tmp_entities;
			hp_dy_entities = hpdy_entities;
			mutex.unlock();
			Sleep(3);
		}
	}

	inline void viewmatrix_thread() {
		__try {
			while (true) {
				//mutex.lock();
				auto viewMatrixVal = SDK->RPM<uint64_t>(SDK->dwGameBase + offset::Address_viewmatrix_base) ^ offset::offset_viewmatrix_xor_key;
				Vector2 WindowSize = SDK->RPM<Vector2>(viewMatrixVal + 0x41C);
				//mutex.unlock();

				static RECT TempRect = { NULL };
				static POINT TempPoint;
				GetClientRect(SDK->Windowsizehd, &TempRect);
				ClientToScreen(SDK->Windowsizehd, &TempPoint);
				TempRect.left = TempPoint.x;
				TempRect.top = TempPoint.y;
				WX = TempRect.right;
				WY = TempRect.bottom;



				//WX = WindowSize.X;
				//WY = WindowSize.Y;
				//mutex.lock();
				//viewMatrix = SDK->RPM<Matrix>(viewMatrixPtr);
				//viewMatrix_xor = SDK->RPM<Matrix>(viewMatrix_xor_ptr);
				//printf("vm:\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", viewMatrix.m11, viewMatrix.m12, viewMatrix.m13, viewMatrix.m14, viewMatrix.m21, viewMatrix.m22, viewMatrix.m23, viewMatrix.m24, viewMatrix.m31, viewMatrix.m32, viewMatrix.m33, viewMatrix.m34, viewMatrix.m41, viewMatrix.m42, viewMatrix.m43, viewMatrix.m44);
				//printf("vmxor:\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n", viewMatrix_xor.m11, viewMatrix_xor.m12, viewMatrix_xor.m13, viewMatrix_xor.m14, viewMatrix_xor.m21, viewMatrix_xor.m22, viewMatrix_xor.m23, viewMatrix_xor.m24, viewMatrix_xor.m31, viewMatrix_xor.m32, viewMatrix_xor.m33, viewMatrix_xor.m34, viewMatrix_xor.m41, viewMatrix_xor.m42, viewMatrix_xor.m43, viewMatrix_xor.m44);
				//mutex.unlock();
				Sleep(5);
			}
		}
		__except (1) {
		}
	}

	inline void skillinfo() {
		if (entities.size() > 0) {
			//mutex.lock();
			int i = 10;
			//for (const c_entity &entity : entities)//传常量引用
			for (c_entity entity : entities) {
				std::string heroname = GetHeroEngNames(entity.HeroID, entity.LinkBase).c_str();
				if (entity.Team && heroname != u8"Bot" && heroname != u8"Unknown" && entity.HeroID != 0x16dd && entity.HeroID != 0x16ee && entity.HeroID != 0x16bb) {
					float xpos = WX;
					float ypos = WY + i;
					std::string skillstring;
					float ult = entity.ultimate;
					//float cd1 = entity.skillcd1;
					//float cd2 = entity.skillcd2;
					skillstring = skCrypt(u8"Enemy:").decrypt() + heroname + skCrypt(u8" Ult:").decrypt() + std::to_string((int)ult);
					//if (entity.skillcd1 != 0 && entity.skillcd2 != 0)
						//skillstring = skCrypt(u8"敌方：").decrypt()+ heroname +skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1 cd：").decrypt() + std::to_string((int)cd1) + skCrypt(u8" 技能2 cd：").decrypt() + std::to_string((int)cd2);
					//else if (entity.skillcd1 == 0 && entity.skillcd2 != 0)
						//skillstring = skCrypt(u8"敌方：").decrypt() + heroname +skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1可用").decrypt() + skCrypt(u8" 技能2 cd：").decrypt() + std::to_string((int)cd2);
					//else if (entity.skillcd1 != 0 && entity.skillcd2 == 0)
						//skillstring = skCrypt(u8"敌方：").decrypt() + heroname +skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1 cd：").decrypt() + std::to_string((int)cd1) + skCrypt(u8" 技能2可用").decrypt();
					//else if (entity.skillcd1 == 0 && entity.skillcd2 == 0)
						//skillstring = skCrypt(u8"敌方：").decrypt() + heroname +skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1可用").decrypt() + skCrypt(u8" 技能2可用").decrypt();
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
				else if (entity.Team && (entity.HeroID == 0x16dd || entity.HeroID == 0x16ee || entity.HeroID == 0x16bb)) {
					std::string skillstring;
					skillstring = skCrypt(u8"Enemy Entity:").decrypt() + heroname + skCrypt(u8" HP：").decrypt() + std::to_string((int)entity.PlayerHealth) + skCrypt(u8"/").decrypt() + std::to_string((int)entity.MaxHealth);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
			}
			i += 60;
			for (c_entity entity : entities) {//传常量引用
				std::string heroname = GetHeroEngNames(entity.HeroID, entity.LinkBase).c_str();
				if (!entity.Team && heroname != u8"Bot" && heroname != u8"Unknown" && entity.HeroID != 0x16dd && entity.HeroID != 0x16ee && entity.HeroID != 0x16bb) {
					float xpos = WX;
					float ypos = WY + i;
					std::string skillstring;
					float ult = entity.ultimate;
					//float cd1 = entity.skillcd1;
					//float cd2 = entity.skillcd2;
					skillstring = skCrypt(u8"Ally:").decrypt() + heroname + skCrypt(u8" Ult:").decrypt() + std::to_string((int)ult);
					//if (entity.skillcd1 != 0 && entity.skillcd2 != 0)
					//	skillstring = skCrypt(u8"己方：").decrypt()+ heroname + skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1 cd：").decrypt() + std::to_string((int)cd1) + skCrypt(u8" 技能2 cd：").decrypt() + std::to_string((int)cd2);
					//else if (entity.skillcd1 == 0 && entity.skillcd2 != 0)
					//	skillstring = skCrypt(u8"己方：").decrypt() + heroname + skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1可用").decrypt() + skCrypt(u8" 技能2 cd：").decrypt() + std::to_string((int)cd2);
					//else if (entity.skillcd1 != 0 && entity.skillcd2 == 0)
					//	skillstring = skCrypt(u8"己方：").decrypt() + heroname + skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1 cd：").decrypt() + std::to_string((int)cd1) + skCrypt(u8" 技能2可用").decrypt();
					//else if (entity.skillcd1 == 0 && entity.skillcd2 == 0)
					//	skillstring = skCrypt(u8"己方：").decrypt() + heroname + skCrypt(u8" 终极技能：").decrypt() + std::to_string((int)ult) + skCrypt(u8"% 技能1可用").decrypt() + skCrypt(u8" 技能2可用").decrypt();
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
				else if (!entity.Team && (entity.HeroID == 0x16dd || entity.HeroID == 0x16ee || entity.HeroID == 0x16bb)) {
					std::string skillstring;
					skillstring = skCrypt(u8"Ally entity:").decrypt() + heroname + skCrypt(u8" HP:").decrypt() + std::to_string((int)entity.PlayerHealth) + skCrypt(u8"/").decrypt() + std::to_string((int)entity.MaxHealth);
					Render::DrawSKILL(ImVec2(70, 30 + i), skillstring);
					i += 20;
				}
			}
			//mutex.unlock();
		}
	}


	inline void Draw_Skel() {
		if (entities.size() > 0) {
			//mutex.lock();
			for (c_entity entity : entities) {
				if (entity.HeroID == 0x16dd) continue;
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0) {
					espBone a = entity.getBoneList(entity.GetSkel());
					DWORD Col = entity.Vis ? Color{ 0, 0, 0, 0 }.RGBA2ARGB(255) : Color{ 0, 0, 0, 0 }.RGBA2ARGB(255);
					DWORD Col2 = entity.Vis ? Color{ 0, 255, 0, 200 }.RGBA2ARGB(255) : Color{ 255, 0, 0, 200 }.RGBA2ARGB(255);
					Vector2 outPos{};
					if (!a.boneerror) {
						if (viewMatrix.WorldToScreen(entity.head_pos, &outPos, Vector2(WX, WY))) {
							if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4) {
								Render::RenderLine(a.head, a.neck, Col, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.l_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.r_1, Col, 2.f);

								Render::RenderLine(a.head, a.neck, Col2, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.l_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.r_1, Col2, 2.f);
							}
							else {
								Render::RenderLine(a.head, a.neck, Col, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col, 2.f);
								Render::RenderLine(a.body_1, a.body_2, Col, 2.f);
								Render::RenderLine(a.neck, a.l_1, Col, 2.f);
								Render::RenderLine(a.neck, a.r_1, Col, 2.f);
								Render::RenderLine(a.l_1, a.l_d_1, Col, 2.f);
								Render::RenderLine(a.r_1, a.r_d_1, Col, 2.f);
								Render::RenderLine(a.l_d_1, a.l_d_2, Col, 2.f);
								Render::RenderLine(a.r_d_1, a.r_d_2, Col, 2.f);
								Render::RenderLine(a.body_2, a.l_a_1, Col, 2.f);
								Render::RenderLine(a.body_2, a.r_a_1, Col, 2.f);
								Render::RenderLine(a.l_a_1, a.l_a_2, Col, 2.f);
								Render::RenderLine(a.r_a_1, a.r_a_2, Col, 4.f);

								Render::RenderLine(a.r_a_2, a.sex, Col, 2.f);
								Render::RenderLine(a.l_a_2, a.sex1, Col, 2.f);
								Render::RenderLine(a.sex, a.sex2, Col, 2.f);
								Render::RenderLine(a.sex1, a.sex3, Col, 2.f);

								Render::RenderLine(a.head, a.neck, Col2, 2.f);
								Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
								Render::RenderLine(a.body_1, a.body_2, Col2, 2.f);
								Render::RenderLine(a.neck, a.l_1, Col2, 2.f);
								Render::RenderLine(a.neck, a.r_1, Col2, 2.f);
								Render::RenderLine(a.l_1, a.l_d_1, Col2, 2.f);
								Render::RenderLine(a.r_1, a.r_d_1, Col2, 2.f);
								Render::RenderLine(a.l_d_1, a.l_d_2, Col2, 2.f);
								Render::RenderLine(a.r_d_1, a.r_d_2, Col2, 2.f);
								Render::RenderLine(a.body_2, a.l_a_1, Col2, 2.f);
								Render::RenderLine(a.body_2, a.r_a_1, Col2, 2.f);
								Render::RenderLine(a.l_a_1, a.l_a_2, Col2, 2.f);
								Render::RenderLine(a.r_a_1, a.r_a_2, Col2, 2.f);

								Render::RenderLine(a.r_a_2, a.sex, Col2, 2.f);
								Render::RenderLine(a.l_a_2, a.sex1, Col2, 2.f);
								Render::RenderLine(a.sex, a.sex2, Col2, 2.f);
								Render::RenderLine(a.sex1, a.sex3, Col2, 2.f);
							}
						}
					}
				}
			}
			//mutex.unlock();
		}
	}
	inline void draw3dbox() {

		if (entities.size() > 0)
		{
			//mutex.lock();
			//for (const c_entity &entity : entities)//传常量引用
			for (c_entity entity : entities) {
				if (entity.HeroID == 0x16dd) continue;
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0)
				{
					Vector3 Vec3 = entity.head_pos;
					Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{}, Vec2_G{}, Vec2_H{};
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y + 0.3f, Vec3.Z - 0.5f), &Vec2_A, Vector2(WX, WY)))
						continue;

					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y + 0.3f, Vec3.Z + 0.5f), &Vec2_B, Vector2(WX, WY)))
						continue;
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y + 0.3f, Vec3.Z - 0.5f), &Vec2_C, Vector2(WX, WY)))
						continue;

					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y + 0.3f, Vec3.Z + 0.5f), &Vec2_D, Vector2(WX, WY)))
						continue;
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y - 1.2f, Vec3.Z - 0.5f), &Vec2_E, Vector2(WX, WY)))
						continue;

					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X - 0.5f, Vec3.Y - 1.2f, Vec3.Z + 0.5f), &Vec2_F, Vector2(WX, WY)))
						continue;
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y - 1.2f, Vec3.Z - 0.5f), &Vec2_G, Vector2(WX, WY)))
						continue;

					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X + 0.5f, Vec3.Y - 1.2f, Vec3.Z + 0.5f), &Vec2_H, Vector2(WX, WY)))
						continue;

					float height = abs(Vec2_A.Y - Vec2_B.Y);
					float width = height * 0.85;
					float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
					float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
					float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
					int num7 = (float)(1500 / (int)Height2);
					float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
					float ypos = Vec2_A.Y + Size / 5;


					Render::DrawLine(Vec2_A, Vec2_B, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_B, Vec2_D, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_D, Vec2_C, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_C, Vec2_A, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_A, Vec2_E, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_B, Vec2_F, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_C, Vec2_G, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_D, Vec2_H, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_E, Vec2_F, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_F, Vec2_H, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_H, Vec2_G, Color(255, 255, 255, 255), 2.f);
					Render::DrawLine(Vec2_G, Vec2_E, Color(255, 255, 255, 255), 2.f);
				}
			}
			//mutex.unlock();
		}

	}

	inline void drawline() {
		if (entities.size() > 0)
		{
			//mutex.lock();
			//for (const c_entity &entity : entities)//传常量引用
			for (c_entity entity : entities)
			{
				if (entity.HeroID == 0x16dd) continue;
				if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0)
				{
					Vector3 Vec3 = entity.head_pos;
					Vector2 Vec2_A{};
					if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
						continue;

					Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(160, 218, 255, 150), 2.f);

				}
			}
			//mutex.unlock();
		}
	}

	/*inline void esp() {
		__try
		{
			ImDrawList* Draw = ImGui::GetBackgroundDrawList();
			ImVec2 CrossHair = ImVec2(WX / 2.0f, WY / 2.0f);
			//std::cout << WX;
			if (Config::draw_info)
				PlayerInfo();

			if (Config::draw_skel)
				Draw_Skel();

			if (Config::skillinfo) {
				skillinfo();
			}

			if (Config::drawbox3d) {
				draw3dbox();
			}

			if (Config::draw_edge)
			{
				if (entities.size() > 0)
				{
					//mutex.lock();
					//for (const c_entity &entity : entities)//传常量引用
					for (c_entity entity : entities)
					{
						if (entity.HeroID == 0x16dd) continue;
						if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0)
						{
							Vector3 Vec3 = entity.head_pos;
							Vector2 Vec2_A{}, Vec2_B{};
							if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 0.1f, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
								continue;

							if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.8f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
								continue;

							float height = abs(Vec2_A.Y - Vec2_B.Y);
							float width = height * 0.85;
							float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
							float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
							float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
							int num7 = (float)(1500 / (int)Height2);
							float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
							float ypos = Vec2_A.Y + Size / 5;

							Render::DrawCorneredBox(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImGui::GetColorU32(Config::EnemyCol));
							Render::DrawFilledRect(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImColor(0, 0, 0, 60));
						}
					}
					//mutex.unlock();
				}
			}

			if (Config::drawline)
			{
				drawline();
			}
			if (Config::draw_fov)
			{
				Draw->AddCircle(CrossHair, Config::Fov, ImGui::GetColorU32(Config::fovcol), 3000);
			}
		}
		__except (1) {

		}
	}*/
	inline void esp() {
		ImDrawList* Draw = ImGui::GetForegroundDrawList();
		ImVec2 CrossHair = ImVec2(WX / 2.0f, WY / 2.0f);
		//std::cout << WX;
		if (Config::draw_hp_pack) {
			for (hpanddy hppack : hp_dy_entities) {
				if (hppack.entityid == 0x400000000002533) continue;
				Vector2 resultvec1{}, resultvec2{}, resultvec3{}, resultvec4{}, resultvec5{}, resultvec6{}, resultvec7{}, resultvec8{}, stringresult{};
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x, hppack.POS.y + 0.5f, hppack.POS.z), &stringresult, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec1, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec1, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y, hppack.POS.z + 0.5f), &resultvec2, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y, hppack.POS.z - 0.5f), &resultvec3, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y, hppack.POS.z + 0.5f), &resultvec4, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y + 1.f, hppack.POS.z - 0.5f), &resultvec5, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x - 0.5f, hppack.POS.y + 1.f, hppack.POS.z + 0.5f), &resultvec6, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y + 1.f, hppack.POS.z - 0.5f), &resultvec7, Vector2(WX, WY)))
					continue;
				if (!viewMatrix.WorldToScreen(Vector3(hppack.POS.x + 0.5f, hppack.POS.y + 1.f, hppack.POS.z + 0.5f), &resultvec8, Vector2(WX, WY)))
					continue;
				float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(Vector3(hppack.POS.x, hppack.POS.y, hppack.POS.z));
				Render::DrawStrokeText(ImVec2(stringresult.X - 5.f, stringresult.Y - 8.f), ImGui::GetColorU32(ImVec4(1, 0, 0.2f, 1)), (skCrypt(u8"HP PACK").decrypt()), 16.f);
				Render::DrawStrokeText(ImVec2(stringresult.X - 5.f, stringresult.Y + 8.f), ImGui::GetColorU32(ImVec4(1, 0, 0.2f, 1)), (skCrypt(u8"[DIST：").decrypt() + std::to_string((int)dist) + skCrypt(u8"]").decrypt()).c_str(), 16.f);
				Render::DrawLine(resultvec1, resultvec2, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec1, resultvec3, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec2, resultvec4, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec3, resultvec4, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec5, resultvec6, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec5, resultvec7, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec6, resultvec8, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec7, resultvec8, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec1, resultvec5, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec2, resultvec6, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec3, resultvec7, Color(102, 153, 255, 100), 2.f);
				Render::DrawLine(resultvec4, resultvec8, Color(102, 153, 255, 100), 2.f);
			}
		}
		if ((Config::draw_info || Config::draw_skel || Config::skillinfo || Config::drawbox3d || Config::draw_edge || Config::drawline || Config::radar || Config::eyeray || Config::crosscircle) && entities.size() > 0) {
			int drawradar = 0;
			for (c_entity entity : entities)
			{
				/*for (int i = 0; i < 150; i++) {
					Vector3 Vec3bone = entity.GetBonePos(i);
					Vector2 Vec2bone{};
					if (!viewMatrix.WorldToScreen(Vector3(Vec3bone.X, Vec3bone.Y, Vec3bone.Z), &Vec2bone, Vector2(WX, WY))) continue;
					Render::DrawStrokeText(ImVec2(Vec2bone.X, Vec2bone.Y), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), std::to_string((int)i).c_str(), 10);
				}*/
				if (Config::radar) {
					if (!drawradar) {
						Draw->AddCircleFilled(ImVec2(WX - 200, WY - 400), 200, ImU32(IM_COL32(0, 0, 0, 120)));
						float fovline = 0;
						if (Config::enablechangefov) {
							fovline = Config::CHANGEFOV;
						}
						else {
							fovline = SDK->RPM<float>(SDK->dwGameBase + offset::changefov);
						}
						fovline /= 2;
						float liney = 200 * cos(DEG2RAD(fovline));
						float linex = sqrt(200 * 200 - liney * liney);
						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200 - linex, WY - 400 - liney), Color(153, 102, 255, 255), 2.f);
						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200 + linex, WY - 400 - liney), Color(153, 102, 255, 255), 2.f);

						Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(WX - 200, WY - 400 - 200), Color(255, 255, 0, 120), 1.f);

						drawradar = 1;
					}
					if (entity == local_entity) {
						Draw->AddCircleFilled(ImVec2(WX - 200, WY - 400), 4.f, ImU32(IM_COL32(255, 255, 51, 255)));
						continue;
					}
					if (entity.pos != Vector3(0, 0, 0) && entity.PlayerHealth > 0)
					{
						DirectX::XMFLOAT3 Result{};
						XMMATRIX rotMatrix = XMMatrixRotationY(local_entity.Rot.X/* - atan2(this->Rot.Z, this->Rot.X)*/);
						XMMATRIX inverseRotMatrix = XMMatrixInverse(nullptr, rotMatrix);

						// 要转换的世界坐标点或向量
						DirectX::XMFLOAT3 worldPoint = { entity.head_pos.X - viewMatrix_xor.get_location().x,entity.head_pos.Y - viewMatrix_xor.get_location().y,entity.head_pos.Z - viewMatrix_xor.get_location().z }; // 世界坐标点或向量
						worldPoint.x *= 2.5;
						worldPoint.y *= 2.5;
						worldPoint.z *= 2.5;
						// 使用逆矩阵将世界坐标点或向量转换到角色坐标系
						DirectX::XMVECTOR worldVector = XMLoadFloat3(&worldPoint);
						DirectX::XMVECTOR characterVector = XMVector3Transform(worldVector, inverseRotMatrix);

						// 将结果存储到一个 DirectX::XMFLOAT3 结构中
						DirectX::XMFLOAT3 characterPoint;
						XMStoreFloat3(&characterPoint, characterVector);
						float therad = sqrt(characterPoint.x * characterPoint.x + characterPoint.z * characterPoint.z);
						if (therad > 200) {
							characterPoint.x = (characterPoint.x / therad) * 200;
							characterPoint.z = (characterPoint.z / therad) * 200;
						}
						ImVec2 pointPosition(WX - 200 - characterPoint.x, WY - 400 - characterPoint.z);
						// 定义点的半径
						float pointRadius = 3.4f;


						//视线
						Vector3 whereiseye;
						Vector3 whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4;
						entity.GetEyeRayPoint(whereiseye, whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4);
						DirectX::XMFLOAT3 eyePoint = { (whereiseye.X - entity.head_pos.X) * 4 + whereiseye.X - viewMatrix_xor.get_location().x,whereiseye.Y - viewMatrix_xor.get_location().y,(whereiseye.Z - entity.head_pos.Z) * 4 + whereiseye.Z - viewMatrix_xor.get_location().z }; // 世界坐标点或向量
						eyePoint.x *= 2.5;
						eyePoint.y *= 2.5;
						eyePoint.z *= 2.5;
						// 使用逆矩阵将世界坐标点或向量转换到角色坐标系
						DirectX::XMVECTOR theeyeworldVector = XMLoadFloat3(&eyePoint);
						DirectX::XMVECTOR theeyeVector = XMVector3Transform(theeyeworldVector, inverseRotMatrix);

						// 将结果存储到一个 DirectX::XMFLOAT3 结构中
						DirectX::XMFLOAT3 theeyePoint;
						XMStoreFloat3(&theeyePoint, theeyeVector);
						float theradforeye = sqrt(theeyePoint.x * theeyePoint.x + theeyePoint.z * theeyePoint.z);
						if (theradforeye > 195) {
							theeyePoint.x = (theeyePoint.x / theradforeye) * 195;
							theeyePoint.z = (theeyePoint.z / theradforeye) * 195;
						}
						ImVec2 eyepointPosition(WX - 200 - theeyePoint.x, WY - 400 - theeyePoint.z);


						// 定义点的颜色（RGBA）
						ImU32 pointColor;
						if (entity.Vis)
							pointColor = IM_COL32(255, 0, 51, 255);
						else pointColor = IM_COL32(204, 0, 153, 255);

						// 使用绘图列表的AddCircle函数来添加一个点
						if (!entity.Team) {
							Draw->AddCircleFilled(pointPosition, pointRadius, IM_COL32(102, 255, 255, 255));
						}
						else {
							Draw->AddCircleFilled(pointPosition, pointRadius, pointColor);
							Render::DrawLine(Vector2(eyepointPosition.x, eyepointPosition.y), Vector2(pointPosition.x, pointPosition.y), Color(255, 0, 0, 255), 2.2f);
							if (Config::radarline) {
								if (entity.Vis)
									Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(pointPosition.x, pointPosition.y), Color(160, 218, 255, 150), 1.f);
								else
									Render::DrawLine(Vector2(WX - 200, WY - 400), Vector2(pointPosition.x, pointPosition.y), Color(204, 102, 102, 150), 1.f);
							}
						}
					}
				}
				if (Config::draw_info) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0) {
						Vector3 Vec3 = entity.head_pos;
						float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(Vec3);
						Vector2 Vec2_A{}, Vec2_B{};
						Vector2 Vec2_C{}, Vec2_D{};
						Vector2 Vec2_E{}, Vec2_F{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.5f, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 1.f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
							continue;
						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;
						Size /= 2;
						if (Size < 16) Size = 16;
						if (Size > 20) Size = 20;
						if (Config::drawbattletag) Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_B.Y - Size), ImGui::GetColorU32(ImVec4(0, 0.9, 1, 1)), (skCrypt(u8"[ID：").decrypt() + entity.battletag + skCrypt(u8"]").decrypt()).c_str(), Size);
						//if (Config::healthbar) Render::DrawHealthBar(Vector2(Vec2_A.X - width / 2, Vec2_A.Y), -Height2, entity.PlayerHealth, entity.PlayerHealthMax);
						if (Config::healthbar) Render::DrawSeerLikeHealth(Vec2_B.X, Vec2_B.Y - 30.f, (int)(1.25f * entity.ultimate), 125, (int)entity.PlayerHealth, (int)entity.PlayerHealthMax);

						if (Config::drawhealth)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), (skCrypt(u8"[HP：").decrypt() + std::to_string((int)entity.PlayerHealth) + skCrypt(u8"]").decrypt()).c_str(), Size);
						if (Config::ult)Render::DrawStrokeText(ImVec2(Vec2_A.X - width / 6, Vec2_A.Y + Size), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), (skCrypt(u8"[ULT：").decrypt() + std::to_string((int)entity.ultimate) + skCrypt(u8"]").decrypt()).c_str(), Size);
						if (Config::dist)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6, Vec2_B.Y), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), (skCrypt(u8"[DIST：").decrypt() + std::to_string((int)dist) + skCrypt(u8"]").decrypt()).c_str(), Size);
						if (Config::name)Render::DrawStrokeText(ImVec2(Vec2_B.X - width / 6, Vec2_B.Y + Size), ImGui::GetColorU32(ImVec4(0.9, 0.9, 1, 1)), (skCrypt(u8"[NAME：").decrypt() + GetHeroEngNames(entity.HeroID, entity.LinkBase) + skCrypt(u8"]").decrypt()).c_str(), Size);
					}
				}
				if (Config::draw_skel) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd) {
						espBone a = entity.getBoneList(entity.GetSkel());
						DWORD Col = entity.Vis ? Color{ 0, 0, 0, 0 }.RGBA2ARGB(255) : Color{ 0, 0, 0, 0 }.RGBA2ARGB(255);
						DWORD Col2 = entity.Vis ? Color{ 0, 255, 0, 200 }.RGBA2ARGB(255) : Color{ 255, 0, 0, 200 }.RGBA2ARGB(255);
						ImU32 pointcolor = IM_COL32(102, 153, 255, 255);
						float radisofpoint = 2.f;
						Vector2 outPos{};
						if (!a.boneerror) {
							if (viewMatrix.WorldToScreen(entity.head_pos, &outPos, Vector2(WX, WY))) {
								if (entity.HeroID == eHero::HERO_TRAININGBOT1 || entity.HeroID == eHero::HERO_TRAININGBOT2 || entity.HeroID == eHero::HERO_TRAININGBOT3 || entity.HeroID == eHero::HERO_TRAININGBOT4) {
									Render::RenderLine(a.head, a.neck, Col, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.l_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.r_1, Col, 2.f);

									Render::RenderLine(a.head, a.neck, Col2, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.l_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.r_1, Col2, 2.f);
									Draw->AddCircleFilled(ImVec2(a.head.X, a.head.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_1.X, a.body_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.neck.X, a.neck.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_1.X, a.l_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_1.X, a.r_1.Y), radisofpoint, pointcolor);
								}
								else {
									Render::RenderLine(a.head, a.neck, Col, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col, 2.f);
									Render::RenderLine(a.body_1, a.body_2, Col, 2.f);
									Render::RenderLine(a.neck, a.l_1, Col, 2.f);
									Render::RenderLine(a.neck, a.r_1, Col, 2.f);
									Render::RenderLine(a.l_1, a.l_d_1, Col, 2.f);
									Render::RenderLine(a.r_1, a.r_d_1, Col, 2.f);
									Render::RenderLine(a.l_d_1, a.l_d_2, Col, 2.f);
									Render::RenderLine(a.r_d_1, a.r_d_2, Col, 2.f);
									Render::RenderLine(a.body_2, a.l_a_1, Col, 2.f);
									Render::RenderLine(a.body_2, a.r_a_1, Col, 2.f);
									Render::RenderLine(a.l_a_1, a.l_a_2, Col, 2.f);
									Render::RenderLine(a.r_a_1, a.r_a_2, Col, 4.f);

									Render::RenderLine(a.r_a_2, a.sex, Col, 2.f);
									Render::RenderLine(a.l_a_2, a.sex1, Col, 2.f);
									Render::RenderLine(a.sex, a.sex2, Col, 2.f);
									Render::RenderLine(a.sex1, a.sex3, Col, 2.f);

									Render::RenderLine(a.head, a.neck, Col2, 2.f);
									Render::RenderLine(a.neck, a.body_1, Col2, 2.f);
									Render::RenderLine(a.body_1, a.body_2, Col2, 2.f);
									Render::RenderLine(a.neck, a.l_1, Col2, 2.f);
									Render::RenderLine(a.neck, a.r_1, Col2, 2.f);
									Render::RenderLine(a.l_1, a.l_d_1, Col2, 2.f);
									Render::RenderLine(a.r_1, a.r_d_1, Col2, 2.f);
									Render::RenderLine(a.l_d_1, a.l_d_2, Col2, 2.f);
									Render::RenderLine(a.r_d_1, a.r_d_2, Col2, 2.f);
									Render::RenderLine(a.body_2, a.l_a_1, Col2, 2.f);
									Render::RenderLine(a.body_2, a.r_a_1, Col2, 2.f);
									Render::RenderLine(a.l_a_1, a.l_a_2, Col2, 2.f);
									Render::RenderLine(a.r_a_1, a.r_a_2, Col2, 2.f);

									Render::RenderLine(a.r_a_2, a.sex, Col2, 2.f);
									Render::RenderLine(a.l_a_2, a.sex1, Col2, 2.f);
									Render::RenderLine(a.sex, a.sex2, Col2, 2.f);
									Render::RenderLine(a.sex1, a.sex3, Col2, 2.f);
									Draw->AddCircleFilled(ImVec2(a.head.X, a.head.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_1.X, a.body_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.body_2.X, a.body_2.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.neck.X, a.neck.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_1.X, a.l_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_1.X, a.r_1.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_d_1.X, a.l_d_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_d_2.X, a.l_d_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.r_d_1.X, a.r_d_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_d_2.X, a.r_d_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.l_a_1.X, a.l_a_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.l_a_2.X, a.l_a_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.r_a_1.X, a.r_a_1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.r_a_2.X, a.r_a_2.Y), radisofpoint, pointcolor);

									Draw->AddCircleFilled(ImVec2(a.sex.X, a.sex.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex2.X, a.sex2.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex1.X, a.sex1.Y), radisofpoint, pointcolor);
									Draw->AddCircleFilled(ImVec2(a.sex3.X, a.sex3.Y), radisofpoint, pointcolor);
								}
							}
						}
					}
				}
				if (Config::drawline)
				{
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector2 Vec2_A{};
						if (viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y, Vec3.Z), &Vec2_A, Vector2(WX, WY))) {
							if(entity.Vis)
								Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(160, 218, 255, 150), 2.f);
							else
								Render::DrawLine(Vector2(WX / 2, WY / 2), Vec2_A, Color(204, 102, 102, 150), 2.f);
						}
					}
				}
				if (Config::draw_edge) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector2 Vec2_A{}, Vec2_B{};
						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y + 0.1f, Vec3.Z), &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(Vector3(Vec3.X, Vec3.Y - 1.8f, Vec3.Z), &Vec2_B, Vector2(WX, WY)))
							continue;

						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						Render::DrawCorneredBox(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImGui::GetColorU32(Config::EnemyCol));
						Render::DrawFilledRect(Vec2_A.X - (width / 2) - 1, Vec2_A.Y - (height / 5) - 1, width + 2, height + 12, ImColor(0, 0, 0, 60));
					}
				}
				if (Config::drawbox3d) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector3 veca{}, vecb{}, vecc{}, vecd{}, vece{}, vecf{}, vecg{}, vech{};
						Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{}, Vec2_G{}, Vec2_H{};
						entity.Get3DBoxPos(veca, vecb, vecc, vecd, vece, vecf, vecg, vech);
						if (!viewMatrix.WorldToScreen(veca, &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecb, &Vec2_B, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vecc, &Vec2_C, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecd, &Vec2_D, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vece, &Vec2_E, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vecf, &Vec2_F, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(vecg, &Vec2_G, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(vech, &Vec2_H, Vector2(WX, WY)))
							continue;

						float height = abs(Vec2_A.Y - Vec2_B.Y);
						float width = height * 0.85;
						float Size = abs(Vec2_A.Y - Vec2_B.Y) / 2.0f;
						float Size2 = abs(Vec2_A.Y - Vec2_B.Y) / 20.0f;
						float Height2 = abs(Vec2_A.Y - Vec2_B.Y);
						int num7 = (float)(1500 / (int)Height2);
						float xpos = (Vec2_A.X + Vec2_B.X) / 2.f;
						float ypos = Vec2_A.Y + Size / 5;

						if (entity.Vis) {
							Render::DrawLine(Vec2_A, Vec2_B, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_C, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_A, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_A, Vec2_E, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_G, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_H, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_E, Vec2_F, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_F, Vec2_H, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_H, Vec2_G, Color(0, 204, 204, 150), 1.2f);
							Render::DrawLine(Vec2_G, Vec2_E, Color(0, 204, 204, 150), 1.2f);
						}
						else {
							Render::DrawLine(Vec2_A, Vec2_B, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_C, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_A, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_A, Vec2_E, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_C, Vec2_G, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_D, Vec2_H, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_E, Vec2_F, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_F, Vec2_H, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_H, Vec2_G, Color(255, 51, 153, 150), 1.2f);
							Render::DrawLine(Vec2_G, Vec2_E, Color(255, 51, 153, 150), 1.2f);
						}
					}
				}
				if (Config::eyeray) {
					if (entity.Alive && entity.Team && local_entity.PlayerHealth > 0 && entity.HeroID != 0x16dd)
					{
						Vector3 Vec3 = entity.head_pos;
						Vector3 eyepoint{}, whereisarrow1{}, whereisarrow2{}, whereisarrow3{}, whereisarrow4{};
						Vector2 Vec2_A{}, Vec2_B{}, Vec2_C{}, Vec2_D{}, Vec2_E{}, Vec2_F{};
						entity.GetEyeRayPoint(eyepoint, whereisarrow1, whereisarrow2, whereisarrow3, whereisarrow4);
						if (!viewMatrix.WorldToScreen(Vec3, &Vec2_A, Vector2(WX, WY)))
							continue;

						if (!viewMatrix.WorldToScreen(eyepoint, &Vec2_B, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow1, &Vec2_C, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow2, &Vec2_D, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow3, &Vec2_E, Vector2(WX, WY)))
							continue;
						if (!viewMatrix.WorldToScreen(whereisarrow4, &Vec2_F, Vector2(WX, WY)))
							continue;

						if (entity.Vis) {
							Render::DrawLine(Vec2_A, Vec2_B, Color(255, 0, 0, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_C, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_E, Color(255, 51, 102, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(255, 51, 102, 255), 3.f);
						}
						else {
							Render::DrawLine(Vec2_A, Vec2_B, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_C, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_D, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_E, Color(102, 153, 255, 255), 3.f);
							Render::DrawLine(Vec2_B, Vec2_F, Color(102, 153, 255, 255), 3.f);
						}
					}
				}
				if (Config::crosscircle && (GetAsyncKeyState(0x1) || GetAsyncKeyState(0x2) || GetAsyncKeyState(VK_XBUTTON2) || GetAsyncKeyState(VK_XBUTTON1))) {
					Vector3 Vec3 = entity.chest_pos;
					Vector2 Vec2_A{};
					if (!viewMatrix.WorldToScreen(Vec3, &Vec2_A, Vector2(WX, WY)))
						continue;
					if (Config::crss == 0) {
						Config::locx = rand() % 20;
						Config::locy = rand() % 10;
						Config::therad = rand() % 30;
						Config::pon = rand() % 10;
						if (Config::pon >= 5) Config::locx = -Config::locx;
						Config::pon = rand() % 10;
						if (Config::pon >= 5) Config::locy = -Config::locy;
					}
					Config::crss++;
					if (Config::crss == 14) Config::crss = 0;
					if (entity.Alive && entity.Vis && local_entity.address && entity.Team)
						Draw->AddCircle(ImVec2(Vec2_A.X + Config::locx, Vec2_A.Y + Config::locy), Config::therad, IM_COL32(255, 255, 255, 255), 3000, 3);
				}
			}
		}

		if (Config::skillinfo) {
			skillinfo();
		}
		if (Config::draw_fov)
		{
			Draw->AddCircle(CrossHair, Config::Fov, ImGui::GetColorU32(Config::fovcol), 3000);
			if (Config::secondaim)
				Draw->AddCircle(CrossHair, Config::Fov2, ImGui::GetColorU32(Config::fovcol2), 3000);
		}
	}
	inline void aimbot_thread()
	{
		__try {
			int hitbotdelaytime = 0;
			int afterdelaytime = 0;
			bool dodelay = 0;
			TCHAR buf[100];
			Vector2 CrossHair = Vector2(GetSystemMetrics(SM_CXSCREEN) / 2.0f, GetSystemMetrics(SM_CYSCREEN) / 2.0f);
			static float origin_sens = 0.f;
			while (true) {
				if (entities.size() > 0) {
					//std::cout<<SDK->RPM<float>(GetSenstivePTR())<< std::endl;
					//std::cout << GetSenstivePTR() << std::endl;
					//bool reloading = IsSkillActive(local_entity.SkillBase, 0, 0x4BF);
					if (SDK->RPM<float>(GetSenstivePTR()))
						origin_sens = SDK->RPM<float>(GetSenstivePTR());
					else if (origin_sens)
						SDK->WPM<float>(GetSenstivePTR(), origin_sens);
					//std::cout << SDK->RPM<float>(GetSenstivePTR()) << std::endl;
					if (Config::triggerbot && GetAsyncKeyState(Config::aim_key)) {
						auto vec = GetVector3(Config::Prediction ? true : false);
						if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI)) {
							auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
							auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
							auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
							auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
							if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
								if (Config::lockontarget)
									SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x1);
								Sleep(2);
								if (Config::lockontarget)
									SDK->WPM<float>(GetSenstivePTR(), origin_sens);
							}
						}
					}

					//Tracking
					if (Config::Tracking) {

						while (GetAsyncKeyState(Config::aim_key) && !Config::reloading)
						{
							//mutex.lock();
							auto vec = GetVector3(Config::Prediction ? true : false);
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 500;
									else Target.X -= (double)(rand()) / RAND_MAX / 500;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 500;
									else Target.Y -= (double)(rand()) / RAND_MAX / 500;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 500;
									else Target.Z -= (double)(rand()) / RAND_MAX / 500;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									if (Config::targetdelay) {
										if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
										if (Config::doingdelay == 1) {
											Config::lastenemy = Config::Targetenemyi;
											if (Config::timebeforedelay == 0) {
												Config::timebeforedelay = GetTickCount();
												continue;
											}
											if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
											else Config::timebeforedelay = 0;
											Config::doingdelay = 0;
										}
									}
									if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
									else
										SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if (Config::health <= Config::meleehealth && dist <= Config::meleedistance && Config::AutoMelee) {
										//std::cout << Config::health << " " << dist << std::endl;
										//SDK->WPM<float>(GetSenstivePTR(), 0);
										SetKey(0x800);
										//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
									}

								}
								if (local_entity.PlayerHealth < Config::SkillHealth) {
									//mutex.unlock();
									break;
								}
							}
							//mutex.unlock();
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
							if (Config::highPriority && GetAsyncKeyState(Config::aim_key2)) break;
							else if (Config::highPriority && !GetAsyncKeyState(Config::aim_key2) && Config::shooted2) Config::shooted2 = false;
						}
					}
					//Flick
					else if (Config::Flick) {
						if (Config::hitboxdelayshoot) {
							if (Config::shooted || !GetAsyncKeyState(Config::aim_key)) {
								dodelay = 1;
								hitbotdelaytime = 0;
							}
						}
						//int a = SDK->RPM<int>(SDK->g_player_controller + 0x1760);
						while (GetAsyncKeyState(Config::aim_key) && !Config::shooted && !Config::reloading)
						{
							//mutex.lock();
							auto vec = GetVector3(Config::Prediction ? true : false);
							if (vec == Vector3(0, 0, 0)) break;
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {
								if (Config::targetdelay) {//目标切换延迟 doingdelay
									if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
									if (Config::doingdelay == 1) {
										Config::lastenemy = Config::Targetenemyi;
										if (Config::timebeforedelay == 0) {
											Config::timebeforedelay = GetTickCount();
											continue;
										}
										if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
										else Config::timebeforedelay = 0;
										Config::doingdelay = 0;
										hitbotdelaytime = GetTickCount();
									}
								}
								else if (Config::doingdelay) Config::doingdelay = 0;
								if (dodelay && !Config::doingdelay) {//超时开枪 dodelay
									hitbotdelaytime = GetTickCount();
									dodelay = 0;
								}
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, Config::accvalue);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 300;
									else Target.X -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 300;
									else Target.Y -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 300;
									else Target.Z -= (double)(rand()) / RAND_MAX / 300;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									//hitbox延迟开枪
									if (Config::hitboxdelayshoot && hitbotdelaytime != 0) {
										afterdelaytime = GetTickCount();
										if (afterdelaytime - hitbotdelaytime > Config::hiboxdelaytime && !Config::doingdelay) {
											if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
												SetKey(0x2);
												//if (Config::dontshot) Config::shotcount++;
											}
											else
												SetKey(0x1);
											Config::shooted = true;
											continue;
										}
									}
									//愤怒
									if (Config::Rage) {
										if (Config::fakesilent) {
											Vector3 orangle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
											if (Config::lockontarget)
												SDK->WPM<float>(GetSenstivePTR(), 0);
											SetKey(0x1);
											Sleep(25);
											if (Config::lockontarget)
												SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, orangle);
											continue;
										}
										else {
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
											SetKey(0x1);
											Sleep(1);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											continue;
										}
									}
									//正常
									SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
										if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
											SetKey(0x2);
											//if (!Config::sskilled) Sleep(10);
											if (Config::dontshot) Config::shotcount++;
										}
										else {
											if ((local_entity.HeroID == eHero::HERO_ANA || local_entity.HeroID == eHero::HERO_WIDOWMAKER || local_entity.HeroID == eHero::HERO_ASHE) && GetAsyncKeyState(0x2))
												SetKeyscopeHold(0x1, 30);
											else SetKey(0x1);
										}
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
										Config::shooted = true;
										if (Config::dontshot) Config::shotcount++;
										break;
										
									}
									//自动空枪
									else if (Config::dontshot && Config::shotcount >= Config::shotmanydont) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::missbox)) {
											Config::shotcount = 0;
											if (local_entity.HeroID == eHero::HERO_GENJI || local_entity.HeroID == eHero::HERO_KIRIKO) {
												SetKey(0x2);
												if (!Config::sskilled) Sleep(10);
											}
											else {
												SetKey(0x1);
											}
											Config::shooted = true;
											continue;
										}
									}
								}
								else hitbotdelaytime = 0;

							}


							//mutex.unlock();
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
							if (Config::highPriority && GetAsyncKeyState(Config::aim_key2)) break;
							else if (Config::highPriority && !GetAsyncKeyState(Config::aim_key2) && Config::shooted2) Config::shooted2 = false;
						}
					}
					else if (Config::hanzo_flick)
					{
						if (Config::hitboxdelayshoot) {
							if (Config::shooted || !GetAsyncKeyState(Config::aim_key)) {
								dodelay = 1;
								hitbotdelaytime = 0;
							}
						}
						while (GetAsyncKeyState(Config::aim_key) && !Config::shooted)
						{
							//mutex.lock();
							auto vec = GetVector3(true);
							if (vec == Vector3(0, 0, 0)) {
								//mutex.unlock();
								break;
							}
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {

								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, Config::accvalue);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (dodelay && !Config::doingdelay) {//超时开枪 dodelay
									hitbotdelaytime = GetTickCount();
									dodelay = 0;
								}
								if (Config::aiaim) {
									int x;
									if (rand() % 10 > 5)
										Target.X += (double)(rand()) / RAND_MAX / 300;
									else Target.X -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Y += (double)(rand()) / RAND_MAX / 300;
									else Target.Y -= (double)(rand()) / RAND_MAX / 300;
									if (rand() % 10 > 5)
										Target.Z += (double)(rand()) / RAND_MAX / 300;
									else Target.Z -= (double)(rand()) / RAND_MAX / 300;
									if (Config::minFov1 > 500)Config::minFov1 = 500;
									if (Config::Fov > 500)Config::Fov = 500;
									if (Config::minFov2 > 500)Config::minFov1 = 500;
									if (Config::Fov2 > 500)Config::Fov = 500;
									if (Config::fov360) Config::fov360 = false;
								}
								if (Target != Vector3(0, 0, 0)) {
									if (Config::hitboxdelayshoot && hitbotdelaytime != 0) {
										afterdelaytime = GetTickCount();
										if (afterdelaytime - hitbotdelaytime > Config::hiboxdelaytime && !Config::doingdelay) {
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											Config::shooted = true;
											continue;
										}
									}
									if (Config::Rage) {
										if (Config::fakesilent) {
											Vector3 orangle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											Sleep(25);
											if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, orangle);
											continue;
										}
										else
										{
											SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
											//SDK->WPM<float>(GetSenstivePTR(), 0);
											if (local_entity.skill2act) SetKey(0x1);
											else SetKeyHold(0x1000, 100);
											//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
											Config::shooted = true;
											continue;
										}
									}

									if (Config::targetdelay) {
										if (Config::lastenemy != Config::Targetenemyi)  Config::doingdelay = 1;
										if (Config::doingdelay == 1) {
											Config::lastenemy = Config::Targetenemyi;
											if (Config::timebeforedelay == 0) {
												Config::timebeforedelay = GetTickCount();
												continue;
											}
											if (GetTickCount() - Config::timebeforedelay < Config::targetdelaytime) continue;
											else Config::timebeforedelay = 0;
											Config::doingdelay = 0;
											hitbotdelaytime = GetTickCount();
										}
									}
									else if (Config::doingdelay) Config::doingdelay = 0;
									SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox)) {
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), 0);
										if (local_entity.skill2act) SetKey(0x1);
										else SetKeyHold(0x1000, 100);
										Sleep(1);
										if (Config::dontshot) Config::shotcount++;
										if (Config::lockontarget) SDK->WPM<float>(GetSenstivePTR(), origin_sens);
										Config::shooted = true;
									}
									else if (Config::dontshot && Config::shotcount >= Config::shotmanydont) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::missbox)) {
											Config::shotcount = 0;
											if (local_entity.skill2act) {
												SetKey(0x1);
											}
											else {
												SetKeyHold(0x1000, 100);
											}
											Config::shooted = true;
											continue;
										}
									}
								}
								else hitbotdelaytime = 0;
							}


							//mutex.unlock();
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
						}
					}
					if (Config::autoscalefov) {
						auto vec = GetVector3forfov();
						if (vec != Vector3(0, 0, 0)) {
							Vector2 high;
							Vector2 low;
							if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
							{
								Config::Fov = -(high.Y - low.Y) * 4;
								if (Config::Fov > 500) Config::Fov = 500;
								else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
								Config::Fov2 = -(high.Y - low.Y) * 4;
								if (Config::Fov2 > 500) Config::Fov2 = 500;
								else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
							}
							else
							{
								Config::Fov = Config::minFov1;
								Config::Fov2 = Config::minFov2;
							}

						}
						else
						{
							Config::Fov = Config::minFov1;
							Config::Fov2 = Config::minFov2;
						}
					}
					if (Config::GenjiBlade && GetAsyncKeyState(0x51) && local_entity.HeroID == eHero::HERO_GENJI && local_entity.ultimate == 100) {
						Config::Qstarttime = GetTickCount();
						Config::Qtime = Config::Qstarttime;
						Config::lastenemy = -1;
						Sleep(1000);
						int detecttoggle = 0;
						int first = 1;
						float speed = 0;
						while (Config::GenjiBlade && (Config::Qtime - Config::Qstarttime) <= 7000) {
							if (!local_entity.skillcd1) speed = Config::Tracking_smooth;
							else speed = Config::bladespeed;
							Config::Qtime = GetTickCount();
							auto vec = GetVector3forgenji();
							/*if (vec != Vector3(0, 0, 0)) {
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
								if (Target != Vector3(0, 0, 0)) {
									SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									if (Config::lastenemy != Config::Targetenemyi && dist < 20) {
										while (!in_range(local_angle, vec_calc_target, local_loc, vec, 0.3)) {
											vec = GetVector3forgenji();
											local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
											calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
											vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
											Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
											local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
											if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
											else SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
										}
										Config::lastenemy = Config::Targetenemyi;
										SetKey(0x8);
										Sleep(45);
									}
									else if (dist >= 20)
										Config::lastenemy = -1;
									else Config::lastenemy = Config::Targetenemyi;
								}
							}
							vec = GetVector3forgenji();*/
							if (vec != Vector3(0, 0, 0)) {
								float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
								if (dist > 20) continue;
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, speed / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Target != Vector3(0, 0, 0)) {
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if ((!local_entity.skillcd1 && dist < 20) || dist < 7)
									{
										if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
										else SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									}
									if (!local_entity.skillcd1 && in_range(local_angle, vec_calc_target, local_loc, vec, 0.8)) {
										if (detecttoggle && !first) {
											detecttoggle = 0;
											Sleep(50);
											//std::cout << "toggled" << std::endl;
											continue;
										}
										SetKeyHold(0x8, 70);
										first = 0;
									}

									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1) && dist < 5) {
										//SDK->WPM<float>(GetSenstivePTR(), 0);
										//if (Config::Rage) SetKey(0x1);
										//else SetKeyHold(0x1, 900);
										SetKey(0x1);
										//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
									}
									if (local_entity.skillcd1 != 0 && !detecttoggle) {
										detecttoggle = 1;
									}
								}
							}
							/*vec = GetVector3forgenji();
							if (vec != Vector3(0, 0, 0)) {
								float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);

								if (Target != Vector3(0, 0, 0)) {
									if (Config::Rage) SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, vec_calc_target);
									else SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if (!local_entity.skillcd1 && in_range(local_angle, vec_calc_target, local_loc, vec, 0.3) && dist < 20)
										SetKeyHold(0x8, 70);
								}
							}*/
							//mutex.unlock();
							Sleep(1);
							Config::lastenemy = Config::Targetenemyi;

						}
					}
					if (Config::trackback) {
						auto vec = GetVector3fortrackback(Config::Prediction ? true : false);
					}
					if (Config::AutoMelee) {
						auto vec = GetVector3(false);
						//printf("%llx\n",SDK->RPM<uint32_t>(SDK->g_player_controller + 0x1244));
						if (vec != Vector3(0, 0, 0) && entities[Config::Targetenemyi].Team) {
							float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
							if (Config::health <= Config::meleehealth && dist <= Config::meleedistance) {
								//std::cout << Config::health << " " << dist << std::endl;
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x800);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Sleep(1);
							}
						}
					}
					if (Config::AutoShiftGenji) {
						auto vec = GetVector3(false);
						if (vec != Vector3(0, 0, 0)) {
							float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
							if (!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) {
								if (!local_entity.skillcd1 && Config::health <= 50 && dist <= 15 && entities[Config::Targetenemyi].HeroID != 0x16dd && entities[Config::Targetenemyi].HeroID != 0x16ee) {
									//SDK->WPM<float>(GetSenstivePTR(), 0);
									auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
									auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
									auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
									auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
									auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1)) {
										//Config::sskilled = true;
										SetKeyHold(0x8, 40);
									}
									//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								}
								else if (!local_entity.skillcd1 && Config::health <= 80 && dist <= 17 && dist >= 15 && entities[Config::Targetenemyi].HeroID != 0x16dd && entities[Config::Targetenemyi].HeroID != 0x16ee) {
									//SDK->WPM<float>(GetSenstivePTR(), 0);
									auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
									auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
									auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
									auto Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth / 10.f);
									auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
									if (in_range(local_angle, vec_calc_target, local_loc, vec, 1)) {
										//Config::sskilled = true;
										SetKey(0x8);
										Sleep(500);
										SetKey(0x800);
									}
									//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								}
							}
							//if (local_entity.skill1act) {
							//	Config::sskilled = true;
							//}
						}
					}

					if (Config::AutoSkill) {
						if (local_entity.PlayerHealth > Config::SkillHealth && Config::skilled) {
							Config::skilled = false;
						}
						else if (local_entity.PlayerHealth < Config::SkillHealth && Config::skilled && local_entity.PlayerHealth < Config::lasthealth && local_entity.HeroID != eHero::HERO_DOOMFIST) {
							Config::skilled = false;
						}
						if (local_entity.PlayerHealth < Config::SkillHealth) {
							if (local_entity.HeroID == eHero::HERO_TRACER && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_SOMBRA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ROADHOG && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_TORBJORN && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_SOLDIER76 && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_REAPER && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x8);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_MEI && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x8);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_DOOMFIST && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x20);
								Sleep(10);
								SetKey(0x10);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_JUNKERQUEEN && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x8);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_MOIRA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x8);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ZARYA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x8);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_WINSTON && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x20);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
							else if (local_entity.HeroID == eHero::HERO_ZENYATTA && local_entity.PlayerHealth != 0 && !Config::skilled) {
								//SDK->WPM<float>(GetSenstivePTR(), 0);
								SetKey(0x20);
								//SDK->WPM<float>(GetSenstivePTR(), origin_sens);
								Config::skilled = true;
								Sleep(1);
								Config::lasthealth = local_entity.PlayerHealth;
							}
						}

					}
					if (Config::AutoShoot && Config::shooted && !(local_entity.HeroID == eHero::HERO_HANJO && !local_entity.skill2act)) {
						int rectime = GetTickCount();
						if (Config::lasttime == 0) Config::lasttime = rectime;
						else {
							int mtime = rectime - Config::lasttime;
							if (mtime >= Config::Shoottime) {
								Config::lasttime = 0;
								Config::shooted = false;
							}
						}
						if (Config::reloading) {
							Config::lasttime = 0;
							Config::shooted = false;
						}
					}
					/*if (Config::AutoShiftGenji && Config::sskilled) {
						int srectime = GetTickCount();
						if (Config::slasttime == 0) Config::slasttime = srectime;
						else {
							int mtime = srectime - Config::slasttime;
							if (mtime >= 500) {
								Config::slasttime = 0;
								Config::sskilled = false;
							}
						}
					}*/
					if (!GetAsyncKeyState(Config::aim_key)) {
						Config::shooted = false;
						Config::lasttime = 0;
						if (Config::reloading) {
							Config::lasttime = 0;
							Config::shooted = false;
						}
						Config::Targetenemyi = -1;
					}
					//死神换弹
					if (local_entity.HeroID == eHero::HERO_REAPER && Config::reloading) {
						Sleep(300);
						SetKey(0x800);
					}

					if (Config::secondaim) {
						while (GetAsyncKeyState(Config::aim_key2) && !Config::shooted2)
						{
							//mutex.lock();
							auto vec = GetVector3aim2(Config::Prediction2 ? true : false);
							if (vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].skill2act && entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI)) {
								auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
								auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
								auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
								Vector3 Target;
								if (Config::Tracking2)
									Target = SmoothLinear(local_angle, vec_calc_target, Config::Tracking_smooth2 / 10.f);
								else if (Config::Flick2)
									Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth2 / 10.f, Config::accvalue2);
								if (Config::Rage) Target = vec_calc_target;
								auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);
								if (Config::aiaim) {
									if (Config::Flick2) {
										int x;
										if (rand() % 10 > 5)
											Target.X += (double)(rand()) / RAND_MAX / 300;
										else Target.X -= (double)(rand()) / RAND_MAX / 300;
										if (rand() % 10 > 5)
											Target.Y += (double)(rand()) / RAND_MAX / 300;
										else Target.Y -= (double)(rand()) / RAND_MAX / 300;
										if (rand() % 10 > 5)
											Target.Z += (double)(rand()) / RAND_MAX / 300;
										else Target.Z -= (double)(rand()) / RAND_MAX / 300;
										if (Config::minFov1 > 500)Config::minFov1 = 500;
										if (Config::Fov > 500)Config::Fov = 500;
										if (Config::minFov2 > 500)Config::minFov1 = 500;
										if (Config::Fov2 > 500)Config::Fov = 500;
										if (Config::fov360) Config::fov360 = false;
									}
									else if (Config::Tracking2) {
										int x;
										if (rand() % 10 > 5)
											Target.X += (double)(rand()) / RAND_MAX / 500;
										else Target.X -= (double)(rand()) / RAND_MAX / 500;
										if (rand() % 10 > 5)
											Target.Y += (double)(rand()) / RAND_MAX / 500;
										else Target.Y -= (double)(rand()) / RAND_MAX / 500;
										if (rand() % 10 > 5)
											Target.Z += (double)(rand()) / RAND_MAX / 500;
										else Target.Z -= (double)(rand()) / RAND_MAX / 500;
										if (Config::minFov1 > 500)Config::minFov1 = 500;
										if (Config::Fov > 500)Config::Fov = 500;
										if (Config::minFov2 > 500)Config::minFov1 = 500;
										if (Config::Fov2 > 500)Config::Fov = 500;
										if (Config::fov360) Config::fov360 = false;
									}
								}
								if (Target != Vector3(0, 0, 0)) {
									float dist = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z).DistTo(vec);
									if (Config::health <= Config::meleehealth && dist <= Config::meleedistance && Config::AutoMelee) {
										SetKey(0x800);
									}

									SDK->WPM<Vector3>(SDK->g_player_controller + 0x12A0, Target);
									if (Config::Flick2) {
										if (in_range(local_angle, vec_calc_target, local_loc, vec, Config::hitbox2)) {
											if (Config::togglekey == 0)
												SetKey(0x1);
											else if (Config::togglekey == 1)
												SetKey(0x2);
											else if (Config::togglekey == 2)
												SetKey(0x8);
											else if (Config::togglekey == 3)
												SetKey(0x10);
											else if (Config::togglekey == 4)
												SetKey(0x20);
											Sleep(1);
											Config::shooted2 = true;
										}
									}
								}
								if (local_entity.PlayerHealth < Config::SkillHealth) {
									break;
								}
							}
							//mutex.unlock();
							Sleep(1);
							if (Config::autoscalefov) {
								auto vec = GetVector3forfov();
								if (vec != Vector3(0, 0, 0)) {
									Vector2 high;
									Vector2 low;
									if (viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].head_pos, &high, Vector2(WX, WY)) && viewMatrix.WorldToScreen(entities[Config::Targetenemyifov].chest_pos, &low, Vector2(WX, WY)))
									{
										Config::Fov = -(high.Y - low.Y) * 4;
										if (Config::Fov > 500) Config::Fov = 500;
										else if (Config::Fov < Config::minFov1) Config::Fov = Config::minFov1;
										Config::Fov2 = -(high.Y - low.Y) * 4;
										if (Config::Fov2 > 500) Config::Fov2 = 500;
										else if (Config::Fov2 < Config::minFov2) Config::Fov2 = Config::minFov2;
									}
									else
									{
										Config::Fov = Config::minFov1;
										Config::Fov2 = Config::minFov2;
									}

								}
								else
								{
									Config::Fov = Config::minFov1;
									Config::Fov2 = Config::minFov2;
								}
							}
						}
						if (Config::shooted2 && !GetAsyncKeyState(Config::aim_key2))
							Config::shooted2 = false;
					}
				}
				Sleep(2);
			}
		}
		__except (1) {

		}
	}
	inline void configsavenloadthread() {
		TCHAR bufsave[100];
		//int timetobegin = 0;
		//timetobegin = GetTickCount();
		//int timepassed = 0;
		if (Config::lastheroid == -2) {
			ImGui::InsertNotification({ ImGuiToastType_Success, 8000, skCrypt(u8"Rigel Internal Loaded\nWelcome！"), "" });
			Config::lastheroid = 0;
		}
		while (1) {
			//timepassed = GetTickCount();
			if (!Config::Menu) {
				if (Config::lastheroid != local_entity.HeroID) {
					if (Config::lastheroid != 0) {
						_stprintf(bufsave, _T("%d"), Config::spinbot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("spinbot"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::highPriority);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("highPriority"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::hanzoautospeed);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_hp_pack);
						WritePrivateProfileString(_T("Global"), _T("draw_hp_pack"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::aiaim);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aiaim"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::autoscalefov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoscalefov"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::crosscircle);
						WritePrivateProfileString(_T("Global"), _T("crosscircle"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::eyeray);
						WritePrivateProfileString(_T("Global"), _T("eyeray"), bufsave, _T(".\\config.ini"));


						_stprintf(bufsave, _T("%d"), Config::lockontarget);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("lockontarget"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::trackcompensate);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("trackc"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::comarea * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comarea"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::comspeed * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comspeed"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)Config::Fov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("FOV"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::hitbox * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::missbox * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("missbox"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::Shoottime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShootTime"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::predit_level);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::aim_key);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::Gravitypredit);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
						int dec = 0;
						if (Config::Tracking) dec = 0;
						else if (Config::Flick) dec = 1;
						else if (Config::hanzo_flick) dec = 2;
						else if (Config::silent) dec = 3;
						else if (Config::triggerbot) dec = 4;
						int autoshoot = 0;
						if (Config::AutoShoot) autoshoot = 1;
						else autoshoot = 0;
						int predictdec = 0;
						if (Config::Prediction) predictdec = 1;
						else predictdec = 0;
						_stprintf(bufsave, _T("%d"), dec);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Aim Mode"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), autoshoot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoshootonoff"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), predictdec);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predictdec"), bufsave, _T(".\\config.ini"));
						int dontshot = 0;
						if (Config::dontshot) dontshot = 1;
						else dontshot = 0;
						_stprintf(bufsave, _T("%d"), dontshot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontshot"), bufsave, _T(".\\config.ini"));
						int targetdelay = 0;
						if (Config::targetdelay) targetdelay = 1;
						else targetdelay = 0;
						_stprintf(bufsave, _T("%d"), targetdelay);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelay"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::targetdelaytime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelaytime"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::shotmanydont);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontmanyshot"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::hitboxdelayshoot);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::hiboxdelaytime);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), bufsave, _T(".\\config.ini"));

						if (Config::lastheroid == eHero::HERO_GENJI) {
							_stprintf(bufsave, _T("%d"), Config::GenjiBlade);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("GenjiBlade"), bufsave, _T(".\\config.ini"));
							_stprintf(bufsave, _T("%d"), Config::AutoShiftGenji);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), bufsave, _T(".\\config.ini"));
							_stprintf(bufsave, _T("%d"), (int)(Config::bladespeed * 10000));
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("bladespeed"), bufsave, _T(".\\config.ini"));
						}

						if (Config::lastheroid == eHero::HERO_WIDOWMAKER) {
							_stprintf(bufsave, _T("%d"), Config::widowautounscope);
							WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("widowautounscope"), bufsave, _T(".\\config.ini"));
						}

						_stprintf(bufsave, _T("%d"), Config::Bone);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::autobone);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::Bone2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::autobone2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone2"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), Config::AutoMelee);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoMelee"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::norecoil);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("norecoil"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::accvalue * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::horizonreco);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("horizonreco"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Gravitypredit);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::switch_team);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::switch_team2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team2"), bufsave, _T(".\\config.ini"));


						_stprintf(bufsave, _T("%d"), Config::secondaim);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("secondaim"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Tracking2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Flick2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Prediction2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Prediction2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::Gravitypredit2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::aim_key2);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)Config::togglekey);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("togglekey"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::predit_level2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::accvalue2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::hitbox2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::Fov2 * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Fov2"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::enablechangefov);
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("enablechangefov"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::CHANGEFOV * 10000));
						WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), bufsave, _T(".\\config.ini"));


						_stprintf(bufsave, _T("%d"), (int)(Config::backtracksecond * 10000));
						WritePrivateProfileString(_T("Global"), _T("backtime"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::trackback);
						WritePrivateProfileString(_T("Global"), _T("trackback"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_info);
						WritePrivateProfileString(_T("Global"), _T("draw_info"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawbattletag);
						WritePrivateProfileString(_T("Global"), _T("drawbattletag"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawhealth);
						WritePrivateProfileString(_T("Global"), _T("drawhealth"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::healthbar);
						WritePrivateProfileString(_T("Global"), _T("healthbar"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::dist);
						WritePrivateProfileString(_T("Global"), _T("dist"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::name);
						WritePrivateProfileString(_T("Global"), _T("name"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::ult);
						WritePrivateProfileString(_T("Global"), _T("ult"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_skel);
						WritePrivateProfileString(_T("Global"), _T("draw_skel"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::skillinfo);
						WritePrivateProfileString(_T("Global"), _T("skillinfo"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::outline);
						WritePrivateProfileString(_T("Global"), _T("outline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::externaloutline);
						WritePrivateProfileString(_T("Global"), _T("externaloutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::teamoutline);
						WritePrivateProfileString(_T("Global"), _T("teamoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::healthoutline);
						WritePrivateProfileString(_T("Global"), _T("healthoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::rainbowoutline);
						WritePrivateProfileString(_T("Global"), _T("rainbowoutline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_edge);
						WritePrivateProfileString(_T("Global"), _T("draw_edge"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawbox3d);
						WritePrivateProfileString(_T("Global"), _T("drawbox3d"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::radar);
						WritePrivateProfileString(_T("Global"), _T("radar"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::radarline);
						WritePrivateProfileString(_T("Global"), _T("radarline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::drawline);
						WritePrivateProfileString(_T("Global"), _T("drawline"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), Config::draw_fov);
						WritePrivateProfileString(_T("Global"), _T("draw_fov"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColy"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("EnemyColw"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcoly"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcolw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2x"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2y"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2z"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("fovcol2w"), bufsave, _T(".\\config.ini"));

						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("invisenargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::enargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("enargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargbw"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2x"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2y"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2z"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("targetargb2w"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.x * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbx"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.y * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargby"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.z * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbz"), bufsave, _T(".\\config.ini"));
						_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.w * 10000));
						WritePrivateProfileString(_T("Global"), _T("allyargbw"), bufsave, _T(".\\config.ini"));


						std::string saveheroname = GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str();
						saveheroname = skCrypt(u8"Saved:").decrypt() + saveheroname;
						ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
					}
					Config::Fov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("FOV"), 200, _T(".\\config.ini"));
					Config::minFov1 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("FOV"), 200, _T(".\\config.ini"));

					Config::comarea = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("comarea"), 100, _T(".\\config.ini"))) / 10000.f;
					Config::comspeed = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("comspeed"), 5000, _T(".\\config.ini"))) / 10000.f;

					Config::hitbox = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitbox"), 1300, _T(".\\config.ini"))) / 10000.f;
					Config::missbox = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("missbox"), 6000, _T(".\\config.ini"))) / 10000.f;
					Config::Tracking_smooth = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Flick_smooth = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick_smooth"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Shoottime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoShootTime"), 500, _T(".\\config.ini"));
					Config::predit_level = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predit_level"), 110, _T(".\\config.ini"));
					Config::aim_key = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aim_key"), 6, _T(".\\config.ini"));
					Config::dontshot = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("dontshot"), 0, _T(".\\config.ini"));
					Config::targetdelay = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("targetdelay"), 0, _T(".\\config.ini"));
					Config::targetdelaytime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("targetdelaytime"), 0, _T(".\\config.ini"));
					Config::shotmanydont = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("dontmanyshot"), 0, _T(".\\config.ini"));
					Config::hitboxdelayshoot = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), 0, _T(".\\config.ini"));
					Config::hiboxdelaytime = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), 200, _T(".\\config.ini"));
					Config::Bone = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Bone"), 1, _T(".\\config.ini"));
					Config::autobone = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autobone"), 0, _T(".\\config.ini"));

					Config::Bone2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Bone2"), 1, _T(".\\config.ini"));
					Config::autobone2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autobone2"), 0, _T(".\\config.ini"));

					Config::AutoMelee = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoMelee"), 0, _T(".\\config.ini"));
					Config::norecoil = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("norecoil"), 0, _T(".\\config.ini"));
					Config::recoilnum = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("recoilnum"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::accvalue = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("accvalue"), 7500, _T(".\\config.ini"))) / 10000.f;
					Config::horizonreco = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("horizonreco"), 0, _T(".\\config.ini"));
					Config::Gravitypredit = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit"), 0, _T(".\\config.ini"));
					Config::switch_team = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("switch_team"), 0, _T(".\\config.ini"));
					Config::draw_hp_pack = GetPrivateProfileInt(_T("Global"), _T("draw_hp_pack"), 0, _T(".\\config.ini"));

					Config::switch_team2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("switch_team2"), 0, _T(".\\config.ini"));
					Config::secondaim = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("secondaim"), 0, _T(".\\config.ini"));
					Config::Tracking2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking2"), 0, _T(".\\config.ini"));
					Config::Flick2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick2"), 0, _T(".\\config.ini"));
					Config::Prediction2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Prediction2"), 0, _T(".\\config.ini"));
					Config::Gravitypredit2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), 0, _T(".\\config.ini"));
					Config::predit_level2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predit_level2"), 1100000, _T(".\\config.ini"))) / 10000.f;
					Config::Fov2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Fov2"), 2000000, _T(".\\config.ini"))) / 10000.f;
					Config::minFov2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Fov2"), 2000000, _T(".\\config.ini"))) / 10000.f;

					Config::hitbox2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hitbox2"), 1300, _T(".\\config.ini"))) / 10000.f;
					Config::Tracking_smooth2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::Flick_smooth2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), 1000, _T(".\\config.ini"))) / 10000.f;
					Config::accvalue2 = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("accvalue2"), 7500, _T(".\\config.ini"))) / 10000.f;

					Config::aim_key2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aim_key2"), 5, _T(".\\config.ini"));
					Config::togglekey = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("togglekey"), 0, _T(".\\config.ini"));
					Config::Gravitypredit = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Gravitypredit"), 0, _T(".\\config.ini"));

					Config::enablechangefov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("enablechangefov"), 0, _T(".\\config.ini"));
					Config::CHANGEFOV = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), 1030000, _T(".\\config.ini"))) / 10000.f;


					Config::backtracksecond = float(GetPrivateProfileInt(_T("Global"), _T("backtime"), 2000, _T(".\\config.ini"))) / 10000.f;

					Config::trackback = GetPrivateProfileInt(_T("Global"), _T("trackback"), 0, _T(".\\config.ini"));
					Config::draw_info = GetPrivateProfileInt(_T("Global"), _T("draw_info"), 0, _T(".\\config.ini"));
					Config::drawbattletag = GetPrivateProfileInt(_T("Global"), _T("drawbattletag"), 0, _T(".\\config.ini"));
					Config::drawhealth = GetPrivateProfileInt(_T("Global"), _T("drawhealth"), 0, _T(".\\config.ini"));
					Config::healthbar = GetPrivateProfileInt(_T("Global"), _T("healthbar"), 0, _T(".\\config.ini"));
					Config::dist = GetPrivateProfileInt(_T("Global"), _T("dist"), 0, _T(".\\config.ini"));
					Config::name = GetPrivateProfileInt(_T("Global"), _T("name"), 0, _T(".\\config.ini"));
					Config::ult = GetPrivateProfileInt(_T("Global"), _T("ult"), 0, _T(".\\config.ini"));
					Config::draw_skel = GetPrivateProfileInt(_T("Global"), _T("draw_skel"), 0, _T(".\\config.ini"));
					Config::skillinfo = GetPrivateProfileInt(_T("Global"), _T("skillinfo"), 0, _T(".\\config.ini"));
					Config::outline = GetPrivateProfileInt(_T("Global"), _T("outline"), 0, _T(".\\config.ini"));
					Config::radar = GetPrivateProfileInt(_T("Global"), _T("radar"), 0, _T(".\\config.ini"));
					Config::radarline = GetPrivateProfileInt(_T("Global"), _T("radarline"), 0, _T(".\\config.ini"));
					Config::externaloutline = GetPrivateProfileInt(_T("Global"), _T("externaloutline"), 0, _T(".\\config.ini"));
					Config::teamoutline = GetPrivateProfileInt(_T("Global"), _T("teamoutline"), 0, _T(".\\config.ini"));
					Config::healthoutline = GetPrivateProfileInt(_T("Global"), _T("healthoutline"), 0, _T(".\\config.ini"));
					Config::rainbowoutline = GetPrivateProfileInt(_T("Global"), _T("rainbowoutline"), 0, _T(".\\config.ini"));
					Config::draw_edge = GetPrivateProfileInt(_T("Global"), _T("draw_edge"), 0, _T(".\\config.ini"));
					Config::drawbox3d = GetPrivateProfileInt(_T("Global"), _T("drawbox3d"), 0, _T(".\\config.ini"));
					Config::drawline = GetPrivateProfileInt(_T("Global"), _T("drawline"), 0, _T(".\\config.ini"));
					Config::draw_fov = GetPrivateProfileInt(_T("Global"), _T("draw_fov"), 0, _T(".\\config.ini"));

					Config::EnemyCol.x = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.y = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColy"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.z = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColz"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::EnemyCol.w = float(GetPrivateProfileInt(_T("Global"), _T("EnemyColw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::fovcol.x = float(GetPrivateProfileInt(_T("Global"), _T("fovcolx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.y = float(GetPrivateProfileInt(_T("Global"), _T("fovcoly"), 9000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.z = float(GetPrivateProfileInt(_T("Global"), _T("fovcolz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol.w = float(GetPrivateProfileInt(_T("Global"), _T("fovcolw"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.x = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2x"), 8550, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.y = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2y"), 4390, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.z = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2z"), 8390, _T(".\\config.ini"))) / 10000.f;
					Config::fovcol2.w = float(GetPrivateProfileInt(_T("Global"), _T("fovcol2w"), 5000, _T(".\\config.ini"))) / 10000.f;

					Config::invisenargb.x = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbx"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.y = float(GetPrivateProfileInt(_T("Global"), _T("invisenargby"), 3700, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.z = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbz"), 9100, _T(".\\config.ini"))) / 10000.f;
					Config::invisenargb.w = float(GetPrivateProfileInt(_T("Global"), _T("invisenargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::enargb.x = float(GetPrivateProfileInt(_T("Global"), _T("enargbx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.y = float(GetPrivateProfileInt(_T("Global"), _T("enargby"), 3000, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.z = float(GetPrivateProfileInt(_T("Global"), _T("enargbz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::enargb.w = float(GetPrivateProfileInt(_T("Global"), _T("enargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::targetargb.x = float(GetPrivateProfileInt(_T("Global"), _T("targetargbx"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.y = float(GetPrivateProfileInt(_T("Global"), _T("targetargby"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.z = float(GetPrivateProfileInt(_T("Global"), _T("targetargbz"), 0, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb.w = float(GetPrivateProfileInt(_T("Global"), _T("targetargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::targetargb2.x = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2x"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.y = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2y"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.z = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2z"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::targetargb2.w = float(GetPrivateProfileInt(_T("Global"), _T("targetargb2w"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::allyargb.x = float(GetPrivateProfileInt(_T("Global"), _T("allyargbx"), 4000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.y = float(GetPrivateProfileInt(_T("Global"), _T("allyargby"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.z = float(GetPrivateProfileInt(_T("Global"), _T("allyargbz"), 10000, _T(".\\config.ini"))) / 10000.f;
					Config::allyargb.w = float(GetPrivateProfileInt(_T("Global"), _T("allyargbw"), 10000, _T(".\\config.ini"))) / 10000.f;

					Config::lockontarget = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("lockontarget"), 1, _T(".\\config.ini"));
					Config::trackcompensate = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("trackc"), 0, _T(".\\config.ini"));
					Config::eyeray = GetPrivateProfileInt(_T("Global"), _T("eyeray"), 0, _T(".\\config.ini"));
					Config::crosscircle = GetPrivateProfileInt(_T("Global"), _T("crosscircle"), 0, _T(".\\config.ini"));
					Config::aiaim = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("aiaim"), 0, _T(".\\config.ini"));
					Config::autoscalefov = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autoscalefov"), 0, _T(".\\config.ini"));
					Config::hanzoautospeed = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), 0, _T(".\\config.ini"));
					Config::spinbot = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("spinbot"), 0, _T(".\\config.ini"));
					Config::highPriority = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("highPriority"), 0, _T(".\\config.ini"));

					switch (Config::aim_key)
					{
					case VK_LBUTTON:
						keys = key_type[0];
						break;
					case VK_RBUTTON:
						keys = key_type[1];
						break;
					case VK_MBUTTON:
						keys = key_type[2];
						break;
					case VK_XBUTTON1:
						keys = key_type[3];
						break;
					case VK_XBUTTON2:
						keys = key_type[4];
						break;
					}

					switch (Config::aim_key2)
					{
					case VK_LBUTTON:
						keys2 = key_type2[0];
						break;
					case VK_RBUTTON:
						keys2 = key_type2[1];
						break;
					case VK_MBUTTON:
						keys2 = key_type2[2];
						break;
					case VK_XBUTTON1:
						keys2 = key_type2[3];
						break;
					case VK_XBUTTON2:
						keys2 = key_type2[4];
						break;
					}

					switch (Config::togglekey)
					{
					case 0:
						keys3 = key_type3[0];
						break;
					case 1:
						keys3 = key_type3[1];
						break;
					case 2:
						keys3 = key_type3[2];
						break;
					case 3:
						keys3 = key_type3[3];
						break;
					case 4:
						keys3 = key_type3[4];
						break;
					}

					int dec1 = 0;
					dec1 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("Aim Mode"), 0, _T(".\\config.ini"));
					if (dec1 == 0) {
						Config::Tracking = true;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 1) {
						Config::Tracking = false;
						Config::Flick = true;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 2) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = false;
						Config::hanzo_flick = true;
					}
					else if (dec1 == 3) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = true;
						Config::triggerbot = false;
						Config::hanzo_flick = false;
					}
					else if (dec1 == 4) {
						Config::Tracking = false;
						Config::Flick = false;
						Config::silent = false;
						Config::triggerbot = true;
						Config::hanzo_flick = false;
					}
					int dec2 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("autoshootonoff"), 0, _T(".\\config.ini"));
					if (dec2 == 0) Config::AutoShoot = false;
					else Config::AutoShoot = true;
					int dec3 = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("predictdec"), 0, _T(".\\config.ini"));
					if (dec3 == 0) Config::Prediction = false;
					else Config::Prediction = true;

					if (local_entity.HeroID != eHero::HERO_GENJI && (Config::GenjiBlade == true || Config::AutoShiftGenji == true)) {
						Config::GenjiBlade = false;
						Config::AutoShiftGenji = false;
					}
					if (local_entity.HeroID == eHero::HERO_GENJI) {
						Config::GenjiBlade = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("GenjiBlade"), 0, _T(".\\config.ini"));
						Config::AutoShiftGenji = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), 0, _T(".\\config.ini"));
						Config::bladespeed = float(GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("bladespeed"), 5000, _T(".\\config.ini"))) / 10000.f;
					}
					if (local_entity.HeroID == eHero::HERO_WIDOWMAKER) {
						Config::widowautounscope = GetPrivateProfileInt(_T(GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str()), _T("widowautounscope"), 0, _T(".\\config.ini"));
					}
					if (local_entity.HeroID != eHero::HERO_WIDOWMAKER && Config::widowautounscope) {
						Config::widowautounscope = false;
					}
					/*if (timepassed - timetobegin >= 50000) {
						_beginthread((_beginthread_proc_type)entity_scan_thread, 0, 0);
						_beginthread((_beginthread_proc_type)entity_thread, 0, 0);
						_beginthread((_beginthread_proc_type)viewmatrix_thread, 0, 0);
						_beginthread((_beginthread_proc_type)aimbot_thread, 0, 0);
						_beginthread((_beginthread_proc_type)overlay_thread, 0, 0);
						timetobegin = GetTickCount();
					}*/
					Config::lastheroid = local_entity.HeroID;
					Sleep(2);
					std::string saveheroname = GetHeroEngNames(local_entity.HeroID, local_entity.LinkBase).c_str();
					Config::nowhero = skCrypt(u8"Now using:").decrypt() + saveheroname;
					saveheroname = skCrypt(u8"Loaded:").decrypt() + saveheroname;
					ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
				}
			}
			else if (Config::manualsave == true && Config::lastheroid != 0) {
				_stprintf(bufsave, _T("%d"), Config::spinbot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("spinbot"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::highPriority);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("highPriority"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::hanzoautospeed);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hanzoautospeed"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_hp_pack);
				WritePrivateProfileString(_T("Global"), _T("draw_hp_pack"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::aiaim);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aiaim"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::autoscalefov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoscalefov"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::crosscircle);
				WritePrivateProfileString(_T("Global"), _T("crosscircle"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::eyeray);
				WritePrivateProfileString(_T("Global"), _T("eyeray"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::lockontarget);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("lockontarget"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::trackcompensate);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("trackc"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::comarea * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comarea"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::comspeed * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("comspeed"), bufsave, _T(".\\config.ini"));


				_stprintf(bufsave, _T("%d"), (int)Config::Fov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("FOV"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::hitbox * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::missbox * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("missbox"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::Shoottime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShootTime"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::predit_level);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::aim_key);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::Gravitypredit);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
				int dec = 0;
				if (Config::Tracking) dec = 0;
				else if (Config::Flick) dec = 1;
				else if (Config::hanzo_flick) dec = 2;
				else if (Config::silent) dec = 3;
				else if (Config::triggerbot) dec = 4;
				int autoshoot = 0;
				if (Config::AutoShoot) autoshoot = 1;
				else autoshoot = 0;
				int predictdec = 0;
				if (Config::Prediction) predictdec = 1;
				else predictdec = 0;
				_stprintf(bufsave, _T("%d"), dec);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Aim Mode"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), autoshoot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autoshootonoff"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), predictdec);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predictdec"), bufsave, _T(".\\config.ini"));
				int dontshot = 0;
				if (Config::dontshot) dontshot = 1;
				else dontshot = 0;
				_stprintf(bufsave, _T("%d"), dontshot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontshot"), bufsave, _T(".\\config.ini"));
				int targetdelay = 0;
				if (Config::targetdelay) targetdelay = 1;
				else targetdelay = 0;
				_stprintf(bufsave, _T("%d"), targetdelay);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelay"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::targetdelaytime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("targetdelaytime"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::shotmanydont);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("dontmanyshot"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::hitboxdelayshoot);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelayshoot"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::hiboxdelaytime);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitboxdelaytime"), bufsave, _T(".\\config.ini"));
				Config::manualsave = false;
				if (Config::lastheroid == eHero::HERO_GENJI) {
					_stprintf(bufsave, _T("%d"), Config::GenjiBlade);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("GenjiBlade"), bufsave, _T(".\\config.ini"));
					_stprintf(bufsave, _T("%d"), Config::AutoShiftGenji);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoShiftGenji"), bufsave, _T(".\\config.ini"));
					_stprintf(bufsave, _T("%d"), (int)(Config::bladespeed * 10000));
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("bladespeed"), bufsave, _T(".\\config.ini"));
				}

				if (Config::lastheroid == eHero::HERO_WIDOWMAKER) {
					_stprintf(bufsave, _T("%d"), Config::widowautounscope);
					WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("widowautounscope"), bufsave, _T(".\\config.ini"));
				}

				_stprintf(bufsave, _T("%d"), Config::Bone);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::autobone);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::Bone2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Bone2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::autobone2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("autobone2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::AutoMelee);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("AutoMelee"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::norecoil);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("norecoil"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::recoilnum * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("recoilnum"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::accvalue * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::horizonreco);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("horizonreco"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Gravitypredit);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::switch_team);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::switch_team2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("switch_team2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::secondaim);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("secondaim"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Tracking2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Flick2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Prediction2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Prediction2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::Gravitypredit2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Gravitypredit2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::aim_key2);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("aim_key2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)Config::togglekey);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("togglekey"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::predit_level2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("predit_level2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Tracking_smooth2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Tracking_smooth2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Flick_smooth2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Flick_smooth2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::accvalue2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("accvalue2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::hitbox2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("hitbox2"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::Fov2 * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("Fov2"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::enablechangefov);
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("enablechangefov"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::CHANGEFOV * 10000));
				WritePrivateProfileString(_T(GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str()), _T("CHANGEFOV"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::backtracksecond * 10000));
				WritePrivateProfileString(_T("Global"), _T("backtime"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), Config::trackback);
				WritePrivateProfileString(_T("Global"), _T("trackback"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_info);
				WritePrivateProfileString(_T("Global"), _T("draw_info"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawbattletag);
				WritePrivateProfileString(_T("Global"), _T("drawbattletag"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawhealth);
				WritePrivateProfileString(_T("Global"), _T("drawhealth"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::healthbar);
				WritePrivateProfileString(_T("Global"), _T("healthbar"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::dist);
				WritePrivateProfileString(_T("Global"), _T("dist"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::name);
				WritePrivateProfileString(_T("Global"), _T("name"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::ult);
				WritePrivateProfileString(_T("Global"), _T("ult"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_skel);
				WritePrivateProfileString(_T("Global"), _T("draw_skel"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::skillinfo);
				WritePrivateProfileString(_T("Global"), _T("skillinfo"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::outline);
				WritePrivateProfileString(_T("Global"), _T("outline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::externaloutline);
				WritePrivateProfileString(_T("Global"), _T("externaloutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::teamoutline);
				WritePrivateProfileString(_T("Global"), _T("teamoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::healthoutline);
				WritePrivateProfileString(_T("Global"), _T("healthoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::rainbowoutline);
				WritePrivateProfileString(_T("Global"), _T("rainbowoutline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_edge);
				WritePrivateProfileString(_T("Global"), _T("draw_edge"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawbox3d);
				WritePrivateProfileString(_T("Global"), _T("drawbox3d"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::radar);
				WritePrivateProfileString(_T("Global"), _T("radar"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::radarline);
				WritePrivateProfileString(_T("Global"), _T("radarline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::drawline);
				WritePrivateProfileString(_T("Global"), _T("drawline"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), Config::draw_fov);
				WritePrivateProfileString(_T("Global"), _T("draw_fov"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColy"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::EnemyCol.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("EnemyColw"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcoly"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcolw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2x"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2y"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2z"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::fovcol2.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("fovcol2w"), bufsave, _T(".\\config.ini"));

				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::invisenargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("invisenargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::enargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("enargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargbw"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2x"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2y"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2z"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::targetargb2.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("targetargb2w"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.x * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbx"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.y * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargby"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.z * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbz"), bufsave, _T(".\\config.ini"));
				_stprintf(bufsave, _T("%d"), (int)(Config::allyargb.w * 10000));
				WritePrivateProfileString(_T("Global"), _T("allyargbw"), bufsave, _T(".\\config.ini"));
				std::string saveheroname = GetHeroEngNames(Config::lastheroid, local_entity.LinkBase).c_str();
				saveheroname = skCrypt(u8"Saved:").decrypt() + saveheroname;
				ImGui::InsertNotification({ ImGuiToastType_Success, 5000,saveheroname.data() , "" });
			}
			//Config::lastheroid = local_entity.HeroID;
			Sleep(2);
		}
	}

	HANDLE hMainThread = USEVEH->GetMainThreadHandle();
	CONTEXT c{};
	int i = 0;
	LONG WINAPI ExceptionHandler(EXCEPTION_POINTERS* e)
	{
		if (e->ExceptionRecord->ExceptionAddress == (void*)(OW::SDK->dwGameBase + OW::offset::GetKeyAdd))
		{

			if (Config::got == 0) {
				SDK->GlobalKey1 = *(uint64_t*)(e->ContextRecord->Rsp + 0x50);
				SDK->GlobalKey2 = *(uint64_t*)(e->ContextRecord->Rsp + 0x58);
				//printf("key1: %llx, key2: %llx\n", key1, key2);
				std::cout << skCrypt("Everything Loaded and Welcome.\n");
				Config::got = 1;
			}
			e->ContextRecord->Rip += offset::GetKeyAddRIP;
			return EXCEPTION_CONTINUE_EXECUTION;
		}

		if (e->ExceptionRecord->ExceptionAddress == (void*)(OW::SDK->dwGameBase + OW::offset::GlowESP))
		{
			if (OW::Config::outline) {
				int ret = 0x14;
				e->ContextRecord->Rsp -= 0x30;
				uint16_t a1 = *(uint16_t*)(e->ContextRecord->Rdx + 0x40);
				uint8_t a2 = *(uint8_t*)(e->ContextRecord->Rbx + 0x78);
				if (a1 == 0x0250 || a1 == 0x01B3 || a1 == 0x02DA)
				{
					if (a2 != 0x0A && a2 != 0x11) ret = 0x16;
					else ret = (*(BYTE*)(e->ContextRecord->Rcx + 0x248) == 0) ? 0x14 : 0x16;
				}
				else
					ret = (*(BYTE*)(e->ContextRecord->Rcx + 0x248) == 0) ? 0x14 : 0x16;
				e->ContextRecord->Rdi = e->ContextRecord->Rdx;
				e->ContextRecord->Rbx = e->ContextRecord->Rcx;
				e->ContextRecord->R8 = e->ContextRecord->Rdx;
				e->ContextRecord->Rip += ret;
			}
			else {
				e->ContextRecord->Rsp -= 0x30;
				e->ContextRecord->Rip += 4;
			}
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		if (e->ExceptionRecord->ExceptionAddress == (void*)(OW::SDK->dwGameBase + OW::offset::Silent))
		{
			uint8_t ws, ad;
			//printf("0x13:%d\n", *(uint8_t*)(e->ContextRecord->R10 + 0x13));
			//printf("0x14:%d\n", *(uint8_t*)(e->ContextRecord->R10 + 0x14));
			// 0x14: 127 w 129 s,0x13 127 d 129 a

			//printf("%d\n", t);

			if (OW::Config::trackback) {
				int trackz = *(uint16_t*)(e->ContextRecord->R10 + 0x20);
				//int trackz1 = OW::Config::tracktime*64;
				int trackz1 = OW::Config::tracktime * trackz;
				//int trackz1 = OW::Config::tracktime * 32;
				//if (trackz1 > 80) trackz1 = 80;
				//if (trackz1 <= 0) trackz1 = -trackz1;
				trackz += trackz1;
				*(uint16_t*)(e->ContextRecord->R10 + 0x20) = trackz;
			}
			//if (trackz == 1) trackz = 250;
			//trackz--;
			//printf("%d\n", trackback);
			if (Config::silent) {
				auto vec = GetVector3(Config::Prediction ? true : false);
				if (GetAsyncKeyState(Config::aim_key) && !Config::shooted && !Config::reloading && vec != Vector3(0, 0, 0) && !(entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI && entities[Config::Targetenemyi].skill2act) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team))
				{
					auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
					auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
					auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
					auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, 75.0f / 100.0f);
					auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);

					if (Target != Vector3(0, 0, 0)) {
						Config::pitch = ANGLE2SHORT(RAD2DEG(atan2f((vec.Y - viewMatrix_xor.get_location().y) * -1.0f, sqrtf(((vec.Z - viewMatrix_xor.get_location().z) * (vec.Z - viewMatrix_xor.get_location().z)) + ((vec.X - viewMatrix_xor.get_location().x) * (vec.X - viewMatrix_xor.get_location().x))))));
						Config::yaw = ANGLE2SHORT(RAD2DEG(atan2f((vec.X - viewMatrix_xor.get_location().x), (vec.Z - viewMatrix_xor.get_location().z))));
						uint8_t mvoe1 = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						uint8_t mvoe2 = *(uint8_t*)(e->ContextRecord->R10 + 0x14);

						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = OW::Config::yaw;//d4a是yaw左右
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = OW::Config::pitch;//d4c是pitch上下
						*(uint8_t*)(e->ContextRecord->R10 + 0x13) = mvoe1;
						*(uint8_t*)(e->ContextRecord->R10 + 0x14) = mvoe2;

						Config::didsilent = 1;
					}
				}
				else if (GetAsyncKeyState(Config::aim_key) && Config::silenttrace && !(entities[Config::Targetenemyi].HeroID == eHero::HERO_GENJI && entities[Config::Targetenemyi].skill2act) && ((!entities[Config::Targetenemyi].imort && !entities[Config::Targetenemyi].barrprot) || Config::switch_team)) {
					auto vec = GetVector3(Config::Prediction ? true : false);
					if (vec != Vector3(0, 0, 0)) {
						auto local_angle = SDK->RPM<Vector3>(SDK->g_player_controller + 0x12A0);
						auto calc_target = CalcAngle(XMFLOAT3(vec.X, vec.Y, vec.Z), viewMatrix_xor.get_location());
						auto vec_calc_target = Vector3(calc_target.x, calc_target.y, calc_target.z);
						auto Target = SmoothAccelerate(local_angle, vec_calc_target, Config::Flick_smooth / 10.f, 75.0f / 100.0f);
						auto local_loc = Vector3(viewMatrix_xor.get_location().x, viewMatrix_xor.get_location().y, viewMatrix_xor.get_location().z);

						if (Target != Vector3(0, 0, 0)) {
							Config::pitch = ANGLE2SHORT(RAD2DEG(atan2f((vec.Y - viewMatrix_xor.get_location().y) * -1.0f, sqrtf(((vec.Z - viewMatrix_xor.get_location().z) * (vec.Z - viewMatrix_xor.get_location().z)) + ((vec.X - viewMatrix_xor.get_location().x) * (vec.X - viewMatrix_xor.get_location().x))))));
							Config::yaw = ANGLE2SHORT(RAD2DEG(atan2f((vec.X - viewMatrix_xor.get_location().x), (vec.Z - viewMatrix_xor.get_location().z))));
							uint8_t mvoe1 = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
							uint8_t mvoe2 = *(uint8_t*)(e->ContextRecord->R10 + 0x14);

							*(uint64_t*)(e->ContextRecord->R10 + 0xa) = OW::Config::yaw;//d4a是yaw左右
							*(uint64_t*)(e->ContextRecord->R10 + 0xc) = OW::Config::pitch;//d4c是pitch上下
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = mvoe1;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = mvoe2;
						}
					}
				}
				else if (OW::Config::Antiaim && !(local_entity.HeroID != eHero::HERO_HANJO && GetAsyncKeyState(0x1)) && !GetAsyncKeyState(0xA0) && !GetAsyncKeyState(0x45) && !GetAsyncKeyState(0x51)) {
					short tempyaw = *(uint64_t*)(e->ContextRecord->R10 + 0xa);
					short temppitch = *(uint64_t*)(e->ContextRecord->R10 + 0xc);
					if (i == 0) {
						ws = *(uint8_t*)(e->ContextRecord->R10 + 0x14);
						ad = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						if (tempyaw > 49150) tempyaw = 0 + 16383 - (65535 - tempyaw);
						else tempyaw += 16383;
						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = (short)tempyaw;
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = (short)16202;
						if (ws == 127 && ad == 129) {//wa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 127 && ad == 127) {//wd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 129 && ad == 129) {//sa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 129 && ad == 127) {//sd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ws == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
						else if (ad == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
					}
					else if (i == 1) {
						ws = *(uint8_t*)(e->ContextRecord->R10 + 0x14);
						ad = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						if (tempyaw < 16385) tempyaw = 65535 - (16383 - tempyaw);
						else tempyaw -= 16383;
						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = tempyaw;
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = (short)16202;
						if (ws == 127 && ad == 129) {//wa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 127 && ad == 127) {//wd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 129 && ad == 129) {//sa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 129 && ad == 127) {//sd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ws == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
						else if (ad == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
					}
					else if (i == 2) {
						ws = *(uint8_t*)(e->ContextRecord->R10 + 0x14);
						ad = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						if (tempyaw > 49150) tempyaw = 0 + 16383 - (65535 - tempyaw);
						else tempyaw += 16383;
						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = (short)tempyaw;
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = (short)16202;
						if (ws == 127 && ad == 129) {//wa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 127 && ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 129 && ad == 129) {//sa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 129 && ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = (uint8_t)127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ws == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = (uint8_t)129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = (uint8_t)129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = (uint8_t)0;
						}
						else if (ad == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = (uint8_t)127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = (uint8_t)0;
						}
					}
					else if (i == 3) {
						ws = *(uint8_t*)(e->ContextRecord->R10 + 0x14);
						ad = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						//*(uint64_t*)(e->ContextRecord->R10 + 0xc) = 49334;
						if (tempyaw < 16385) tempyaw = 65535 - (16383 - tempyaw);
						else tempyaw -= 16383;
						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = tempyaw;
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = (short)16202;
						if (ws == 127 && ad == 129) {//wa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 127 && ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 129 && ad == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 129 && ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ws == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 0;
						}
						else if (ad == 127) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
						else if (ad == 129) {
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 0;
						}
					}
					else if (i == 4) {
						ws = *(uint8_t*)(e->ContextRecord->R10 + 0x14);
						ad = *(uint8_t*)(e->ContextRecord->R10 + 0x13);
						if (tempyaw < 32770) tempyaw = 65535 - (32766 - tempyaw);
						else tempyaw -= 32766;
						*(uint64_t*)(e->ContextRecord->R10 + 0xa) = tempyaw;
						*(uint64_t*)(e->ContextRecord->R10 + 0xc) = (short)16202;
						if (ws == 127 && ad == 129) {//wa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 127 && ad == 127) {//wd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 129 && ad == 129) {//sa
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
						}
						else if (ws == 129 && ad == 127) {//sd
							*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
							*(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						}
						else if (ws == 127)*(uint8_t*)(e->ContextRecord->R10 + 0x14) = 129;
						else if (ws == 129) *(uint8_t*)(e->ContextRecord->R10 + 0x14) = 127;
						else if (ad == 127) *(uint8_t*)(e->ContextRecord->R10 + 0x13) = 129;
						else if (ad == 129) *(uint8_t*)(e->ContextRecord->R10 + 0x13) = 127;
					}
					if (Config::spinbot)
						i++;
					else i = 4;
					if (i >= 5) i = 0;
				}
			}

			/*if (OW::Config::startsilent) {
				*(uint64_t*)(e->ContextRecord->R10 + 0xa) = OW::Config::yaw;//d4a是yaw左右
				*(uint64_t*)(e->ContextRecord->R10 + 0xc) = OW::Config::pitch;//d4c是pitch上下
				if(!Config::silenttrace){
					Config::didsilent = 1;
				}
			}*/
			//printf("0x13ad:%d\n", *(uint8_t*)(e->ContextRecord->R10 + 0x13));
			//printf("0x14ws:%d\n", *(uint8_t*)(e->ContextRecord->R10 + 0x14));
			//else *(uint64_t*)(e->ContextRecord->R10 + 0xc) = 939736;
			e->ContextRecord->Rip += 5;

			return EXCEPTION_CONTINUE_EXECUTION;
		}

		return EXCEPTION_CONTINUE_SEARCH;
	}

	void USEVEHIT()
	{
		OW::SDK->Initialize();
		hMainThread = USEVEH->GetMainThreadHandle();
		srand(GetTickCount());
		PVOID pHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
		c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		/*SuspendThread(hMainThread);
		c.Dr0 = OW::SDK->dwGameBase + OW::offset::GetKeyAdd;
		c.Dr1 = OW::SDK->dwGameBase + OW::offset::GlowESP;
		c.Dr2 = OW::SDK->dwGameBase + OW::offset::Silent;
		c.Dr7 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);
		SetThreadContext(hMainThread, &c);
		ResumeThread(hMainThread);*/
	}


	inline void looprpmthread() {
		USEVEHIT();
		int dohook = 0;
		srand(GetTickCount());
		PVOID pHandler = AddVectoredExceptionHandler(1, ExceptionHandler);
		c.ContextFlags = CONTEXT_DEBUG_REGISTERS;
		while (1) {
			if (OW::Config::silent && OW::Config::didsilent &&!Config::silenttrace) {
				if (local_entity.HeroID == eHero::HERO_HANJO) {
					if (local_entity.skill2act) {
						SetKeyHold(0x1, 40);
					}
					else {
						SetKeyHold(0x1000, 100);
					}
				}
				else {
					if ((local_entity.HeroID == eHero::HERO_ANA || local_entity.HeroID == eHero::HERO_WIDOWMAKER || local_entity.HeroID == eHero::HERO_ASHE) && GetAsyncKeyState(0x2))
						SetKeyscopeHold(0x1, 40);
					else SetKeyHold(0x1, 40);
				}
				Config::didsilent = 0;
				Config::shooted = true;
			}

			if (entities.size() > 0) {
				if (local_entity.AngleBase && (GetAsyncKeyState(Config::aim_key) || GetAsyncKeyState(Config::aim_key2) || GetAsyncKeyState(0x01) || GetAsyncKeyState(0x02))) {
					//if (SDK->g_player_controller) {
					if (Config::horizonreco) {
						//mutex.lock();
						SDK->WPM<float>(local_entity.AngleBase + 0x1768, 0);
						//mutex.unlock();
					}
					if (Config::norecoil) {
						//mutex.lock();
						SDK->WPM<float>(local_entity.AngleBase + 0x1764, Config::recoilnum);
						//mutex.unlock();
					}

				}
				if (Config::enablechangefov) {
					SDK->WPM<float>(SDK->dwGameBase + offset::changefov, Config::CHANGEFOV);
				}
			}

			if ((Config::silent || Config::outline||Config::got==0||Config::trackback)&&!dohook) {
				SuspendThread(hMainThread);
				c.Dr0 = OW::SDK->dwGameBase + OW::offset::GetKeyAdd;
				c.Dr1 = OW::SDK->dwGameBase + OW::offset::GlowESP;
				c.Dr2 = OW::SDK->dwGameBase + OW::offset::Silent;
				c.Dr7 = (1 << 0) | (1 << 2) | (1 << 4) | (1 << 6);
				SetThreadContext(hMainThread, &c);
				ResumeThread(hMainThread);
				dohook = 1;
			}
			else if (!Config::silent && !Config::outline && dohook && Config::got == 1&&!Config::trackback) {
				dohook = 0;
				SuspendThread(hMainThread);
				c.Dr0 = OW::SDK->dwGameBase + OW::offset::GetKeyAdd;
				c.Dr1 = OW::SDK->dwGameBase + OW::offset::GlowESP;
				c.Dr2 = OW::SDK->dwGameBase + OW::offset::Silent;
				c.Dr7 = 000000;
				SetThreadContext(hMainThread, &c);
				ResumeThread(hMainThread);
			}
			BOOL bPressed = FALSE;

			while (GetAsyncKeyState(VK_INSERT) & 0x8000)
			{
				if (!bPressed)
					Config::Menu = !Config::Menu;

				bPressed = TRUE;

				Sleep(10);
			}

			//float current_fov = MODULE->RPM<float>(MODULE->BaseAddress + offset::Fov);
			//bool result = MODULE->WPM<float>(MODULE->BaseAddress + offset::Fov, settings::fov_setting);
			Sleep(5);
			//SDK->WPM<short>(SDK->g_player_controller + 0xd4a, 5000);
			//SDK->WPM<short>(SDK->g_player_controller + 0xd4c, 5000);
		}
	}
	/*inline void playmusicthread() {
		TCHAR path[80] = _T(".\\ChocolateCream.mp3");//音乐文件位置
		HWND MyPlayer = hwnd;// MCIWndCreate(NULL, GetModuleHandle(NULL), 0, path);
		//mciSendString("setaudio .\\ChocolateCream.mp3 volume to 10", NULL, 0, NULL);
		//MCIWndSetVolume(MyPlayer, 30);
		int playing = 0;
		while (Config::musicplay) {
			if (playing == 0) {
				//mciSendString("setaudio .\\ChocolateCream.mp3 volume to 10", NULL, 0, NULL);
				mciSendString("play .\\ChocolateCream.mp3 repeat", NULL, 0, NULL);
				//mciSendString("setaudio .\\ChocolateCream.mp3 volume to 300", NULL, 0, NULL);
				//MCIWndSetRepeat(MyPlayer, TRUE);
				//MCIWndPlay(MyPlayer);//开始播放
				playing = 1;
				//while (Config::musicplay) {}//控制台程序下,让程序暂停一下或来个死循环等,要不要直接退出,没法播放
			}
			if (!Config::musicplay) {
				//MCIWndStop(MyPlayer);
				mciSendString("pause .\\ChocolateCream.mp3", NULL, 0, NULL);
				playing = 0;
			}
		}
	}
*/
}
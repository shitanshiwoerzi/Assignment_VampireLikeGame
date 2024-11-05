﻿#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
#include <fstream>
#include<string>
#include <cmath>
#include <corecrt_math_defines.h>
#include "global.h"
#include "Vector2D.h"
#include "camera.h"
#include "health.h"

using namespace GamesEngineeringBase;
using namespace std;

namespace LogicBase {
	const int maxSiz = 10000;

	class Character {
	protected:
		Health* hp;
		float speed;
		string filename;

	public:
		Vector2D pos;
		Image sprite;

		Character(int _x, int _y, string _filename) {
			hp = new Health(100.f);
			speed = 0.f;
			pos.x = _x;
			pos.y = _y;
			filename = _filename;
			sprite.load(filename);
		}

		// default
		void update(float _x, float _y) {
			pos.x += _x;
			pos.y += _y;
		}

		// npc update
		virtual void update(Window& canvas, Character& h, float dt) {
			Vector2D vec = h.pos - pos;
			vecNormalize(vec);
			float arg = speed * dt * 10;
			pos.x += vec.x * arg;
			pos.y += vec.y * arg;
		}

		// hero update
		virtual void update(int _x, int _y, float mapWidth, float mapHeight, Camera& cm) {
			pos.x += _x;
			pos.y += _y;

			if (pos.x < 0) pos.x = 0;
			if (pos.y < 0) pos.y = 0;
			if (pos.x + sprite.width > mapWidth) pos.x = mapWidth - sprite.width;
			if (pos.y + sprite.height > mapHeight) pos.y = mapHeight - sprite.height;
			cm.update(pos.x, pos.y, mapWidth, mapHeight, sprite);
		}

		virtual void draw(Window& canvas, Camera& cm) {
			// draw the npc
			renderImg(canvas, sprite, pos, cm);
		}

		bool collide(Character& h) {
			if (checkImageCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
				return true;
			return false;
		}

		Health& getHealth() {
			return *hp;
		}

		string getFilename() {
			return filename;
		}

		virtual void save(ofstream& binout) {
			hp->save(binout);
			pos.save(binout);
			binout.write((char*)&speed, sizeof(speed));
		}

		virtual void load(ifstream& infile) {
			hp->load(infile);
			pos.load(infile);
			infile.read((char*)&speed, sizeof(speed));
		}
	};

	class Projectile {
		Vector2D vel;
	public:
		Image sprite;
		Vector2D pos;
		float speed = 15.f;


		Projectile(string filename) { sprite.load(filename); }

		Projectile(Vector2D _pos, Vector2D _vel, string filename) {
			pos = _pos;
			vel = _vel;
			sprite.load(filename);
		}

		void update(int _speed, float dt) {
			speed = _speed;
			vecNormalize(vel);
			float arg = speed * dt * 10;
			pos.x += vel.x * arg;
			pos.y += vel.y * arg;
		}

		void draw(Window& canvas, Camera& cm) {
			renderImg(canvas, sprite, pos, cm);
		}

		bool collide(Character& h) {
			if (&h == nullptr) return false;
			if (this == nullptr) return false;
			if (checkImageCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
				return true;
			return false;
		}

		void save(ofstream& binout) {
			vel.save(binout);
			pos.save(binout);
			binout.write((char*)&speed, sizeof(speed));
		}

		void load(ifstream& infile) {
			vel.load(infile);
			pos.load(infile);
			infile.read((char*)&speed, sizeof(speed));
		}
	};

	class npc1 : public Character {
	public:
		npc1(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete hp;
			hp = new Health(100.f);
			speed = 10.f;
		}
	};

	class npc2 : public Character {
	public:
		npc2(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete hp;
			hp = new Health(200.f);
			speed = 5.f;
		}
	};

	/// <summary>
	/// static npc but can launch projectiles
	/// </summary>
	class npc3 : public Character {
		float timeElapsed = 0.f;

	public:
		int currentSize = 0;
		Projectile* parray[maxSiz];

		npc3(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete hp;
			hp = new Health(50.f);
			speed = 0.f;
		}

		void generateProjectile(Character& h, float dt) {
			timeElapsed += dt;
			if (timeElapsed > 4.f) {
				Projectile* p = new Projectile(pos, h.pos - pos, "Resources/npcProjectile.png");
				parray[currentSize++] = p;
				timeElapsed = 0.f;
			}
		}

		// 删除子弹
		void deleteProjectile(int i) {
			Projectile* p = parray[i];
			parray[i] = nullptr;
			delete p;
		}

		void update(float mapWidth, float mapHeight, Character& h, float dt) {
			// 定时生成子弹
			generateProjectile(h, dt);

			// 子弹移动
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update(parray[i]->speed, dt);
					if (parray[i]->pos.y + 2 > static_cast<int>(mapHeight)
						|| parray[i]->pos.x + 2 > static_cast<int>(mapWidth)
						|| parray[i]->pos.y - 2 < 0
						|| parray[i]->pos.x - 2 < 0) {
						deleteProjectile(i);
					}
				}
			}
		}

		// 渲染
		void draw(Window& canvas, Camera& cm) override {
			// draw the npc
			renderImg(canvas, sprite, pos, cm);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
					parray[i]->draw(canvas, cm);
			}
		}

		void save(ofstream& binout) override {
			Character::save(binout);
			binout.write((char*)&timeElapsed, sizeof(timeElapsed));
			binout.write((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists = (parray[i] != nullptr);
				binout.write((char*)&exists, sizeof(exists));
				if (exists) {
					parray[i]->save(binout);
				}
			}
		}

		void load(ifstream& infile) override {
			Character::load(infile);
			infile.read((char*)&timeElapsed, sizeof(timeElapsed));
			infile.read((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists;
				infile.read((char*)&exists, sizeof(exists));
				if (exists) {
					if (parray[i] == nullptr) {
						parray[i] = new Projectile("Resources/npcProjectile.png");
					}
					parray[i]->load(infile);
				}
				else {
					if (parray[i] != nullptr) {
						delete parray[i];
						parray[i] = nullptr;
					}
				}
			}
		}
	};

	class npc4 : public Character {
	public:
		npc4(int _x, int _y, string _filename) :Character(_x, _y, _filename) {
			delete hp;
			hp = new Health(150.f);
			speed = 15.f;
		}
	};

	class powerUp {
		Vector2D pos;
		Image sprite;
	public:
		//powerUp() {}

		powerUp(int x, int y) {
			pos.x = x;
			pos.y = y;
			sprite.load("Resources/item_Powerup.png");
		}

		bool collide(Character& h) {
			if (&h == nullptr) return false;
			if (this == nullptr) return false;
			if (checkImageCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
				return true;
			return false;
		}

		void draw(Window& canvas, Camera& cm) {
			renderImg(canvas, sprite, pos, cm);
		}

		void save(ofstream& binout) {
			pos.save(binout);
		}

		void load(ifstream& infile) {
			pos.load(infile);
		}
	};

	class swarm {
		float timeElapsed = 0.f;
		float timeThreshold = 3.f;

	public:
		int currentSize = 0;
		Character* sarray[maxSiz];

		swarm() {
			for (int i = 0; i < maxSiz; ++i) {
				sarray[i] = nullptr;
			}
		}

		// 删除Npc
		void deleteNpc(Window& canvas, int i) {
			Character* p = sarray[i];
			sarray[i] = nullptr;
			delete p;
		}

		// npc逻辑更新
		void update(Window& canvas, float dt, Character& h, float mapWidth, float mapHeight) {
			int npcType = rand() % 4;
			generateNpc(canvas, dt, npcType);
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					if (dynamic_cast<npc3*>(sarray[i])) {
						npc3* n3 = dynamic_cast<npc3*>(sarray[i]);
						n3->update(mapWidth, mapHeight, h, dt);
					}
					else
						sarray[i]->update(canvas, h, dt);
					// 待修改(npc跑到地图外后删除的条件需完善)
				}
			}
			checkCollision(canvas, h, dt);
		}

		// 绘制在canvas上
		void draw(Window& canvas, Camera& cm) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr)
					sarray[i]->draw(canvas, cm);
			}
		}

		void save(ofstream& binout) {
			binout.write((char*)&timeElapsed, sizeof(timeElapsed));
			binout.write((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists = (sarray[i] != nullptr);
				binout.write((char*)&exists, sizeof(exists));
				if (exists) {
					saveString(binout, sarray[i]->getFilename());
					sarray[i]->save(binout);
				}
			}
		}

		void load(ifstream& infile) {
			infile.read((char*)&timeElapsed, sizeof(timeElapsed));
			infile.read((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists;
				infile.read((char*)&exists, sizeof(exists));
				if (exists) {
					if (sarray[i] == nullptr) {
						string filename = loadString(infile); // 读取文件路径
						sarray[i] = new Character(0, 0, filename); // 使用文件路径创建对象
					}

					sarray[i]->load(infile);
				}
				else {
					if (sarray[i] != nullptr) {
						delete sarray[i];
						sarray[i] = nullptr;
					}
				}
			}
		}

	private:
		// 定时生成npc
		void generateNpc(Window& canvas, float dt, int npcType) {
			timeElapsed += dt;
			if (timeElapsed > timeThreshold) {
				Character* p;
				int direction = rand() % 4;
				int x, y; // generated postion
				switch (direction) {
				case 0: x = rand() % canvas.getWidth(); y = -200; break; // top
				case 1: x = rand() % canvas.getWidth(); y = canvas.getHeight() + 200; break; // bottom
				case 2: x = -200; y = rand() % canvas.getHeight(); break; // left
				case 3: x = canvas.getWidth() + 200; y = rand() % canvas.getHeight(); break; // right
				}

				if (npcType == 0) p = new npc1(x, y, "Resources/shoom.png");
				else if (npcType == 1) p = new npc2(x, y, "Resources/gab.png");
				else if (npcType == 2) p = new npc4(x, y, "Resources/skeleton.png");
				else p = new npc3(rand() % canvas.getWidth(), rand() % canvas.getHeight(), "Resources/wizard.png");

				if (p == nullptr) {
					cout << "Character is null" << endl;
					return;
				}

				sarray[currentSize++] = p;
				timeElapsed = 0.f;
				timeThreshold -= 0.1f;
				timeThreshold = max(0.5f, timeThreshold);
			}
		}

		// 生成的npc与hero的碰撞检测, 即npc的攻击碰撞检测(无projectile)
		void checkCollision(Window& canvas, Character& h, float dt) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					if (dynamic_cast<npc3*>(sarray[i])) {
						npc3* npc = dynamic_cast<npc3*>(sarray[i]);
						for (int j = 0; j < npc->currentSize; j++) {
							if (npc->parray[j]->collide(h)) {
								h.getHealth().takeDamage(20);
								cout << "hero was attacked";
								cout << ", current hp is " << h.getHealth().getHp();
								cout << endl;
								if (h.getHealth().isDead()) {
									cout << "game over" << endl;
									running = false;
								}
								npc->deleteProjectile(j);
							}
						}
					}
					if (sarray[i]->collide(h)) {
						timeElapsed += dt;
						if (timeElapsed > 1.f) {
							// 定时检测碰撞, 防止过多的碰撞判定
							h.getHealth().takeDamage(20);
							cout << "npc touched the hero " << i << endl;
							cout << "hero was attacked";
							cout << ", current hp is " << h.getHealth().getHp();
							cout << endl;
							timeElapsed = 0.f;
						}
						if (h.getHealth().isDead()) {
							cout << "game over" << endl;
							running = false;
						}
					}
				}
			}
		}
	};

	class hero : public Character {
		float timeElapsed = 0;
		float aoeTimeElapsed = 0;
		int currentFrame;
		float frameTime;
		float aoeCooldownDuration = 10.0f;
		Image frames[4];
	public:
		float projectileInterval = 1.5f;
		int currentSize = 0;
		Projectile* parray[maxSiz];

		hero(int _x, int _y, string filename) :Character(_x, _y, filename) {
			delete hp;
			hp = new Health(200.f);
			speed = 5.f;
			pos.x = _x;
			pos.y = _y;

			for (int i = 0; i < 4; ++i) {
				frames[i].load("Resources/hero_" + std::to_string(i + 1) + ".png");
			}
		}

		void hUpdate(Window& canvas, float _x, float _y, float dt, swarm& s, float mapWidth, float mapHeight, Camera& cm) {
			updateFrame(dt);
			generateProjectile(*this, dt, s);
			checkCollision(canvas, s, dt);

			// 子弹移动
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update(heroProjectileSpeed, dt);
					if (parray[i]->pos.y + 2 > static_cast<int>(mapHeight)
						|| parray[i]->pos.x + 2 > static_cast<int>(mapWidth)
						|| parray[i]->pos.y - 2 < 0
						|| parray[i]->pos.x - 2 < 0) {
						deleteProjectile(i);
					}
				}
			}
		}

		void draw(Window& canvas, Camera& cm) override {
			// draw hero
			renderImg(canvas, frames[currentFrame], pos, cm);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
					parray[i]->draw(canvas, cm);
			}
		}

		void save(ofstream& binout) {
			binout.write((char*)&timeElapsed, sizeof(timeElapsed));
			binout.write((char*)&aoeTimeElapsed, sizeof(aoeTimeElapsed));
			binout.write((char*)&currentSize, sizeof(currentSize));
			binout.write((char*)&speed, sizeof(speed));
			pos.save(binout);
			hp->save(binout);

			// ensure whether the parray[i] exists and save the bool variable
			for (int i = 0; i < currentSize; i++) {
				bool exists = (parray[i] != nullptr);
				binout.write((char*)&exists, sizeof(exists));
				if (exists) {
					parray[i]->save(binout);
				}
			}
		}

		void load(ifstream& infile) {
			infile.read((char*)&timeElapsed, sizeof(timeElapsed));
			infile.read((char*)&aoeTimeElapsed, sizeof(aoeTimeElapsed));
			infile.read((char*)&currentSize, sizeof(currentSize));
			infile.read((char*)&speed, sizeof(speed));
			pos.load(infile);
			hp->load(infile);

			for (int i = 0; i < currentSize; i++) {
				bool exists;
				infile.read((char*)&exists, sizeof(exists));
				if (exists) {
					if (parray[i] == nullptr) {
						parray[i] = new Projectile("Resources/blueProjectile.png");
					}
					parray[i]->load(infile);
				}
				else {
					if (parray[i] != nullptr) {
						delete parray[i];
						parray[i] = nullptr;
					}
				}
			}
		}

	private:
		void updateFrame(float dt) {
			// 更新动画帧
			frameTime += dt;
			if (frameTime >= 0.5f) {
				currentFrame = (currentFrame + 1) % 4;
				frameTime = 0.f;
			}
		}

		// find the closest npc
		int findClosest(Character& h, swarm& s) {
			int closestIndex = -1;
			float minDistance = 1e30f;
			for (int i = 0; i < s.currentSize; i++) {
				if (s.sarray[i] != nullptr) {
					float dis = h.pos.distance(s.sarray[i]->pos);
					if (dis < minDistance) {
						minDistance = dis;
						closestIndex = i;
					}
				}
			}
			return closestIndex;
		}

		// hero的攻击碰撞检测
		void checkCollision(Window& canvas, swarm& s, float dt) {
			// linear attack 
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					for (int j = 0; j < s.currentSize; j++) {
						if (parray[i]->collide(*s.sarray[j])) {
							deleteProjectile(i);
							s.sarray[j]->getHealth().takeDamage(50.f);
							if (s.sarray[j]->getHealth().isDead())
								s.deleteNpc(canvas, j);
						}

					}
				}
			}

			// aoe attack, aoeTriggered的作用是让技能仅触发一次
			aoeTimeElapsed += dt;
			if (canvas.keyPressed('J') && aoeTimeElapsed >= aoeCooldownDuration) {
				if (!aoeTriggered) {
					cout << "Use Aoe skill" << endl;
					triggerAOE(canvas, s, aoeNumber); // the number should not be fixed
					aoeTriggered = true;
					aoeTimeElapsed = 0;
				}
			}
			else if (canvas.keyPressed('J') && aoeTimeElapsed < aoeCooldownDuration) {
				if (!aoeTriggered) {
					float cd = aoeCooldownDuration - aoeTimeElapsed;
					cout << "Aoe cooldown left: " << cd << endl;
					aoeTriggered = true;
				}
			}
			else
				aoeTriggered = false;
		}

		void generateProjectile(Character& h, float dt, swarm& s) {
			timeElapsed += dt;
			Vector2D cp = pos + Vector2D(sprite.width / 2, sprite.height / 2);
			int closestIndex = findClosest(h, s);
			if (closestIndex != -1) {
				if (timeElapsed > projectileInterval) {
					int w = s.sarray[closestIndex]->sprite.width;
					int h = s.sarray[closestIndex]->sprite.height;
					Vector2D closest = s.sarray[closestIndex]->pos + Vector2D(w / 2, h / 2);
					Projectile* p = new Projectile(cp, closest - cp, "Resources/blueProjectile.png");
					parray[currentSize++] = p;
					timeElapsed = 0.f;
				}
			}
		}

		// 选择生命值最高的前N个NPC并对其进行攻击
		void triggerAOE(Window& canvas, swarm& s, int N) {
			for (int n = 0; n < N; n++) {
				int maxIndex = -1;
				float maxHealth = -100.f;
				// 找到当前最大生命值的NPC
				for (int i = 0; i < s.currentSize; i++) {
					if (s.sarray[i] != nullptr) {
						float hp = s.sarray[i]->getHealth().getHp();
						if (hp > maxHealth) {
							maxHealth = hp;
							maxIndex = i;
						}
					}
				}

				if (maxIndex != -1) {
					Character* npc = s.sarray[maxIndex];
					// 对该NPC进行攻击
					cout << "Attacking NPC" << typeid(*s.sarray[maxIndex]).name()
						<< "with health : " << npc->getHealth().getHp() << endl;
					// to-do 实现aoe的视觉效果

					// 假设攻击减少50点生命值
					npc->getHealth().takeDamage(200);
					if (npc->getHealth().isDead())
						s.deleteNpc(canvas, maxIndex);
				}
			}
		}

		// 删除子弹
		void deleteProjectile(int i) {
			Projectile* p = parray[i];
			parray[i] = nullptr;
			delete p;
		}
	};

	class items {
		float timeElapsed = 0.f;
		float timeThreshold = 10.f;

	public:
		int currentSize = 0;
		powerUp* upItems[maxSiz];

		items() {
			for (int i = 0; i < maxSiz; ++i) {
				upItems[i] = nullptr;
			}
		}

		void update(Window& canvas, float dt, hero& h) {
			generateItem(canvas, dt);
			for (int i = 0; i < currentSize; i++) {
				// 碰撞到增强道具并获得增强效果
				if (upItems[i]->collide(h)) {
					heroProjectileSpeed += 10.f;
					h.projectileInterval -= 0.2f;
					heroProjectileSpeed = min(heroProjectileSpeed, 200.f);
					h.projectileInterval = max(0.1f, h.projectileInterval);
					aoeNumber += 1;
					deleteItem(i);
				}
			}
		}

		void draw(Window& canvas, Camera& cm) {
			for (int i = 0; i < currentSize; i++) {
				if (upItems[i] != nullptr)
					upItems[i]->draw(canvas, cm);
			}
		}

		void save(ofstream& binout) {
			binout.write((char*)&timeElapsed, sizeof(timeElapsed));
			binout.write((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists = (upItems[i] != nullptr);
				binout.write((char*)&exists, sizeof(exists));
				if (exists) {
					upItems[i]->save(binout);
				}
			}
		}

		void load(ifstream& infile) {
			infile.read((char*)&timeElapsed, sizeof(timeElapsed));
			infile.read((char*)&currentSize, sizeof(currentSize));
			for (int i = 0; i < currentSize; i++) {
				bool exists;
				infile.read((char*)&exists, sizeof(exists));
				if (exists) {
					if (upItems[i] == nullptr) {
						upItems[i] = new powerUp(0, 0);
					}
					upItems[i]->load(infile);
				}
				else {
					if (upItems[i] != nullptr) {
						delete upItems[i];
						upItems[i] = nullptr;
					}
				}
			}
		}

	private:
		void generateItem(Window& canvas, float dt) {
			timeElapsed += dt;
			if (timeElapsed > timeThreshold) {
				powerUp* p = new powerUp(rand() % canvas.getWidth(), rand() % canvas.getHeight());
				if (p == nullptr) return;
				upItems[currentSize++] = p;
				timeElapsed = 0.f;
			}
		}

		void deleteItem(int i) {
			powerUp* p = upItems[i];
			upItems[i] = nullptr;
			delete p;
		}
	};
}
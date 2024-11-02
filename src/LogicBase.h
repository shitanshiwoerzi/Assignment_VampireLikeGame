#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
#include <cmath>
#include <corecrt_math_defines.h>
#include "global.h"

using namespace GamesEngineeringBase;
using namespace std;

namespace LogicBase {
	const int maxSiz = 1000;

	// 图片像素级碰撞检测
	bool checkImageCollision(Image& img1, unsigned int x1, unsigned int y1, Image& img2, unsigned int x2, unsigned int y2)
	{
		// 计算图片的重叠区域
		unsigned int x_overlap_start = max(x1, x2);
		unsigned int y_overlap_start = max(y1, y2);
		unsigned int x_overlap_end = min(x1 + img1.width, x2 + img2.width);
		unsigned int y_overlap_end = min(y1 + img1.height, y2 + img2.height);

		// 如果没有重叠, 返回false
		if (x_overlap_start >= x_overlap_end || y_overlap_start >= y_overlap_end) return false;

		// Check each pixel in the overlapping area
		for (unsigned int y = y_overlap_start; y < y_overlap_end; ++y)
		{
			for (unsigned int x = x_overlap_start; x < x_overlap_end; ++x)
			{
				//Get the relative positions in each image, check if both pixels are non-transparent
				if (img1.alphaAt(x - x1, y - y1) > 0 && img2.alphaAt(x - x2, y - y2) > 0)
					return true;
			}
		}

		return false;
	}

	class Vector2D {
	public:
		float x;
		float y;

		Vector2D() {
			x = 0;
			y = 0;
		}

		Vector2D(float _x, float _y) {
			x = _x;
			y = _y;
		}

		// 返回两点之间的距离
		float distance(Vector2D pos) {
			return sqrtf(pow(x - pos.x, 2) + pow(y - pos.y, 2));
		}

		Vector2D operator+(Vector2D const& vec) {
			Vector2D res;
			res.x = x + vec.x;
			res.y = y + vec.y;
			return res;
		}

		Vector2D operator-(Vector2D const& vec) {
			Vector2D res;
			res.x = x - vec.x;
			res.y = y - vec.y;
			return res;
		}
	};

	class Shape {
	public:
		virtual bool inside(Vector2D vec) = 0;
		virtual float area() = 0;
		virtual void Draw(Window& canvas) = 0;
	};

	class Circle : public Shape {
	public:
		Vector2D cp;
		float radius;

		Circle() {
			cp = Vector2D(0, 0);
			radius = 0;
		}

		Circle(Vector2D _cp, float _radius) {
			cp = _cp;
			radius = _radius;
		}

		bool inside(Vector2D vec) {
			// (point - cp )length
			float length = sqrtf(pow((vec.x - cp.x), 2) + pow((vec.y - cp.y), 2));
			if (length < radius) return true;
			return false;
		}

		float area() {
			return M_PI * pow(radius, 2);
		}

		void Draw(Window& canvas) {
			for (int i = (cp.x - radius); i < (cp.x + radius); i++) {
				for (int j = (cp.y - radius); j < (cp.y + radius); j++) {
					if (inside(Vector2D(i, j)))
						canvas.draw(i, j, 0, 255, 255);
				}
			}
		}
	};

	class Rect : public Shape {
	public:
		Vector2D s;
		Vector2D e;
		int r = 0;
		int g = 0;
		int b = 255;

		Rect() {
			s = Vector2D(0, 0);
			e = Vector2D(0, 0);
		}

		Rect(Vector2D _s, Vector2D _e) {
			s = _s;
			e = _e;
		}

		Rect(Vector2D _s, Vector2D _e, int _r, int _g, int _b) {
			s = _s;
			e = _e;
			r = _r;
			g = _g;
			b = _b;
		}
		bool inside(Vector2D vec) {
			if (vec.x > s.x && vec.x < e.x) {
				if (vec.y > s.y && vec.y < e.y) {
					return true;
				}
			}
			return false;
		}

		float area() {
			float xs;
			float ys;
			xs = e.x - s.x;
			ys = e.y - s.y;
			return(xs * ys);
		}

		void Draw(Window& canvas) {
			for (int i = s.x; i < e.x; i++) {
				if (i < canvas.getWidth() && i >= 0) {
					for (int j = s.y; j < e.y; j++) {
						if (j < canvas.getHeight() && j >= 0) {
							if (inside(Vector2D(i, j)))
								canvas.draw(i, j, r, g, b);
						}
					}
				}

			}
		}
	};

	class Health {
		float maxHp;
		float currentHp;
	public:
		Health() {}

		Health(float _max) {
			maxHp = _max;
			currentHp = maxHp;
		}

		void takeDamage(float damage) {
			currentHp -= damage;
			if (currentHp < 0) currentHp = 0;
		}

		void heal(float amount) {
			currentHp += amount;
			if (currentHp > maxHp) currentHp = maxHp;
		}

		bool isDead() const {
			return currentHp <= 0;
		}

		float getHp() const {
			return currentHp;
		}

		float getMaxHp() const {
			return maxHp;
		}
	};

	class Camera {
	public:
		Vector2D m_Position;

		float width;
		float height;

		Camera(int w, int h) : m_Position(Vector2D()), width(w), height(h) {}

		void update(float targetX, float targetY, Image& sprite) {
			// Center the camera on the target (player)
			m_Position.x = targetX + sprite.width - width / 2;
			m_Position.y = targetY + sprite.width - height / 2;
		}
	};

	class Tile {
		Image sprite;
	public:
		Tile() {}

		void load(string filename) {
			sprite.load(filename);
		}

		void draw(Window& canvas, int y) {
			for (unsigned int i = 0; i < sprite.height; i++) {
				// bounds checking
				if (y + i > 0 && (y + i) < canvas.getHeight()) {
					for (unsigned int j = 0; j < sprite.width; j++)
					{
						canvas.draw(j, y + i, sprite.atUnchecked(j, i));
					}
				}

			}
		}
	};

	const unsigned int tileSiz = 24;
	class TileSet {
		Tile tiles[tileSiz];
		int currentSize = 0;
		unsigned int size = tileSiz;
	public:
		TileSet() {}

		void load() {
			for (int i = 0; i < size; i++) {
				string filename;
				filename = "Resources/" + to_string(i) + ".png";
				tiles[i].load(filename);
			}
		}

		Tile& operator[](int index) { return tiles[index]; }
	};

	void renderImg(Window& canvas, Image& sprite, Vector2D& pos, Camera& cm) {
		for (unsigned int i = 0; i < sprite.height; i++) {
			if (pos.y + i - cm.m_Position.y < canvas.getHeight() && pos.y + i - cm.m_Position.y >= 0) {
				for (unsigned int n = 0; n < sprite.width; n++)
				{
					if (pos.x + n - cm.m_Position.x < canvas.getWidth() && pos.x + n - cm.m_Position.x >= 0) {
						if (sprite.alphaAt(n, i) > 200)
							canvas.draw(pos.x + n - cm.m_Position.x, pos.y + i - cm.m_Position.y, sprite.at(n, i));
					}
				}
			}
		}
	}


	class Character {
	protected:
		Health* hp;
		float speed;

	public:
		Vector2D pos;
		Image sprite;

		Character(int _x, int _y, string filename) {
			speed = 0.f;
			sprite.load(filename);
			pos.x = _x;
			pos.y = _y;
		}

		void update(float _x, float _y) {
			pos.x += _x;
			pos.y += _y;
		}

		virtual void update(Window& canvas, Character& h, float dt) {
			float arg = speed / 5000;
			Vector2D vec = h.pos - pos;
			pos.x += vec.x * arg;
			pos.y += vec.y * arg;
		}

		void update(Window& canvas, int _x, int _y) {
			pos.x += _x;
			pos.y += _y;

			if (pos.x < 0) pos.x = 0;
			if (pos.x < 0) pos.x = 0;
			if (pos.x + sprite.width > canvas.getWidth()) pos.x = canvas.getWidth() - sprite.width;
			if (pos.x + sprite.height > canvas.getHeight()) pos.x = canvas.getHeight() - sprite.height;
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
	};

	class Projectile {
	public:
		Image sprite;
		Vector2D pos;
		Vector2D vel;

		Projectile(Vector2D _pos, Vector2D _vel, string filename) {
			pos = _pos;
			vel = _vel;
			sprite.load(filename);
		}

		void update(int arg) {
			pos.x += vel.x / arg;
			pos.y += vel.y / arg;
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
	};

	class npc1 : public Character {
	public:
		npc1(int _x, int _y, string filename) :Character(_x, _y, filename) {
			hp = new Health(100.f);
			speed = 10.f;
		}
	};

	class npc2 : public Character {
	public:
		npc2(int _x, int _y, string filename) :Character(_x, _y, filename) {
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

		npc3(int _x, int _y, string filename) :Character(_x, _y, filename) {
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
			cout << "Destroyed: " << i << endl;
		}

		void update(Window& canvas, Character& h, float dt) override {
			// 定时生成子弹
			generateProjectile(h, dt);

			// 子弹移动
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update(300);
					if (parray[i]->pos.y + 2 > static_cast<int>(canvas.getHeight())
						|| parray[i]->pos.x + 2 > static_cast<int>(canvas.getWidth())
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
	};

	class npc4 : public Character {
	public:
		npc4(int _x, int _y, string filename) :Character(_x, _y, filename) {
			hp = new Health(150.f);
			speed = 15.f;
		}
	};

	class swarm {
		float timeElapsed = 0.f;
		float timeThreshold = 3.f;

	public:
		int currentSize = 0;
		Character* sarray[maxSiz];
		swarm() {}

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

		// 删除Npc
		void deleteNpc(Window& canvas, int i) {
			Character* p = sarray[i];
			sarray[i] = nullptr;
			delete p;
			cout << "Destroyed: " << i << endl;
		}

		// npc逻辑更新
		void npcUpdate(Window& canvas, float dt, Character& h) {
			int npcType = rand() % 4;
			generateNpc(canvas, dt, npcType);
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					sarray[i]->update(canvas, h, dt);
					//if (sarray[i]->pos.y > static_cast<int>(canvas.getHeight()))
					//	deleteNpc(canvas, i); // 待修改(npc跑到地图外后删除的条件需完善)
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

	private:
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
		int currentSize = 0;
		float projectileInterval = 1.f;
	public:
		Projectile* parray[maxSiz];

		hero(int _x, int _y, string filename) :Character(_x, _y, filename) {
			hp = new Health(200.f);
			speed = 5.f;
		}

		void heroUpdate(Window& canvas, float _x, float _y, float dt, swarm& s) {
			update(canvas, _x, _y);
			generateProjectile(*this, dt, s);
			checkCollision(canvas, s);

			// 子弹移动
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update(100);
					if (parray[i]->pos.y + 2 > static_cast<int>(canvas.getHeight())
						|| parray[i]->pos.x + 2 > static_cast<int>(canvas.getWidth())
						|| parray[i]->pos.y - 2 < 0
						|| parray[i]->pos.x - 2 < 0) {
						deleteProjectile(i);
					}
				}
			}
		}

		void draw(Window& canvas, Camera& cm) override {
			// draw the npc
			renderImg(canvas, sprite, pos, cm);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
					parray[i]->draw(canvas, cm);
			}
		}

	private:
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
		void checkCollision(Window& canvas, swarm& s) {
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

		// 删除子弹
		void deleteProjectile(int i) {
			Projectile* p = parray[i];
			parray[i] = nullptr;
			delete p;
			cout << "Destroyed: " << i << endl;
		}
	};
}
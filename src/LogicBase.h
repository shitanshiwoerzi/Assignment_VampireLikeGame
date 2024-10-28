#pragma once
#include "GamesEngineeringBase.h"
#include <iostream>
#include <cmath>
#include <corecrt_math_defines.h>

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

	void renderImg(Window& canvas, Image& sprite, Vector2D& pos) {
		for (unsigned int i = 0; i < sprite.height; i++)
			if (pos.y + i < canvas.getHeight() && pos.y + i >= 0) {
				for (unsigned int n = 0; n < sprite.width; n++)
				{
					if (pos.x + n < canvas.getWidth() && pos.x + n >= 0) {
						if (sprite.alphaAt(n, i) > 200)
							canvas.draw(pos.x + n, pos.y + i, sprite.at(n, i));
					}
				}
			}
	}


	class Character {
	protected:
		float hp;
		float speed;

	public:
		Vector2D pos;
		Image sprite;

		Character(int _x, int _y, string filename) {
			hp = 0.f;
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

		virtual void draw(Window& canvas) {
			// draw the npc
			renderImg(canvas, sprite, pos);
		}

		bool collide(Character& h) {
			if (checkImageCollision(sprite, pos.x, pos.y, h.sprite, h.pos.x, h.pos.y))
				return true;
			return false;
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

		void update() {
			pos.x += vel.x / 500;
			pos.y += vel.y / 500;
		}

		void draw(Window& canvas) {
			renderImg(canvas, sprite, pos);
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
			hp = 100.f;
			speed = 10.f;
		}
	};

	class npc2 : public Character {
	public:
		npc2(int _x, int _y, string filename) :Character(_x, _y, filename) {
			hp = 200.f;
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
			hp = 50.f;
			speed = 0.f;
		}

		void generateProjectile(Character& h, float dt) {
			timeElapsed += dt;
			if (timeElapsed > 1.f) {
				Projectile* p = new Projectile(pos, h.pos - pos, "Resources/projectile.png");
				parray[currentSize++] = p;
				timeElapsed = 0.f;
			}
		}

		// 删除子弹
		void deleteProjectile(Window& canvas, int i) {
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
					parray[i]->update();
					if (parray[i]->pos.y + 2 > static_cast<int>(canvas.getHeight())
						|| parray[i]->pos.x + 2 > static_cast<int>(canvas.getWidth())
						|| parray[i]->pos.y - 2 < 0
						|| parray[i]->pos.x - 2 < 0) {
						deleteProjectile(canvas, i);
					}
				}
			}
		}

		// 渲染
		void draw(Window& canvas) override {
			// draw the npc
			renderImg(canvas, sprite, pos);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
					parray[i]->draw(canvas);
			}
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
		void generateNpc(Window& canvas, float dt) {
			timeElapsed += dt;
			if (timeElapsed > 1.f) {
				Character* p;
				if (rand() % 2 == 0) p = new npc1(rand() % canvas.getWidth(), -200, "Resources/23.png");
				else if (rand() % 2 == 1) p = new npc2(rand() % canvas.getWidth(), -200, "Resources/22.png");
				else p = new npc3(rand() % canvas.getWidth(), rand() % canvas.getHeight(), "Resources/wizard.png");
				//switch (rand() % 3) {
				//case 0: p = new npc1(rand() % canvas.getWidth(), -200, "Resources/L2.png"); break;
				//case 1: p = new npc2(rand() % canvas.getWidth(), -200, "Resources/L.png"); break;
				//}
				if (p == nullptr) {
					cout << "Character is null" << endl;
					return;
				}

				sarray[currentSize++] = p;
				timeElapsed = 0.f;
				timeThreshold -= 0.2f;
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

		// npc移动
		void update(Window& canvas, float dt, Character& h) {
			generateNpc(canvas, dt);
			int move = static_cast<int>(500.f * dt);
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					sarray[i]->update(canvas, h, dt);
					if (sarray[i]->pos.y > static_cast<int>(canvas.getHeight()))
						deleteNpc(canvas, i); // 待修改(npc跑到地图外后删除的条件需完善)
				}
			}
		}

		// 绘制在canvas上
		void draw(Window& canvas) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr)
					sarray[i]->draw(canvas);
			}
		}

		// 生成的npc与hero的碰撞检测, 即npc的攻击碰撞检测(无projectile)
		void checkCollision(Window& canvas, Character& h) {
			for (int i = 0; i < currentSize; i++) {
				if (sarray[i] != nullptr) {
					if (dynamic_cast<npc3*>(sarray[i])) {
						npc3* npc = dynamic_cast<npc3*>(sarray[i]);
						for (int j = 0; j < npc->currentSize; j++) {
							if (npc->parray[j]->collide(h)) {
								// 待修改 (添加功能：hero受到远程子弹攻击会受伤)
								cout << "hero was attacked by a projectile" << endl;
								npc->deleteProjectile(canvas, j);
							}
						}
					}
					if (sarray[i]->collide(h)) {
						// (待添加功能) 如果碰到npc, hero掉血
						cout << "npc touched the hero " << i << endl;
						deleteNpc(canvas, i);
					}
				}
			}
		}
	};

	class hero : public Character {
		float timeElapsed = 0;
		int currentSize = 0;
	public:
		Projectile* parray[maxSiz];

		hero(int _x, int _y, string filename) :Character(_x, _y, filename) {
			hp = 200.f;
			speed = 5.f;
		}

		void update(Window& canvas, float _x, float _y, float dt) {
			pos.x += _x;
			pos.y += _y;

			generateProjectile(*this, dt);

			// 子弹移动
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					parray[i]->update();
					if (parray[i]->pos.y + 2 > static_cast<int>(canvas.getHeight())
						|| parray[i]->pos.x + 2 > static_cast<int>(canvas.getWidth())
						|| parray[i]->pos.y - 2 < 0
						|| parray[i]->pos.x - 2 < 0) {
						deleteProjectile(canvas, i);
					}
				}
			}
		}

		// hero的攻击碰撞检测
		void checkCollision(Window& canvas, swarm& s) {
			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr) {
					for (int j = 0; j < s.currentSize; j++) {
						if (parray[i]->collide(*s.sarray[j]))
							s.deleteNpc(canvas, j);
					}
				}
			}
		}

		void draw(Window& canvas) override {
			// draw the npc
			renderImg(canvas, sprite, pos);

			for (int i = 0; i < currentSize; i++) {
				if (parray[i] != nullptr)
					parray[i]->draw(canvas);
			}
		}

	private:
		void generateProjectile(Character& h, float dt) {
			timeElapsed += dt;
			Vector2D cp = pos + Vector2D(sprite.width / 2, sprite.height / 2);
			if (timeElapsed > 1.f) {
				Projectile* p = new Projectile(cp, Vector2D(500, 0), "Resources/blueProjectile.png");
				Projectile* p1 = new Projectile(cp, Vector2D(-500, 0), "Resources/blueProjectile.png");
				parray[currentSize++] = p;
				parray[currentSize++] = p1;
				timeElapsed = 0.f;
			}
		}

		// 删除子弹
		void deleteProjectile(Window& canvas, int i) {
			Projectile* p = parray[i];
			parray[i] = nullptr;
			delete p;
			cout << "Destroyed: " << i << endl;
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
}
#include "global.h"

bool running = true;
bool aoeTriggered = false; // status of AOE trigger key 'J' 
float heroProjectileSpeed = 100.f; 
int aoeNumber = 5;

// vector normalization
void vecNormalize(Vector2D& vec) {
	float length = std::sqrtf(std::pow(vec.x, 2) + std::pow(vec.y, 2));
	if (length != 0) {
		vec.x /= length;
		vec.y /= length;
	}
}

void saveString(std::ofstream& binout, const std::string& str) {
    size_t length = str.size();
    binout.write((char*)&length, sizeof(length)); // 写入字符串长度
    binout.write(str.c_str(), length); // 写入字符串内容
}

std::string loadString(std::ifstream& infile) {
	size_t length;
	infile.read((char*)&length, sizeof(length)); // 读取字符串长度
	if (!infile || length >= 99999) {
		std::cerr << "Error reading length from file." << std::endl;
		return "";
	}
	char* buffer = new char[length + 1];
	infile.read(buffer, length); // 读取字符串内容
	buffer[length] = '\0'; // 添加字符串结束符
	std::string str(buffer);
	delete[] buffer;
	return str;
}

void renderImg(GamesEngineeringBase::Window& canvas, GamesEngineeringBase::Image& sprite, Vector2D& pos, Camera& cm) {
	for (unsigned int i = 0; i < sprite.height; i++) {
		int cmY = pos.y + i - cm.m_Position.y;
		if (cmY < canvas.getHeight() && cmY >= 0) {
			for (unsigned int n = 0; n < sprite.width; n++)
			{
				int cmX = pos.x + n - cm.m_Position.x;
				if (cmX < canvas.getWidth() && cmX >= 0) {
					if (sprite.alphaAt(n, i) > 200)
						canvas.draw(cmX, cmY, sprite.at(n, i));
				}
			}
		}
	}
}

// 图片像素级碰撞检测
bool checkImageCollision(GamesEngineeringBase::Image& img1, unsigned int x1, unsigned int y1, GamesEngineeringBase::Image& img2, unsigned int x2, unsigned int y2)
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
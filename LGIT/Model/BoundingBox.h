#pragma once
namespace LGIT::Model {
	struct BoundingBox {
	public:
		int left;   // L (¿ŞÂÊ ÁÂÇ¥)
		int top;    // T (À§ÂÊ ÁÂÇ¥)
		int right;  // R (¿À¸¥ÂÊ ÁÂÇ¥)
		int bottom; // B (¾Æ·¡ÂÊ ÁÂÇ¥)
		int width() const { return right - left; }
		int height() const { return bottom - top; }
	};
}
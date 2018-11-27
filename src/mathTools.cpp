#include <math.h>

#define MinNum(x, y) ((x) < (y) ? (x) : (y))
#define MaxNum(x, y) ((x) > (y) ? (x) : (y))
#define PI 3.14159
#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define Clamp(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

struct Rect;
struct Matrix3;
struct Vec2;

float roundToNearest(float num, float nearest=1);
bool pointInRect(float px, float py, float rx, float ry, float rw, float rh);
bool circleIntersection(float x1, float y1, float r1, float x2, float y2, float r2);
bool pointInCircle(float px, float py, float cx, float cy, float r);

float rnd();
float rndFloat(float min, float max);
int rndInt(int min, int max);
bool rndBool();

float distanceBetween(float x1, float y1, float x2, float y2);
float radsBetween(float x1, float y1, float x2, float y2);
Vec2 vectorBetween(float x1, float y1, float x2, float y2);

float lerp(float min, float max, float perc);
Vec2 lerpVec2(Vec2 *min, Vec2 *max, float perc);
int lerpColour(int colour1, int colour2, float perc);

int argbToHex(unsigned char a, unsigned char r, unsigned char g, unsigned char b);

float toDeg(float rads);
float toRad(float degs);

enum Ease {
	LINEAR = 0,
	QUAD_IN, QUAD_OUT, QUAD_IN_OUT,
	CUBIC_IN, CUBIC_OUT, CUBIC_IN_OUT,
	QUART_IN, QUART_OUT, QUART_IN_OUT,
	QUINT_IN, QUINT_OUT, QUINT_IN_OUT,
	SINE_IN, SINE_OUT, SINE_IN_OUT,
	CIRC_IN, CIRC_OUT, CIRC_IN_OUT,
	EXP_IN, EXP_OUT, EXP_IN_OUT,
	ELASTIC_IN, ELASTIC_OUT, ELASTIC_IN_OUT,
	BACK_IN, BACK_OUT, BACK_IN_OUT,
	BOUNCE_IN, BOUNCE_OUT, BOUNCE_IN_OUT
};

float tweenEase(float p, Ease ease);

struct Vec2 {
	float x;
	float y;

	void setTo(float x=0, float y=0);
	void normalize(float scale=1);

	void add(Vec2 *other);
	void subtract(Vec2 *other);
	void multiply(Vec2 *other);
	void divide(Vec2 *other);

	void add(float other);
	void subtract(float other);
	void multiply(float other);
	void divide(float other);

	float distance(Vec2 *other);
	float distance(float x, float y);

	bool isZero();
};

struct Vec3 {
	float x;
	float y;
	float z;

	void setTo(float x=0, float y=0, float z=0);
};


struct Rect {
	float x;
	float y;
	float width;
	float height;

	void setTo(float x=0, float y=0, float width=0, float height=0);
	bool equals(Rect *other);
	bool intersects(Rect *other);
	bool contains(Vec2 *point);
	bool contains(float px, float py);
	float distanceToPerimeter(float px, float py);
	void randomVec2(Vec2 *point);
	void print();
};

struct Matrix3 {
	float data[9];

	void setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i);
	void setTo(float *array);

	void identity();
	void project(float width, float height);
	void multiply(float *array);
	void translate(float x, float y);
	void rotate(float deg);
	void scale(float x, float y);

	void print();
};

struct Matrix4 {
	float data[16];

	void setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p);
	void setTo(float *array);

	void identity();
	void project(float width, float height, float depth);
	void multiply(float *array);
	void translate(float x, float y, float z);
	void rotate2d(float deg);
	void scale(float x, float y, float z);

	void print();
};

/// End header

float roundToNearest(float num, float nearest) {
	return roundf(num / nearest) * nearest;
}

bool pointInRect(float px, float py, float rx, float ry, float rw, float rh) {
	return px >= rx && px <= rx+rw && py >= ry && py <= ry+rh;
}

bool circleIntersection(float x1, float y1, float r1, float x2, float y2, float r2) {
	float a = r1 + r2;
	float x = x1 - x2;
	float y = y1 - y2;

	return a > sqrt((x*x) + (y*y));
}

bool pointInCircle(float px, float py, float cx, float cy, float r) {
	return distanceBetween(px, py, cx, cy) <= r;
}

float rnd() {
	float max = 10000;
	float result = rndInt(0, max)/max;
	return result;
}

float rndFloat(float min, float max) {
	float result = min + rnd() * (max - min);
	return result;
}

int rndInt(int min, int max) {
	int result = rand() % (max + 1 - min) + min;
	return result;
}

inline bool rndBool() {
	return rnd() > 0.5;
}

float distanceBetween(float x1, float y1, float x2, float y2) {
	float a = x2 - x1;
	float b = y2 - y1;

	float c = sqrt(a*a + b*b);
	return c;
}

float radsBetween(float x1, float y1, float x2, float y2) {
	return atan2(y2 - y1, x2 - x1);
}

Vec2 vectorBetween(float x1, float y1, float x2, float y2) {
	float rads = radsBetween(x1, y1, x2, y2);

	Vec2 point = {(float)cos(rads), (float)sin(rads)};
	return point;
}

float lerp(float min, float max, float perc) {
	return min + (max - min) * perc;
}

Vec2 lerpVec2(Vec2 *min, Vec2 *max, float perc) {
	Vec2 point = {};
	point.x = lerp(min->x, max->x, perc);
	point.y = lerp(min->y, max->y, perc);
	return point;
}

int lerpColour(int colour1, int colour2, float perc) {
	unsigned char a1 = (colour1 >> 24) & 0xFF;
	unsigned char r1 = (colour1 >> 16) & 0xFF;
	unsigned char g1 = (colour1 >> 8) & 0xFF;
	unsigned char b1 = (colour1     ) & 0xFF;

	unsigned char a2 = (colour2 >> 24) & 0xFF;
	unsigned char r2 = (colour2 >> 16) & 0xFF;
	unsigned char g2 = (colour2 >> 8) & 0xFF;
	unsigned char b2 = (colour2     ) & 0xFF;

	return argbToHex(
		lerp(a1, a2, perc),
		lerp(r1, r2, perc),
		lerp(g1, g2, perc),
		lerp(b1, b2, perc)
	);
}

int argbToHex(unsigned char a, unsigned char r, unsigned char g, unsigned char b) {
	return ((a & 0xff) << 24) + ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

float toDeg(float rads) {
	return rads * 57.2958;
}

float toRad(float degs) {
	return degs * 0.0174533;
}

void Vec2::setTo(float x, float y) {
	this->x = x;
	this->y = y;
}

void Vec2::normalize(float scale) {
	float norm = sqrt(this->x*this->x + this->y*this->y);
	if (norm != 0) {
		this->x = scale * this->x / norm;
		this->y = scale * this->y / norm;
	}
}

void Vec2::add(Vec2 *other) {
	this->x += other->x;
	this->y += other->y;
}

void Vec2::subtract(Vec2 *other) {
	this->x -= other->x;
	this->y -= other->y;
}

void Vec2::multiply(Vec2 *other) {
	this->x *= other->x;
	this->y *= other->y;
}

void Vec2::divide(Vec2 *other) {
	this->x /= other->x;
	this->y /= other->y;
}

void Vec2::add(float other) {
	this->x += other;
	this->y += other;
}

void Vec2::subtract(float other) {
	this->x -= other;
	this->y -= other;
}

void Vec2::multiply(float other) {
	this->x *= other;
	this->y *= other;
}

void Vec2::divide(float other) {
	this->x /= other;
	this->y /= other;
}

float Vec2::distance(Vec2 *other) {
	return distanceBetween(this->x, this->y, other->x, other->y);
}

float Vec2::distance(float x, float y) {
	return distanceBetween(this->x, this->y, x, y);
}

bool Vec2::isZero() {
	return this->x == 0 && this->y == 0;
}

void Vec3::setTo(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

void Rect::setTo(float x, float y, float width, float height) {
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
}

bool Rect::equals(Rect *other) {
	return this->x == other->x && this->y == other->y && this->width == other->width && this->height == other->height;
}

bool Rect::intersects(Rect *other) {
	float r1x = this->x;
	float r1y = this->y;
	float r1w = this->width;
	float r1h = this->height;

	float r2x = other->x;
	float r2y = other->y;
	float r2w = other->width;
	float r2h = other->height;

	return r1x < r2x + r2w &&
		r1x + r1w > r2x &&
		r1y < r2y + r2h &&
		r1h + r1y > r2y;

	bool intercects = !(
		r2x > r1x+r1w ||
		r2x+r2w < r1x ||
		r2y > r1y+r1h ||
		r2y+r2h < r1y);

	//@cleanup Not sure this contains formula does what I think it does
	bool contains = 
		(r1x+r1w) < (r1x+r1w) &&
		(r2x) > (r1x) &&
		(r2y) > (r1y) &&
		(r2y+r2h) < (r1y+r1h);

	return intercects || contains;
}

bool Rect::contains(Vec2 *point) { return pointInRect(point->x, point->y, this->x, this->y, this->width, this->height); }
bool Rect::contains(float px, float py) { return pointInRect(px, py, this->x, this->y, this->width, this->height); }

float Rect::distanceToPerimeter(float px, float py) {
	Rect *rect = this;
	float l = rect->x;
	float t = rect->y;
	float r = rect->x + rect->width;
	float b = rect->y + rect->height;

	float x = Clamp(px, l, r);
	float y = Clamp(py, t, b);

	float dl = fabs(x-l);
	float dr = fabs(x-r);
	float dt = fabs(y-t);
	float db = fabs(y-b);

	float m;
	m = MinNum(dl, dr);
	m = MinNum(m, dt);
	m = MinNum(m, db);

	float minX;
	float minY;

	if (m == dt) {
		minX = x;
		minY = t;
	} else if (m == db) {
		minX = x;
		minY = b;
	} else if (m == dl) {
		minX = l;
		minY = y;
	} else {
		minX = r;
		minY = y;
	}

	return distanceBetween(minX, minY, px, py);
}

void Rect::randomVec2(Vec2 *point) {
	point->x = rndFloat(this->x, this->x + this->width);
	point->y = rndFloat(this->y, this->y + this->height);
}

void Rect::print() {
	printf("Rect: {%f, %f, %f, %f}\n", this->x, this->y, this->width, this->height);
}

void Matrix3::identity() {
	this->setTo(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
	);
}

void Matrix3::project(float width, float height) {
	float array[9] = {
		2/width,  0,          0,
		0,        -2/height,  0,
		-1,       1,          1
	};
	this->multiply(array);
}

void Matrix3::setTo(float *array) {
	this->data[0] = array[0];
	this->data[1] = array[1];
	this->data[2] = array[2];
	this->data[3] = array[3];
	this->data[4] = array[4];
	this->data[5] = array[5];
	this->data[6] = array[6];
	this->data[7] = array[7];
	this->data[8] = array[8];
}

void Matrix3::setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
	this->data[0] = a;
	this->data[1] = b;
	this->data[2] = c;
	this->data[3] = d;
	this->data[4] = e;
	this->data[5] = f;
	this->data[6] = g;
	this->data[7] = h;
	this->data[8] = i;
}

void Matrix3::translate(float x, float y) {
	float array[9] = {
		1, 0, 0,
		0, 1, 0,
		x, y, 1
	};
	this->multiply(array);
}

void Matrix3::rotate(float deg) {
	float s = sin(deg*PI/180);
	float c = cos(deg*PI/180);
	float array[9] = {
		c, -s, 0,
		s,  c, 0,
		0,  0, 1
	};
	this->multiply(array);
}


void Matrix3::scale(float x, float y) {
	float array[9] = {
		x, 0, 0,
		0, y, 0,
		0, 0, 1
	};
	this->multiply(array);
}

void Matrix3::multiply(float *array) {
	float temp[9] = {};

	temp[0] += this->data[0] * array[0] + this->data[3] * array[1] + this->data[6] * array[2];
	temp[1] += this->data[1] * array[0] + this->data[4] * array[1] + this->data[7] * array[2];
	temp[2] += this->data[2] * array[0] + this->data[5] * array[1] + this->data[8] * array[2];
	temp[3] += this->data[0] * array[3] + this->data[3] * array[4] + this->data[6] * array[5];
	temp[4] += this->data[1] * array[3] + this->data[4] * array[4] + this->data[7] * array[5];
	temp[5] += this->data[2] * array[3] + this->data[5] * array[4] + this->data[8] * array[5];
	temp[6] += this->data[0] * array[6] + this->data[3] * array[7] + this->data[6] * array[8];
	temp[7] += this->data[1] * array[6] + this->data[4] * array[7] + this->data[7] * array[8];
	temp[8] += this->data[2] * array[6] + this->data[5] * array[7] + this->data[8] * array[8];

	this->setTo(temp);
}

void Matrix3::print() {
	printf(
		"%0.2f\t%0.2f\t%0.2f\n"
		"%0.2f\t%0.2f\t%0.2f\n"
		"%0.2f\t%0.2f\t%0.2f\n",
		this->data[0], this->data[1], this->data[2],
		this->data[3], this->data[4], this->data[5],
		this->data[6], this->data[7], this->data[8]
	);
}

void Matrix4::identity() {
	this->setTo(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	);
}

void Matrix4::setTo(float *array) {
	this->data[0] = array[0];
	this->data[1] = array[1];
	this->data[2] = array[2];
	this->data[3] = array[3];
	this->data[4] = array[4];
	this->data[5] = array[5];
	this->data[6] = array[6];
	this->data[7] = array[7];
	this->data[8] = array[8];
	this->data[9] = array[9];
	this->data[10] = array[10];
	this->data[11] = array[11];
	this->data[12] = array[12];
	this->data[13] = array[13];
	this->data[14] = array[14];
	this->data[15] = array[15];
}

void Matrix4::setTo(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float m, float n, float o, float p) {
	this->data[0] = a;
	this->data[1] = b;
	this->data[2] = c;
	this->data[3] = d;
	this->data[4] = e;
	this->data[5] = f;
	this->data[6] = g;
	this->data[7] = h;
	this->data[8] = i;
	this->data[9] = j;
	this->data[10] = k;
	this->data[11] = l;
	this->data[12] = m;
	this->data[13] = n;
	this->data[14] = o;
	this->data[15] = p;
}

void Matrix4::multiply(float *array) {
	float temp[16] = {};

	for (int j = 0; j < 4; j++) {
		int j4 = j * 4;
		temp[j4 + 0] = this->data[j4]*array[0] + this->data[j4 + 1]*array[0 + 4] + this->data[j4 + 2]*array[0 + 8] + this->data[j4 + 3]*array[0 + 12];
		temp[j4 + 1] = this->data[j4]*array[1] + this->data[j4 + 1]*array[1 + 4] + this->data[j4 + 2]*array[1 + 8] + this->data[j4 + 3]*array[1 + 12];
		temp[j4 + 2] = this->data[j4]*array[2] + this->data[j4 + 1]*array[2 + 4] + this->data[j4 + 2]*array[2 + 8] + this->data[j4 + 3]*array[2 + 12];
		temp[j4 + 3] = this->data[j4]*array[3] + this->data[j4 + 1]*array[3 + 4] + this->data[j4 + 2]*array[3 + 8] + this->data[j4 + 3]*array[3 + 12];
	}

	temp[0] += this->data[0] * array[0] + this->data[3] * array[1] + this->data[6] * array[2];
	temp[1] += this->data[1] * array[0] + this->data[4] * array[1] + this->data[7] * array[2];
	temp[2] += this->data[2] * array[0] + this->data[5] * array[1] + this->data[8] * array[2];
	temp[3] += this->data[0] * array[3] + this->data[3] * array[4] + this->data[6] * array[5];
	temp[4] += this->data[1] * array[3] + this->data[4] * array[4] + this->data[7] * array[5];
	temp[5] += this->data[2] * array[3] + this->data[5] * array[4] + this->data[8] * array[5];
	temp[6] += this->data[0] * array[6] + this->data[3] * array[7] + this->data[6] * array[8];
	temp[7] += this->data[1] * array[6] + this->data[4] * array[7] + this->data[7] * array[8];
	temp[8] += this->data[2] * array[6] + this->data[5] * array[7] + this->data[8] * array[8];

	this->setTo(temp);
}

void Matrix4::translate(float x, float y, float z) {
	float array[16] = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1,
	};
	this->multiply(array);
}

void Matrix4::rotate2d(float deg) {
	float s = sin(deg*PI/180);
	float c = cos(deg*PI/180);
	float array[16] = {
		c, -s, 0, 0,
		s,  c, 0, 0,
		0,  0, 1, 0,
		0,  0, 0, 1,
	};
	this->multiply(array);
}

void Matrix4::scale(float x, float y, float z) {
	float array[16] = {
		x, 0, 0, 0,
		0, y, 0, 0,
		0, 0, z, 0,
		0, 0, 0, 1,
	};
	this->multiply(array);
}

void Matrix4::project(float width, float height, float depth) {
	float array[16] = {
		2/width,  0,          0,        0,
		0,        -2/height,  0,        0,
		0,        0,          -2/depth, 0,
		-1,       1,          -1,       1,
	};
	this->multiply(array);
}

void Matrix4::print() {
	printf(
		"%0.2f\t%0.2f\t%0.2f\t%0.2f\n"
		"%0.2f\t%0.2f\t%0.2f\t%0.2f\n"
		"%0.2f\t%0.2f\t%0.2f\t%0.2f\n"
		"%0.2f\t%0.2f\t%0.2f\t%0.2f\n",
		this->data[0], this->data[1], this->data[2], this->data[3],
		this->data[4], this->data[5], this->data[6], this->data[7],
		this->data[8], this->data[9], this->data[10], this->data[11],
		this->data[12], this->data[13], this->data[14], this->data[15]
	);
}

float tweenEase(float p, Ease ease) {
	float piOver2 = 3.14159/2;
	if (ease == LINEAR) {
		return p;
	} else if (ease == QUAD_IN) {
		return p * p;
	} else if (ease == QUAD_OUT) {
		return -(p * (p - 2));
	} else if (ease == QUAD_IN_OUT) {
		if (p < 0.5) return 2 * p * p;
		else return (-2 * p * p) + (4 * p) - 1;
	} else if (ease == CUBIC_IN) {
		return p * p * p;
	} else if (ease == CUBIC_OUT) {
		float f = (p - 1);
		return f * f * f + 1;
	} else if (ease == CUBIC_IN_OUT) {
		float f = ((2 * p) - 2);
		if (p < 0.5) return 4 * p * p * p;
		else return 0.5 * f * f * f + 1;
	} else if (ease == QUART_IN) {
		return p * p * p * p;
	} else if (ease == QUART_OUT) {
		float f = (p - 1);
		return f * f * f * (1 - p) + 1;
	} else if (ease == QUART_IN_OUT) {
		float f = (p - 1);
		if (p < 0.5) return 8 * p * p * p * p;
		else return -8 * f * f * f * f + 1;
	} else if (ease == QUINT_IN) {
		return p * p * p * p * p;
	} else if (ease == QUINT_OUT) {
		float f = (p - 1);
		return f * f * f * f * f + 1;
	} else if (ease == QUINT_IN_OUT) {
		float f = ((2 * p) - 2);
		if (p < 0.5) return 16 * p * p * p * p * p;
		else return  0.5 * f * f * f * f * f + 1;
	} else if (ease == SINE_IN) {
		return sin((p - 1) * piOver2) + 1;
	} else if (ease == SINE_OUT) {
		return sin(p * piOver2);
	} else if (ease == SINE_IN_OUT) {
		return 0.5 * (1 - cos(p * M_PI));
	} else if (ease == CIRC_IN) {
		return 1 - sqrt(1 - (p * p));
	} else if (ease == CIRC_OUT) {
		return sqrt((2 - p) * p);
	} else if (ease == CIRC_IN_OUT) {
		if (p < 0.5) return 0.5 * (1 - sqrt(1 - 4 * (p * p)));
		else return 0.5 * (sqrt(-((2 * p) - 3) * ((2 * p) - 1)) + 1);
	} else if (ease == EXP_IN) {
		return (p == 0.0) ? p : pow(2, 10 * (p - 1));
	} else if (ease == EXP_OUT) {
		return (p == 1.0) ? p : 1 - pow(2, -10 * p);
	} else if (ease == EXP_IN_OUT) {
		if (p == 0.0 || p == 1.0) return p;
		if (p < 0.5) return 0.5 * pow(2, (20 * p) - 10);
		else return -0.5 * pow(2, (-20 * p) + 10) + 1;
	} else if (ease == ELASTIC_IN) {
		return sin(13 * piOver2 * p) * pow(2, 10 * (p - 1));
	} else if (ease == ELASTIC_OUT) {
		return sin(-13 * piOver2 * (p + 1)) * pow(2, -10 * p) + 1;
	} else if (ease == ELASTIC_IN_OUT) {
		if (p < 0.5) return 0.5 * sin(13 * piOver2 * (2 * p)) * pow(2, 10 * ((2 * p) - 1));
		else return 0.5 * (sin(-13 * piOver2 * ((2 * p - 1) + 1)) * pow(2, -10 * (2 * p - 1)) + 2);
	} else if (ease == BACK_IN) {
		return p * p * p - p * sin(p * M_PI);
	} else if (ease == BACK_OUT) {
		float f = (1 - p);
		return 1 - (f * f * f - f * sin(f * M_PI));
	} else if (ease == BACK_IN_OUT) {
		if (p < 0.5) {
			float f = 2 * p;
			return 0.5 * (f * f * f - f * sin(f * M_PI));
		} else {
			float f = (1 - (2*p - 1));
			return 0.5 * (1 - (f * f * f - f * sin(f * M_PI))) + 0.5;
		}
	} else if (ease == BOUNCE_IN) {
		return 1 - tweenEase(1 - p, BOUNCE_OUT);
	} else if (ease == BOUNCE_OUT) {
		if (p < 4/11.0) return (121 * p * p)/16.0;
		else if (p < 8/11.0) return (363/40.0 * p * p) - (99/10.0 * p) + 17/5.0;
		else if (p < 9/10.0) return (4356/361.0 * p * p) - (35442/1805.0 * p) + 16061/1805.0;
		else return (54/5.0 * p * p) - (513/25.0 * p) + 268/25.0;
	} else if (ease == BOUNCE_IN_OUT) {
		if (p < 0.5) return 0.5 * tweenEase(p*2, BOUNCE_IN);
		else return 0.5 * tweenEase(p * 2 - 1, BOUNCE_OUT) + 0.5;
	}

	return 0;
}

#include <math.h>

#define Min(x, y) ((x) < (y) ? (x) : (y))
#define Max(x, y) ((x) > (y) ? (x) : (y))
#define PI 3.14159
#define ArrayLength(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))
#define Clamp(x, min, max) ((x) < (min) ? (min) : (x) > (max) ? (max) : (x))

struct Rect;
struct Matrix3;
struct Point;

float roundToNearest(float num, float nearest=1);
bool pointInRect(float px, float py, float rx, float ry, float rw, float rh);

float rnd();
float rndFloat(float min, float max);
int rndInt(int min, int max);
bool rndBool();

float distanceBetween(float x1, float y1, float x2, float y2);
float radsBetween(float x1, float y1, float x2, float y2);
float toDeg(float rads);
float toRad(float degs);

struct Point {
	float x;
	float y;

	void setTo(float x=0, float y=0);
	void normalize(float scale=1);

	void add(Point *other);
	void subtract(Point *other);
	void multiply(Point *other);
	void divide(Point *other);

	void add(float other);
	void subtract(float other);
	void multiply(float other);
	void divide(float other);

	bool isZero();
};

struct Rect {
	float x;
	float y;
	float width;
	float height;

	void setTo(float x=0, float y=0, float width=0, float height=0);
	bool equals(Rect *other);
	bool intersects(Rect *other);
	bool containsPoint(Point *point);
	bool containsPoint(float px, float py);
	float distanceToPerimeter(float px, float py);
	void randomPoint(Point *point);
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

float rnd() {
	return (float)rand()/(float)RAND_MAX;
}

float rndFloat(float min, float max) {
	return min + rnd() * (max - min);
}

int rndInt(int min, int max) {
	return round((rndFloat(min, max)));
}

inline bool rndBool() {
	return rndInt(0, 1);
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

float toDeg(float rads) {
	return rads * 57.2958;
}

float toRad(float degs) {
	return degs * 0.0174533;
}

void Point::setTo(float x, float y) {
	this->x = x;
	this->y = y;
}

void Point::normalize(float scale) {
	float norm = sqrt(this->x*this->x + this->y*this->y);
	if (norm != 0) {
		this->x = scale * this->x / norm;
		this->y = scale * this->y / norm;
	}
}

void Point::add(Point *other) {
	this->x += other->x;
	this->y += other->y;
}

void Point::subtract(Point *other) {
	this->x -= other->x;
	this->y -= other->y;
}

void Point::multiply(Point *other) {
	this->x *= other->x;
	this->y *= other->y;
}

void Point::divide(Point *other) {
	this->x /= other->x;
	this->y /= other->y;
}

void Point::add(float other) {
	this->x += other;
	this->y += other;
}

void Point::subtract(float other) {
	this->x -= other;
	this->y -= other;
}

void Point::multiply(float other) {
	this->x *= other;
	this->y *= other;
}

void Point::divide(float other) {
	this->x /= other;
	this->y /= other;
}

bool Point::isZero() {
	return this->x == 0 && this->y == 0;
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

bool Rect::containsPoint(Point *point) { return pointInRect(point->x, point->y, this->x, this->y, this->width, this->height); }
bool Rect::containsPoint(float px, float py) { return pointInRect(px, py, this->x, this->y, this->width, this->height); }


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
	m = Min(dl, dr);
	m = Min(m, dt);
	m = Min(m, db);

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

void Rect::randomPoint(Point *point) {
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

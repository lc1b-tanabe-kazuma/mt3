#include <Novice.h>

#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>

const char kWindowTitle[] = "LE2C_19_タナベ_カズマ_MT3_02_03";

struct Matrix4x4 {
	float m[4][4];
};

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Sphere {

	// 中心座標
	Vector3 center;

	// 半径
	float radius;

	// 色
	uint32_t color;
};

struct Line {
	Vector3 origin;
	Vector3 diff;
};

struct Ray {
	Vector3 origin;
	Vector3 direction;
};

struct Plane {
	Vector3 normal; // 法線ベクトル
	float distance; // 原点からの距離
	uint32_t color;// 色
};

// 線
struct Segment {
	Vector3 origin; // 始点
	Vector3 diff; // 終点
	uint32_t color;
};

// 長さ
float Length(const Vector3& v) {

	float result = {};

	result = sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));

	return result;
};

// 行列の加算
Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] + m2.m[i][j];
		}
	}
	return result;
};

// 行列の減算
Matrix4x4 Subtract(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m1.m[i][j] - m2.m[i][j];
		}
	}
	return result;
};

// 行列の積
Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = 0.0f;
			for (int k = 0; k < 4; k++) {
				result.m[i][j] += m1.m[i][k] * m2.m[k][j];
			}
		}
	}
	return result;
};

// 逆行列
// 3x3行列式（余因子計算用）
float Det3(float a1, float a2, float a3,
	float b1, float b2, float b3,
	float c1, float c2, float c3) {
	return a1 * (b2 * c3 - b3 * c2)
		- a2 * (b1 * c3 - b3 * c1)
		+ a3 * (b1 * c2 - b2 * c1);
}

Matrix4x4 Inverse(const Matrix4x4& m) {
	Matrix4x4 result = {};

	float det =
		m.m[0][0] * Det3(m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]) -
		m.m[0][1] * Det3(m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]) +
		m.m[0][2] * Det3(m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]) -
		m.m[0][3] * Det3(m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]);

	if (det == 0.0f) {
		// 行列が正則でない場合、単位行列または0行列などを返してもOK
		return result;
	}

	float invDet = 1.0f / det;

	// 余因子 + 転置で逆行列を一気に展開（各要素ベタ書き）
	result.m[0][0] = Det3(m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]) * invDet;
	result.m[1][0] = -Det3(m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]) * invDet;
	result.m[2][0] = Det3(m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]) * invDet;
	result.m[3][0] = -Det3(m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]) * invDet;

	result.m[0][1] = -Det3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[2][1], m.m[2][2], m.m[2][3], m.m[3][1], m.m[3][2], m.m[3][3]) * invDet;
	result.m[1][1] = Det3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[2][0], m.m[2][2], m.m[2][3], m.m[3][0], m.m[3][2], m.m[3][3]) * invDet;
	result.m[2][1] = -Det3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[2][0], m.m[2][1], m.m[2][3], m.m[3][0], m.m[3][1], m.m[3][3]) * invDet;
	result.m[3][1] = Det3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[2][0], m.m[2][1], m.m[2][2], m.m[3][0], m.m[3][1], m.m[3][2]) * invDet;

	result.m[0][2] = Det3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[1][1], m.m[1][2], m.m[1][3], m.m[3][1], m.m[3][2], m.m[3][3]) * invDet;
	result.m[1][2] = -Det3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[1][0], m.m[1][2], m.m[1][3], m.m[3][0], m.m[3][2], m.m[3][3]) * invDet;
	result.m[2][2] = Det3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[1][0], m.m[1][1], m.m[1][3], m.m[3][0], m.m[3][1], m.m[3][3]) * invDet;
	result.m[3][2] = -Det3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[1][0], m.m[1][1], m.m[1][2], m.m[3][0], m.m[3][1], m.m[3][2]) * invDet;

	result.m[0][3] = -Det3(m.m[0][1], m.m[0][2], m.m[0][3], m.m[1][1], m.m[1][2], m.m[1][3], m.m[2][1], m.m[2][2], m.m[2][3]) * invDet;
	result.m[1][3] = Det3(m.m[0][0], m.m[0][2], m.m[0][3], m.m[1][0], m.m[1][2], m.m[1][3], m.m[2][0], m.m[2][2], m.m[2][3]) * invDet;
	result.m[2][3] = -Det3(m.m[0][0], m.m[0][1], m.m[0][3], m.m[1][0], m.m[1][1], m.m[1][3], m.m[2][0], m.m[2][1], m.m[2][3]) * invDet;
	result.m[3][3] = Det3(m.m[0][0], m.m[0][1], m.m[0][2], m.m[1][0], m.m[1][1], m.m[1][2], m.m[2][0], m.m[2][1], m.m[2][2]) * invDet;

	return result;
}

// 転置行列
Matrix4x4 Transepose(const Matrix4x4& m) {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.m[i][j] = m.m[j][i];
		}
	}
	return result;
};

// 単位行列の作成
Matrix4x4 MakeIndetity4x4() {
	Matrix4x4 result = {};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == j) {
				result.m[i][j] = 1.0f;
			} else {
				result.m[i][j] = 0.0f;
			}
		}
	}
	return result;
};

// 平行移動行列
Matrix4x4 MakeTransMatrix(const Vector3& v) {
	Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = 1.0f;
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	result.m[3][0] = v.x;
	result.m[3][1] = v.y;
	result.m[3][2] = v.z;
	return result;
}

//拡大縮小行列
Matrix4x4 MakeScaleMatrix(const Vector3& v) {
	Matrix4x4 result = {};
	result.m[0][0] = v.x;
	result.m[1][1] = v.y;
	result.m[2][2] = v.z;
	result.m[3][3] = 1.0f;
	return result;
}

// 座標変換
Vector3 Transform(const Matrix4x4& m, const Vector3& v) {
	Vector3 result = {};
	result.x = m.m[0][0] * v.x + m.m[1][0] * v.y + m.m[2][0] * v.z + m.m[3][0];
	result.y = m.m[0][1] * v.x + m.m[1][1] * v.y + m.m[2][1] * v.z + m.m[3][1];
	result.z = m.m[0][2] * v.x + m.m[1][2] * v.y + m.m[2][2] * v.z + m.m[3][2];

	// w成分
	float w = v.x * m.m[0][3] + v.y * m.m[1][3] + v.z * m.m[2][3] + m.m[3][3];

	// wが0でない場合、結果をwで割る
	if (w != 0.0f) {
		result.x /= w;
		result.y /= w;
		result.z /= w;
	}
	return result;
}

// X軸の回転行列
Matrix4x4 MakeRotXMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = 1.0f;
	result.m[1][1] = std::cosf(radian);
	result.m[1][2] = std::sinf(radian);
	result.m[2][1] = -std::sinf(radian);
	result.m[2][2] = std::cosf(radian);
	result.m[3][3] = 1.0f;
	return result;
}

// Y軸の回転行列
Matrix4x4 MakeRotYMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = std::cosf(radian);
	result.m[0][2] = -std::sinf(radian);
	result.m[1][1] = 1.0f;
	result.m[2][0] = std::sinf(radian);
	result.m[2][2] = std::cosf(radian);
	result.m[3][3] = 1.0f;
	return result;
}

// Z軸の回転行列
Matrix4x4 MakeRotZMatrix(float radian) {
	Matrix4x4 result = {};
	result.m[0][0] = std::cosf(radian);
	result.m[0][1] = std::sinf(radian);
	result.m[1][0] = -std::sinf(radian);
	result.m[1][1] = std::cosf(radian);
	result.m[2][2] = 1.0f;
	result.m[3][3] = 1.0f;
	return result;
}

// 3次元アフィン変換行列
Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate) {
	// スケーリング行列の作成
	Matrix4x4 matScale = MakeScaleMatrix(scale);

	Matrix4x4 matRotX = MakeRotXMatrix(rotate.x);
	Matrix4x4 matRotY = MakeRotYMatrix(rotate.y);
	Matrix4x4 matRotZ = MakeRotZMatrix(rotate.z);

	// 回転行列の合成
	Matrix4x4 matRot = Multiply(Multiply(matRotY, matRotX), matRotZ);

	// 平行移動行列の作成
	Matrix4x4 matTrans = MakeTransMatrix(translate);

	// スケーリング、回転、平行移動の合成
	Matrix4x4 matTransform = Multiply(Multiply(matScale, matRot), matTrans);

	return matTransform;
}

// 透視投影行列
Matrix4x4 MakePerspectiveFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip) {
	Matrix4x4 result = {};
	float f = 1.0f / std::tanf(fovY / 2.0f);
	result.m[0][0] = (f * (1.0f / aspectRatio));
	result.m[1][1] = f;
	result.m[2][2] = (farClip) / (farClip - nearClip);
	result.m[2][3] = 1.0f;
	result.m[3][2] = -nearClip * farClip / (farClip - nearClip);
	return result;
}

// 正射影行列
Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip) {
	Matrix4x4 result = {};
	result.m[0][0] = 2.0f / (right - left);
	result.m[1][1] = 2.0f / (top - bottom);
	result.m[2][2] = 1.0f / (farClip - nearClip);
	result.m[3][0] = (right + left) / (left - right);
	result.m[3][1] = (top + bottom) / (bottom - top);
	result.m[3][2] = (nearClip) / (nearClip - farClip);
	result.m[3][3] = 1.0f;
	return result;
}

// ビューポート変換行列
Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float mindepth, float maxDepth) {
	Matrix4x4 result = {};
	result.m[0][0] = width / 2.0f;
	result.m[1][1] = -height / 2.0f;
	result.m[2][2] = (maxDepth - mindepth);
	result.m[3][0] = left + width / 2.0f;
	result.m[3][1] = top + height / 2.0f;
	result.m[3][2] = mindepth;
	result.m[3][3] = 1.0f;
	return result;
}

// クロス積
Vector3 Cross(const Vector3& v1, const Vector3& v2) {
	Vector3 result = {};
	result.x = v1.y * v2.z - v1.z * v2.y;
	result.y = v1.z * v2.x - v1.x * v2.z;
	result.z = v1.x * v2.y - v1.y * v2.x;
	return result;
}

// Gridを描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	const float kGridhalfWidth = 2.0f;
	const uint32_t kSubdvision = 10;
	const float kGridEvery = (kGridhalfWidth * 2.0f) / float(kSubdvision);

	// 奥から手前に描画
	for (uint32_t xIndex = 0; xIndex <= kSubdvision; ++xIndex) {
		float x = -kGridhalfWidth + kGridEvery * float(xIndex);
		Vector3 a = { x, 0.0f, -kGridhalfWidth };
		Vector3 b = { x, 0.0f, kGridhalfWidth };
		Vector3 aScreen = Transform(viewProjectionMatrix, a);
		Vector3 bScreen = Transform(viewProjectionMatrix, b);
		aScreen = Transform(viewportMatrix, aScreen);
		bScreen = Transform(viewportMatrix, bScreen);

		// 中央線（x == 0）のみ黒、それ以外はグレー
		uint32_t color = (std::abs(x) < 0.0001f) ? 0x000000FF : 0xAAAAAAFF;
		Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
	}

	for (uint32_t zIndex = 0; zIndex <= kSubdvision; ++zIndex) {
		float z = -kGridhalfWidth + kGridEvery * float(zIndex);
		Vector3 a = { -kGridhalfWidth, 0.0f, z };
		Vector3 b = { kGridhalfWidth, 0.0f, z };
		Vector3 aScreen = Transform(viewProjectionMatrix, a);
		Vector3 bScreen = Transform(viewProjectionMatrix, b);
		aScreen = Transform(viewportMatrix, aScreen);
		bScreen = Transform(viewportMatrix, bScreen);

		// 中央線（z == 0）のみ黒、それ以外はグレー
		uint32_t color = (std::abs(z) < 0.0001f) ? 0x000000FF : 0xAAAAAAFF;
		Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
	}
}

// Sphereを描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	const uint32_t kSubdivision = 20;
	const float kLoneEvery = float(M_PI * 2.0f / float(kSubdivision));  // 経度1区分
	const float kLatEvery = float(M_PI / float(kSubdivision));         // 緯度1区分

	// === 緯線（横方向） ===
	for (uint32_t latIndex = 1; latIndex < kSubdivision; ++latIndex) {
		float lat = -float(M_PI / 2.0f) + kLatEvery * float(latIndex);
		Vector3 prevPoint = {};
		for (uint32_t lonIndex = 0; lonIndex <= kSubdivision; ++lonIndex) {
			float lon = lonIndex * kLoneEvery;

			Vector3 point = {
				sphere.center.x + sphere.radius * std::cosf(lat) * std::cosf(lon),
				sphere.center.y + sphere.radius * std::sinf(lat),
				sphere.center.z + sphere.radius * std::cosf(lat) * std::sinf(lon)
			};

			Vector3 screenPoint = Transform(viewProjectionMatrix, point);
			screenPoint = Transform(viewportMatrix, screenPoint);

			if (lonIndex > 0) {
				Novice::DrawLine(
					static_cast<int>(prevPoint.x), static_cast<int>(prevPoint.y),
					static_cast<int>(screenPoint.x), static_cast<int>(screenPoint.y),
					color
				);
			}

			prevPoint = screenPoint;
		}
	}

	// === 経線（縦方向） ===
	for (uint32_t lonIndex = 0; lonIndex < kSubdivision; ++lonIndex) {
		float lon = lonIndex * kLoneEvery;
		Vector3 prevPoint = {};
		for (uint32_t latIndex = 0; latIndex <= kSubdivision; ++latIndex) {
			float lat = -float(M_PI / 2.0f) + kLatEvery * float(latIndex);

			Vector3 point = {
				sphere.center.x + sphere.radius * std::cosf(lat) * std::cosf(lon),
				sphere.center.y + sphere.radius * std::sinf(lat),
				sphere.center.z + sphere.radius * std::cosf(lat) * std::sinf(lon)
			};

			Vector3 screenPoint = Transform(viewProjectionMatrix, point);
			screenPoint = Transform(viewportMatrix, screenPoint);

			if (latIndex > 0) {
				Novice::DrawLine(
					static_cast<int>(prevPoint.x), static_cast<int>(prevPoint.y),
					static_cast<int>(screenPoint.x), static_cast<int>(screenPoint.y),
					color
				);
			}

			prevPoint = screenPoint;
		}
	}
}

// ベクトルの加算
Vector3 VectorAdd(const Vector3& v1, const Vector3& v2) {

	Vector3 result = {};

	result.x = v1.x + v2.x;
	result.y = v1.y + v2.y;
	result.z = v1.z + v2.z;

	return result;
};

// ベクトルの減算
Vector3 VectorSubtract(const Vector3& v1, const Vector3& v2) {

	Vector3 result = {};

	result.x = v1.x - v2.x;
	result.y = v1.y - v2.y;
	result.z = v1.z - v2.z;

	return result;
};

// ベクトルの正規化
Vector3 Normalize(const Vector3& v) {

	Vector3 result = {};
	float length = Length(v);
	if (length != 0.0f) {
		result.x = v.x / Length(v);
		result.y = v.y / Length(v);
		result.z = v.z / Length(v);
	}
	return result;
};

// スカラー倍
Vector3 ScalarMultiply(float scalar, const Vector3& v) {
	Vector3 result = {};

	result.x = v.x * scalar;
	result.y = v.y * scalar;
	result.z = v.z * scalar;

	return result;
};

// 内積
float Dot(const Vector3& v1, const Vector3& v2) {

	float result = {};

	result = (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);

	return result;
};

Matrix4x4 MakeLookAtMatrix(Vector3 eye, Vector3 target, Vector3 up) {
	Vector3 zAxis = Normalize(VectorSubtract(target, eye));
	Vector3 xAxis = Normalize(Cross(up, zAxis));
	Vector3 yAxis = Cross(zAxis, xAxis);

	Matrix4x4 result = {};
	result.m[0][0] = xAxis.x;
	result.m[1][0] = xAxis.y;
	result.m[2][0] = xAxis.z;
	result.m[0][1] = yAxis.x;
	result.m[1][1] = yAxis.y;
	result.m[2][1] = yAxis.z;
	result.m[0][2] = zAxis.x;
	result.m[1][2] = zAxis.y;
	result.m[2][2] = zAxis.z;
	result.m[3][0] = -Dot(xAxis, eye);
	result.m[3][1] = -Dot(yAxis, eye);
	result.m[3][2] = -Dot(zAxis, eye);
	result.m[3][3] = 1.0f;
	return result;
}

Vector3 Project(const Vector3& v1, const Vector3& v2) {
	Vector3 result = {};
	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;
	return result;
}

Vector3 ClossPoint(const Vector3& point, const Segment& segment) {
	Vector3 result = {};
	Vector3 segmentVector = { segment.diff.x, segment.diff.y, segment.diff.z };
	Vector3 pointVector = { point.x - segment.origin.x, point.y - segment.origin.y, point.z - segment.origin.z };
	float t = (segmentVector.x * pointVector.x + segmentVector.y * pointVector.y + segmentVector.z * pointVector.z) /
		(segmentVector.x * segmentVector.x + segmentVector.y * segmentVector.y + segmentVector.z * segmentVector.z);
	result.x = t * segmentVector.x + segment.origin.x;
	result.y = t * segmentVector.y + segment.origin.y;
	result.z = t * segmentVector.z + segment.origin.z;
	return result;
}

// 球と球の衝突判定
bool Iscollision(Sphere& s1, Sphere& s2) {
	float distance = Length(VectorSubtract(s1.center, s2.center));
	uint32_t CollisionColor = WHITE;

	// 衝突している場合
	if (distance <= (s1.radius + s2.radius)) {

		// 色を赤にする
		CollisionColor = RED;
		s1.color = CollisionColor;
	} else {
		CollisionColor = WHITE;
		s1.color = CollisionColor;
	}

	return s1.color;
}

// 球と平面の衝突判定
bool StoPIscollision(Sphere& sphere, Plane& plane) {

	// 球の中心から平面までの距離を計算
	float distance = std::abs(plane.normal.x * sphere.center.x + plane.normal.y * sphere.center.y + plane.normal.z * sphere.center.z - plane.distance) / Length(plane.normal);

	// 衝突している場合
	if (distance <= sphere.radius) {
		// 色を赤にする
		sphere.color = RED;
	} else {
		sphere.color = WHITE;
	}

	return sphere.color;
}

// 線分と平面の衝突判定
bool IsCollision(Segment& segment, Plane& plane) {

	// 垂直判定を行なう為に、法線と線の内積を求める
	float dot = Dot(plane.normal, segment.diff);

	// 垂直=平行であるので衝突していない
	if (dot == 0.0f) {
		return false;
	}

	// tを求める
	float t = (plane.distance - Dot(segment.origin, plane.normal)) / dot;

	// tが線分の範囲内にあれば衝突している
	return (t >= 0.0f && t <= 1.0f);
}

Vector3 Perpendicular(const Vector3& vector) {
	if (vector.x != 0.0f || vector.y != 0.0f) {
		return { -vector.y, vector.x, 0.0f };
	}
	return { 0.0f, -vector.z, vector.y };
};

// 平面の描画
void DrawPlane(const Plane& plane, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 center = ScalarMultiply(plane.distance, plane.normal);
	Vector3 perpendicular[4];
	perpendicular[0] = Normalize(Perpendicular(plane.normal));
	perpendicular[1] = { -perpendicular[0].x,-perpendicular[0].y,-perpendicular[0].z };
	perpendicular[2] = Cross(plane.normal, perpendicular[0]);
	perpendicular[3] = { -perpendicular[2].x,-perpendicular[2].y,-perpendicular[2].z };

	Vector3 points[4];
	for (int32_t index = 0; index < 4; ++index) {
		Vector3 extend = ScalarMultiply(2.0f, perpendicular[index]);
		Vector3 point = VectorAdd(center, extend);
		points[index] = Transform(viewportMatrix, Transform(viewProjectionMatrix, point));
	}

	// pointsをそれぞれ結んでDrawLineで矩形を描画する
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[1].x), int(points[1].y), color);
	Novice::DrawLine(int(points[1].x), int(points[1].y), int(points[3].x), int(points[3].y), color);
	Novice::DrawLine(int(points[3].x), int(points[3].y), int(points[0].x), int(points[0].y), color);
	Novice::DrawLine(int(points[2].x), int(points[2].y), int(points[0].x), int(points[0].y), color);
}

//
static const int kColumnWidth = 60;

//
static const int kRowHeight = 20;

// 4x4の結果を表示する関数
void MatrixScreenPrintf(int x, int y, const Matrix4x4& m, const char* label) {

	Novice::ScreenPrintf(x, y, "%s", label);
	for (int row = 0; row < 4; ++row) {
		for (int colum = 0; colum < 4; ++colum) {
			Novice::ScreenPrintf(x + colum * kColumnWidth, y + (row + 1) * kRowHeight, "%6.2f", m.m[row][colum]);
		}

	}
}

// Vectorの結果を表示する関数
void VectorScreenPrintf(int x, int y, const Vector3& vector, const char* label) {
	Novice::ScreenPrintf(x, y, "%.02f", vector.x);
	Novice::ScreenPrintf(x + kColumnWidth, y, "%.02f", vector.y);
	Novice::ScreenPrintf(x + kColumnWidth * 2, y, "%.02f", vector.z);
	Novice::ScreenPrintf(x + kColumnWidth * 3, y, "%s", label);
};

//ウィンドウの幅
int kWindowWidth = 1280;

//ウィンドウの高さ
int kWindowHeight = 720;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	///==========================================================================-
	// カメラの位置は課題が変わってもいじらない
	///==========================================================================-
#pragma region
	// カメラの位置
	Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
	Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };
	Vector3 SphereCenter = { 0.0f, 0.0f, 0.0f };
	float cameraFovY = 0.45f;
	Vector3 target = { 0.0f, 0.0f, 0.0f };
	Vector3 up = { 0.0f, 1.0f, 0.0f };

	// 極座標からカメラ位置を計算（cameraRotate.x = Pitch, cameraRotate.y = Yaw）
	Vector3 cameraPosition;
	float radius = 6.0f; // カメラ距離
#pragma endregion

	Segment segment;
	segment.origin = { 0.0f, 0.0f, 0.0f };
	segment.diff = { 1.0f, 1.0f, 1.0f };
	segment.color = WHITE;

	// 平面の初期化
	Plane plane = {};
	plane.normal = { 0.0f, 1.0f, 0.0f };
	plane.distance = 1.0f;
	plane.color = WHITE;

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// 衝突判定
		bool isHit = IsCollision(segment, plane);

		if (isHit) {
			segment.color = RED;
		} else {
			segment.color = WHITE;
		}

		cameraPosition.x = radius * std::cosf(cameraRotate.x) * std::sinf(cameraRotate.y);
		cameraPosition.y = radius * std::sinf(cameraRotate.x);
		cameraPosition.z = radius * std::cosf(cameraRotate.x) * std::cosf(cameraRotate.y);

		// ビュー行列（LookAt式）
		Matrix4x4 cameraViewMatrix = MakeLookAtMatrix(cameraPosition, target, up);

		// 射影行列（透視投影）
		Matrix4x4 cameraProjectionMatrix = MakePerspectiveFovMatrix(
			cameraFovY,
			float(kWindowWidth) / float(kWindowHeight),
			0.1f,
			100.0f
		);

		// ビュー×プロジェクション行列
		Matrix4x4 cameraViewProjectionMatrix = Multiply(cameraViewMatrix, cameraProjectionMatrix);

		// ビューポート
		Matrix4x4 viewportMatrix = MakeViewportMatrix(0, 0, float(kWindowWidth), float(kWindowHeight), 0.0f, 1.0f);

		Vector3 start = Transform(viewportMatrix, Transform(cameraViewProjectionMatrix, segment.origin));

		Vector3 end = Transform(viewportMatrix, Transform(cameraViewProjectionMatrix, VectorAdd(segment.origin, segment.diff)));

		// リセット
		if (keys[DIK_R]) {
			cameraTranslate = { 0.0f, 1.9f, -6.49f };
			cameraRotate = { 0.26f, 0.0f, 0.0f };
			SphereCenter = { 0.0f, 0.0f, 0.0f };
			plane.normal = { 0.0f, 1.0f, 0.0f };
			plane.distance = 1.0f;
			segment = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
		}

		// ImGuiの初期化
		ImGui::Begin("Window");
		ImGui::DragFloat("PlaneDistance", &plane.distance, 0.01f);
		if (ImGui::DragFloat3("PlaneNormal", &plane.normal.x, 0.01f)) {
			plane.normal = Normalize(plane.normal);
		}
		ImGui::DragFloat3("segment.origin", &segment.origin.x, 0.01f);
		ImGui::DragFloat3("segment.diff", &segment.diff.x, 0.01f);

		// マウス操作
		ImGui::DragFloat("Yaw", &cameraRotate.y, 0.01f);
		ImGui::DragFloat("Pitch", &cameraRotate.x, 0.01f);

		// マウス操作
		float sensitivity = 0.0025f; // 感度をここで調整
		ImGuiIO& io = ImGui::GetIO();

		if (ImGui::IsMouseDown(1)) { // 右ドラッグで回転
			cameraRotate.y += io.MouseDelta.x * sensitivity;  // Yaw
			cameraRotate.x += io.MouseDelta.y * sensitivity;  // Pitch
		}

		if (ImGui::IsMouseDown(2)) { // 中ドラッグでパン（平行移動）
			Vector3 right = { std::cos(cameraRotate.y), 0, -std::sin(cameraRotate.y) };
			up = { 0, 1, 0 };
			cameraTranslate.x -= io.MouseDelta.x * 0.01f * right.x;
			cameraTranslate.z -= io.MouseDelta.x * 0.01f * right.z;
			cameraTranslate.x += io.MouseDelta.y * 0.01f * up.x;
			cameraTranslate.y += io.MouseDelta.y * 0.01f * up.y;
			cameraTranslate.z += io.MouseDelta.y * 0.01f * up.z;
		}

		// ホイールでズーム（FOV）
		cameraFovY -= io.MouseWheel * 0.05f;
		cameraFovY = std::clamp(cameraFovY, 0.1f, 1.5f);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		// Grid線の描画
		DrawGrid(cameraViewProjectionMatrix, viewportMatrix);

		// 線分を描画
		Novice::DrawLine(int(start.x), int(start.y), int(end.x), int(end.y), segment.color);

		// 平面の描画
		DrawPlane(plane, cameraViewProjectionMatrix, viewportMatrix, plane.color);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}
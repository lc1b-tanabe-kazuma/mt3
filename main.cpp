#define NOMINMAX
#define _USE_MATH_DEFINES
#include <cmath>
#include <assert.h>
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <Novice.h>

const char kWindowTitle[] = "LE2C_19_タナベ_カズマ_MT3_03_02";

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

// 三角形
struct Triangle {
	Vector3 vertices[3]; // 頂点
};
// 箱
struct AABB
{
	Vector3 min;
	Vector3 max;
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

Matrix4x4 MakeRotateMatrix(const Vector3& rotate) {
	// Z→X→Yの順で回転（一般的なT-poseベースの腕構造に適している場合が多い）
	Matrix4x4 rotX = MakeRotXMatrix(rotate.x);
	Matrix4x4 rotY = MakeRotYMatrix(rotate.y);
	Matrix4x4 rotZ = MakeRotZMatrix(rotate.z);

	// 合成（順序に注意！Z→X→Y）
	Matrix4x4 result = Multiply(Multiply(rotY, rotX), rotZ);
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

// ベクトルの要素ごとの乗算
Vector3 VectorMultiply(const Vector3& v1, const Vector3& v2) {

	Vector3 result = {};

	result.x = v1.x * v2.x;
	result.y = v1.y * v2.y;
	result.z = v1.z * v2.z;

	return result;
}

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

// ベクトルの長さの二乗を計算する関数を追加
float LengthSquared(const Vector3& v) {
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

// 線形補間
float Lerp(float x1, float x2, float t) { return (1.0f - t) * x1 + t * x2; }

// ベクトルの線形補間
Vector3 VectorLerp(const Vector3& v1, const Vector3& v2, float t) {
	return Vector3(Lerp(v1.x, v2.x, t), Lerp(v1.y, v2.y, t), Lerp(v1.z, v2.z, t));
}

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
	float dot = Dot(v1, v2);
	float lenSq = LengthSquared(v2);
	if (lenSq == 0.0f) {
		return { 0.0f, 0.0f, 0.0f }; // ゼロベクトルへの射影はゼロベクトルにする
	}
	float scale = dot / lenSq;
	return {
		v2.x * scale,
		v2.y * scale,
		v2.z * scale
	};
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

// 三角形と線分の当たり判定
bool IscollisionTriangle(Segment& segment, Triangle& triangle) {
	const float EPSILON = 1e-6f;

	Vector3 p0 = triangle.vertices[0];
	Vector3 p1 = triangle.vertices[1];
	Vector3 p2 = triangle.vertices[2];

	Vector3 edge1 = VectorSubtract(p1, p0);
	Vector3 edge2 = VectorSubtract(p2, p0);

	Vector3 dir = segment.diff;
	Vector3 orig = segment.origin;

	Vector3 h = Cross(dir, edge2);
	float a = Dot(edge1, h);
	if (std::abs(a) < EPSILON) {
		segment.color = 0xFFFFFFFF; // 白にリセット
		return false; // 平行なので衝突しない
	}

	float f = 1.0f / a;
	Vector3 s = VectorSubtract(orig, p0);
	float u = f * Dot(s, h);
	if (u < 0.0f || u > 1.0f) {
		segment.color = 0xFFFFFFFF; // 白にリセット
		return false;
	}

	Vector3 q = Cross(s, edge1);
	float v = f * Dot(dir, q);
	if (v < 0.0f || u + v > 1.0f) {
		segment.color = 0xFFFFFFFF; // 白にリセット
		return false;
	}

	float t = f * Dot(edge2, q);
	if (t < 0.0f || t > 1.0f) {
		segment.color = 0xFFFFFFFF; // 白にリセット
		return false; // 線分の範囲外なら交差しない
	}

	// 交差しているので色を赤に変更
	segment.color = 0xFF0000FF;
	return true; // 交差あり
}

// AABBの当たり判定
bool IsCollisionAABB(const AABB& aabb1, const AABB& aabb2) {
	return (aabb1.min.x <= aabb2.max.x && aabb1.max.x >= aabb2.min.x) && // x軸
		(aabb1.min.y <= aabb2.max.y && aabb1.max.y >= aabb2.min.y) && // y軸
		(aabb1.min.z <= aabb2.max.z && aabb1.max.z >= aabb2.min.z);   // z軸
}

// AABBと球の衝突判定
bool IsCollisionAABBToSphere(const AABB& aabb, const Sphere& sphere) {

	// 最近接点を求める
	Vector3 closestPoint{ std::clamp(sphere.center.x, aabb.min.x, aabb.max.x),
		std::clamp(sphere.center.y, aabb.min.y, aabb.max.y),
		std::clamp(sphere.center.z, aabb.min.z, aabb.max.z) };

	// 最近接点と球の中心の距離を計算
	float distance = Length(VectorSubtract(closestPoint, sphere.center));

	return (distance <= sphere.radius);
}

// AABBと線の当たり判定
bool IsCollisionAABBtoSegment(const AABB& aabb, const Segment& segment) {

	Vector3 dir = VectorSubtract(segment.diff, segment.origin); // 線分の方向ベクトル
	Vector3 invDir = {
		1.0f / (dir.x != 0.0f ? dir.x : 1e-6f), // 0除算対策
		1.0f / (dir.y != 0.0f ? dir.y : 1e-6f),
		1.0f / (dir.z != 0.0f ? dir.z : 1e-6f),
	};

	// AABBの最小・最大座標
	Vector3 tMin = VectorMultiply(VectorSubtract(aabb.min, segment.origin), invDir);
	Vector3 tMax = VectorMultiply(VectorSubtract(aabb.max, segment.origin), invDir);

	// スラブのmin/maxを補正（符号によって反転している場合があるため）
	Vector3 t1 = { std::min(tMin.x, tMax.x), std::min(tMin.y, tMax.y), std::min(tMin.z, tMax.z) };
	Vector3 t2 = { std::max(tMin.x, tMax.x), std::max(tMin.y, tMax.y), std::max(tMin.z, tMax.z) };

	float tNear = std::max(std::max(t1.x, t1.y), t1.z);
	float tFar = std::min(std::min(t2.x, t2.y), t2.z);

	// 線分の範囲 [0, 1] において交差しているか？
	if (tNear <= tFar && tFar >= 0.0f && tNear <= 1.0f) {
		return true; // 衝突
	}
	return false;
}

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

// 三角形の描画
void DrawTriangle(Triangle& triangle, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	// 三角形の各頂点を変換する（ワールド → ビュー射影 → ビューポート）
	Vector3 ndc0 = Transform(viewProjectionMatrix, triangle.vertices[0]);
	Vector3 ndc1 = Transform(viewProjectionMatrix, triangle.vertices[1]);
	Vector3 ndc2 = Transform(viewProjectionMatrix, triangle.vertices[2]);

	Vector3 screen0 = Transform(viewportMatrix, ndc0);
	Vector3 screen1 = Transform(viewportMatrix, ndc1);
	Vector3 screen2 = Transform(viewportMatrix, ndc2);

	Novice::DrawTriangle(int(screen0.x), int(screen0.y), int(screen1.x), int(screen1.y), int(screen2.x), int(screen2.y), color, kFillModeWireFrame);
}

// AABBの描画
void DrawAABB(AABB& aabb, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {
	Vector3 corners[8] = {
		{ aabb.min.x, aabb.min.y, aabb.min.z}, // 0
		{ aabb.max.x, aabb.min.y, aabb.min.z}, // 1
		{ aabb.max.x, aabb.max.y, aabb.min.z}, // 2
		{ aabb.min.x, aabb.max.y, aabb.min.z}, // 3
		{ aabb.min.x, aabb.min.y, aabb.max.z}, // 4
		{ aabb.max.x, aabb.min.y, aabb.max.z}, // 5
		{ aabb.max.x, aabb.max.y, aabb.max.z}, // 6
		{ aabb.min.x, aabb.max.y, aabb.max.z}  // 7
	};

	// 各点を射影・ビューポート変換
	Vector3 projected[8];
	for (int i = 0; i < 8; i++) {
		Vector3 v = Transform(viewProjectionMatrix, corners[i]);
		projected[i] = Transform(viewportMatrix, v);
	}

	// AABBの12辺を描画（intにキャストして使用）
	auto drawEdge = [&](int i, int j) {
		Novice::DrawLine(
			static_cast<int>(projected[i].x), static_cast<int>(projected[i].y),
			static_cast<int>(projected[j].x), static_cast<int>(projected[j].y),
			color
		);
		};

	// 前面
	drawEdge(0, 1); drawEdge(1, 2); drawEdge(2, 3); drawEdge(3, 0);

	// 背面
	drawEdge(4, 5); drawEdge(5, 6); drawEdge(6, 7); drawEdge(7, 4);

	// 側面
	drawEdge(0, 4); drawEdge(1, 5); drawEdge(2, 6); drawEdge(3, 7);
}

void DrawBezier(const Vector3& controlPoint0, const Vector3& controlPoint1, const Vector3& controlPoint2,
	const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color) {

	// 最初の点
	Vector3 prev = Transform(viewportMatrix, Transform(viewProjectionMatrix, controlPoint0));

	// 分割数
	const int steps = 50;

	for (int i = 1; i <= steps; ++i) {
		float t = i / (float)steps;

		// 2次ベジェ曲線の計算（De Casteljau アルゴリズム）
		Vector3 a = VectorLerp(controlPoint0, controlPoint1, t);
		Vector3 b = VectorLerp(controlPoint1, controlPoint2, t);
		Vector3 point = VectorLerp(a, b, t);

		// 座標変換（ワールド → ビュー射影 → ビューポート）
		Vector3 screenPoint = Transform(viewportMatrix, Transform(viewProjectionMatrix, point));

		// 線分を描画（前の点と現在の点をつなぐ）
		Novice::DrawLine(
			static_cast<int>(prev.x), static_cast<int>(prev.y),
			static_cast<int>(screenPoint.x), static_cast<int>(screenPoint.y),
			color
		);

		prev = screenPoint; // 次のループのために現在の点を保存
	}
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

void DrawLineBetweenParts(
	const Vector3& posA, // 始点（例：肩）
	const Vector3& posB, // 終点（例：手首）
	const Matrix4x4& viewProjMatrix,
	const Matrix4x4& viewportMatrix,
	uint32_t color
) {
	Line line;
	line.origin = posA;
	line.diff = VectorSubtract(posB, posA);

	Vector3 screenStart = Transform(viewportMatrix, Transform(viewProjMatrix, line.origin));
	Vector3 screenEnd = Transform(viewportMatrix, Transform(viewProjMatrix, VectorAdd(line.origin, line.diff)));

	Novice::DrawLine(
		static_cast<int>(screenStart.x),
		static_cast<int>(screenStart.y),
		static_cast<int>(screenEnd.x),
		static_cast<int>(screenEnd.y),
		color
	);
}

void DrawArmHierarchy(
	const Vector3 translates[3],
	const Vector3 rotates[3],
	const Vector3 scales[3],
	const Matrix4x4& viewProjectionMatrix,
	const Matrix4x4& viewportMatrix,
	uint32_t color
) {
	Matrix4x4 parentMatrix = MakeIndetity4x4();

	Vector3 prevWorldPosition{};
	bool isFirst = true;

	for (int i = 0; i < 3; ++i) {

		// 部位毎のワールド変換
		Matrix4x4 translateMat = MakeTransMatrix(translates[i]);
		Matrix4x4 rotateMat = MakeRotateMatrix(rotates[i]);
		Matrix4x4 scaleMat = MakeScaleMatrix(scales[i]);
		Matrix4x4 localMatrix = Multiply(Multiply(scaleMat, rotateMat), translateMat);

		// 親のワールド座標と掛け合わせて、子のワールド座標を求める
		Matrix4x4 worldMatrix = Multiply(localMatrix, parentMatrix);

		// ワールド座標を取得
		Vector3 worldPosition = {
			worldMatrix.m[3][0],
			worldMatrix.m[3][1],
			worldMatrix.m[3][2]
		};

		// 色定義（部位ごと）
		const uint32_t colors[3] = {
			RED, // 肩（赤）
			GREEN, // 腕（緑）
			BLUE  // 手首（青）
		};

		// 部位ごとに描画
		DrawSphere({ worldPosition, 0.1f }, viewProjectionMatrix, viewportMatrix, colors[i]);

		// 最初の部位でなければ、前の部位との線を描画
		if (!isFirst) {
			// スクリーン座標に変換
			Vector3 screenStart = Transform(viewportMatrix, Transform(viewProjectionMatrix, prevWorldPosition));
			Vector3 screenEnd = Transform(viewportMatrix, Transform(viewProjectionMatrix, worldPosition));

			// 部位から部位への線を描画
			Novice::DrawLine(static_cast<int>(screenStart.x), static_cast<int>(screenStart.y), static_cast<int>(screenEnd.x), static_cast<int>(screenEnd.y), color);
		}

		// 前の部位のワールド座標を更新
		prevWorldPosition = worldPosition;
		isFirst = false;

		// 次の部位の親行列を更新
		parentMatrix = worldMatrix;
	}
}

// 単項演算子のオーバーロード
Vector3 operator+(const Vector3& v) { return v; }
Vector3 operator-(const Vector3& v) { return Vector3(-v.x, -v.y, -v.z); }

// 2項演算子のオーバーロード
Vector3 operator+(const Vector3& v1, const Vector3& v2) {
	return VectorAdd(v1, v2);
}

// 2項演算子のオーバーロード
Vector3 operator-(const Vector3& v1, const Vector3& v2) {
	return VectorSubtract(v1, v2);
}

// 2項演算子のオーバーロード
Vector3 operator*(const Vector3& v1, const Vector3& v2) {
	return VectorMultiply(v1, v2);
}

// 2項演算子のオーバーロード
Vector3 operator*(Vector3& v, float s) {
	v.x *= s;
	v.y *= s;
	v.z *= s;
	return v;
}

// 2項演算子のオーバーロード
Vector3 operator/(const Vector3& v, float s) {
	Vector3 result = v;
	result.x /= (1.0f / s);
	result.y /= (1.0f / s);
	result.z /= (1.0f / s);
	return result;
}

// 2項演算子のオーバーロード
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Add(m1, m2);
}

// 2項演算子のオーバーロード
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Subtract(m1, m2);
}

// 2項演算子のオーバーロード
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2) {
	return Multiply(m1, m2);
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

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

		Vector3 a{ 0.2f,1.0f,0.0f };
		Vector3 b{ 2.4f,3.1f,1.2f };
		Vector3 c = a + b;
		Vector3 d = a - b;
		Vector3 e = a * 2.4f;
		Vector3 rotate{ 0.4f,1.43f,-0.8f };
		Matrix4x4 rotateXMatrix = MakeRotXMatrix(rotate.x);
		Matrix4x4 rotateYMatrix = MakeRotYMatrix(rotate.y);
		Matrix4x4 rotateZMatrix = MakeRotZMatrix(rotate.z);
		Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;

		// ImGuiの初期化
		ImGui::Begin("Window");

		ImGui::Text("c: %f, %f, %f",c.x,c.y,c.z);
		ImGui::Text("d: %f, %f, %f", d.x, d.y, d.z);
		ImGui::Text("e: %f, %f, %f", e.x, e.y, e.z);
		ImGui::Text("matrix:\n %f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f\n %f, %f, %f, %f\n",
			rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3],
			rotateMatrix.m[1][0], rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3],
			rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3],
			rotateMatrix.m[3][0], rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
		ImGui::End();

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

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
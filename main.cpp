#include "MyMath.h"
#include <Novice.h>
#include <cstdint>
#include <imgui.h>
#define _USE_MATH_DEFINES
#include <cmath>

const char kWindowTitle[] = "LC1A_16_タナハラ_コア_タイトル";

struct Sphere {
    Vector3 center;
    float radius;
};

//==============================
// 関数定義
//==============================

// グリッドを描画
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);

// スフィアを描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color);

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    // ライブラリの初期化
    Novice::Initialize(kWindowTitle, 1280, 720);

    // キー入力結果を受け取る箱
    char keys[256] = { 0 };
    char preKeys[256] = { 0 };

    //==============================
    // 変数初期化
    //==============================

    Vector3 cameraTranslate = { 0.0f, 1.9f, -6.49f };
    Vector3 cameraRotate = { 0.26f, 0.0f, 0.0f };

    Matrix4x4 cameraWorldMatrix = makeAffineMatrix({ 1.0, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
    Matrix4x4 viewMatrix = Inverse(cameraWorldMatrix);
    Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
    /* Matrix4x4 worldViewProjectionMatrix = Multiply(cameraWorldMatrix, Multiply(viewMatrix, projectionMatrix));*/
    Matrix4x4 viewPortMatrix = MakeViewportMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 1.0f);

    // 球の変数
    Sphere sphere;
    sphere.center = { 0.0f, 0.0f, 0.0f };
    sphere.radius = 0.5f;

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

        // カメラや球の位置を更新
        ImGui::Begin("Window");
        ImGui::SliderFloat3("Translate", &cameraTranslate.x, -5.0f, 5.0f, "%.2f");
        ImGui::SliderFloat3("Rotation Angle", &cameraRotate.x, -0.52f, 0.52f, "%.2f");
        // 球の位置を更新
        ImGui::SliderFloat3("Center", &sphere.center.x, -1.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Radius", &sphere.radius, 0.1f, 1.0f, "%.2f");

        // リセットボタン
        if (ImGui::Button("Reset")) {
            cameraTranslate = { 0.0f, 1.9f, -6.49f };
            cameraRotate = { 0.26f, 0.0f, 0.0f };

            sphere.center = { 0.0f, 0.0f, 0.0f };
            sphere.radius = 0.5f;
        }
        ImGui::End();

        cameraWorldMatrix = makeAffineMatrix({ 1.0, 1.0f, 1.0f }, cameraRotate, cameraTranslate);
        viewMatrix = Inverse(cameraWorldMatrix);

        ///
        /// ↑更新処理ここまで
        ///

        ///
        /// ↓描画処理ここから
        ///

        // グリッドを描画
        DrawGrid((viewMatrix * projectionMatrix), viewPortMatrix);

        // 球を描画
        DrawSphere(sphere, Multiply(viewMatrix, projectionMatrix), viewPortMatrix, 0x000000FF);

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

// グリッドを描画する
void DrawGrid(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix)
{
    // グリッド半分の幅
    const float kGridHalfWidth = 2.0f;
    // 分割数
    const uint32_t kSubDivision = 10;
    // １つ分の長さ
    const float kGridEvery = (kGridHalfWidth * 2.0f) / float(kSubDivision);

    // 奥から手前への線を順番に描画
    for (uint32_t i = 0; i <= kSubDivision; ++i) {
        // 上の情報を使ってワールド座標系上の始点と終点を計算

        Vector3 start = { -kGridHalfWidth + kGridEvery * i, 0.0f, -kGridHalfWidth };
        Vector3 end = { -kGridHalfWidth + kGridEvery * i, 0.0f, kGridHalfWidth };

        Vector3 ndcStart = TransformCoord(start, viewProjectionMatrix);
        Vector3 ndcEnd = TransformCoord(end, viewProjectionMatrix);

        // スクリーン座標系まで掛ける
        Vector3 startScreen = TransformCoord(ndcStart, viewportMatrix);
        Vector3 endScreen = TransformCoord(ndcEnd, viewportMatrix);

        // 変換した座標を使い、線を描画
        Novice::DrawLine(
            int(startScreen.x), int(startScreen.y),
            int(endScreen.x), int(endScreen.y),
            0xAAAAAAFF);

        if (i == 5) {
            Novice::DrawLine(
                int(startScreen.x), int(startScreen.y),
                int(endScreen.x), int(endScreen.y),
                0x000000FF);
        }
    }

    for (uint32_t i = 0; i <= kSubDivision; ++i) {
        // 上の情報を使ってワールド座標系上の始点と終点を計算

        Vector3 start = { -kGridHalfWidth, 0.0f, -kGridHalfWidth + kGridEvery * i };
        Vector3 end = { kGridHalfWidth, 0.0f, -kGridHalfWidth + kGridEvery * i };

        Vector3 ndcStart = TransformCoord(start, viewProjectionMatrix);
        Vector3 ndcEnd = TransformCoord(end, viewProjectionMatrix);

        // スクリーン座標系まで掛ける
        Vector3 startScreen = TransformCoord(ndcStart, viewportMatrix);
        Vector3 endScreen = TransformCoord(ndcEnd, viewportMatrix);

        // 変換した座標を使い、線を描画
        Novice::DrawLine(
            int(startScreen.x), int(startScreen.y),
            int(endScreen.x), int(endScreen.y),
            0xAAAAAAFF);

        if (i == 5) {
            Novice::DrawLine(
                int(startScreen.x), int(startScreen.y),
                int(endScreen.x), int(endScreen.y),
                0x000000FF);
        }
    }
}

// スフィアを描画
void DrawSphere(const Sphere& sphere, const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix, uint32_t color)
{
    // 球の分割数
    const uint32_t kSubDivision = 20;

    // 経度分割１つ分の角度
    const float kLongEvery = 2.0f * static_cast<float>(M_PI) / static_cast<float>(kSubDivision);

    // 緯度分割１つ分の角度
    const float kLatEvery = static_cast<float>(M_PI) / static_cast<float>(kSubDivision);

    // 緯度の方向に分割 -π/2 ~ π/2
    for (uint32_t latIndex = 0; latIndex < kSubDivision; ++latIndex) {

        // 現在の緯度
        float lat = -static_cast<float>(M_PI) / 2.0f + kLatEvery * latIndex;

        // 経度の方向に分割 0 ~ 2π
        for (uint32_t lonIndex = 0; lonIndex < kSubDivision; ++lonIndex) {

            // 現在の経度
            float lon = kLongEvery * lonIndex;

            // 各頂点の計算
            Vector3 a, b, c;

            a.x = std::cos(lat) * std::cos(lon) * sphere.radius + sphere.center.x;
            a.y = std::sin(lat) * sphere.radius + sphere.center.y;
            a.z = std::cos(lat) * std::sin(lon) * sphere.radius + sphere.center.z;

            b.x = std::cos(lat + kLatEvery) * std::cos(lon) * sphere.radius + sphere.center.x;
            b.y = std::sin(lat + kLatEvery) * sphere.radius + sphere.center.y;
            b.z = std::cos(lat + kLatEvery) * std::sin(lon) * sphere.radius + sphere.center.z;

            c.x = sphere.radius * std::cos(lat) * std::cos(lon + kLongEvery) + sphere.center.x;
            c.y = sphere.radius * std::sin(lat) + sphere.center.y;
            c.z = sphere.radius * std::cos(lat) * std::sin(lon + kLongEvery) + sphere.center.z;

            // ビュー座標系に変換
            Vector3 aView = TransformCoord(a, viewProjectionMatrix);
            Vector3 bView = TransformCoord(b, viewProjectionMatrix);
            Vector3 cView = TransformCoord(c, viewProjectionMatrix);

            // スクリーン座標系に変換
            Vector3 aScreen = TransformCoord(aView, viewportMatrix);
            Vector3 bScreen = TransformCoord(bView, viewportMatrix);
            Vector3 cScreen = TransformCoord(cView, viewportMatrix);

            // 線を描画
            Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(bScreen.x), int(bScreen.y), color);
            Novice::DrawLine(int(aScreen.x), int(aScreen.y), int(cScreen.x), int(cScreen.y), color);
        }
    }
}

#include "GameScene.h"
#include "TextureManager.h"
#include "MathUtilityForText.h"
#include <cassert>

//コンストラクタ
GameScene::GameScene() {}
//デストラクタ
GameScene::~GameScene() { 
	//背景
	delete spriteBG_; 
	//ステージ
	delete modelStage_;
	//プレイヤー
	delete modelPlayer_;
	//ビーム
	delete modelBeam_;


}
//初期化
void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	//背景
	textureHandleBG_ = TextureManager::Load("bg.jpg");
	spriteBG_ = Sprite::Create(textureHandleBG_, {0, 0});

	//カメラ
	viewProjection_.translation_.y = 1;
	viewProjection_.translation_.z = -6;
	viewProjection_.Initialize();

	//ステージ
	textureHandleStage_ = TextureManager::Load("stage.jpg");
	modelStage_ = Model::Create();
	worldTransformStage_.Initialize();

	//ステージの位置を変更
	worldTransformStage_.translation_ = {0, -1.5f,0};
	worldTransformStage_.scale_ = {4.5f, 1, 40};
	//変換行列を更新
	worldTransformStage_.matWorld_ = MakeAffineMatrix(
	    worldTransformStage_.scale_, 
		worldTransformStage_.rotation_,
	    worldTransformStage_.translation_);
	//変換行列を定数バッファに転送
	worldTransformStage_.TransferMatrix();

	//プレイヤー
	textureHandlePlayer_ = TextureManager::Load("player.png");
	modelPlayer_ = Model::Create();
	worldTransformPlayer_.scale_ = {0.5f, 0.5f, 0.5f};
	worldTransformPlayer_.Initialize();

	//ビーム
	textureHandleBeam_ = TextureManager::Load("Beam.png");
	modelBeam_ = Model::Create();
	worldTransformBeam_.scale_ = {0.2f, 0.2f, 0.2f};
	worldTransformBeam_.Initialize();
	

}
//更新
void GameScene::Update() { 
	
	PlayerUpdate();
	BeamUpdate();

}

void GameScene::PlayerUpdate() {
	if (input_->PushKey(DIK_RIGHT)) {
		worldTransformPlayer_.translation_.x += 0.1f;

	}

	if (input_->PushKey(DIK_LEFT)) {
		worldTransformPlayer_.translation_.x -= 0.1f;
	}
	if (worldTransformPlayer_.translation_.x>=4.0f) {
		worldTransformPlayer_.translation_.x = 4.0f;
	}
	if (worldTransformPlayer_.translation_.x<=-4.0f) {
		worldTransformPlayer_.translation_.x = -4.0f;
	}
	

	worldTransformPlayer_.matWorld_ = MakeAffineMatrix(
	    worldTransformPlayer_.scale_, worldTransformPlayer_.rotation_,
	    worldTransformPlayer_.translation_);

	worldTransformPlayer_.TransferMatrix();

}
void GameScene::BeamUpdate() {

	
	BeamMove();
	BeamBorn();


	worldTransformBeam_.matWorld_ = MakeAffineMatrix(
		worldTransformBeam_.scale_,worldTransformBeam_.rotation_,
		worldTransformBeam_.translation_
	);
	worldTransformBeam_.TransferMatrix();
	


}
void GameScene::BeamMove() {
	if (BeamFlag_==0) {
		return;
	}
	worldTransformBeam_.translation_.z += 0.3f;
	worldTransformBeam_.rotation_.x += 0.1f;
	if (worldTransformBeam_.translation_.z>40.f) {
		BeamFlag_ = 0;
	}
	
}
void GameScene::BeamBorn() {
	if (BeamFlag_ == 1) {
		return;
	}
	if (input_->TriggerKey(DIK_SPACE)) {
		BeamFlag_ = 1;
		worldTransformBeam_.translation_.x = worldTransformPlayer_.translation_.x;
		worldTransformBeam_.translation_.z = worldTransformPlayer_.translation_.z;
	}
}
    //描画
void GameScene::Draw() {

	
	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);
	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>
	spriteBG_->Draw();

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	modelStage_->Draw(worldTransformStage_, viewProjection_, textureHandleStage_);

	//プレイヤー
	modelStage_->Draw(worldTransformPlayer_, viewProjection_, textureHandlePlayer_);

	//ビーム
	if (BeamFlag_ == 1) {
		modelStage_->Draw(worldTransformBeam_, viewProjection_, textureHandleBeam_);
	}
	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}

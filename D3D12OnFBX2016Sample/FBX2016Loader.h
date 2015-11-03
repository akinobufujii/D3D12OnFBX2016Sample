#pragma once

//==============================================================================
// インクルード
//==============================================================================
#include <DirectXMath.h>
#include <vector>

//==============================================================================
// 各種定義
//==============================================================================
// 頂点データ
struct VertexData
{
	DirectX::XMFLOAT3 pos;		// 座標
	DirectX::XMFLOAT4 normal;	// 法線
	DirectX::XMFLOAT2 uv;		// UV座標
};

// 頂点データ配列
typedef std::vector<VertexData>	VertexDataArray;

//==============================================================================
// 関数定義
//==============================================================================

// FBXデータから頂点データにコンバート
// ※パスは「/」でしか通らない
//const char* filename = "../datas/box.fbx";
bool LoadFBXConvertToVertexData(const char* filename, VertexDataArray& outVertexData);

//==============================================================================
// End Of File
//==============================================================================

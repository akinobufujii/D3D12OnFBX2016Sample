#include <cstdio>
#include <fbxsdk.h>
#include "FBX2016Loader.h"

// 頂点データ獲得
static void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData);

// マテリアル表示
static void GetMatrialData(FbxSurfaceMaterial* mat);

// メッシュ情報処理(再帰関数)
static void GetMeshData(FbxNode *child, VertexDataArray& outVertexData);

// FBXデータから頂点データにコンバート
// ※パスは「/」でしか通らない
//const char* filename = "../datas/box.fbx";
bool LoadFBXConvertToVertexData(const char* filename, VertexDataArray& outVertexData)
{
	//==============================================================================
	// FBXオブジェクト初期化
	//==============================================================================
	// FBXマネージャー作成
	FbxManager* pFBXManager = FbxManager::Create();

	// シーン作成
	FbxScene* pScene = FbxScene::Create(pFBXManager, "");

	// FBXのIO設定オブジェクト作成
	FbxIOSettings *pIO = FbxIOSettings::Create(pFBXManager, IOSROOT);
	pFBXManager->SetIOSettings(pIO);

	// インポートオブジェクト作成
	FbxImporter* pImporter = FbxImporter::Create(pFBXManager, "");

	// ファイルインポート
	if(pImporter->Initialize(filename, -1, pFBXManager->GetIOSettings()) == false)
	{
		return false;
	}

	// シーンへインポート
	if(pImporter->Import(pScene) == false)
	{
		return false;
	}

	// ※この時点でインポートオブジェクトはいらない
	pImporter->Destroy();

	//==============================================================================
	// FBXオブジェクトの処理
	//==============================================================================
	// シーンのものすべてを三角化
	FbxGeometryConverter geometryConverte(pFBXManager);
	geometryConverte.Triangulate(pScene, true);

	// メッシュ情報処理
	GetMeshData(pScene->GetRootNode(), outVertexData);

	//==============================================================================
	// FBXオブジェクト色々破棄
	//==============================================================================
	pIO->Destroy();
	pScene->Destroy();
	pFBXManager->Destroy();

	getchar();

	return true;
}

// メッシュ情報処理(再帰関数)
void GetMeshData(FbxNode *parent, VertexDataArray& outVertexData)
{
	// メッシュだけ処理
	int numKids = parent->GetChildCount();
	for(int i = 0; i < numKids; i++)
	{
		FbxNode *child = parent->GetChild(i);

		// メッシュを見つけたら
		if(child->GetMesh())
		{
			FbxMesh* pMesh = child->GetMesh();// static_cast<FbxMesh*>(child->GetNodeAttribute());
			printf("メッシュ発見\n");

			printf("名前:%s\n", pMesh->GetName());
			printf("ポリゴン数:%d\n", pMesh->GetPolygonCount());
			printf("マテリアル数:%d\n", pMesh->GetElementMaterialCount());

			printf("コントロールポイント数(頂点座標):%d\n", pMesh->GetControlPointsCount());
			printf("UV数:%d\n", pMesh->GetTextureUVCount());

			FbxArray<FbxVector4> normals;
			pMesh->GetPolygonVertexNormals(normals);
			printf("法線数:%d\n", normals.GetCount());

			// 頂点情報取得
			GetFBXVertexData(pMesh, outVertexData);
		}

		// マテリアル
		int numMat = child->GetMaterialCount();
		for(int j = 0; j < numMat; ++j)
		{
			FbxSurfaceMaterial* mat = child->GetMaterial(j);
			if(mat)
			{
				GetMatrialData(mat);
			}
		}

		if(numMat == 0)
		{
			printf("マテリアルなし\n");
		}

		child->GetChild(0);

		// 更に子を処理
		GetMeshData(child, outVertexData);
	}
}

// 頂点データ獲得
void GetFBXVertexData(FbxMesh* pMesh, VertexDataArray& outVertexData)
{
	// 頂点座標と法線ベクトル獲得
	std::vector<FbxVector4> positions, normals;
	FbxVector4 normal;

	// メモ:GetPolygonCount = 面数、GetPolygonSize = 頂点数
	for(int i = 0; i < pMesh->GetPolygonCount(); i++)
	{
		for(int j = 0; j < pMesh->GetPolygonSize(i); j++)
		{
			// 頂点座標
			positions.push_back(pMesh->GetControlPointAt(pMesh->GetPolygonVertex(i, j)));

			// 法線ベクトル
			pMesh->GetPolygonVertexNormal(i, j, normal);
			normals.push_back(normal);
		}
	}

	// 配列初期化
	outVertexData.resize(positions.size());

	// 頂点座標と法線情報を格納
	for(size_t i = 0; i < outVertexData.size(); ++i)
	{
		fbxsdk::FbxVector4& pos		= positions[i];
		fbxsdk::FbxVector4& normal	= normals[i];

		outVertexData[i].pos	= DirectX::XMFLOAT3(pos.mData[0], pos.mData[1] , pos.mData[2]);
		outVertexData[i].normal = DirectX::XMFLOAT4(normal.mData[0], normal.mData[1], normal.mData[2], normal.mData[3]);
	}

	// UVセットの名前配列獲得
	FbxStringList uvSetNames;
	pMesh->GetUVSetNames(uvSetNames);

	bool unmapped = false;
	int UVCount = 0;

	for(int i = 0; i < uvSetNames.GetCount(); ++i)
	{
		for(int j = 0; j < pMesh->GetPolygonCount(); ++j)
		{
			for(int k = 0; k < pMesh->GetPolygonSize(j); ++k)
			{
				FbxVector2 UV;
				pMesh->GetPolygonVertexUV(j, k, uvSetNames.GetStringAt(i), UV, unmapped);

				if(outVertexData.size() > UVCount)
				{
					outVertexData[UVCount].uv = DirectX::XMFLOAT2(UV.mData[0], UV.mData[1]);
				}
				UVCount++;
			}
		}
	}
}

// マテリアルプロパティ獲得
FbxDouble3 GetMaterialProperty(
	const FbxSurfaceMaterial * pMaterial,
	const char * pPropertyName,
	const char * pFactorPropertyName)
{
	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if(lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if(lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}
	}

	if(lProperty.IsValid())
	{
		printf("テクスチャ\n");
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();
		for(int i = 0; i<lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if(lFileTexture)
			{
				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				printf("UVSet名=%s\n", uvSetString.c_str());
				printf("テクスチャ名=%s\n", filepath.c_str());
			}
		}
		puts("");

		printf("レイヤードテクスチャ\n");
		const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();
		for(int i = 0; i<lLayeredTextureCount; i++)
		{
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for(int j = 0; j<lTextureFileCount; j++)
			{
				FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
				if(lFileTexture)
				{
					FbxString uvsetName = lFileTexture->UVSet.Get();
					std::string uvSetString = uvsetName.Buffer();
					std::string filepath = lFileTexture->GetFileName();

					printf("UVSet名=%s\n", uvSetString.c_str());
					printf("テクスチャ名=%s\n", filepath.c_str());
				}
			}
		}
		puts("");
	}

	return lResult;
}

// マテリアル表示
void GetMatrialData(FbxSurfaceMaterial* mat)
{
	if(mat == nullptr)
	{
		return;
	}

	puts("");

	if(mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		printf("ランバートタイプ\n");
	}
	else if(mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		printf("フォンタイプ\n");
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat, FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor);
	printf("エミッシブカラー:r = %f, g = %f, b = %f\n", lEmissive.mData[0], lEmissive.mData[1], lEmissive.mData[2]);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat, FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor);
	printf("アンビエントカラー:r = %f, g = %f, b = %f\n", lAmbient.mData[0], lAmbient.mData[1], lAmbient.mData[2]);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat, FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor);
	printf("ディフューズカラー:r = %f, g = %f, b = %f\n", lDiffuse.mData[0], lDiffuse.mData[1], lDiffuse.mData[2]);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat, FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor);
	printf("スペキュラカラー:r = %f, g = %f, b = %f\n", lSpecular.mData[0], lSpecular.mData[1], lSpecular.mData[2]);

	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if(lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		printf("透明度 = %lf\n", lTransparencyFactor);
	}

	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if(lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		printf("スペキュラ = %lf\n", lShininess);
	}
}

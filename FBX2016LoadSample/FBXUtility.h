#ifndef	__AKI_FBX_UTILITY_H__
#define	__AKI_FBX_UTILITY_H__

#include <fbxsdk.h>

FbxString nodeTypeName(FbxNodeAttribute::EType attrType)
{
	switch(attrType)
	{
	case FbxNodeAttribute::eBoundary:
		return FbxString("eBoundary");
		break;
	case FbxNodeAttribute::eCachedEffect:
		return FbxString("eCachedEffect");
		break;
	case FbxNodeAttribute::eCamera:
		return FbxString("eCamera");
		break;
	case FbxNodeAttribute::eCameraStereo:
		return FbxString("eCameraStereo");
		break;
	case FbxNodeAttribute::eCameraSwitcher:
		return FbxString("eCameraSwitcher");
		break;
	case FbxNodeAttribute::eLight:
		return FbxString("eLight");
		break;
	case FbxNodeAttribute::eLine:
		return FbxString("eLine");
		break;
	case FbxNodeAttribute::eLODGroup:
		return FbxString("eLODGroup");
		break;
	case FbxNodeAttribute::eMarker:
		return FbxString("eMarker");
		break;
	case FbxNodeAttribute::eMesh:
		return FbxString("eMesh");
		break;
	case FbxNodeAttribute::eNull:
		return FbxString("eNull");
		break;
	case FbxNodeAttribute::eNurbs:
		return FbxString("eNurbs");
		break;
	case FbxNodeAttribute::eNurbsCurve:
		return FbxString("eNurbsCurve");
		break;
	case FbxNodeAttribute::eNurbsSurface:
		return FbxString("eNurbsSurface");
		break;
	case FbxNodeAttribute::eOpticalMarker:
		return FbxString("eOpticalMarker");
		break;
	case FbxNodeAttribute::eOpticalReference:
		return FbxString("eOpticalReference");
		break;
	case FbxNodeAttribute::ePatch:
		return FbxString("ePatch");
		break;
	case FbxNodeAttribute::eShape:
		return FbxString("eShape");
		break;
	case FbxNodeAttribute::eSkeleton:
		return FbxString("eSkeleton");
		break;
	case FbxNodeAttribute::eSubDiv:
		return FbxString("eSubDiv");
		break;
	case FbxNodeAttribute::eTrimNurbsSurface:
		return FbxString("eTrimNurbsSurface");
		break;
	case FbxNodeAttribute::eUnknown:
		return FbxString("eUnknown");
		break;
	default:
		return FbxString("TYPE NOT MATCHED");
		break;
	}
}


void traverseScene(FbxNode *root, int level = 0)
{
	// Level indentation
	for(int i = 0; i < level; i++)
	{
		if(i + 1 == level)
			FBXSDK_printf("+-");
		else
			FBXSDK_printf("  ");
	}

	//	Print out the name/type of this node
	//
	FbxString nodeName = root->GetName();
	FbxNodeAttribute *nodeAttr = root->GetNodeAttribute();
	FbxString nodeType = (nodeAttr == NULL) ? FbxString("IS NULL") : nodeTypeName(nodeAttr->GetAttributeType());

	FBXSDK_printf("Node %s (type %s)\n", nodeName.Buffer(), nodeType.Buffer());

	// Recurse on the node children
	int numKids = root->GetChildCount();
	for(int k = 0; k < numKids; k++)
	{
		FbxNode *child = root->GetChild(k);
		traverseScene(child, level + 1);
	}
}

#endif // __AKI_FBX_UTILITY_H__

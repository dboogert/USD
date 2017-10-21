#pragma once

#include "pxr/imaging/hd/sceneDelegate.h"


class SceneDelegate : public pxr::HdSceneDelegate
{
public:
	SceneDelegate(pxr::HdRenderIndex *parentIndex, pxr::SdfPath const& delegateID);

	void AddRenderTask(pxr::SdfPath const &id);
	void AddRenderSetupTask(pxr::SdfPath const &id);

	void SetCamera(pxr::GfMatrix4d const &viewMatrix, pxr::GfMatrix4d const &projMatrix);
	void SetCamera(pxr::SdfPath const &cameraId, pxr::GfMatrix4d const &viewMatrix, pxr::GfMatrix4d const &projMatrix);

	// interface Hydra uses query information from the SceneDelegate when processing an 
	// Prim in the RenderIndex
	pxr::VtValue Get(pxr::SdfPath const &id, const pxr::TfToken &key) override;
	bool GetVisible(pxr::SdfPath const &id) override;

	pxr::GfRange3d GetExtent(pxr::SdfPath const &id) override;
	pxr::GfMatrix4d GetTransform(pxr::SdfPath const &id) override;

	pxr::TfTokenVector GetPrimVarVertexNames(pxr::SdfPath const &id) override;
	pxr::TfTokenVector GetPrimVarFacevaryingNames(pxr::SdfPath const& id) override;
	pxr::TfTokenVector GetPrimVarInstanceNames(pxr::SdfPath const &id) override;

	void UpdateTransform();

	pxr::VtIntArray GetInstanceIndices(pxr::SdfPath const &instancerId, pxr::SdfPath const &prototypeId) override;

	pxr::GfMatrix4d GetInstancerTransform(pxr::SdfPath const &instancerId, pxr::SdfPath const &prototypeId) override;

	pxr::SdfPath GetPathForInstanceIndex(const pxr::SdfPath &protoPrimPath, int instanceIndex, int *absoluteInstanceIndex,
										 pxr::SdfPath *rprimPath,
										 pxr::SdfPathVector *instanceContext) override;

	pxr::HdMeshTopology GetMeshTopology(pxr::SdfPath const &id) override;

private:
	// per location (SdfPath) cache of dictionaries (TfToken -> VtValue) 
	typedef pxr::TfHashMap<pxr::TfToken, pxr::VtValue, pxr::TfToken::HashFunctor> _ValueCache;
	typedef pxr::TfHashMap<pxr::SdfPath, _ValueCache, pxr::SdfPath::Hash> _ValueCacheMap;
	_ValueCacheMap _valueCacheMap;

	// location of Camera (setup in ctor)
	pxr::SdfPath cameraPath;
	float rotation;
};
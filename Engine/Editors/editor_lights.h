#ifndef  INC_EDITOR_LIGHTS_H_
#define  INC_EDITOR_LIGHTS_H_

#define DEFAULT_LIGHTS "default_lights"

#include <vector>

//type of lights
class CHandle;
class TCompLightPoint;
class TCompLightDir;
class TCompLightDirShadows;

class CEditorLights {
public:

	enum TypeLight {
		POINT = 1,
		DIR,
		DIR_SHADOWS,
	};
	typedef std::vector<TypeLight> VTypeLights;

private:
	bool m_activated_editor;
	bool m_show_axis = false;
	std::string m_base_path;
	VHandles m_Lights;
	VTypeLights m_Types;

	//Temporal lights not fixed
	VHandles m_LightsTemp;
	VTypeLights m_TypesTemp;

public:

	bool* GetLightsEditorState() { return &m_activated_editor; }
	void SetLightEditorState(bool value) {
		m_activated_editor = value;
	}

	void ToggleEditorState() {
		m_activated_editor = !m_activated_editor;
	}

	void update(float dt);

	bool LoadLights();
	bool SaveLights(std::string filename = DEFAULT_LIGHTS);
	bool AddLightToSave(CHandle h, TypeLight type);
	bool AddLightToEngine(TypeLight type, bool* rooms);
	bool RemoveLight(CHandle h, std::vector<CHandle>& v_lights, std::vector<TypeLight>& v_types);
	bool HideLight(CHandle h);

	void SetRenderDebug(bool value, std::vector<CHandle> v_lights, std::vector<TypeLight> v_types);

	template <typename TLight>
	void renderLightComp(TLight * pl);
	//void renderLightDir(TCompLightDir * pl);
	//void renderLightDirShadows(TCompLightDirShadows * pl);

	bool GetShowAxis() const { return m_show_axis; }

	void RenderInMenu();
	void RenderLightList(VHandles& lights, VTypeLights& types, bool temporal);

	template <typename TLight>
	void RenderLight(CHandle& hlight, TypeLight type, bool temporal);

	void RenderTemporalLight(CHandle& light, TypeLight& type, bool& enabled);
};

#endif
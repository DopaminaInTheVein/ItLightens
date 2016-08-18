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

private:
	bool m_activated_editor;
	bool m_show_axis = false;
	std::string m_base_path;
	std::vector<CHandle> m_Lights;
	std::vector<TypeLight> m_Types;

	//Temporal lights not fixed
	std::vector<CHandle> m_LigthsTemp;
	std::vector<TypeLight> m_TypesTemp;


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
	bool AddLightToEngine(TypeLight type);
	bool RemoveLight(CHandle h, std::vector<CHandle>& v_lights, std::vector<TypeLight>& v_types);
	bool HideLight(CHandle h);

	void SetRenderDebug(bool value, std::vector<CHandle> v_lights, std::vector<TypeLight> v_types);

	void renderLightPoint(TCompLightPoint * pl);
	void renderLightDir(TCompLightDir * pl);
	void renderLightDirShadows(TCompLightDirShadows * pl);

	bool GetShowAxis() const { return m_show_axis; }

	void RenderInMenu();

	
};

#endif
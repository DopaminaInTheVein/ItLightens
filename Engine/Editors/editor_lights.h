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
	//CEditorLights();
	enum TypeLight {
		POINT = 1,
		DIR,
		DIR_SHADOWS,
	};

	enum MultiEditState {
		IDLE,
		EDITING,
	};

	typedef std::vector<TypeLight> VTypeLights;

	struct LightList {
		bool rcheck[ROOMS_SIZE];
	};

	struct EditLight {
		enum EditMode {
			OFFSET = 0
			, PROP
			, REPLACE
			, SIZE
		};
		float ar = 0;
		bool mode_ar[3] = { true, false, false };
		char mode_names[SIZE][20] = { "Offset(+)", "Proportional(*)", "Replace(=)" };
		int rgb[SIZE] = { 0 };
		bool mode_rgb[SIZE] = { true, false, false };
		void renderInMenu();
		template <typename TLight>
		void updateLight(TLight* light);
	};

private:
	bool m_activated_editor;
	bool m_show_axis = false;
	bool multi_editing = false;
	EditLight multi_edit_light;
	std::string m_base_path;
	VHandles m_Lights;
	VTypeLights m_Types;

	//Temporal lights not fixed
	VHandles m_LightsTemp;
	VTypeLights m_TypesTemp;
	LightList engine_list;
	LightList temp_list;
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

	bool GetShowAxis() const { return m_show_axis; }

	void RenderInMenu();
	void RenderGeneral();
	void RenderNewLight();
	void RenderAllLights();
	void RenderMultiEdit();

	void RenderLightList(VHandles& lights, VTypeLights& types, bool temporal, LightList& list);

	void RenderLight(CHandle& hlight, TypeLight& type, bool temporal);
	template <typename TLight>
	void RenderLight(CHandle& hlight, TypeLight& type, bool temporal);

	void RenderTemporalLight(CHandle& light, TypeLight& type, bool& enabled);
	void StartEditLight(CHandle hlight);
	void CancelEditLight(CHandle hlight);
	void SetSelected(CHandle hlight, bool sel);
	bool IsSelected(CHandle hlight);
	void UpdateEditingLight(CHandle hlight);
};

#endif
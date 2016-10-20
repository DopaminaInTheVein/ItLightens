#ifndef  INC_EDITOR_LIGHTS_H_
#define  INC_EDITOR_LIGHTS_H_

#define DEFAULT_LIGHTS ""

#include <vector>

//type of lights
class CHandle;
class TCompLightPoint;
class TCompLightDir;
class TCompLightDirShadows;

#define LightTemplate template <typename TLight>

class CEditorLights {
public:
	//CEditorLights();
	enum TypeLight {
		POINT = 1,
		DIR,
		DIR_SHADOWS,
		DIR_SHADOWS_DYN,
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
		struct LightParam {
			std::string name = "unnamed";
			float v = 0;
			EditMode mode = OFFSET;
			bool vmode[SIZE] = { false };
			bool changed_by_user = false;
			//float vmin, vmax;
			float rmin, rmax;
			float vspeed;
			void renderInMenu();
			void update(float* orig, float* dest);
			explicit LightParam(const char* _name, float speed, /*float min_edit, float max_edit, */float res_min, float res_max)
				: name(_name)
				, vspeed(speed)
				//, vmin(min_edit)
				//, vmax(max_edit)
				, rmin(res_min)
				, rmax(res_max)
			{
				vmode[mode] = true;
			}
			virtual void ToDisplay(float*) {}
			virtual void ToIntern(float*) {}
		};
		struct FovParam : LightParam {
			using LightParam::LightParam;
			void ToDisplay(float* f) override { *f = rad2deg(*f); }
			void ToIntern(float* f) override { *f = deg2rad(*f); }
		};

		static char mode_names[EditMode::SIZE][20];// = { "Offset(+)", "Proportional(*)", "Replace(=)" };
		LightParam pIntensity = LightParam("Intensity", 0.01f, 0.f, 5.f);
		LightParam pRed = LightParam("Red", 0.1f, 0.f, 1.f);
		LightParam pGreen = LightParam("Green", 0.1f, 0.f, 1.f);
		LightParam pBlue = LightParam("Blue", 0.1f, 0.f, 1.f);
		LightParam pNear = LightParam("Near", 0.1f, 0.01f, 1000.f);
		LightParam pFar = LightParam("Far", 0.1f, 1.f, 1000.f);
		FovParam pFov = FovParam("Fov", 0.1f, 1.f, 120.f);
		void renderInMenu();
		template <typename TLight>
		void updateLight(CHandle hlight);
	};

private:
	bool m_activated_editor;
	bool m_show_axis = false;
	bool multi_editing = false;
	bool delete_lights;
	EditLight multi_edit_light;
	std::string m_base_path;
	VHandles m_Lights;
	VTypeLights m_Types;

	//Temporal lights not fixed
	VHandles m_LightsTemp;
	VTypeLights m_TypesTemp;
	LightList engine_list;
	LightList temp_list;
	std::string last_scene_name = "";
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
	LightTemplate void LoadLights(TypeLight tl);

	bool SaveLights(std::string filename = DEFAULT_LIGHTS);
	LightTemplate bool SaveLight(CHandle hlight, std::ofstream& ofs, MKeyValue& atts);

	bool AddLightToSave(CHandle h, TypeLight type);
	bool AddLightToEngine(TypeLight type, bool* rooms);
	bool RemoveLight(CHandle h, std::vector<CHandle>& v_lights, std::vector<TypeLight>& v_types);

	bool HideLight(CHandle h);
	LightTemplate bool HideLight(CEntity* e);

	//void SetRenderDebug(bool value, std::vector<CHandle> v_lights, std::vector<TypeLight> v_types);
	LightTemplate void SetRenderDebug(bool value);

	LightTemplate void renderLightComp(TLight * pl);

	LightTemplate void DestroyLights();

	bool GetShowAxis() const { return m_show_axis; }
	void RenderInMenu();
	void RenderGeneral();
	void RenderNewLight();
	void RenderAllLights();
	void RenderMultiEdit();
	void DestroySelected();

	void RenderLightList(VHandles& lights, VTypeLights& types, bool temporal, LightList& list);

	void RenderLight(CHandle& hlight, TypeLight& type, bool temporal);
	LightTemplate void RenderLight(CHandle& hlight, TypeLight& type, bool temporal);

	void RenderTemporalLight(CHandle& light, TypeLight& type, bool& enabled);
	void StartEditLight(CHandle hlight);
	LightTemplate void StartEditLight(CHandle hlight);

	void CancelEditLight(CHandle hlight);
	LightTemplate void CancelEditLight(CHandle hlight);

	void SetSelected(CHandle hlight, bool sel);
	LightTemplate void SetSelected(CHandle hlight, bool sel);

	bool IsSelected(CHandle hlight);
	LightTemplate bool IsSelected(CHandle hlight);

	void UpdateEditingLight(CHandle hlight);
};

#endif
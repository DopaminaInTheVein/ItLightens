--
FILE_OPTIONS = "./data/json/options.json"
--

-- TEXTS AUXILIAR DECLARATION
---------------------------------------
--Difficulty
DIFF_SIZE = 0
TXT_DIFFICULTIES = {}
function DECLARE_DIFFICULTY(name)
	TXT_DIFFICULTIES[DIFF_SIZE] = "::"..name
	DIFF_SIZE = DIFF_SIZE + 1
end

--Languages
LANGS_SIZE = 0
LANGS_ID = {}
TXT_LANGS = {}
function DECLARE_LANG(name, id)
	TXT_LANGS[LANGS_SIZE] = name
	LANGS_ID[LANGS_SIZE] = id
	LANGS_SIZE = LANGS_SIZE + 1
end

--TEXTS--
---------------------------------------
--Axis
TXT_OPTION_AXIS_NORMAL = "::normal"
TXT_OPTION_AXIS_INVERTED = "::inverted"

--Difficulty
DECLARE_DIFFICULTY("easy")
DECLARE_DIFFICULTY("medium")
DECLARE_DIFFICULTY("hard")

--Languages
DECLARE_LANG("Català", "CAT")
DECLARE_LANG("Castellano", "ES")
DECLARE_LANG("English", "EN")
DECLARE_LANG("Galego", "GA")
---------------------------------------

-- EVENTS --
-------------------------------------------------------------------------------

--Create Guis
function OnCreateGui_opt_lang( )
	InitLanguage()
end
function OnCreateGui_opt_diff( )
	InitDifficulty()
end
function OnCreateGui_opt_axisX( )
	InitOptAxis("x")
end
function OnCreateGui_opt_axisY( )
	InitOptAxis("y")
end
function OnCreateGui_opt_music( )
	InitMusic()
end
function OnCreateGui_opt_sfx( )
	InitSfx()
end
function OnCreateGui_btn_opt_back( )
	p:wait_escape("OnClicked_btn_opt_back();")
end

--Choose Options
function OnChoose_opt_axisX(option)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	SaveAxis(option, "x")
end
function OnChoose_opt_axisY(option)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	SaveAxis(option, "y")
end
function OnChoose_opt_diff(option)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	SaveDifficulty(option)
end
function OnChoose_opt_lang(option)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	SaveLanguage(option)
end
function OnValueChanged_opt_music(value)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	MusicChanged(value)
end
function OnValueChanged_opt_sfx(value)
	p:play_sound("event:/OnMenuChange", 1.0, false)
	SfxChanged(value)
end

--Click go back
function OnClicked_btn_opt_back( )
	p:play_sound("event:/OnMenuCancel", 1.0, false)
	destroyOptions()
	if options_from == "pause" then
		OnCreateGui_btn_resume() --Simulamos q se crea otra vez el boton resume del menu
	end
	options_from = ""
end

-- Auxiliar functions --
-------------------------------------------------------------------------------
-- Language
function InitLanguage()
	h:getHandleCaller()
	lang = p:json_read_str(FILE_OPTIONS, "language", "lang")
	lang_int = -1
	for i=0, LANGS_SIZE-1 do
		--p:print("Add option "..TXT_LANGS[i])
		h:add_option(TXT_LANGS[i])
		if lang == LANGS_ID[i] then
			lang_int = i
		end
	end
	h:select_option(lang_int)
end

function SaveLanguage(option)
	p:json_edit_str(FILE_OPTIONS, "language", "lang", LANGS_ID[option])
	p:set_language(LANGS_ID[option])
end

-- Difficulty
function InitDifficulty()
	h:getHandleCaller()
	for i=0, DIFF_SIZE-1 do
		--p:print("Add option "..TXT_DIFFICULTIES[i])
		h:add_option(TXT_DIFFICULTIES[i])
	end
	diff = p:json_read(FILE_OPTIONS, "game", "difficulty")
	h:select_option(diff)
	if not g_is_menu then
		h:set_gui_enabled(false)
	end
end

function SaveDifficulty(option)
	p:json_edit(FILE_OPTIONS, "game", "difficulty", option)
end

--Axis
function InitOptAxis(axis)
	h:getHandleCaller()
	GUI_OPTION_AXIS_NORMAL = h:add_option(TXT_OPTION_AXIS_NORMAL)
	GUI_OPTION_AXIS_INVERTED = h:add_option(TXT_OPTION_AXIS_INVERTED)
	axis_inverted = p:json_read(FILE_OPTIONS, "controls", axis.."-axis_inverted")
	if axis_inverted == 1.0 then
		h:select_option(GUI_OPTION_AXIS_INVERTED)
	else
		h:select_option(GUI_OPTION_AXIS_NORMAL)
	end	
end
function SaveAxis(option, axis)
	switch(option) {
		[GUI_OPTION_AXIS_NORMAL] = function() val = 0 end,
		[GUI_OPTION_AXIS_INVERTED] = function() val = 1 end,
		default = function() val = -1 end, -- Esto no puede pasar
	}
	p:json_edit(FILE_OPTIONS, "controls", axis.."-axis_inverted", val)
end

--Sound
function InitMusic()
	p:print("Init music")
	h:getHandleCaller()
	val = p:json_read(FILE_OPTIONS, "sound", "music")
	h:set_drag_value(val)
end

function MusicChanged(value)
	p:print("Music Changed: "..value)
	p:json_edit(FILE_OPTIONS, "sound", "music", value)
	p:set_music_volume(value)
end

sfx_volume = 0.0
function InitSfx()
	p:print("Init sfx")
	h:getHandleCaller()
	sfx_volume = p:json_read(FILE_OPTIONS, "sound", "sfx")
	h:set_drag_value(val)
	sfx_prev = sfx_volume
	sfx_prev_prev = sfx_prev
	testSound()
end

function SfxChanged(value)
	p:print("Sfx Changed: "..value)
	p:json_edit(FILE_OPTIONS, "sound", "sfx", value)
	p:set_sfx_volume(value)
	sfx_volume = value
end

sfx_prev = 0.0
sfx_prev_prev = 0.0
function testSound()
	-- p:print("sfx_prev: "..sfx_prev)
	-- p:print("sfx_prev_prev: "..sfx_prev_prev)
	-- p:print("sfx: "..sfx_volume)
	if sfx_prev == sfx_volume then
		if sfx_prev_prev ~= sfx_prev then
			p:play_sound("event:/OnMoleStepParquetL1", 1.0, false)
			sfx_prev_prev = sfx_prev
		end
	end
	sfx_prev_prev = sfx_prev
	sfx_prev = sfx_volume
	p:exec_command("testSound();", 0.05)
end

--Destroy Menu
function destroyOptions( )
	DestroyAllByTag("options")
end
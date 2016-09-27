--
FILE_OPTIONS = "./data/json/options.json"
--

-- TEXTS AUXILIAR DECLARATION
---------------------------------------
--Difficulty
DIFF_SIZE = 0
TXT_DIFFICULTIES = {}
function DECLARE_DIFFICULTY(name)
	TXT_DIFFICULTIES[DIFF_SIZE] = name
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
TXT_OPTION_AXIS_NORMAL = "normal"
TXT_OPTION_AXIS_INVERTED = "inverted"

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

--Choose Options
function OnChoose_opt_axisX(option)
	SaveAxis(option, "x")
end
function OnChoose_opt_axisY(option)
	SaveAxis(option, "y")
end
function OnChoose_opt_diff(option)
	SaveDifficulty(option)
end
function OnChoose_opt_lang(option)
	SaveLanguage(option)
end

--Click go back
function OnClicked_btn_opt_back( )
	destroyOptions()
end

-- Auxiliar functions --
-------------------------------------------------------------------------------
-- Language
function translate_opt(text)
	return p:get_text("title", text)
end

function InitLanguage()
	h:getHandleCaller()
	lang = p:json_read_str(FILE_OPTIONS, "language", "lang")
	lang_int = -1
	for i=0, LANGS_SIZE-1 do
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
		h:add_option(translate_opt(TXT_DIFFICULTIES[i]))
	end
	diff = p:json_read(FILE_OPTIONS, "game", "difficulty")
	h:select_option(diff)
end

function SaveDifficulty(option)
	p:json_edit(FILE_OPTIONS, "game", "difficulty", option)
end

--Axis
function InitOptAxis(axis)
	h:getHandleCaller()
	GUI_OPTION_AXIS_NORMAL = h:add_option(translate_opt(TXT_OPTION_AXIS_NORMAL))
	GUI_OPTION_AXIS_INVERTED = h:add_option(translate_opt(TXT_OPTION_AXIS_INVERTED))
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

--Destroy Menu
function destroyOptions( )
	DestroyAllByTag("options")
end
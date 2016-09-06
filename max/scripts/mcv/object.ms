-- Some general data
TAG_PLAYER = "player"
TAG_CAMERA = "camera_main"

------ INDEXES of Descriptor Info --------
NAME_IDX = 1
TAGS_IDX = 2
ATTRIBUTES_IDX = 3
COMPONENTS_IDX = 4
HELPERS_IDX = 5
--------- Object Descriptor --------------
struct ObjectDescriptor (
	theName = undefined
	,theTags = #()
	,theAttributes = #()
	,theComponents = #()
	,theHelpers = #()
	
	,fn create info = (
		theName = info[NAME_IDX]
		theTags = info[TAGS_IDX]
		theAttributes = info[ATTRIBUTES_IDX]
		theComponents = info[COMPONENTS_IDX]
		theHelpers = info[HELPERS_IDX]
	)
	
	,fn getType = ( return theName )
	,fn getTags = ( return theTags )
	,fn getAttributes = ( return theAttributes )
	,fn getComponents = ( return theComponents )
	,fn getHelpers = ( return theHelpers )
)
---------------------------------------------------------------------

----------Object Manager -----------------------------------------
struct OM (
	hColliders = #("colliderBox","colliderConvex","colliderSphere")
	,hPolarized = #("pol_minus","pol_neutral","pol_plus", "area_plus", "area_minus")
	,hPatrol = #("patrol_go", "patrol_look")
	,hNpcs = #("patrol_go", "patrol_look", "jurisdiction", "charController")
	,allTheObjectsDescription = #(
		----------------------------------------------------------------------------------------------------------------------------------------------------
		#("undefined" --Default Object. Must be the first!
			-- Tags
			,#()
			-- Attributes
			,#("id", "slept", "mesh", "static_mesh", "skeleton", "logic")
			--Components
			,#("name", "transform",  "render", "physx", "polarized", "tracker", "polarizedArea", "room")
			--Helpers
			,hPolarized + hColliders
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Beacon"
			-- Tags
			,#("beacon")
			-- Attributes
			,#()
			--Components
			,#("beacon")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Box"
			-- Tags
			,#("box","platform", "removable", "interactive")
			-- Attributes
			,#("fixed")
			--Components
			,#("box", "bone_tracker")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("BoxPlacer"
			-- Tags
			,#("box_placer")
			-- Attributes
			,#()
			--Components
			,#("box_placer")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Pila"
			-- Tags
			,#("platform", "pila", "interactive")
			-- Attributes
			,#()
			--Components
			,#("pila", "bone_tracker")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("PilaContainer"
			-- Tags
			,#("pila_container", "interactive")
			-- Attributes
			,#()
			--Components
			,#("pila_container")
			--Helpers
			,#()
		)

		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Drone"
			-- Tags
			,#("drone","platform")
			-- Attributes
			,#("textureLeft", "textureRight")
			--Components
			,#("drone")
			--Helpers
			,#("patrol_go", "area_minus", "area_plus")
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("BreakableWall"
			-- Tags
			,#("breakable_wall", "platform")
			-- Attributes
			,#()
			--Components
			,#()
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("CamController"
			-- Tags
			,#(TAG_CAMERA)
			--- Attributes
			,#()
			--Components
			,#("camera_main", "controller_3rd_person", "FX_fade_screen")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("CrystalWall"
			-- Tags
			,#("crystal")
			-- Attributes
			,#()
			--Components
			,#()
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Player"
			-- Tags
			,#(TAG_PLAYER, "raijin")
			-- Attributes
			,#("life")
			--Components
			,#("player", "life", "skc_player", "skeleton_lookat")
			--Helpers
			,#("charController")
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Guard"
			-- Tags
			,#("AI","AI_guard")
			-- Attributes
			--,#("life", "skeleton")
			,#("skeleton")
			--Components
			--,#("life","bt_guard", "skc_guard", "skeleton_lookat")
			,#("bt_guard", "skc_guard", "skeleton_lookat")
			--Helpers
			,hNpcs + #("Formation_Go", "Formation_Look")
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Mole"
			-- Tags
			,#("AI","AI_mole","AI_poss")
			-- Attributes
			,#("life")
			--Components
			--,#("bt_mole", "player_mole", "life", "skc_mole")
			,#("bt_mole", "player_mole", "skc_mole")
			--Helpers
			,hNpcs
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Scientist"
			-- Tags
			,#("AI","AI_cientifico","AI_poss")
			-- Attributes
			,#("life")
			--Components
			--,#("bt_scientist", "player_cientifico", "life", "skc_scientist")
			,#("bt_scientist", "player_cientifico", "skc_scientist")
			--Helpers
			,hNpcs
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Speedy"
			-- Tags
			,#("AI", "AI_speedy", "AI_poss")
			-- Attributes
			,#("life")
			--Components
			--,#("bt_speedy", "player_speedy", "life")
			,#("bt_speedy", "player_speedy")
			--Helpers
			,hNpcs
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Trigger"
			-- Tags
			,#()
			-- Attributes
			,#("onEnter", "onLeave", "component")
			--Components
			,#("trigger")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Water"
			-- Tags
			,#("water")
			-- Attributes
			,#()
			--Components
			,#("water")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("AreaPol"
			-- Tags
			,#()
			-- Attributes
			,#()
			--Components
			,#("polarizedArea")
			--Helpers
			,#()
		)----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("railCamera"
			-- Tags
			,#("guided_camera")
			-- Attributes
			,#(speed)
			--Components
			,#("guided_camera")
			--Helpers
			,#()
		)----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("helperArrow"
			-- Tags
			,#("helper_arrow")
			-- Attributes
			,#(speed)
			--Components
			,#("helper_arrow")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("magnetDoor"
			-- Tags
			,#("door")
			-- Attributes
			,#("speedOpening", "speedClosing", "locked")
			--Components
			,#("magnet_door")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("elevator"
			-- Tags
			,#("elevator", "interactive")
			-- Attributes
			,#("speedUp", "speedDown")
			--Components
			,#("elevator")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Workstation"
			-- Tags
			,#("workstation")
			-- Attributes
			,#("animation")
			--Components
			,#("workstation")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("workbench"
			-- Tags
			,#("workbench", "interactive")
			-- Attributes
			,#()
			--Components
			,#("workbench")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("LookTarget"
			-- Tags
			,#("look_target")
			-- Attributes
			,#()
			--Components
			,#("look_target", "abs_aabb")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Gui"
			-- Tags
			,#("gui")
			-- Attributes
			,#()
			--Components
			,#("gui")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("GuiCursor"
			-- Tags
			,#("gui", "gui_cursor")
			-- Attributes
			,#()
			--Components
			,#("gui", "gui_cursor")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("GuiButton"
			-- Tags
			,#("gui", "gui_button", "clickable")
			-- Attributes
			,#()
			--Components
			,#("gui", "gui_button")
			--Helpers
			,#()
		)
----------------------------------------------------------------------------------------------------------------------------------------------------
		,#(""
			-- Tags
			,#()
			-- Attributes
			,#()
			--Components
			,#()
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
	)
	-- Functions --
	,fn getDescriptor obj = (
		-- Object
		local theName = getUserProp obj "typeObject"
		local tDescriptor = allTheObjectsDescription[1]
		for tDesc in allTheObjectsDescription do (
			if tDesc[NAME_IDX] == (theName as string) then (
				tDescriptor = tDesc
			)
		)
		
		-- Si no undefined añadimos las cosas de undefined
		if theName != undefined then (
			tDefault = allTheObjectsDescription[1]
			local i
			for i = TAGS_IDX to tDescriptor.count do (
				for elem in tDefault[i] do (
					appendIfUnique tDescriptor[i] elem
				)
			)
		)
		
		local oDesc = ObjectDescriptor()
		oDesc.create tDescriptor

		return oDesc
	)
)
OManager = OM()
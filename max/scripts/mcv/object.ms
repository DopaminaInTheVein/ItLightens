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
	hColliders = #("colliderBox","colliderConvex")
	,hPolarized = #("pol_minus","pol_neutral","pol_plus")
	,hPatrol = #("patrol_go", "patrol_look")
	,hNpcs = #("patrol_go", "patrol_look", "jurisdiction", "charController")
	,allTheObjectsDescription = #(
		----------------------------------------------------------------------------------------------------------------------------------------------------
		#("undefined" --Default Object. Must be the first!
			-- Tags
			,#()
			-- Attributes
			,#("mesh", "static_mesh", "skeleton")
			--Components
			,#("name", "transform",  "render", "physx", "polarized", "tracker")
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
			,#("box","platform")
			-- Attributes
			,#("fixed")
			--Components
			,#("box")
			--Helpers
			,#()
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
			,#("camera", "controller_3rd_person")
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
			,#(TAG_PLAYER)
			-- Attributes
			,#("life")
			--Components
			,#("player", "life")
			--Helpers
			,#("charController")
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Guard"
			-- Tags
			,#("AI","AI_guard")
			-- Attributes
			,#("life")
			--Components
			,#("life","bt_guard")
			--Helpers
			,hNpcs
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Mole"
			-- Tags
			,#("AI","AI_mole","AI_poss")
			-- Attributes
			,#("life")
			--Components
			,#("bt_mole", "player_mole", "life")
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
			,#("bt_speedy", "player_speedy", "life")
			--Helpers
			,#()
		)
		----------------------------------------------------------------------------------------------------------------------------------------------------
		,#("Trigger"
			-- Tags
			,#()
			-- Attributes
			,#("onEnter", "onExit", "component")
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
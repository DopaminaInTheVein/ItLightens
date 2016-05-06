clearListener()

struct  TSkeletonsExporter (
	project_path = data_dir,
	skel_name = "unknown",
	skel_root_path,
	skel_file, 

	fn exportModel obj = (
		local modelFilePath = (skel_root_path + skel_name + ".skeleton")
		local fsm = createFile modelFilePath
		try (
			format "<model>\n" to:fsm
			format "  <skeleton    src =\"%\"/>\n" ("skeletons/" + skel_name + "/" + skel_name + ".csf") to:fsm
			format "  <animations src =\"%\"/>\n" ("skeletons/" + skel_name + "/") to:fsm
			format "  <mesh     src=\"%\"/>\n" ("skeletons/" + skel_name + "/" + skel_name + ".cmf") to:fsm
			format "</model>\n" to:fsm
		) catch (
			throw()
			close fsm
		)
		close fsm		
	),
	
	fn isBone obj = ( 
		-- Discard black bones
		return classof obj == Biped_Object and obj.wireColor != Black
	),
	
	-- Search the root bones in the scene to define the .csf
	fn findSkelCandidates = (
		local obj_bones = #()
		for obj in $* do (
			if isBone obj and obj.parent == undefined then (
				format "% is a root bone\n" obj.name
				append obj_bones obj
			)
		)
		return obj_bones
	),
	
	-- Find in the scene the bones which will be exported as part of the animation
	-- Change the color to black to discard the bones
	fn findBoneCandidates = (
		local objs = #()
		for obj in $* do (
			if isBone obj then (
				format "% is a bone for the animation\n" obj.name
				append objs obj
			)
		)
		return objs
	),
	
	-- Returns an array of meshes which have the skin modifier and will be exported as .cmf
	fn findMeshCandidates = (
		local objs = #()
		for obj in $* do (
			if (classof obj == PolyMeshObject) or (classof obj == Editable_Mesh) then (
				if obj.modifiers[#Skin] != undefined then (
					format "% is a skinned mesh \n" obj.name
					append objs obj
				)
			)
		)
		return objs
	),
	
	
	fn setSkelName new_name = (
		skel_name = new_name
		skel_root_path = project_path + "skeletons/" + skel_name + "/"
		if not (doesFileExist skel_root_path) then (
			makedir skel_root_path
		)
		skel_file = skel_root_path + skel_name + ".csf"
	),
	
	fn findRootBone = (
		local root_bones = findSkelCandidates()
		local root = root_bones[1]
		setSkelName root.name
		return root_bones
	),
	
	fn exportSkels = (
		
		local root_bones = findRootBone()
		local root = root_bones[1]
		
		-- Pasar a modo figura si esta en modo pose
		local was_in_figure_mode = root.controller.figureMode
		root.controller.figureMode = true
		local rc = ExportCalSkel skel_file root_bones off
		
		-- Export la mesh...
		local obj_meshes = findMeshCandidates()
		for obj in obj_meshes do (
			local mesh_file = skel_root_path + obj.name + ".cmf"
			-- 0.01 threshold, no LOD, no springs
			local rc = ExportCalMesh mesh_file skel_file obj 4 0.01 false false
					
			-- Export static mesh skel
			tr = TResources()
			tr.exportStaticMeshSkel obj.name obj
		)

		-- Export model (.skeleton)
		exportModel(root)
		
		-- Restaurar el modo pose si es q estaba en modo pose inicialmente
		if was_in_figure_mode == false then root.controller.figureMode = false
    ),
	
	fn exportAnim = (
		local root_bones = findRootBone()
		
		-- Export anims
		local anim_name = getFilenameFile  maxfilename
		local animation_file = skel_root_path + anim_name + ".caf"
		local bones_to_export = findBoneCandidates()
		-- frames to integer-frames
		-- From 60f we need to get the 60 as integer
		local begin_time =  animationRange.start as integer * frameRate / 4800  
		local end_time = animationRange.end as integer  * frameRate / 4800
		local rc = ExportCalAnim animation_file skel_file bones_to_export begin_time end_time 0 frameRate
	),
	
	fn prueba = (
		local begin_time =  animationRange.start as integer * frameRate / 4800  
		local end_time = animationRange.end as integer  * frameRate / 4800
	
		local root_bones = findRootBone()
		local root = root_bones[1]	
		
		local yaw = (root.transform.rotationPart as eulerAngles).z
		local deltaYaws = #()
		local finish = false
		sliderTime = 1
		while not finish do (
			local new_yaw = (root.transform.rotationPart as eulerAngles).z
			append deltaYaws (new_yaw - yaw)
			yaw = new_yaw
			sliderTime += 1
			finish = (sliderTime >= end_time)
		)
		print deltaYaws
	)
)

gc()
--se = TSkeletonsExporter();
--se.prueba()
--se.exportSkels()
--se.exportAnim()
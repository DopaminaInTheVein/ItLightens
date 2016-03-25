struct TResources (
	project_path = data_dir ,
	mode = "all",
	
	fn getResourceName obj nameResource = (
		local res = getUserProp obj nameResource
		case mode of (
			"all":
			(
				if res == undefined do (res = obj.name)
			)
			"old":
			(
				--Nothing to do
			)
			"new":
			(
				if res != undefined then res = undefined
				else res = obj.name
			)
			"forceNew":
			(
				res = obj.name
			)
		)
		return res
	),
	
	fn saveTextureSlot fs slot bitmap_texture = (
		local textures_path = "textures/"
		-- if not found, use some default textures
		if bitmap_texture == undefined then (
			--local aname = "missing_" + slot + ".dds"
			--format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		) else (
			local full_name = bitmap_texture.bitmap.filename
			local aname = filenameFromPath full_name
			format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		)
	),

	fn saveStdMaterial fs m = (
		if (m != undefined) then (
			if classof m != Standardmaterial then throw "Invalid material recv"
			format "<material tech=\"textured.tech\">\n" to:fs
			saveTextureSlot fs "diffuse" m.diffusemap
			saveTextureSlot fs "specular" m.specularmap
			format "</material>\n" to:fs
		) else (
			format "WARNING: - - Undefined material ignored!! - -\n"
		)
	),

	fn saveMultiMaterial fs multi_mat = (
		if classof multi_mat != Multimaterial then throw "Invalid material recv. Need a Multimaterial"
		local m
		for m in multi_mat do (
			-- Create a file for each material?
			saveStdMaterial fs m
		)
	),
	
	fn saveMaterial fs mat = (
		if classof mat == Multimaterial then (
			saveMultiMaterial fs mat
		) else if classof mat == Standardmaterial then (
			saveStdMaterial fs mat
		) else 
			throw ("Unsupported material " + ((classof mat) as string))
	),
	
	fn getMeshMaterialInfo obj = (
		local mat = obj.mat
		local mes = obj.mesh
		
		local res
		if classof mat == MultiMaterial then (
			res = #()
			for f = 1 to mes.faces.count do (
				local m = getFaceMatID mes f
				if res[m] == undefined then (
					res[m] = #(f)
				) else (
					append res[m] f
				)
			)
		) else (
			res = #(#()) 
			for f = 1 to mes.faces.count do (
				res[1][f] = f
			)
		)
		return res
	),
		
	fn exportStaticMesh out_filename objs = (
		local fsm = createFile out_filename
		format "<static_mesh>\n" to:fsm
		
		local obj
		for obj in objs do (
			local m = obj.mat
			if m == undefined then (
				throw ("Object " + obj.name + " requires a material\n")
			) 
			
			-- #(   #(faces_with_mat1) undef undef #(faces_with_mat4) )
			local mat_infos = getMeshMaterialInfo obj
			
			local meshName = getResourceName obj "mesh"
			if meshName == undefined then (
				throw "-- ERROR -- [" + obj.name + "] Objeto corrupto! ¡Tiene mesh pero no staticMesh!\n"
			)
			local filenameXML = "meshes/" + meshName + mesh_ext
			export_mesh obj meshName mat_infos
			
			local used_mats = #()
			local mat_info
			local mesh_idx = 0
			local mat_idx = 0
			for mat_info in mat_infos do (
				local mat_used = m
				mat_idx = mat_idx + 1
				if mat_info == undefined then continue
				
				if classof m == MultiMaterial then (
					mat_used = m[ mat_idx ]
					if( mat_used == undefined ) then (
						throw ("El obj" + obj.name + " uses a multimaterial which is not defining the submaterial index " + mat_idx )
					)
					if( classof mat_used != StandardMaterial ) then (
						throw ("El obj" + obj.name + " uses a multimaterial which in slot "+ mat_idx + " does not contain a std material" )
					)
				)
				local mat_name = mat_used.name
				
				tieneEspacios = (findString mat_name " ")
				tieneAlmohadilla = (findString mat_name "#")
				if (tieneEspacios != undefined) do (
					throw ("ERROR: -- Nombre de material incorrecto: " + mat_name)
				)
				if (tieneEspacios != undefined) do (
					throw ("ERROR: -- Nombre de material incorrecto: " + mat_name)
				)
				
				if not appendifunique used_mats mat_name then (
					throw ("El obj '" + obj.name + "' uses the same material '" + mat_name + "' with more than one material ids (one is mat_id " + (mat_idx as string ) + "). Please, combine both material id's in a single number.")
				)
				
				local mat_res_name = "materials/" + mat_name + ".material"
				local mat_file = createFile (materials_dir + mat_name + ".material")
				saveMaterial mat_file mat_used
				close mat_file
				format "  <slot mesh=\"%\" material=\"%\" mesh_idx=\"%\"/>\n" filenameXML mat_res_name mesh_idx to:fsm
				mesh_idx = mesh_idx + 1
			)
			
		)
		format "</static_mesh>\n" to:fsm
		close fsm
	),

	fn exportCompStaticMesh obj = (
		rStMesh = getResourceName obj "staticMesh"
		if rStMesh != undefined do (
			local static_mesh_name = "static_meshes/" + rStMesh + ".static_mesh"
			--format "  <render_static_mesh name=\"%\"/>\n" static_mesh_name to:fs
			exportStaticMesh (project_path + static_mesh_name) #(obj)
			setUserProp obj "staticMesh" rStMesh
			local rMesh = getResourceName obj "mesh"
			setUserProp obj "mesh" rMesh
		)
	),
	
/* 	fn exportCompHierarchy obj = (
* 		--local my_tmx = obj.transform
* 		--local parent_tmx = obj.parent.transform
* 		--local delta = in sys_local
* 		local delta = matrix3 1   -- esto esta mal!!
* 		
* 		local extras = "parent=\"" + obj.parent.name + "\""
* 		exportTransform delta "hierarchy" extras
* 	),
* 
* 		fn exportCompTransform obj = (
* 		local max_tmx = obj.transform		-- matrix3 
* 		exportTransform max_tmx "transform" ""
* 	),
*/
	
	fn exportObject obj = (
		--exportCompName obj
		if classof obj != Editable_mesh and classof obj != Editable_Poly then (
			ConvertToMesh obj
		)
		exportCompStaticMesh obj
		--if obj.parent != undefined then exportCompHierarchy fs obj
		
		
		-- Export the children
		for c in obj.children do (
			exportObject fs c 
		)
	),
	
	fn exportSelected = (
		if $ == undefined then return "(nothing to export)\n"
		
		for obj in $ do (
			exportObject obj
		)
		mode = "all"
	),
	
	fn rexport = (
		mode = "old"
		exportSelected()
	),

	fn exportSelectedForceNew = (
		mode = "forceNew"
		exportSelected()	
	),
	
	fn exportSelectedNew = (
		mode = "new"
		exportSelected()	
	),
	
	fn exportScene = (--scene_name = ( 
		-- For each obj in the scene
		local obj
		for obj in $ do (
			-- Only export the root objects of the scene
			-- The children are exported by the parents
			if obj.parent == undefined then (
				--try (
					exportObject obj
				--) catch (
					--format "-- (%): % --\n" getCurrentException()
				--)
			)
		)
	)
)

--gc()
--tr = TResources();
--tr.exportScene
--tr.exportSelected();

clearListener()

struct TSceneExporter (
	project_path = "C:/Code/engine/bin/data/",
	
	fn getMeshMaterialInfo obj = (
		
		local faces_by_material_id = #()
		local face_id
		local num_faces = getNumFaces obj
		for face_id = 1 to num_faces do (
			
			local mat_id = getFaceMatID obj face_id 
			if faces_by_material_id[ mat_id ] == undefined then (
				faces_by_material_id[ mat_id ] = #()
			)
			append faces_by_material_id[ mat_id ] face_id 
			
		)
		
		return faces_by_material_id
	),
	
	fn saveTextureSlot fs slot bitmap_texture = (
		local textures_path = "textures/"
		-- if not found, use some default textures
		if bitmap_texture == undefined then (
			local aname = "missing_" + slot + ".dds"
			format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		) else (
			local in_full_name = bitmap_texture.bitmap.filename			-- c:\escritorio\pep.jpg
			local in_name = getFilenameFile in_full_name				-- pep
			local in_ext = getFilenameType in_full_name                 -- .jpg
			local out_ext = ".dds"                                      -- .dds
			local game_name = textures_path + in_name + out_ext         -- textures/pep.dds
			local out_folder = project_path + textures_path              -- c:\engine...data\textures
			local full_game_name = project_path + game_name
			
			if not doesFileExist full_game_name then (
				-- Convert from jpg to .dds
				local cmd = "\"" + (getDir #maxroot) 			--  "c:\program files\...\3ds max 2015\"
				cmd += "texconvex.exe\""
				cmd += " -o " + out_folder
				cmd += " \"" + in_full_name + "\""
				cmd = substituteString  cmd "\\" "/"
				format "Executing '%'\n" cmd
				DOSCommand cmd
			)	
			
			format "<texture slot=\"%\" name=\"%\"/>\n" slot game_name to:fs 
		)
	),

	fn saveStdMaterial fs m = (
		if classof m != Standardmaterial then throw "Invalid material recv"
		format "<material>\n" to:fs
		saveTextureSlot fs "diffuse" m.diffusemap
		saveTextureSlot fs "specular" m.specularmap
		saveTextureSlot fs "normalmap" m.bumpmap
		format "</material>\n" to:fs
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
	
	fn matrixMax2MCV mtx = (
		local tm = rotateXMatrix -90
		local tm_inv = inverse tm
		return tm_inv * mtx * tm
	),
	
	
	fn exportCompName fs obj = (
		format "  <name name=\"%\"/>\n" obj.name to:fs
	),
	
	fn exportTransform fs max_tmx tag extras = (
		local mcv_tmx = matrixMax2MCV max_tmx
		local t = mcv_tmx.pos
		local q = mcv_tmx.rotationPart as quat
		local s = mcv_tmx.scalePart
		if s.x < 0 or s.y < 0 or s.z < 0 then throw ("Object " + obj.name + " has scale transform with negative values. Please apply a ResetTransform, fix the culling and retry")
		--if s.x != s.y or s.y != s.z then throw ("Object " + obj.name + " has non-uniform scales. Please fix it and try again " + (s as string) )
		-- We change the sign of the quaternion because max conversion from matrix to quaternion
		-- rotates in the opposite direction from DX libs
		format "  <% pos=\"% % %\" quat=\"% % % %\" scale=\"%\" %/>\n" tag t.x t.y t.z -q.x -q.y -q.z q.w s.x extras to:fs
	),
	
	fn exportCompTransform fs obj = (
		local max_tmx = obj.transform		-- matrix3 
		exportTransform fs max_tmx "transform" ""
	),
	
	fn exportStaticMesh out_filename objs = (
	
		
		local fsm = createFile out_filename
		format "<static_mesh>\n" to:fsm
		
		local obj
		for given_obj in objs do (
			
			local m = given_obj.mat
			if m == undefined then (
				format ("Object " + given_obj.name + " requires a material\n")
				continue
			) 
					
			-- Is the object valid?...
			local obj = given_obj
			if classof given_obj != Editable_Mesh then (
				obj = copy given_obj
				ConvertToMesh obj
			)
			
			-- #(   #(faces_with_mat1) undef undef #(faces_with_mat4) )
			local mat_infos = getMeshMaterialInfo obj
			
			local filename = "meshes/" + obj.name + ".mesh"
			export_mesh obj (project_path + filename) mat_infos
			
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
				
				if not appendifunique used_mats mat_name then (
					throw ("El obj '" + obj.name + "' uses the same material '" + mat_name + "' with more than one material ids (one is mat_id " + (mat_idx as string ) + "). Please, combine both material id's in a single number.")
				)
				
				local mat_res_name = "materials/" + mat_name + ".material"
				
				-- Export the material as xml
				local out_material_filename = project_path + "materials/" + mat_used.name + ".material"
				local fs_mat = createFile out_material_filename
				saveMaterial fs_mat mat_used
				close fs_mat
				
				format "  <slot mesh=\"%\" material=\"%\" mesh_idx=\"%\"/>\n" filename mat_res_name mesh_idx to:fsm
				mesh_idx = mesh_idx + 1
			)
			
			--if I did a copy, it's time to delete it
			if obj != given_obj then 
				delete obj
			
		)
		format "</static_mesh>\n" to:fsm
		close fsm
	),
		
	
	fn exportCompStaticMesh fs obj = (
		local static_mesh_name = "static_meshes/" +obj.name  + ".static_mesh"
		format "  <render_static_mesh name=\"%\"/>\n" static_mesh_name to:fs
		exportStaticMesh (project_path + static_mesh_name) #(obj)
	),
	
	fn exportCompHierarchy fs obj = (
		--local my_tmx = obj.transform
		--local parent_tmx = obj.parent.transform
		--local delta = in sys_local
		local my_transform = obj.transform
		local parent_transform = obj.parent.transform
		local delta = my_transform * (inverse parent_transform)
		
		local extras = "parent=\"" + obj.parent.name + "\""
		exportTransform fs delta "hierarchy" extras
	),

	fn exportCompTransform fs obj = (
		local max_tmx = obj.transform		-- matrix3 
		exportTransform fs max_tmx "transform" ""
	),
	
	fn isGeometry obj = (
		return  classof obj == Editable_mesh or classof obj == Editable_Poly or classof obj == Plane or classof obj ==  Teapot or classof obj == GeoSphere or classof obj ==Torus or classof obj == Box
	),
	
	fn exportObject fs obj = (
		
		format "<entity>\n" to:fs
		exportCompName fs obj
		exportCompTransform fs obj
		if isGeometry obj then (
			exportCompStaticMesh fs obj
		)
		if obj.parent != undefined then exportCompHierarchy fs obj
		
		-- Export the children
		for c in obj.children do (
			exportObject fs c 
		)
		
		format "</entity>\n" to:fs
		--<camera znear="1" zfar="6" fov="45"/>
	),
	
	fn exportScene scene_name = (
		-- For each obj in the scene
		local out_filename = project_path + "scenes/" + scene_name + ".xml"
		local fs = createFile out_filename
		format "<entities>\n" to:fs
		local obj
		for obj in $* do (
			-- Only export the root objects of the scene
			-- The children are exported by the parents
			if obj.parent == undefined then
				exportObject fs obj
		)
		format "</entities>\n" to:fs
		if fs != listener then close fs
	)
)

gc()
se = TSceneExporter();
se.exportScene "scene_ui"
--se.exportStaticMesh "c:\\code\\unit_sphere.xml" #($)
--se.getMeshMaterialInfo  $
 

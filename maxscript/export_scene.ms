clearListener()

struct TSceneExporter (
	project_path = "c:\\code\\engine\\bin\\data\\",
	
	fn saveTextureSlot fs slot bitmap_texture = (
		local textures_path = "textures/"
		-- if not found, use some default textures
		if bitmap_texture == undefined then (
			--local aname = "missing_" + slot + ".dds"
			--format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		) else (
			local full_name = bitmap_texture.bitmap.filename
			local aname = filenameFromPath full_name
			format "<texture type=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		)
	),

	fn saveStdMaterial fs m = (
		if classof m != Standardmaterial then throw "Invalid material recv"
		format "<material tech=\"tech_textured_colored.tech\">\n" to:fs
		saveTextureSlot fs "diffuse" m.diffusemap
		saveTextureSlot fs "specular" m.specularmap
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
		for obj in objs do (
			local m = obj.mat
			if m == undefined then (
				format ("Object " + obj.name + " requires a material\n")
				continue
			) 
			local filename = project_path + "meshes/" + obj.name + ".mesh"
			local filename_xml = "meshes/" + obj.name + ".mesh"
			export_mesh obj (filename)
			local mat_name = m.name			-- bronze
			local mesh_idx = 0
			
			local mat_res_name = "materials/" + mat_name + ".material"
			
			fsmat_name = project_path + mat_res_name
			local fmaterial = createFile fsmat_name
			saveMaterial fmaterial obj.mat
			close fmaterial
			format "  <slot mesh=\"%\" material=\"%\" mesh_idx=\"%\"/>\n" filename_xml mat_res_name mesh_idx to:fsm
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
		local delta = matrix3 1   -- esto esta mal!!
		
		local extras = "parent=\"" + obj.parent.name + "\""
		exportTransform fs delta "hierarchy" extras
	),

		fn exportCompTransform fs obj = (
		local max_tmx = obj.transform		-- matrix3 
		exportTransform fs max_tmx "transform" ""
	),
	
	fn exportObject fs obj = (
		
		format "<entity>\n" to:fs
		exportCompName fs obj
		exportCompTransform fs obj
		if classof obj == Editable_mesh or classof obj == Editable_Poly then (
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
		local out_filename = project_path + "scenes\\" + scene_name + ".xml"
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
se.exportScene "scene01"


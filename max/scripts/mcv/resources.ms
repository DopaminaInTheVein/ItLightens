_list_objs = #()
struct TResources (
	project_path = data_dir ,
	mode = "all",
	folder = "",
	error = "",
	copy_textures = false,
	move_resources = false,
	exported_obj = undefined,
	only_mesh = false,
	is_ok = true,
	
	fn report_error msg = (
		error += (exported_obj.name + ": " + msg + "\n")
		is_ok = false
	),
	
	fn getResourceName obj nameResource = (
		local res = getUserProp obj nameResource
		case mode of (
			"all":
			(
				if res == undefined then (
					res = folder + obj.name
				)
			)
			"old":
			(
				--Nothing to do
			)
			"new":
			(
				if res != undefined then res = undefined
				else res = folder + obj.name
			)
			"forceNew":
			(
				res = folder + obj.name
			)
		)
		return res
	),
	
	fn getTexturePathAndSave bmap = (
		local full_name = bmap.filename
		--messagebox (full_name as string)
		local res = ""
		local pos = findString full_name textures_dir
		local local_path = ""
		if pos != undefined then (
			pos += textures_dir.count
			local_path = strReplace (substring full_name pos full_name.count) "\\" "/"
		) else (
			--local_path = folder + (filenameFromPath full_name)
			local_path = filenameFromPath full_name
		)
		res = "textures/" + local_path
		--bmap.filename = strReplace (textures_dir + local_path) "/" "\\"
		local dest = strReplace (textures_dir + local_path) "/" "\\"
		--messagebox full_name
		--messagebox dest
-- 		if move_resources then (
-- 			local path_root = strReplace (textures_dir + (filenameFromPath full_name)) "/" "\\"
-- 			local path_dest = strReplace (textures_dir + folder + (filenameFromPath full_name)) "/" "\\"
-- 			if (copyFileAndDirs path_root path_dest) then (
-- 				deletefile path_root
-- 			)
-- 		)
		
		if copy_textures then (
			if not (copyFileAndDirs full_name dest) then (
				report_error ("Can't copy texture " + full_name)
			)
		)
		--messagebox (copied as string)
		return res
	),
	
	fn saveTextureSlot fs slot bitmap_texture = (
		local res = true
		-- if not found, use some default textures
		if bitmap_texture == undefined then (
			--local  = "missing_" + slot + ".dds"
			--format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		) else (
			local full_name = bitmap_texture.bitmap.filename
			local ext = getSuffixAfter full_name "."
			if (ext != "dds" and ext != "DDS") then (
				report_error ("This texture is not DDS! [" + full_name + "]\n")
				res = false
				--throw ( "This texture is not DDS! [" + full_name + "]")
			)
			--messagebox (full_name as string)
			local aname = getTexturePathAndSave bitmap_texture.bitmap --filenameFromPath full_name
			--messagebox (aname as string)
			format "<texture slot=\"%\" name=\"%\"/>\n" slot aname to:fs
		)
		return res
	),

	fn checkMaterialName mat_name = (
		local res = true
		tieneEspacios = (findString mat_name " ")
		tieneAlmohadilla = (findString mat_name "#")
		if (tieneEspacios != undefined) do (
			report_error ("ERROR: -- Nombre de material incorrecto: " + mat_name)
			res = false
		)
		if (tieneAlmohadilla != undefined) do (
			report_error ("ERROR: -- Nombre de material incorrecto: " + mat_name)
			res = false
		)
		return res
	),
	
	fn saveMaterialTech m technique = (
		if (m != undefined) then (
			if classof m != Standardmaterial then (throw "Invalid material recv")
			local mat_file = undefined
			local path_mat = strReplace (materials_dir + folder + m.name + ".material") "/" "\\"
			try (
				mat_file =  createFile path_mat

				--Special tech by material name
				local tech = technique
				local tech_by_name = getSuffixAfter m.name "_tech_"
				if tech_by_name != undefined then tech = tech_by_name
				
				--Transparent techinque
				if tech == "deferred" and m.opacitymap != undefined then (
					tech = "transparent"
				)
				
				--Write material file
				format ("<material tech=\"" + tech + ".tech\">\n") to:mat_file
				saveTextureSlot mat_file "diffuse" m.diffusemap
				saveTextureSlot mat_file "specular" m.specularmap
				local self_ilum = m.selfillummap
				if self_ilum != undefined then (
					if (classof self_ilum) == Mix then (
						saveTextureSlot mat_file "selfilum" self_ilum.mask
						saveTextureSlot mat_file "selfilum_plus" self_ilum.map1
						saveTextureSlot mat_file "selfilum_minus" self_ilum.map2
					) else (
						saveTextureSlot mat_file "selfilum" self_ilum
					)
				)
				if (classof m.bumpmap) == Normal_Bump then (
					saveTextureSlot mat_file "normalmap" m.bumpmap.normal
				) else (
					saveTextureSlot mat_file "normalmap" m.bumpmap
				)
				format "</material>\n" to:mat_file
				close mat_file
				if move_resources then (
					local path_mat_root = strReplace (materials_dir + m.name + ".material") "/" "\\"
					if (path_mat_root != path_mat) then (
						deletefile path_mat_root
					)
				)
			) catch (
				report_error( "Can't create file [" + path_mat + "]")	
				if mat_file != undefined then (close mat_file)
			)
		) else (
			format "WARNING: - - Undefined material ignored!! - -\n"
		)
	),
	
	fn saveStdMaterial m = (
		--local tech = "textured"
		--if m.bumpMap != undefined then (
		--	tech += "_normals"
		--)
		saveMaterialTech m "deferred"
	),
	
	fn saveStdMaterialSkel m = (
		saveMaterialTech m "skin_textured"
	),

	fn saveMultiMaterial multi_mat = (
		if classof multi_mat != Multimaterial then throw "Invalid material recv. Need a Multimaterial"
		local m
		for m in multi_mat do (
			if m != undefined then (
				-- Create a file for each material?
				saveStdMaterial m			
			)
		)
	),
	
	fn saveMaterial mat = (
		if classof mat == Multimaterial then (
			saveMultiMaterial mat
		) else if classof mat == Standardmaterial then (
			saveStdMaterial mat
		) else 
			report_error ("Unsupported material " + ((classof mat) as string))
	),
	
	fn saveMaterialSkel mat = (
		if classof mat == Standardmaterial then (
			saveStdMaterialSkel mat
		) else 
			throw ("Unsupported material " + ((classof mat) as string))
	),
	
	fn getMeshMaterialInfo obj = (
		local mat = obj.mat
		local mes = obj.mesh
		
		local res
		if (mat != undefined) and (classof mat == MultiMaterial) then (
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
		
-- STATIC MESH
--------------------
	fn exportStaticMesh out_filename objs = (
		--Simplificamos a Array de solo un elemento
		-- (Si a�adimos multimaterial tampoco le veo utilidad a que pueda ser mayor)
		-- Obtenemos el unico elementoq ue esperamos en [obj]
		local obj
		if (classof objs == Array) then (
			if objs.count > 1 then (
				report_error("Function exportStaticMesh is not implemented for more than one object!")
				return false
			) else (
				obj = objs[1]
			)
		) else (
			obj = objs
		)		
		
		--Material de la mesh principal
		local m = obj.mat

		if m == undefined then (
			report_error ("Requires a material")
			return false
		)
		
		if classof m == Multimaterial then (
			report_error ("Multimaterial is not supported!")
			return false
		)
		
		-- Name of mesh and material
		local mat_name = m.name
		local meshName = getResourceName obj "mesh"
		if meshName == undefined then (
			report_error("Corrupt object! Has static mesh but not mesh!")
		)
		
		-- Check material is correct
		checkMaterialName mat_name
		if is_ok then (
			-- Save material(s) resource
			saveMaterial m
			if is_ok then(
				-- Save mesh resource
				-- #(   #(faces_with_mat1) undef undef #(faces_with_mat4) )
				local mat_infos = getMeshMaterialInfo obj
				local filenameXML = "meshes/" + meshName + mesh_ext
				try (
					tm = TMeshes()
					tm.export_mesh obj meshName mat_infos
				) catch (
					report_error("Error exporting mesh!") -- TODO: class TMeshes message / is_ok
				)
				if is_ok then (
					local fsm = createFile out_filename
					format "<static_mesh>\n" to:fsm
					-- A�adimos mesh slot 
					local mat_res_name = "materials/" + folder + mat_name + ".material"
					format "  <slot mesh=\"%\" material=\"%\"/>\n" filenameXML mat_res_name to:fsm
					
					format "</static_mesh>\n" to:fsm
					close fsm
				)
			)
		)
	),
	
-- SKEL - Static Mesh - 
----------------------------------------------------------------------
	fn exportStaticMeshSkel nameSkel obj = (
		local staticMeshPath = (project_path + "skeletons/" + nameSkel + "/" + nameSkel + staticMesh_ext)
		local fsm = createFile staticMeshPath
		format "<static_mesh>\n" to:fsm
		
		try (
			--Material de la mesh
			local m = obj.mat
		
			if classOf m == MultiMaterial then (
				throw "No se puede exportar animacion con multimaterial!"
			)
						
			-- Name of mesh and material
			local mat_name = m.name
	
			-- Check material is correct
			checkMaterialName mat_name

			-- A�adimos mesh slot
			local meshPath =("skeletons/" + nameSkel + "/" + nameSkel + mesh_ext)
			local mat_res_name = "materials/" + mat_name + ".material"
			format "  <slot mesh=\"%\" material=\"%\" />\n" meshPath mat_res_name to:fsm
			
			-- Save material resource
			saveMaterialSkel m
			
		) catch (
			close fsm
			throw()
		)
		format "</static_mesh>\n" to:fsm
		close fsm
	),
----------------------------------------------------------------------	


	fn exportCompStaticMesh obj = (
		rStMesh = getResourceName obj "staticMesh"
		if rStMesh != undefined do (
			local static_mesh_name = "static_meshes\\" + rStMesh + ".static_mesh"
			local out = project_path + static_mesh_name
			out = strReplace out "/" "\\"
			exportStaticMesh out #(obj)
			setUserProp obj "staticMesh" rStMesh
			--messagebox ("Obtengo mesh de " + obj.name)
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
	fn hasMeshProtected = (
		local obj = exported_obj
		if (getUserProp obj "mesh_protected") == "yes" then (return true)
		if classof obj == Delegate then (return true)
		if obj.isHidden then (return true)
		if (getUserProp obj "typeHelper") != undefined then (return true)
		if (getUserProp obj "typeObject") == "Trigger" then (return true)
		
		return false
	),
	fn exportOnlyMesh = (
		local mat_infos = getMeshMaterialInfo exported_obj -- needed for null material as well
		local meshName = getResourceName exported_obj "mesh"
		if meshName == undefined then (
			report_error("Corrupt object! Has static mesh but not mesh!")
		)
		local filenameXML = "meshes/" + meshName + mesh_ext
		--try (
			local tm = TMeshes()
			tm.use_material = false
			tm.export_mesh exported_obj meshName mat_infos
		--) catch (
		--	report_error("Error exporting mesh!") -- TODO: class TMeshes message / is_ok
		--)
	),
		
	fn exportObject obj = (
		exported_obj = obj
		--if ( (not (classof obj == Delegate)) and  (not obj.isHidden) and (getUserProp obj "typeHelper") == undefined) and ((getUserProp obj "typeObject") != "Trigger") then (
		if ( not hasMeshProtected() ) then (	
			--exportCompName obj
			local canExport = true
			if classof obj != Editable_mesh and classof obj != Editable_Poly then (
				if (ConvertToMesh obj) == undefined then ( canExport = false)
			)
			if canExport then (
				append _list_objs obj.name
				--messageBox ("Only mesh:" + (only_mesh as string))
				if only_mesh then (
					exportOnlyMesh()
				) else (
					exportCompStaticMesh obj
				)
			) else (					
				-- Export the children (may be a group, layer, etc)
				for c in obj.children do (
					exportObject c 
				)
			)
		)
	),
	
	fn exportSelected = (
		_list_objs = #()
		folder += "/"
		if $ == undefined then return "(nothing to export)\n"
		local all_objs = $
		for obj in all_objs do (
			is_ok = true
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

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

	fn saveStdMaterial m = (
		if (m != undefined) then (
			if classof m != Standardmaterial then (throw "Invalid material recv")
			local mat_file = createFile (materials_dir + m.name + ".material")
			format "<material tech=\"textured.tech\">\n" to:mat_file
			saveTextureSlot mat_file "diffuse" m.diffusemap
			saveTextureSlot mat_file "specular" m.specularmap
			format "</material>\n" to:mat_file
			close mat_file
		) else (
			format "WARNING: - - Undefined material ignored!! - -\n"
		)
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
			throw ("Unsupported material " + ((classof mat) as string))
	),
	
	fn getSubMeshesByMaterial obj = (
		-- Obtenemos el material
		local mat = obj.mat
		if mat == undefined then (
			throw ("Object " + obj.name + " requires a material\n")
		) 
		
		local meshPrefix = getResourceName obj "mesh"
		-- Si llegamos hasta aqui y no sabemos que mesh poner, es que:
		-- 1)  reexportamos una staticMesh y no conocemos su mesh
		-- 2) exportamos una static mesh nueva pero ya tenía mesh
		if meshPrefix == undefined then (
			throw "-- ERROR -- [" + obj.name + "] Objeto corrupto! ¡Tiene mesh pero no staticMesh!\n"
		)
		
		local res = #()
		
		if classof mat != MultiMaterial then (
			newObj = copy obj
			newObj.name = meshPrefix
			setUserProp newObj "mesh" meshPrefix
			res = #(newObj)
			print res
		) else (
			local polyObj = copy obj --Objeto temporal solo en esta funcion
			try (
				
				for m=1 to mat.count do (
					local subMesh = undefined
					if mat[m] != undefined then(
						polyObj.selectByMaterial m
						faces = getFaceSelection polyObj
						
						if faces.count > 0 then (
							local the_name = meshPrefix + "__" + (mat[m].name)
							polyOp.detachFaces polyObj faces asnode:true keepOriginal:on name:the_name
							subMesh = (getnodebyname the_name)
							setUserProp subMesh "mesh" the_name
						)
					)
					append res subMesh
				)
			) catch (
				delete polyObj
				throw()
			)
			delete polyObj
		)
		
		return res
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
		
		--Simplificamos a Array de solo un elemento
		-- (Si añadimos multimaterial tampoco le veo utilidad a que pueda ser mayor)
		-- Obtenemos el unico elementoq ue esperamos en [obj]
		local obj
		if (classof objs == Array) then (
			if objs.count > 1 then (
				throw "Function exportStaticMesh is not implemented for more than one object!"
			) else (
				obj = objs[1]
			)
		) else (
			obj = objs
		)
		
		-- Creamos submeshes temporales
		local subMeshes = getSubMeshesByMaterial obj
		--Material de la mesh principal
		local m = obj.mat
		
		try (
			-- Tratamos de crear la static mesh con sus meshes
			local mesh_idx = 0
			local mat_idx = 0
			local used_mats = #()
			
			for obj in subMeshes do (
				mat_idx = mat_idx + 1
				if obj != undefined then (
					--Current material
					local mat_used = m
					if classOf m == MultiMaterial then (
						mat_used = m[mat_idx]
					)
					
					-- Name of mesh and material
					local mat_name = mat_used.name
					local meshName = obj.name
					
					-- Check material is correct
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
					
					-- Save mesh resource
					-- #(   #(faces_with_mat1) undef undef #(faces_with_mat4) )
					local mat_infos = getMeshMaterialInfo obj
					local filenameXML = "meshes/" + meshName + mesh_ext
					export_mesh obj meshName mat_infos
					
					-- Añadimos mesh slot 
					local mat_res_name = "materials/" + mat_name + ".material"
					format "  <slot mesh=\"%\" material=\"%\" mesh_idx=\"%\"/>\n" filenameXML mat_res_name mesh_idx to:fsm
					mesh_idx = mesh_idx + 1
				)
			)
			-- Save material(s) resource
			saveMaterial m
		) catch (
			-- Si falla debemos de borrar los objetos temporales de todas formas!
			for sm in subMeshes do (
				if sm != undefined then (delete sm)
			)
			close fsm
			throw()
		)
		for sm in subMeshes do (
			if sm != undefined then (delete sm)
		)
		format "</static_mesh>\n" to:fsm
		close fsm
	),

	fn exportCompStaticMesh obj = (
		rStMesh = getResourceName obj "staticMesh"
		if rStMesh != undefined do (
			local static_mesh_name = "static_meshes\\" + rStMesh + ".static_mesh"
			--format "  <render_static_mesh name=\"%\"/>\n" static_mesh_name to:fs
			exportStaticMesh (project_path + static_mesh_name) #(obj)
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

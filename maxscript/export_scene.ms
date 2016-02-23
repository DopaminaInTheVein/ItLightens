clearListener()

struct TSceneExporter (
	project_path,
	
	fn saveTextureSlot fs slot bitmap_texture = (
		local textures_path = "textures/"
		-- if not found, use some default textures
		if bitmap_texture == undefined then (
			local aname = "missing_" + slot + ".dds"
			format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		) else (
			local full_name = bitmap_texture.bitmap.filename
			local aname = filenameFromPath full_name
			format "<texture slot=\"%\" name=\"%%\"/>\n" slot textures_path aname to:fs 
		)
	),

	fn saveStdMaterial fs m = (
		if classof m != Standardmaterial then throw "Invalid material recv"
		format "<material>\n" to:fs
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
	
	fn exportObject obj = (
		if classof obj != Editable_mesh then return false
		-- Export the material
		saveMaterial listener obj.mat
		-- Export the mesh
		export_mesh obj
	),
	
	fn exportScene = (
		-- For each obj in the scene
		local obj
		for obj in $* do (
			exportObject obj
		)
	)
)

se = TSceneExporter();
se.exportScene()


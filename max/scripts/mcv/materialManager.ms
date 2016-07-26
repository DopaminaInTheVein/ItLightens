struct MatManager (
	function test = (
		resNotExist = ""
		resNotDiffuse = ""
		for aMtlType in material.classes do 
		(
			for aMtl in (getClassInstances aMtlType processAllAnimatables:true) do
			(
				-- Does this material exist in the scene or not?
				if (findItem sceneMaterials aMtl) == 0 then (
					resNotExist += ((aMtl as string) + "\n")
				)
				else (
					if not (hasProperty aMtl "diffuse") then (
						if (classof aMtl) != MultiMaterial then (
							resNotDiffuse += ((aMtl as string) + "\n")
						)
					)
				)
			)
		)
		clearListener()
		print "These materials dont exist in the scene:\n" + resNotExist + "This material has not diffuse:\n" + resNotDiffuse
		messagebox("These materials dont exist in the scene:\n-----\n" + resNotExist)
		messageBox("This material has not diffuse:\n-----\n" + resNotDiffuse)
	),
	
	function searchTexture texture = (
		res = #()
		for aMtlType in material.classes do (
			for aMtl in (getClassInstances aMtlType processAllAnimatables:true) do
			(
				if (hasProperty aMtl "diffuseMap") then (
					local prop = aMtl.diffuseMap
					if (classof prop) == BitMapTexture then (
						local txtName = FilterString prop.filename "\\ /"
						txtName = txtName[txtName.count]
						--print ("Diffuse: " + aMtl.name + "-" + (aMtl.diffuseMap as string) + " " + txtName)
						local index = findString txtName texture
						if (index != undefined) then (
							append res ("Diffuse: " + aMtl.name + "/" + (prop as string) + "/" + txtName)
						)
					)
				)
				
				if (hasProperty aMtl "specularMap") then (
					local prop = aMtl.specularMap
					if (classof prop) == BitMapTexture then (
						local txtName = FilterString prop.filename "\\ /"
						txtName = txtName[txtName.count]
						--print ("Diffuse: " + aMtl.name + "-" + (aMtl.diffuseMap as string) + " " + txtName)
						local index = findString txtName texture
						if (index != undefined) then (
							append res ("Specular: " + aMtl.name + "/" + (prop as string) + "/" + txtName)
						)
					)
				)
				
				if (hasProperty aMtl "bumpmap") then (
					local prop = aMtl.bumpMap
					if (classof prop) == Normal_Bump then (
						local txtName = FilterString prop.normal.filename "\\ /"
						txtName = txtName[txtName.count]
						--print ("Diffuse: " + aMtl.name + "-" + (aMtl.diffuseMap as string) + " " + txtName)
						local index = findString txtName texture
						if (index != undefined) then (
							append res ("Normal: " + aMtl.name + "/" + (prop as string) + "/" + txtName)
						)
						--print ("Ojo" + ( (classof prop) as string))
					) else (
					 --print ("Ojo" + ( (classof prop) as string))
					)
				)
			)
		)
		clearListener()
		for  r in res do (
			print r
		)
		return res
	)
)

mm = MatManager()
--mm.searchTexture "pared"
mm.test()
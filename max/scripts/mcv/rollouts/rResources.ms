try(DestroyDialog RL_Resources)catch()
rollout RL_Resources "IT Lightens! - Resources" width:296 height:230
(
	local selectedObj = undefined

	label 'lblTitle' "MESHES ( Export and Set )" pos:[79,12] width:137 height:16 align:#left
	GroupBox 'grpSelObj' "Select Object" pos:[16,39] width:232 height:48 align:#left
	label 'lblSelObj' "Selected Object:" pos:[32,59] width:88 height:16 align:#left
	pickbutton 'pckObj' "-- None --" pos:[128,59] width:112 height:16 align:#left
	button 'btnSetMesh' "Set Mesh" pos:[78,169] width:135 height:16 align:#left
	GroupBox 'grpSetMesh' "Set Mesh" pos:[15,101] width:259 height:119 align:#left

	label 'lblMesh' "Mesh:" pos:[63,123] width:34 height:16 align:#left
	label 'lblStaticMesh' "Static Mesh:" pos:[32,142] width:66 height:16 align:#left
	edittext 'txtMesh' "" pos:[101,121] width:157 height:16 align:#left
	edittext 'txtStaticMesh' "" pos:[100,142] width:157 height:16 align:#left
	button 'btnHelp' "Help" pos:[79,192] width:135 height:16 align:#left
	
	
	
	on pckObj picked obj do
	(
		pckObj.text = obj.name
		selectedObj = obj
	)
	on btnSetMesh pressed do
	(
		local err = ""
		local stMesh = trimRight (trimLeft txtStaticMesh.text)
		local stMeshPath = staticMeshes_dir + stMesh + staticMesh_ext
		local _mesh = trimRight (trimLeft txtMesh.text)
		local meshPath = meshes_dir + _mesh + mesh_ext
		local newMeshCreated = false
		try (

			if selectedObj == undefined do (
				throw "Debes seleccionar un objeto!!" 
			)					
			
			if stMesh == "" do (
				throw "Debes indicar la mesh!" 
			)		

			if _mesh == "" do (
				throw "Debes indicar la staticMesh!" 
			)
			
			-- Static Mesh existe
			if (doesFileExist stMeshPath) then (
				local resp = (queryBox "Esta staticMesh ya existe y se sobreescribirá. ¿Continuar?")
				if (not resp) do (
					return #abort
				)			
			)
					
			-- Mesh existe
			if (doesFileExist meshPath) then (
				local resp = (queryBox "Esta mesh ya existe y se sobreescribirá. ¿Continuar?")
				if (not resp) do (
					return #abort
				)
			)
			
			--Exportar Mesh
			try (
				export_mesh selectedObj meshPath
				newMeshCreated = true
			) catch (
				err = "Error exportando Mesh!"
				throw()
			)
			
			-- Exportar Static Mesh
			try (
				tr = TResources();
				tr.exportStaticMesh stMeshPath #(selectedObj)
			) catch (
				err = "Error exportando static_mesh!"
				throw()
			)
			
			-- Guardamos staticMesh en propiedades del objeto
			setUserProp selectedObj "staticMesh" stMesh
			
		) catch (
			err += "\n***\n"+getCurrentException()+"\n***\n"
			try (
				if newMeshCreated do (
					messageBox "Borro Mesh"
					deleteFile meshPath
				)
			) catch ( )
			messageBox err
		)
	)
	on btnHelp pressed do
	(
		local help  = "Ayuda para artistas\n\n"
		help += "Mesh: nombre del fichero .mesh (guarda la geometría del objeto)\n"
		help += "Static Mesh: nombre del fichero .static_mesh (referencia a .mesh + info adicional de render)\n"
		help += "\nUso:\n"

		help += "\nRecomedaciones:\n"
		help += "1) Utilizar mismo nombre para mesh que para static_mesh (a excepción de reutilizar misma geometría con distintas texturas, etc.)\n" 
		help += "2) Exportar nuevas mallas sólo desde los .max de los objetos sueltos\n"
		help += "3) Preguntad lo que sea que no entendáis antes de liarla :P"
		messageBox help
	)
)
CreateDialog RL_Meshes

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------------------
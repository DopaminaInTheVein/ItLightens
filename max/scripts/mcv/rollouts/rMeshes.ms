try(DestroyDialog RL_Meshes)catch()
rollout RL_Meshes "IT Lightens!" width:296 height:554
(
	local selectedObj = undefined

	label 'lblTitle' "MESHES ( Export and Set )" pos:[79,12] width:137 height:16 align:#left
	GroupBox 'grpSelObj' "Select Object" pos:[16,39] width:232 height:48 align:#left
	label 'lblSelObj' "Selected Object:" pos:[32,59] width:88 height:16 align:#left
	pickbutton 'pckObj' "-- None --" pos:[128,59] width:112 height:16 align:#left
	button 'btnSetMesh' "Export Mesh" pos:[78,169] width:135 height:16 align:#left
	GroupBox 'grpSetMesh' "Export Mesh" pos:[15,101] width:259 height:119 align:#left

	label 'lblMesh' "Mesh:" pos:[63,123] width:34 height:16 align:#left
	label 'lblStaticMesh' "Static Mesh:" pos:[32,142] width:66 height:16 align:#left
	edittext 'txtMesh' "" pos:[101,121] width:157 height:16 align:#left
	edittext 'txtStaticMesh' "" pos:[100,142] width:157 height:16 align:#left
	button 'btnHelp' "Help" pos:[79,192] width:135 height:16 align:#left
	
	
	
	GroupBox 'grpCopy' "Copy" pos:[16,229] width:259 height:89 align:#left
	button 'btnCopy' "Set this mesh to selected objects" pos:[30,248] width:230 height:16 align:#left
	label 'lblCopy' "Para asignar la misma mesh a otros objetos (sólo actualiza la info para exportar nivel, no cambiará la apariencia en max)" pos:[27,269] width:237 height:46 align:#left
	GroupBox 'grpAll' "Export All Selected" pos:[15,329] width:259 height:217 align:#left
	button 'btnExportSelectedNew' "Only New Meshes" pos:[29,396] width:114 height:16 align:#left
	button 'btnRexport' "Only Re-export" pos:[150,396] width:112 height:16 align:#left
	button 'btnExportSelected' "Export New and Reexport Old" pos:[29,374] width:234 height:16 align:#left
	button 'btnForceNew' "Force all as new (caution!)" pos:[75,520] width:145 height:16 align:#left
	label 'lbl11' "BASIC" pos:[133,351] width:29 height:17 align:#left
	label 'lbl12' "ADVANCED" pos:[119,498] width:58 height:17 align:#left
	label 'lbl20' "Las nuevas meshes (objetos sin mesh asignada), se guardarán con el mismo nombre que el objeto. Las reexportadas con el mismo nombre con el que ya se exportaron" pos:[26,420] width:237 height:61 align:#left
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
			
			-- Exportar Static Mesh
			try (
				setUserProp selectedObj "mesh" _mesh
				setUserProp selectedObj "staticMesh" stMesh
				
				tr = TResources();
				tr.exportStaticMesh stMeshPath #(selectedObj)
			) catch (
				err = "Error exportando static_mesh!"
				throw()
			)
			
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
	
		help += "\n¿Ésto qué #!@?=%& hace? \n"
		help += "Al pulsar Export Mesh, se exportará los ficheros de las meshes, materiales, etc. del objeto en subcarpetas del directorio data. "
		help += "Además se guardará en las propiedades de este objeto (Object properties-> User defined), el vínculo a la mesh exportada. "
		help += "(Esta información la utiliza el exportador del nivel, ¡procura que todos los objetos la tengan!)"
		
		help += "\nRecomedaciones:\n"
		help += "1) Utilizar mismo nombre para mesh que para static_mesh (a excepción de reutilizar misma geometría con distintas texturas, etc.)\n" 
		help += "2) Exportar nuevas mallas sólo desde los .max de los objetos sueltos\n"
		help += "3) Preguntad lo que sea que no entendáis antes de liarla :P"
		messageBox help
	)
	on btnCopy pressed do
	(
		rMesh = getUserProp selectedObj "mesh"
		rStMesh = getUserProp selectedObj "staticMesh"
		if rMesh == undefined then return #abort
		if rStMesh == undefined then return #abort
		if $ == undefined then return #abort
			
		if (classOf $ == ObjectSet) then objs = $ as array
		else objs = #($)
		for o in objs do(
			setUserProp o "mesh" rMesh
			setUserProp o "staticMesh" rStMesh
		)
	)
	on btnExportSelectedNew pressed do
	(
		tr = TResources()
		tr.exportSelectedNew()
	)
	on btnRexport pressed do
	(
		tr = TResources()
		tr.rexport()	
	)
	on btnExportSelected pressed do
	(
		tr = TResources()
		tr.exportSelected()		
	)
	on btnForceNew pressed  do
	(
		tr = TResources()
		tr.exportSelectedForceNew()		
	)
)
CreateDialog RL_Meshes

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------
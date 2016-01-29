fn areEqual v1 v2 = (
	local c = v1.count
	local idx = 1 
	while idx <= c do (
		if v1[idx] != v2[idx] then return false
		idx = idx + 1
	)
	return true
)

fn registerVertex all_vtxs new_vtx = (
	
	local i0 = 1
	for v in all_vtxs do (
		if areEqual v new_vtx then (
			return i0
		)
		i0 = i0 + 1
	)
	-- 
	append all_vtxs new_vtx
	return all_vtxs.count 
)

fn saveBinaryMesh obj vtxs idxs = (
	
	local data_path = "c:\\code\\engine\\bin\\data\\meshes\\"
	local full_path = data_path + obj.name + ".mesh"
	
	-- Open file
	local fs = fopen full_path "wb"
	if fs == undefined then throw ("Can't create output file " + full_path )
	
	-- Save Header
	local num_bytes_of_header = 9 * 4
	local version = 1
	local primitive_type = 2000
	local vertex_type = 1000			-- Pos UV
	local bytes_per_float = 4		-- Assuming all vtxs are float
	local bytes_per_vertex = vtxs[1].count * bytes_per_float
	local bytes_per_index = if idxs.count > 65535 then 4 else 2
	local num_ranges = 1
	
	WriteLong fs 0x44221100  -- Mesh magic
	WriteLong fs num_bytes_of_header
	
	WriteLong fs version
	WriteLong fs vtxs.count
	WriteLong fs idxs.count
	WriteLong fs primitive_type
	
	WriteLong fs vertex_type
	WriteLong fs bytes_per_vertex
	WriteLong fs bytes_per_index			-- 2 or 4
	WriteLong fs num_ranges
	WriteLong fs 0x44222200  -- terminator Mesh header
	
	-- Save vtxs
	WriteLong fs 0x44221101  -- vtxs magic
	WriteLong fs ( bytes_per_vertex * vtxs.count )
	for v in vtxs do (
		for c in v do (			-- Each member of the vtx
			WriteFloat fs c		-- Write a float
		)
	)

	-- Save idxs
	WriteLong fs 0x44221102  -- idx magic
	WriteLong fs ( bytes_per_index * idxs.count )
	for i in idxs do (
		if bytes_per_index == 2 then (
			WriteShort fs (i-1)			-- Base 0
		) else (
			WriteLong fs (i-1)
		)
	)
	
	WriteLong fs 0x44221144  -- magic mesh end
	WriteLong fs 0
	
	FClose fs
)

fn export_mesh given_obj = (
	
	-- Is the object valid?...
	local obj = given_obj
	if classof given_obj != Editable_Mesh then (
		obj = copy given_obj
		ConvertToMesh obj
	)
	
	-- Basic info
	local vtx_type = 1001
	local channelA = 1
	
	local world_max_to_local_mcv = (inverse obj.transform) * ( rotateXMatrix -90)
	
	-- 
	local all_vtxs = #()
	local all_idxs = #()
	
	if not meshop.getMapSupport obj channelA then (
		throw ("Object " + given_obj.name + " does not have tex coords for channel " + channelA )
	)
	
	local num_faces = getNumFaces obj
	for face_id = 1 to num_faces do (
		local pos_idx = getFace obj face_id
		local uv_A = meshop.getMapFace obj channelA face_id
		
		--format "Face % %\n" face_id pos_idx
		for vtx_id in #(1,3,2) do (
			local world_pos = getVert obj pos_idx[vtx_id]
			local pos =  world_pos * world_max_to_local_mcv
			
			local uvA = meshop.getMapVert obj channelA uv_A[vtx_id]
			
			local vtx = #(pos.x, pos.y, pos.z, uvA.x, uvA.y)
			local idx = registerVertex all_vtxs vtx
			append all_idxs idx
			--format "  % % => %\n" pos uvA idx
		)
	)
	
	-- If I did a copy, it's time to delete it
	if obj != given_obj then 
		delete obj

	---------------------------------------------
	-- Save everything
	---------------------------------------------
	saveBinaryMesh given_obj all_vtxs all_idxs
	
)

export_mesh $

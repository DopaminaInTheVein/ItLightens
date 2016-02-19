clearListener()

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
)

fn saveStdMaterial fs m = (
	if classof m != Standardmaterial then throw "Invalid material recv"
	format "<material>\n" to:fs
	saveTextureSlot fs "diffuse" m.diffusemap
	saveTextureSlot fs "specular" m.specularmap
	format "</material>\n" to:fs
)

saveStdMaterial listener $.mat[1]


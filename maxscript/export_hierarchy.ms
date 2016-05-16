
fn atomTag tag atts = (
		local num_props = ( atts.count / 2 )
		format "<%" tag 
		for i = 1 to num_props do (
			format " %=\"%\"" ( atts[i*2-1] as string ) atts[i*2] 
		)
		format "/>\n" 
	)
	
fn exportTransform obj = (
	if obj.parent == undefined then (
		atomTag "transform" #("pos", obj.position)
	) else (
		local my_transform = obj.transform
		local parent_transform = obj.parent.transform
		local delta = my_transform * (inverse parent_transform)
		
		-- Delta is still in 3DStudio coords!!
		
		
		atomTag "parent" #("name", obj.parent.name)
		atomTag "transform" #("pos", delta.position)
	)
)


--exportTransform $
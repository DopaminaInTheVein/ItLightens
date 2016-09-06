try(DestroyDialog RL_Instancier)catch()
rollout RL_Instancier "IT Lightens!" width:280 height:70
(
	label lblTitle "INSTANCIER" pos:[7,5] width:102 height:18
	button btnSrc "<Select and click>" pos:[52,23] width:213 height:15
	label lbl3 "Source" pos:[7,24] width:38 height:12

	button btnConvert "Convert selected to instances" pos:[7,43] width:258 height:15
	
--
	local selected
--	

on btnSrc pressed do
(
	if $ == undefined then (messagebox "Select an object!")
	if classof $ == ObjectSet then (messagebox "Select only once!")
	selected = $
	btnSrc.text = ("<" + $.name + ">")
)
on btnConvert pressed do
(
	if $ == undefined then (messagebox "Select an object to convert!")
	if classof $ == ObjectSet then (
		for obj in $ do(
			instanceReplace obj selected
		)
	) else (
		instanceReplace $ selected
	)
)

)
CreateDialog RL_Instancier

---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------------------
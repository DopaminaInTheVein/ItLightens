-- macros for "Detach_Elements.ms" which need to be placed in "scripts" folder
macroScript macro_Detach_Elements
	category:"Tools" 
	internalCategory:"Tools" 
	tooltip:"Detach Elements" 
	buttontext:"Detach Elements" 
(
	local sFile = (GetDir #scripts) + "/Detach_Elements.ms"
	if (doesFileExist sFile) then (fileIn sFile)
	else (messageBox "Unable to locate the script.")
)

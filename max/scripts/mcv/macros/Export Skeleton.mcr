macroScript ExportSkeleton
	category:"_ItLightens!"
	toolTip:"Export Skeleton"
(
	gc();
	se = TSkeletonsExporter();
	se.exportSkels();
	se.exportAnim();
)
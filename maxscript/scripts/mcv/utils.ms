fn matrixMax2MCV mtx = (
	local tm = rotateXMatrix -90
	local tm_inv = inverse tm
	return tm_inv * mtx * tm
)
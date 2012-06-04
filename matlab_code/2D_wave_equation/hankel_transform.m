function transformed = hankel_transform (v, fun, cut_off, k_vec, tol = 1.0e-6)
	% Generate the transformed function
	transformed = [];
	for k = k_vec;
		transformed = [transformed, quad(@(r) fun(r)*besselj(v, k*r)*r, 0, cut_off, tol)];
	endfor
endfunction
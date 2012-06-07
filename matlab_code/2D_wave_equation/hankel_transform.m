function transformed = hankel_transform (v, fun, cut_off, k_vec, tol = 1.0e-6)
 % v = order of the Hankel transform
	% fun = function to be transformed
	% cut_off = range to carry out the integral on (starting from zero)
	% k_vec = the vector of k_values for which the Hankel transform should be calculated
	% tol = the error tolerance for the integration
	% Generate the transformed function
	transformed = [];
	for k = k_vec;
		transformed = [transformed, quad(@(r) fun(r)*besselj(v, k*r)*r, 0, cut_off, tol)];
	endfor
endfunction
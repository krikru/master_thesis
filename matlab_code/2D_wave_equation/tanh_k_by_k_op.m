function conv = tanh_k_by_k_op()
	tol = 1.0e-6; % Default (1.0e-6)	
	fun = @(r) 1./(8*pi^3*r.*(1+r)) .* e.^(-r.^2/4);
	cut_off = 20;
	res = 0.1;
	r_vec = [0 : res : 25];
	ref_fun = @(k) (!k) + (!!k).*tanh(k)./k/(2*pi)^2;
	F = (2*pi) * hankel_transform(0, fun, cut_off, r_vec, tol);
	F_ref = ref_fun(r_vec);
	
	% Compare functions
	h = plot(r_vec, F_ref, r_vec, F);
	print("C:/Work/master_thesis/matlab_code/2D_wave_equation/compare.tex", "-dtex");
	print("C:/Work/master_thesis/matlab_code/2D_wave_equation/compare_tikz.tex", "-dtikz");
	%h = plot(r_vec, F_ref, r_vec, F);
	%print("C:/Work/master_thesis/matlab_code/2D_wave_equation/compare_logarithmic.tex", "-dtex");
	%print("C:/Work/master_thesis/matlab_code/2D_wave_equation/compare_logarithmic_tikz.tex", "-dtikz");
	% Ratio
	h = plot(r_vec, F ./ F_ref);
	print("C:/Work/master_thesis/matlab_code/2D_wave_equation/ratio.tex", "-dtex");
	print("C:/Work/master_thesis/matlab_code/2D_wave_equation/ratio_tikz.tex", "-dtikz");
endfunction
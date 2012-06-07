function conv = tanh_k_by_k_op()
	tol = 1.0e-6; % Default (1.0e-6)	
	fun = @(r) 1./(8*pi^3*r.*(1+r)) .* e.^(-r.^2/4);
	%fun = @(r) r<1;
	cut_off = 20;
	res = 0.01;
	r_vec = [0 : res : 20];
	ref_fun = @(k) (!k) + (!!k).*tanh(k)./k/(2*pi)^2;
	F = (2*pi) * hankel_transform(0, fun, cut_off, r_vec, tol);
	F_ref = ref_fun(r_vec);
	plot(r_vec, F, r_vec, F_ref);
	%plot(r_vec, F_ref - F);
	%plot(r_vec, F_ref ./ F);
endfunction
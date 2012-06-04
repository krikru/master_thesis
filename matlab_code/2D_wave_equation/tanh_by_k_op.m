function conv = tanh_by_k_op()
	tol = 1.0e-6; % Default (1.0e-6)
	if false
		a = 1;
		fun = @(k) e^(-(a*k)^2/2);
		cut_off = 10;
		r_vec = [0 : .1 : 10];
		ref_fun = @(r) 1/(2*pi) * e.^(-(r/a).^2/2)/a^2;
	elseif false
		fun = @(k) (!k) + (!!k)*tanh(k)/k;
		cut_off = 350;
		res = 0.01;
		r_vec = [res : res : 5];
		ref_fun = @(r) 1./((2*pi*r).*(1+r)) .* e.^(-r.^2/4);
	else
		fun = @(r) 1./((2*pi*r).*(1+r)) .* e.^(-r.^2/4);
		%fun = @(r) r<1;
		cut_off = 20;
		res = 0.01;
		r_vec = [0 : res : 20];
		ref_fun = @(k) (!k) + (!!k).*tanh(k)./k/(2*pi)^2;
	end
	F = 1/(2*pi) * hankel_transform(0, fun, cut_off, r_vec, tol);
	F_ref = ref_fun(r_vec);
	plot(r_vec, F, r_vec, F_ref);
	%plot(r_vec, F_ref - F);
	%plot(r_vec, F_ref ./ F);
endfunction
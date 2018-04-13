# A muffler
algebraic3d
solid chamber0 = cylinder(0, 0, 0; 0, 0.05, 0; 0.05)
	and plane(0, 0, 0; 0, -1, 0)
	and plane(0, 0.05, 0; 0, 1, 0);
solid chamber1 = cylinder(0, 0.05, 0; 0, 0.15, 0; 0.1)
	and plane(0, 0.05, 0; 0, -1, 0)
	and plane(0, 0.15, 0; 0, 1, 0);
solid muffer = chamber0 or chamber1  -maxh=0.013;
tlo muffer;



text_h = 0.4;
tag_h = 0.8;
size = 4;
hole_d = 3;
min_hole_border = 3*0.42;
hole_n = 64;

module doit(number, hole_pox_x, hole_pox_y){
	translate([0, (number - 8.5)*size, 0]) difference(){
		union(){
			hull(){
				linear_extrude(tag_h) text(str(number), size);
				translate([hole_pox_x, hole_pox_y, 0]) cylinder(h=tag_h, d=hole_d + 2*min_hole_border, $fn=hole_n);
			}
			linear_extrude(tag_h + text_h) text(str(number), size);
		}
		translate([hole_pox_x, hole_pox_y, 0]) cylinder(h = size, d = hole_d, $fn=hole_n, center=true);
	}
}


off = [[0.25, 0.45],
       [0.25, 0.52],
       [0.2, 0.48],
       [0.41, 0.15], //4
       [0.18, 0.32], //5
       [0.57, 0.65], //6
       [0.23, 0.62], //7
       [0.13, 0.5055], //8
       [0.18, 0.28],
       [0.25, 0.45],
       [0.25, 0.45],
       [0.25, 0.45],
       [0.25, 0.45],
       [0.25, 0.45],
       [0.25, 0.45]];

for(number = [1:15]){
	doit(number, off[number - 1][0]*size, off[number - 1][1]*size);
}
handle_d = 25;
handle_h = 250;
handle_cut = handle_d*0.15;

hole_dist = 215;
hole_d = 4 + 0.1;
plastic_hole_d = 6;
plastic_hole_l = 10;
attach_up = 3;
attach_left = 3;
attach_down = 4;
attach_right = 4;
attach_chamfer_size = 30;
attach_l = 23 + handle_d/2;
nut_start_pos = 10;
nut_h = 5;
nut_w = 7;
nut_max_r = 4.5;

text_depth = 0.55;
text_1 = "No Metal";
text_1_pos = handle_h - 35;
text_1_spacing = 20;
text_1_size = 17.8;
text_2 = "A4";
text_2_pos = 16;
text_2_size = 11;
text_3 = "µ~";
text_3_pos = 33;
text_3_size = 11;
line_size = 7;
line_pos = 50;

brand_size = 8;
brand_pos_x = 18;
brand_pos_z = -1.5;

inf = 1000;
$fn = 200;
sacrificial_layer_height = 0.2;

module brand(){
	translate([0, -brand_size/2, 0]) linear_extrude(0.2) text("Ar", size=brand_size, halign="left");
	translate([brand_size/5, brand_size*2/3 - brand_size/2, 0]) linear_extrude(0.2) text("42", size=brand_size*2/3, halign="right");
}

module attach(dir){
	difference(){
		union(){
			translate([0, -attach_left - plastic_hole_d/2, -attach_down - plastic_hole_d/2]) cube([attach_l, attach_left + plastic_hole_d + attach_right, attach_down + plastic_hole_d + attach_up]);
			rotate([0, 90, 0]) cylinder(d=plastic_hole_d, h=attach_l + plastic_hole_l);
			translate([0, -attach_left - plastic_hole_d/2, -attach_down - plastic_hole_d/2]) rotate([-90, 0, 0]) linear_extrude(attach_left + plastic_hole_d + attach_right) polygon([
				[0, 0],
				[attach_chamfer_size, 0],
				[0, attach_chamfer_size]]);
		}
		union(){
			rotate([0, 90, 0]) cylinder(d=hole_d, h=inf);
			if(dir){
				translate([-nut_h + attach_l - nut_start_pos, -nut_w/2, -inf + nut_max_r]) cube([nut_h, nut_w, inf]);
			}else{				
				translate([-nut_h + attach_l - nut_start_pos, -nut_w/2, -nut_max_r]) cube([nut_h, nut_w, inf]);
			}
		}
	}
	s = plastic_hole_d*1.001;
	translate([attach_l - nut_start_pos, -s/2, -s/2]) cube([sacrificial_layer_height, s, s]);
}

module handle(){
	difference(){
		difference(){
			cylinder(d=handle_d, h=handle_h);
			translate([-inf/2 - handle_d/2 + handle_cut, 0, 0]) cube([inf, inf, inf], center=true);
		}
		union(){
			translate([0, -inf/2, 0]) rotate([0, -90 - 45, 0]) cube([inf, inf, inf]);
			translate([0, -inf/2, handle_h]) rotate([0, -90 - 45, 0]) cube([inf, inf, inf]);
			for(i = [0:len(text_1)]){
				translate([handle_cut - handle_d/2, 0, text_1_pos - text_1_spacing*i]) mirror([0, 1, 0]) rotate([90, 0, 90]) linear_extrude(text_depth) text(text_1[i], size=text_1_size, halign="center");
			}
			translate([handle_cut - handle_d/2, 0, text_2_pos]) mirror([0, 1, 0]) rotate([90, 0, 90]) linear_extrude(text_depth) text(text_2, size=text_2_size, halign="center");
			translate([-handle_d/2 + handle_cut, 0, line_pos]) rotate([90, 0, 0]) rotate([0, 0, 360/12]) cylinder(d=line_size, h=inf, $fn=6, center=true);
			translate([handle_cut - handle_d/2, 0, text_3_pos]) mirror([0, 1, 0]) rotate([90, 0, 90]) linear_extrude(text_depth) text(text_3, size=text_2_size, halign="center");
		}
	}
	
}

translate([0, 0, -handle_h/2]) handle();
translate([0, 0, hole_dist/2]){
	attach(true);
	translate([brand_pos_x, -plastic_hole_d/2 - attach_left, brand_pos_z]) rotate([90, 0, 0]) brand();
}
mirror([0, 0, -1]) translate([0, 0, hole_dist/2]) attach(false);

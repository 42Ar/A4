

hole_dist = 160;
hole_d = 4.5;
hole_head_d = 10;
hole_depth = 4;
hole_handle_dist = 40;

attach_width = 6;
attach_w = 45;
hole_margin = 9;

handle_d = 25;
handle_h = attach_w + hole_dist;
handle_cut = handle_d*0.15;

text_depth = 0.4;
text_1 = "HaDiKo";
text_1_pos = handle_h - 35;
text_1_spacing = 20;
text_1_size = 18;
text_2 = "A4";
text_2_pos = 16;
text_2_size = 11;

line_size = 7;
line_pos = 36;

inf = 1000;

$fn = 100;

module attach(){
	difference(){
		s = attach_w/(1 + 2/sqrt(2));
		t = s/sqrt(2);
		hull(){
			translate([0, 0, t]) cube([handle_d/2 + hole_handle_dist + hole_margin, attach_width, s]);
			cube([handle_d/2 + hole_handle_dist + hole_margin - t, attach_width, attach_w]);
		}
		translate([handle_d/2 + hole_handle_dist, attach_width, attach_w/2]) rotate([90, 0, 0]){
			cylinder(d = hole_d, h = attach_width);
			cylinder(d = hole_head_d, h = attach_width - hole_depth);
		}
	}
}

difference(){
	union(){
		attach();
		translate([0, 0, handle_h - attach_w]) attach();
		difference(){
			cylinder(d=handle_d, h=handle_h);
			translate([-inf/2 - handle_d/2 + handle_cut, 0, 0]) cube([inf, inf, inf], center=true);
		}
	}
	union(){
		translate([0, -inf/2, 0]) rotate([0, -90 - 45, 0]) cube([inf, inf, inf]);
		translate([0, -inf/2, handle_h]) rotate([0, -90 - 45, 0]) cube([inf, inf, inf]);
		for(i = [0:len(text_1)]){
			translate([handle_cut - handle_d/2, 0, text_1_pos - text_1_spacing*i]) mirror([0, 1, 0]) rotate([90, 0, 90]) linear_extrude(text_depth) text(text_1[i], size=text_1_size, halign="center");
		}
		translate([handle_cut - handle_d/2, 0, text_2_pos]) mirror([0, 1, 0]) rotate([90, 0, 90]) linear_extrude(text_depth) text(text_2, size=text_2_size, halign="center");
		translate([-handle_d/2 + handle_cut, 0, line_pos]) rotate([90, 0, 0]) rotate([0, 0, 360/12]) cylinder(d=line_size, h=inf, $fn=6, center=true);
	}
	
	echo(handle_h);
}
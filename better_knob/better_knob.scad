

upper_chamfer_size = 1;
lower_chamfer_size = 1;
inner_chamfer_size = 4;
outer_d = 39.25;
inner_d = 35.7;
lower_w = 2;
knob_d = 46.9;
total_h = 25.6 + upper_chamfer_size;
grip_h = 0.7*inner_d/2;
grip_w = lower_w;
top_w = 1;
shift = 4;
text_w = 0.6;
text_x = 18;
text_y = -7.6;
label = "03";

$fn = 100;

inf = 10*outer_d;

difference(){
	union(){
		x1 = (outer_d - inner_d)/2;
		rotate_extrude() polygon([
			[inner_d/2, 0],
			[knob_d/2 - lower_chamfer_size, 0],
			[knob_d/2, lower_chamfer_size],
			[knob_d/2, lower_w],
			[outer_d/2, lower_w],
			[outer_d/2, total_h - upper_chamfer_size],
			[outer_d/2 - upper_chamfer_size, total_h],
			[0, total_h],
			[0, total_h - top_w],
			[inner_d/2 - inner_chamfer_size, total_h - top_w],
			[inner_d/2, total_h - top_w - inner_chamfer_size]
		]);
		intersection(){
			cylinder(d=inner_d, h=total_h);
			union(){
				translate([inner_d/2 - grip_h + grip_w, -outer_d/2, 0]) union(){
					cube([outer_d, outer_d, grip_w]);
					rotate([-90, 0, 0]) cylinder(d=grip_w*2, h=outer_d);
				}
				translate([shift, -inf/2, total_h]) mirror([1, 0, 0]) rotate([0, 45, 0]) cube([inf, inf, inf]);
			}
		}
	}
	rotate([180, 0, 0]) translate([text_x, text_y, -text_w]) rotate([0, 0, 90]) linear_extrude(text_w) text(label);
}
sphere_d = 20;
sphere_center_h = 13.6;
cylinder_d = 10;
strip_width = 10;
max_screw_length = 17;
screw_d = 4.1;
sacrificial_layer_height = 0.2;
hole_dist = 452;  // equals unadjusted diameter of ring
strip_center_height = 28.5 - 3.5;
led_per_meter = 60;
led_holder_width = 3;
dist_offset = -2;
$fn = 100;
fn_big = 600;
washer_r = 7.8/2 + 0.2;
clearance = 1;
extension_length = 3;
zip_tie_width = 2.8;
zip_tie_h = 1.1;
lily_pad_size = 10;
lily_pad_height = 0.3;
hole_move_down = 2;

n_led = floor(PI*(hole_dist - 2*dist_offset)*led_per_meter/1e3);
echo("LED count:", n_led);
adjusted_r = n_led*1e3/(2*PI*led_per_meter);
upper_h = strip_center_height + strip_width/2 - sphere_center_h;


module holder_ring(h, outer){
    difference(){
        cylinder(r=outer, h=h, $fn=fn_big);
        translate([0, 0, -1]) cylinder(r=adjusted_r - led_holder_width, h=h + 2, $fn=fn_big);
        translate([-4*outer, -2*outer, -1]) cube([4*outer, 4*outer, h + 2]);
        translate([-2*outer, -4*outer, -1]) cube([4*outer, 4*outer, h + 2]);
    }
}

module end_cap(){
    translate([0, -hole_dist/2, 0]) rotate([0, 0, 45]) translate([0, adjusted_r - extension_length - led_holder_width + 0.001, strip_center_height - strip_width/2]) union(){
        translate([0, -lily_pad_size + extension_length, strip_width - lily_pad_height]) cube([lily_pad_size, lily_pad_size, lily_pad_height]);
        difference(){
            cube([led_holder_width, extension_length, strip_width]);
            translate([0, extension_length - zip_tie_h, strip_width/2 - zip_tie_width/2]) cube([led_holder_width, zip_tie_h, zip_tie_width]);
        }
    }
}

difference(){
    union(){
        cylinder(h=sphere_center_h, d=cylinder_d);
        translate([0, 0, sphere_center_h - 0.001]) difference(){ // very hacky hack
            sphere(d=sphere_d, $fn=99); // also need different fn
            translate([-sphere_d, -sphere_d, 0]) cube([2*sphere_d, 2*sphere_d, 2*sphere_d]);
        }
        difference(){
            translate([0, 0, sphere_center_h]) cylinder(sphere_d/2, d1=sphere_d, d2=0);
            translate([-sphere_d/2, -sphere_d/2, strip_center_height - strip_width/2 - clearance]) cube([sphere_d, sphere_d, 10*sphere_d]);
        }
        translate([0, -hole_dist/2, strip_center_height - strip_width/2]) rotate([0, 0, 45]) holder_ring(strip_width, adjusted_r);
        end_cap();
        mirror([1, 0, 0]) end_cap();
        translate([0, 0, sphere_center_h]) intersection(){
            cylinder(h=upper_h, d=sphere_d);
            translate([0, -hole_dist/2, 0]) rotate([0, 0, 90 + 45]) cylinder(r=adjusted_r, h=upper_h, $fn=fn_big);        
        }
    }
    translate([0, 0, -1]) cylinder(d=screw_d, h=sphere_center_h + upper_h + 2);
    translate([0, 0, sphere_center_h - hole_move_down]) cylinder(h=upper_h + 2 + hole_move_down, r=washer_r, $fn=6);
}
translate([-washer_r/2, -washer_r/2, sphere_center_h - sacrificial_layer_height - hole_move_down]) cube([washer_r, washer_r, sacrificial_layer_height]);

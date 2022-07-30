h = 4;
inner_d = 3.2;
outer_d = inner_d + 2*2;
$fn=100;

difference(){
    cylinder(h=h, d=outer_d);
    cylinder(h=h, d=inner_d);
}
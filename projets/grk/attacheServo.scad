largeur=36;
largeurser=24;
hauteur=12;
epaisseur=2;
profondeur=15;
rayon=1.5;
$fn=10;

rt=1;
lt=32-5;

difference(){
union(){
cube([largeur,profondeur-epaisseur,epaisseur]);
cube([(largeur-largeurser)/2,epaisseur,hauteur+epaisseur]);
translate([largeur-(largeur-largeurser)/2,0,0])cube([(largeur-largeurser)/2,epaisseur,hauteur+epaisseur]);
}

translate([(largeur-lt)/2,epaisseur,epaisseur+hauteur/2])rotate([90,0,0])cylinder(r=1,h=epaisseur);
translate([largeur-(largeur-lt)/2,epaisseur,epaisseur+hauteur/2])rotate([90,0,0])cylinder(r=1,h=epaisseur);


translate([rayon+1,rayon+profondeur/2,0])cylinder(r=1.5,h=epaisseur);
translate([largeur-(rayon+1),rayon+profondeur/2,0])cylinder(r=1.5,h=epaisseur);
}
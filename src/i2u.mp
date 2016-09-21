prologues:=2;  % for proper handling of bounding box in eps
outputtemplate:="%j.eps"; %or "%j-%c.eps" for j=filename c=figurenumber
%outputformat:="eps";  % default 
beginfig(1)
	color bkgrd; bkgrd:=(.6,.6,0);
	color iu; iu:=(0,0,1);
	color ar; ar:=(.6,0,0);
	%fill fullcircle scaled 64 shifted (32,32) withcolor bkgrd;
	fill unitsquare scaled 186 withcolor bkgrd;
	pickup pensquare scaled 6;
	draw unitsquare scaled 186 withcolor black;
	%fill
	pair A,B,C,D,E,F,G,H,J,K,L;
	A:=(73,163);
	B:=(32,79);
	C:=(148,93);
	D:=(122,41);
	E:=(76,23);
	F:=(61,70);
	G:=(87,122);
	H:=(116,105);
	J:=(116,151);
	K:=(99,145);
	L:=(128,157);
	fill K--L{dir -60}..{dir -135}H{dir 75}..{dir 150}cycle withcolor ar;
	pickup pencircle scaled 23;
	draw A--B withcolor iu;
	draw C--D..E..F--G withcolor iu;
endfig;

end;


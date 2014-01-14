TriOsc foo => Sigmund siggy => blackhole;
0.1 => foo.gain;

while (true)
{
	Math.random2f(220,1600) => foo.freq;
	second => now;
	<<< "Real frequency:",foo.freq(),"Sigmund found this:",siggy.freq() >>>;
}
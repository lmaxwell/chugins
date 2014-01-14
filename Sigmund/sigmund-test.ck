TriOsc foo => Sigmund siggy => blackhole;
//0.1 => foo.gain;

while (true)
{
	Math.random2f(220,1600) => foo.freq;
	Math.random2f(0,1) => foo.gain;
	second => now;
	<<< "Real frequency:",foo.freq(),"real gain:",foo.gain(),"Sigmund found this:",siggy.freq(), "power:", siggy.power() >>>;
}
public class Scape extends Chubgraph
{
	inlet => LADSPA ladspa => outlet;
	"/usr/local/lib/ladspa/caps.so" => ladspa.load;
	"Scape" => ladspa.activate;

	fun float constrain (float test, float low, float high)
	{
		if (test < low) return low;
		if (test > high) return high;
		return test;
	}
	
	fun float bpm (float x)
	{
		constrain(x,30,164) => float val;
		ladspa.set(0,val);
		return val;
	}

	fun float divider (float x)
	{
		constrain(x,2,4) => float val;
		ladspa.set(1,val);
		return val;
	}

		fun float feedback (float x)
	{
		constrain(x,0,1) => float val;
		ladspa.set(2,val);
		return val;
	}

		fun float dry (float x)
	{
		constrain(x,0,1) => float val;
		ladspa.set(3,val);
		return val;
	}

		fun float blend (float x)
	{
		constrain(x,0,1) => float val;
		ladspa.set(4,val);
		return val;
	}

		fun float tune (float x)
	{
		constrain(x,415,467) => float val;
		ladspa.set(5,val);
		return val;
	}

	fun float constrain (float test, float low, float high)
	{
		if (test < low) return low;
		if (test > high) return high;
		return test;
	}
}
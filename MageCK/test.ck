MageCK mageck => dac;
mageck.init("awb","data/configFiles/cmu-artic/awb.conf","data/labels/cmu-artic/alice01.lab",0.55);
[48,50,52,53,55,56,58,59] @=> int scale[];

//spork ~dur();
//spork ~compute();
//spork ~control();
Event e;
mageck.startSynthesis();
0=>int i;
while(true)
{
	scale[0] + Math.random2(-1,1)*12 => Std.mtof => mageck.freq;
	//0.1*48000.0/mageck.getDur()=>mageck.dur;
	//0.001=>mageck.dur;
	//<<<mageck.getFreq()>>>;
	1::second=>now;	
	(i+2)%8=>i;
}


fun void control()
{
	while(true)
	{
		mageck.startSynthesis();
		1::second=>now;
		mageck.stopSynthesis();
	}
}

fun void dur()
{
	1::second=>now;
	0=>int i;
	while(true)
	{

	//	<<<i,"th turn">>>;
		//<<<mageck.ready()>>>;
		if(mageck.ready()==1)
		{	
			if(mageck.popLabel()==1)
			{
				//<<<"popLAbel">>>;
		//		<<<mageck.computeDuration()>>>;
			 	mageck.computeDuration();
			 	50::ms=>now;
			}
			else
			{
				<<<"pop failed">>>;
			}
		}
		else 
		{
			1000::ms=>now;	
		}
		
		//i++;
	}
}

fun void compute()
{
	while(true)
	{
		mageck.computeParameters();
		e=>now;
	}
}




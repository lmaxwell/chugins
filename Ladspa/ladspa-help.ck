"./filter.so" => string plugname;
if (me.args()) me.arg(0) => plugname;
Noise n => Ladspa plugin => dac;
<<< 0.494 => plugin.param >>>;
plugname => plugin.load;
plugin.info();

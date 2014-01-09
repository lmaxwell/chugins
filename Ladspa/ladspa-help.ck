"./filter.so" => string plugname;
if (me.args()) me.arg(0) => plugname;
Noise n => Ladspa plugin => dac;
plugname => plugin.load;
plugin.info();
"hpf" => plugin.label;

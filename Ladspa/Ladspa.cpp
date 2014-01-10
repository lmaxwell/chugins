//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chugerate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"
#include "ladspa.h"
//#include "utils.h"

// general includes
#include <math.h>
#include <stdio.h>
#include <limits.h>
#include <dlfcn.h>

#define DEFAULT_BUFSIZE 1

// declaration of chugin constructor
CK_DLL_CTOR(ladspa_ctor);
// declaration of chugin desctructor
CK_DLL_DTOR(ladspa_dtor);

CK_DLL_MFUN(ladspa_load);
CK_DLL_MFUN(ladspa_list);
CK_DLL_MFUN(ladspa_info);
CK_DLL_MFUN(ladspa_label);
CK_DLL_MFUN(ladspa_set);
CK_DLL_MFUN(ladspa_get);

// for Chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICKF(ladspa_tick);

// this is a special offset reserved for Chugin internal data
t_CKINT ladspa_data_offset = 0;

// class definition for internal Chugin data
// (note: this isn't strictly necessary, but serves as example
// of one recommended approach)
class Ladspa
{
public:
  // constructor
  Ladspa( t_CKFLOAT fs)
  {
    pPlugin = (LADSPA_Handle *)malloc(sizeof(LADSPA_Handle));
    pluginLoaded = false;
    srate = fs;
    bufsize = DEFAULT_BUFSIZE;
  }
  
  ~Ladspa()
  {
	//const LADSPA_Descriptor * thisDescriptor = psDescriptor;
    psDescriptor->cleanup(pPlugin);
    if (pluginLoaded) dlclose(pvPluginHandle);
    printf("closed LADSPA plugin.\n");
    //free (pvPluginHandle);
  }
  
  // for Chugins extending UGen
  void tick( SAMPLE *in, SAMPLE *out, int nframes )
  {
	if (pluginLoaded)
	  {
		for (int i=0; i<inports; i++) inbuf[i][0] = (LADSPA_Data)in[i%2];
		psDescriptor->run(pPlugin, 1);
		for (int i=0; i<2; i++) out[i%2] = (SAMPLE)outbuf[i%outports][0];
	  }
  }

  float set( float val, int param)
  {
	//int param = (int)fparam;
	if (param < kinports)
	  {
		printf ("LADSA: setting parameter \"%s\" to %f\n",
				psDescriptor->PortNames[kinbufRef[param]], val);
		kinbuf[param] = (LADSPA_Data)val;
	  }
	else
	  if (kinports>1)
		printf ("LADSPA Error: param must be between 0 and %d.\n", kinports-1);
	  else
		printf ("LADSPA Error: param must be 0.\n");
  	return val;
  }

  float get (int param)
  {
	if (koutports == 0)
	  {
		printf ("LADSPA Error: plugin has no control output ports.\n");
		return 0;
	  }	
	if (param < koutports)
	  return koutbuf[param];
	if (koutports>0)
	  printf ("LADSPA Error: param must be between 0 and %d.\n", koutports-1);
	else
	  printf ("LADSPA Error: param must be 0.\n");
	return 0;
  }
  
  int choosePlugin ( Chuck_String *p)
  {
    const char * pcPluginLabel = p->str.c_str();	
    for (int i=0;; i++)
      {
	psDescriptor = pfDescriptorFunction(i);
	if (psDescriptor == NULL)
	  {
	    printf("LADSPA error: unable to find lable \"%s\" in plugin.\n", pcPluginLabel);
	    return 0;
	  }
	if (strcmp(psDescriptor->Label, pcPluginLabel) == 0)
	  {
	    printf("LADSPA: Activating plugin \"%s\"\n", pcPluginLabel);
	    pPlugin = psDescriptor->instantiate(psDescriptor, srate);
	    connectPorts();
	    return 1;
	  }
      }
    return 0;
  }

  int LADSPA_load ( Chuck_String * p)
  {
    const char * pcPluginFilename = p->str.c_str();
    printf("Loading LADSPA plugin %s\n", pcPluginFilename);
    pvPluginHandle = dlopen(pcPluginFilename, RTLD_NOW);
    dlerror();
    
    pfDescriptorFunction = (LADSPA_Descriptor_Function)dlsym(pvPluginHandle, "ladspa_descriptor");
    if (!pfDescriptorFunction)
      {
		const char * pcError = dlerror();
		if (pcError) 
		  printf("Unable to find ladspa_descriptor() function in plugin file "
				 "\"%s\"\n"
				 "Are you sure this is a LADSPA plugin file?\n", 
				 pcPluginFilename);
		return 0;
      }
	//LADSPA_list();
    //printf ("Valid LADSPA file found!\n");
	return 1;
  }

  // TODO: error checking  
  int LADSPA_list ()
  {
    printf ("Plugins available under this LADSPA file:\n");
    for (int i = 0;; i++)
      {
		
		psDescriptor = pfDescriptorFunction(i);
		if (!psDescriptor)
		  break;
		
	putchar('\n');
	
	printf("Plugin Label: \"%s\"\n", psDescriptor->Label);
	printf("Plugin Name: \"%s\"\n", psDescriptor->Name);
      }
    printf("--------------------------------------------------\n");
	//putchar('\n');
	
    return 1;
  }		

  // TODO: error checking  
int LADSPA_info ()
  {
  int bFound;
  int knum;
  unsigned long lIndex;
  LADSPA_PortRangeHintDescriptor iHintDescriptor;
  LADSPA_Data fBound;
  printf("--------------------------------------------------\n");	
  printf(
	  "Plugin \"%s\" has the following control inputs:\n",
	  psDescriptor->Name);

  knum = 0;
  bFound = 0;
  for (lIndex = 0; lIndex < psDescriptor->PortCount; lIndex++)
    if (LADSPA_IS_PORT_INPUT(psDescriptor->PortDescriptors[lIndex])
	&& LADSPA_IS_PORT_CONTROL(psDescriptor->PortDescriptors[lIndex])) {
      printf(
	      "\tControl %d: %s",
	      knum++, psDescriptor->PortNames[lIndex]);
      bFound = 1;
      iHintDescriptor = psDescriptor->PortRangeHints[lIndex].HintDescriptor;
      if (LADSPA_IS_HINT_BOUNDED_BELOW(iHintDescriptor)
	  || LADSPA_IS_HINT_BOUNDED_ABOVE(iHintDescriptor)) {
	printf( " (");
	if (LADSPA_IS_HINT_BOUNDED_BELOW(iHintDescriptor)) {
	  fBound = psDescriptor->PortRangeHints[lIndex].LowerBound;
	  if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor)) {
	    if (fBound == 0)
	      printf( "0");
	    else
	      printf( "%g * sample rate", fBound);
	  }
	  else
	    printf( "%g", fBound);
	}
	else
	  printf( "...");
	printf( " to ");
	if (LADSPA_IS_HINT_BOUNDED_ABOVE(iHintDescriptor)) {
	  fBound = psDescriptor->PortRangeHints[lIndex].UpperBound;
	  if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor)) {
	    if (fBound == 0)
	      printf( "0");
	    else
	      printf( "%g * sample rate", fBound);
	  }
	  else
	    printf( "%g", fBound);
	}
	else
	  printf( "...");
	printf( ")\n");
      }
      else
	printf( "\n");
    }
      
  if (!bFound)
    printf( "\tnone\n");
  printf("--------------------------------------------------\n");
  }

private:
  
  void connectPorts()
  {
	//const LADSPA_Descriptor * thisDescriptor = psDescriptor;
    //printf("Connecting LADSPA audio ports...\n\n");

    // Count ports
    kinports = 0;
    koutports = 0;
    inports = 0;
    outports = 0;
	
    for (int i=0; i<psDescriptor->PortCount; i++)
      {
		iPortDescriptor = psDescriptor->PortDescriptors[i];
		if (LADSPA_IS_PORT_AUDIO(iPortDescriptor))
		  {
			if (LADSPA_IS_PORT_INPUT(iPortDescriptor)) inports++;
			else if (LADSPA_IS_PORT_OUTPUT(iPortDescriptor)) outports++;
		  }
		else if (LADSPA_IS_PORT_CONTROL(iPortDescriptor))
		  {
			if (LADSPA_IS_PORT_INPUT(iPortDescriptor)) kinports++;
			else if (LADSPA_IS_PORT_OUTPUT(iPortDescriptor)) koutports++;
		  }
      }
	
    inbuf = (LADSPA_Data **)malloc(sizeof(LADSPA_Data *)*inports);
    outbuf = (LADSPA_Data **)malloc(sizeof(LADSPA_Data *)*outports);
    kinbuf = (LADSPA_Data *)malloc(sizeof(LADSPA_Data)*inports);
    koutbuf = (LADSPA_Data *)malloc(sizeof(LADSPA_Data)*outports);
    kinbufRef = (short *)malloc(sizeof(short)*inports);
    koutbufRef = (short *)malloc(sizeof(short)*outports);
    for (int i=0; i<inports; i++)
      {
		inbuf[i] = (LADSPA_Data *)malloc(sizeof(LADSPA_Data)*bufsize);
      }
    for (int i=0; i<outports; i++)
      {
		outbuf[i] = (LADSPA_Data *)malloc(sizeof(LADSPA_Data)*bufsize);
      }
	for (int i=0; i<kinports; i++)
	  {
		kinbuf[i] = 0.0;
		kinbufRef[i] = 0;
	  }
	for (int i=0; i<koutports; i++)
	  {
		koutbuf[i] = 0.0;
		koutbufRef[i] = 0;
	  }
	
    //printf("Kinports: %d, Koutputs: %d, Audio inports: %d, outports: %d\n",
	//kinports, koutports, inports, outports);
	
    int inbufIndex = 0;
    int outbufIndex = 0;
    int kinbufIndex = 0;
    int koutbufIndex = 0;
	
    // connect ports
    for (int i=0; i<psDescriptor->PortCount; i++)
      {
		iPortDescriptor = psDescriptor->PortDescriptors[i];
		if (LADSPA_IS_PORT_AUDIO(iPortDescriptor))
		  {
			if (LADSPA_IS_PORT_INPUT(iPortDescriptor))
			  psDescriptor->connect_port(pPlugin, i, inbuf[inbufIndex++]);
			else if (LADSPA_IS_PORT_OUTPUT(iPortDescriptor))
			  psDescriptor->connect_port(pPlugin, i, outbuf[outbufIndex++]);
		  }
		else if (LADSPA_IS_PORT_CONTROL(iPortDescriptor))
		  {
			if (LADSPA_IS_PORT_INPUT(iPortDescriptor))
			  {
				psDescriptor->connect_port(pPlugin, i, &kinbuf[kinbufIndex]);
				LADSPA_Data portDefault = get_default( i );
				kinbuf[kinbufIndex] = portDefault;
				kinbufRef[kinbufIndex++] = i;
			  }
			else if (LADSPA_IS_PORT_OUTPUT(iPortDescriptor))
			  {
				psDescriptor->connect_port(pPlugin, i, &koutbuf[koutbufIndex]);
				koutbufRef[koutbufIndex++] = i;
			  }
		  }
      }
	
	if (psDescriptor->activate != NULL)
	  {
		//printf("  Activating plugin...\n");
		psDescriptor->activate(pPlugin);
		//printf("  Activated!\n");
		pluginLoaded = true;
	  }
  }

  LADSPA_Data get_default ( int index )
  {
	iHintDescriptor = psDescriptor->PortRangeHints[index].HintDescriptor;
	switch (iHintDescriptor & LADSPA_HINT_DEFAULT_MASK) {
	case LADSPA_HINT_DEFAULT_NONE:
	  break;
	case LADSPA_HINT_DEFAULT_MINIMUM:
	  fDefault = psDescriptor->PortRangeHints[index].LowerBound;
	  /*if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor) && fDefault != 0) 
		printf(", default %g*srate", fDefault);
	  else 
	  printf(", default %g", fDefault);*/
	  break;
	case LADSPA_HINT_DEFAULT_LOW:
	  if (LADSPA_IS_HINT_LOGARITHMIC(iHintDescriptor)) {
		fDefault 
		  = exp(log(psDescriptor->PortRangeHints[index].LowerBound) 
				* 0.75
				+ log(psDescriptor->PortRangeHints[index].UpperBound) 
				* 0.25);
	  }
	  else {
		fDefault 
		  = (psDescriptor->PortRangeHints[index].LowerBound
			 * 0.75
			 + psDescriptor->PortRangeHints[index].UpperBound
			 * 0.25);
	  }
	  /*
	  if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor) && fDefault != 0) 
		printf(", default %g*srate", fDefault);
	  else 
	  printf(", default %g", fDefault);*/
	  break;
	case LADSPA_HINT_DEFAULT_MIDDLE:
	  if (LADSPA_IS_HINT_LOGARITHMIC(iHintDescriptor)) {
		fDefault 
		  = sqrt(psDescriptor->PortRangeHints[index].LowerBound
				 * psDescriptor->PortRangeHints[index].UpperBound);
	  }
	  else {
		fDefault 
		  = 0.5 * (psDescriptor->PortRangeHints[index].LowerBound
				   + psDescriptor->PortRangeHints[index].UpperBound);
	  }
	  /*
	  if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor) && fDefault != 0) 
		printf(", default %g*srate", fDefault);
	  else 
	  printf(", default %g", fDefault);*/
	  break;
	case LADSPA_HINT_DEFAULT_HIGH:
	  if (LADSPA_IS_HINT_LOGARITHMIC(iHintDescriptor)) {
		fDefault 
		  = exp(log(psDescriptor->PortRangeHints[index].LowerBound) 
				* 0.25
				+ log(psDescriptor->PortRangeHints[index].UpperBound) 
				* 0.75);
	  }
	  else {
		fDefault 
		  = (psDescriptor->PortRangeHints[index].LowerBound
			 * 0.25
			 + psDescriptor->PortRangeHints[index].UpperBound
			 * 0.75);
	  }
	  /*
	  if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor) && fDefault != 0) 
		printf(", default %g*srate", fDefault);
	  else 
	  printf(", default %g", fDefault);*/
	  break;
	case LADSPA_HINT_DEFAULT_MAXIMUM:
	  fDefault = psDescriptor->PortRangeHints[index].UpperBound;
	  /*if (LADSPA_IS_HINT_SAMPLE_RATE(iHintDescriptor) && fDefault != 0) 
		printf(", default %g*srate", fDefault);
	  else 
	  printf(", default %g", fDefault);*/
	  break;
	case LADSPA_HINT_DEFAULT_0:
	  //printf(", default 0");
	  fDefault = 0;
	  break;
	case LADSPA_HINT_DEFAULT_1:
	  //printf(", default 1");
	  fDefault = 1;
	  break;
	case LADSPA_HINT_DEFAULT_100:
	  //printf(", default 100");
	  fDefault = 100;
	  break;
	case LADSPA_HINT_DEFAULT_440:
	  //printf(", default 440");
	  fDefault = 440;
	  break;
	default:
	  printf("LADSPA warning: UNKNOWN DEFAULT CODE\n");
	  /* (Not necessarily an error - may be a newer version.) */
	  break;
	}
	return fDefault;
  }
  
  // instance data
  LADSPA_Descriptor_Function pfDescriptorFunction;
  const LADSPA_Descriptor * psDescriptor;
  LADSPA_PortRangeHintDescriptor iHintDescriptor;
  LADSPA_PortDescriptor iPortDescriptor;
  LADSPA_Data fBound;
  LADSPA_Data fDefault;
  LADSPA_Handle pPlugin;
  LADSPA_Data ** inbuf, ** outbuf; // audio in and out buffers (multichannel)
  LADSPA_Data * kinbuf, * koutbuf; // control in and out buffers
  short * kinbufRef, * koutbufRef;
  void * pvPluginHandle;
  bool pluginLoaded;
  int bufsize;
  int numchans;
  int kinports, koutports, inports, outports;
  float srate;
};


// query function: chuck calls this when loading the Chugin
// NOTE: developer will need to modify this function to
// add additional functions to this Chugin
CK_DLL_QUERY( Ladspa )
{
  // hmm, don't change this...
  QUERY->setname(QUERY, "LADSPA");
  
  // begin the class definition
  // can change the second argument to extend a different ChucK class
  QUERY->begin_class(QUERY, "LADSPA", "UGen");
  
  // register the constructor (probably no need to change)
  QUERY->add_ctor(QUERY, ladspa_ctor);
  // register the destructor (probably no need to change)
  QUERY->add_dtor(QUERY, ladspa_dtor);
  
  // for UGen's only: add tick function
  QUERY->add_ugen_funcf(QUERY, ladspa_tick, NULL, 2, 2);
  
  // NOTE: if this is to be a UGen with more than 1 channel, 
  // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
  // and declare a tickf function using CK_DLL_TICKF
  
  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_load, "int", "load");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "string", "filename");
  
  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_label, "int", "activate");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "string", "label");

  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_set, "float", "set");
  // example of adding argument to the above method
  QUERY->add_arg(QUERY, "int", "param");
  QUERY->add_arg(QUERY, "float", "val");

  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_get, "float", "get");
  QUERY->add_arg(QUERY, "int", "param");
  
  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_info, "int", "info");

  // example of adding setter method
  QUERY->add_mfun(QUERY, ladspa_list, "int", "list");
  
  // this reserves a variable in the ChucK internal class to store 
  // referene to the c++ class we defined above
  ladspa_data_offset = QUERY->add_mvar(QUERY, "int", "@l_data", false);
  
  // end the class definition
  // IMPORTANT: this MUST be called!
  QUERY->end_class(QUERY);
  
  // wasn't that a breeze?
  return TRUE;
}


// implementation for the constructor
CK_DLL_CTOR(ladspa_ctor)
{
  // get the offset where we'll store our internal c++ class pointer
  OBJ_MEMBER_INT(SELF, ladspa_data_offset) = 0;
  
  // instantiate our internal c++ class representation
  Ladspa * bcdata = new Ladspa(API->vm->get_srate());
  
  // store the pointer in the ChucK object member
  OBJ_MEMBER_INT(SELF, ladspa_data_offset) = (t_CKINT) bcdata;
}


// implementation for the destructor
CK_DLL_DTOR(ladspa_dtor)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // check it
  if( bcdata )
    {
      // clean up
      delete bcdata;
      OBJ_MEMBER_INT(SELF, ladspa_data_offset) = 0;
      bcdata = NULL;
    }
}

// implementation for tick function
CK_DLL_TICKF(ladspa_tick)
{
  // get our c++ class pointer
  Ladspa * c = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  
  // invoke our tick function; store in the magical out variable
  //if(c) *out = c->tick(in);
  if(c) c->tick(in, out, nframes);

  // yes
  return TRUE;
}

// example implementation for setter
CK_DLL_MFUN(ladspa_load)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  RETURN->v_int = bcdata->LADSPA_load(GET_NEXT_STRING(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(ladspa_label)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  RETURN->v_int = bcdata->choosePlugin(GET_NEXT_STRING(ARGS));
}

// example implementation for setter
CK_DLL_MFUN(ladspa_info)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  RETURN->v_int = bcdata->LADSPA_info();
}

// example implementation for setter
CK_DLL_MFUN(ladspa_list)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  RETURN->v_int = bcdata->LADSPA_list();
}

// example implementation for setter
CK_DLL_MFUN(ladspa_set)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  t_CKINT param = GET_NEXT_INT(ARGS);
  t_CKFLOAT val = GET_NEXT_FLOAT(ARGS);

  RETURN->v_float = bcdata->set(val, param);
}

// example implementation for setter
CK_DLL_MFUN(ladspa_get)
{
  // get our c++ class pointer
  Ladspa * bcdata = (Ladspa *) OBJ_MEMBER_INT(SELF, ladspa_data_offset);
  // set the return value
  t_CKINT param = GET_NEXT_INT(ARGS);

  RETURN->v_float = bcdata->get(param);
}

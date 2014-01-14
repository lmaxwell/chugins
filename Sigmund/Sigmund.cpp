//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chugerate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"
#include "Sigmund.h"

// general includes
#include <math.h>

// declaration of chugin constructor
CK_DLL_CTOR(sigmund_ctor);
// declaration of chugin desctructor
CK_DLL_DTOR(sigmund_dtor);

CK_DLL_MFUN(sigmund_clear);
CK_DLL_MFUN(sigmund_getFreq);
CK_DLL_MFUN(sigmund_getPower);
CK_DLL_MFUN(sigmund_getPeaks);
CK_DLL_MFUN(sigmund_getTracks);

CK_DLL_MFUN(sigmund_setNpts);
CK_DLL_MFUN(sigmund_setNpeak);
CK_DLL_MFUN(sigmund_setStabletime);
CK_DLL_MFUN(sigmund_setMaxfreq);
CK_DLL_MFUN(sigmund_setMinpower);
CK_DLL_MFUN(sigmund_setParam1);
CK_DLL_MFUN(sigmund_setParam2);
CK_DLL_MFUN(sigmund_setParam3);



// for Chugins extending UGen, this is mono synthesis function for 1 sample
CK_DLL_TICK(sigmund_tick);

// this is a special offset reserved for Chugin internal data
t_CKINT sigmund_data_offset = 0;

extern "C"
{
  extern void mayer_fft(int n, t_sample *real, t_sample *imag);
  extern void mayer_realfft(int n, t_sample *real);
  void sigmund_getrawpeaks(int npts, t_float *insamps,
			   int npeak, t_peak *peakv, int *nfound, t_float *power, t_float srate, int loud,
			   t_float hifreq);
  void sigmund_getpitch(int npeak, t_peak *peakv, t_float *freqp,
			t_float npts, t_float srate, t_float nharmonics, t_float amppower, int loud);
  void notefinder_doit(t_notefinder *x, t_float freq, t_float power,
		       t_float *note, t_float vibrato, int stableperiod, t_float powerthresh,
		       t_float growththresh, int loud);
  void sigmund_peaktrack(int ninpeak, t_peak *inpeakv, 
			 int noutpeak, t_peak *outpeakv, int loud);
  int sigmund_ilog2(int n);
} 

// class definition for internal Chugin data
// (note: this isn't strictly necessary, but serves as example
// of one recommended approach)
class Sigmund
{
public:
  // constructor
  Sigmund( t_CKFLOAT fs)
  {
    npts = NPOINTS_DEF;
    param1 = 6;
    param2 = 0.5;
    param3 = 0;
    mode = MODE_STREAM;
    npeak = NPEAK_DEF;
    vibrato = VIBRATO_DEF;
    stabletime = STABLETIME_DEF;
    growth = GROWTH_DEF;
    minpower = MINPOWER_DEF;
    maxfreq = 1000000;
    loud = 0;
    srate = fs;
    trackv = 0;
    dopitch = true;
    dotracks = false;
    inbufIndex = 0;
    freq = 0;
	power = 0;
	note = 0;
    inbuf = new SAMPLE[npts];//(SAMPLE*)malloc(sizeof(SAMPLE)*npts);
    for (int i=0; i<npts; i++)
      inbuf[i] = 0;
	peakv = new t_peak[npeak];
	trackv = new t_peak[npeak];
  }
  
  ~Sigmund()
  {
    delete inbuf;
	delete peakv;
	delete trackv;
  }
    
  // for Chugins extending UGen
  SAMPLE tick ( SAMPLE in )
  {
    // default: this passes whatever input is patched into Chugin
    // fill sample buffer
    inbuf[inbufIndex++] = in;
    if (inbufIndex >= npts)
      {
	inbufIndex = 0;
	// THE MAGIC HAPPENS!!
	int nfound, i, cnt;
	sigmund_getrawpeaks(npts, inbuf, npeak, peakv,
						&nfound, &power, srate, loud, maxfreq);
	if (dopitch)
	  sigmund_getpitch(nfound, peakv, &freq, npts, srate, 
					   param1, param2, loud);
	if (dotracks)
	  sigmund_peaktrack(nfound, peakv, npeak, trackv, loud);
      }
	return in;
  }
    
  // get parameter example
  float getFreq() { return freq; }

  // get parameter example
  float getPower() { return sigmund_powtodb(power); }

  Chuck_Object* getPeaks()
  {
	return NULL;
  }

  Chuck_Object* getTracks()
  {
	return NULL;
  }

  void clear()
  {
	for (int i=0; i<npts; i++)
	  inbuf[i] = 0;
	inbufIndex = 0;
  }

  float setNpts ( t_CKINT x)
  {
	float nwas = npts;
	npts = x;
	if (npts < NPOINTS_MIN)
	  npts = NPOINTS_MIN;
	if (npts != (1 << sigmund_ilog2(npts)))
	  printf("Sigmund: adjusting analysis size to %d points\n", (npts = (1 << sigmund_ilog2(npts))));
	if (npts != nwas)
	  inbufIndex = 0;
	if (mode==MODE_STREAM)
	  {
		if (inbuf)
		  {
			delete inbuf;
		  }
		inbuf = new SAMPLE[npts];
	  }
	else inbuf = 0;
	return x;
  }

  float setNpeak ( t_CKINT x)
  {
	npeak = x;
	if (npeak < 1) npeak = 1;
	return x;
  }

  float setMaxfreq ( t_CKFLOAT x)
  {
	maxfreq = x;
	return x;
  }

  float setStabletime ( t_CKDUR x)
  {
	stabletime = x/srate;
	if (stabletime < 0) stabletime = 0;
	return x;
  }

  float setMinpower ( t_CKFLOAT x)
  {
	minpower = 0;
	if (minpower < 0) minpower = 0;
	return x;
  }

  float setParam1 ( t_CKFLOAT x)
  {
	param1 = x;
	return x;
  }

  float setParam2 ( t_CKFLOAT x)
  {
	param2 = x;
	return x;
  }

  float setParam3 ( t_CKFLOAT x)
  {
	param3 = x;
	return x;
  }

  int doPitch (t_CKINT x)
  {
	if (x>0) dopitch = true;
	else dopitch = false;
	return x;
  }

  int doTracks (t_CKINT x)
  {
	if (x>0) dotracks = true;
	else dotracks = false;
	return x;
  }
  
private:

  t_float sigmund_powtodb(t_float f)
  {
    if (f <= 0) return (0);
    else
	  {
        t_float val = 100 + 10./LOG10 * log(f);
        return (val < 0 ? 0 : val);
	  }
  }

  // instance data
  t_float srate;       // sample rate 
  int mode;         // MODE_STREAM, etc. 
  int npts;         // number of points in analysis window 
  int npeak;        // number of peaks to find 
  int loud;         // debug level 
  int infill;       // number of points filled 
  int countdown;    // countdown to start filling buffer 
  t_float maxfreq;    // highest-frequency peak to report  
  t_float vibrato;    // vibrato depth in half tones  
  t_float stabletime; // period of stability needed for note  
  t_float growth;     // growth to set off a new note  
  t_float minpower;   // minimum power, in DB, for a note  
  t_float param1;     // three parameters for temporary use 
  t_float param2;
  t_float param3;
  t_notefinder notefinder;  // note parsing state 
  t_peak *trackv;           // peak tracking state 
  t_peak *peakv;
  bool dopitch, dotracks;
  t_float freq, power, note;
  SAMPLE* inbuf;
  unsigned int inbufIndex;
};

// query function: chuck calls this when loading the Chugin
// NOTE: developer will need to modify this function to
// add additional functions to this Chugin
CK_DLL_QUERY( Sigmund )
{
  // hmm, don't change this...
  QUERY->setname(QUERY, "Sigmund");
  
  // begin the class definition
  // can change the second argument to extend a different ChucK class
  QUERY->begin_class(QUERY, "Sigmund", "UGen");
  
  // register the constructor (probably no need to change)
  QUERY->add_ctor(QUERY, sigmund_ctor);
  // register the destructor (probably no need to change)
  QUERY->add_dtor(QUERY, sigmund_dtor);
  
  // for UGen's only: add tick function
  QUERY->add_ugen_func(QUERY, sigmund_tick, NULL, 1, 1);
  
  // NOTE: if this is to be a UGen with more than 1 channel, 
  // e.g., a multichannel UGen -- will need to use add_ugen_funcf()
  // and declare a tickf function using CK_DLL_TICKF
    
  // example of adding getter method
  QUERY->add_mfun(QUERY, sigmund_clear, "void", "clear");

  // example of adding getter method
  QUERY->add_mfun(QUERY, sigmund_getFreq, "float", "freq");

  // example of adding getter method
  QUERY->add_mfun(QUERY, sigmund_getPower, "float", "env");

  // example of adding getter method
  QUERY->add_mfun(QUERY, sigmund_getPeaks, "Object", "peaks");

  // example of adding getter method
  QUERY->add_mfun(QUERY, sigmund_getTracks, "Object", "tracks");

  QUERY->add_mfun(QUERY, sigmund_setNpts, "int", "npts");
  QUERY->add_arg(QUERY, "int", "npts");

  QUERY->add_mfun(QUERY, sigmund_setNpeak, "int", "npeak");
  QUERY->add_arg(QUERY, "int", "npeak");

  QUERY->add_mfun(QUERY, sigmund_setStabletime, "dur", "stabletime");
  QUERY->add_arg(QUERY, "dur", "stabletime");

  QUERY->add_mfun(QUERY, sigmund_setMaxfreq, "float", "maxfreq");
  QUERY->add_arg(QUERY, "float", "arg");

  QUERY->add_mfun(QUERY, sigmund_setMinpower, "float", "minpower");
  QUERY->add_arg(QUERY, "float", "arg");

  QUERY->add_mfun(QUERY, sigmund_setParam1, "float", "param1");
  QUERY->add_arg(QUERY, "float", "arg");

  QUERY->add_mfun(QUERY, sigmund_setParam2, "float", "param2");
  QUERY->add_arg(QUERY, "float", "arg");

  QUERY->add_mfun(QUERY, sigmund_setParam3, "float", "param3");
  QUERY->add_arg(QUERY, "float", "arg");
  
  // this reserves a variable in the ChucK internal class to store 
  // referene to the c++ class we defined above
  sigmund_data_offset = QUERY->add_mvar(QUERY, "int", "@s_data", false);
  
  // end the class definition
  // IMPORTANT: this MUST be called!
  QUERY->end_class(QUERY);
  
  // wasn't that a breeze?
  return TRUE;
}


// implementation for the constructor
CK_DLL_CTOR(sigmund_ctor)
{
  // get the offset where we'll store our internal c++ class pointer
  OBJ_MEMBER_INT(SELF, sigmund_data_offset) = 0;
  
  // instantiate our internal c++ class representation
  Sigmund * bcdata = new Sigmund(API->vm->get_srate());
  
  // store the pointer in the ChucK object member
  OBJ_MEMBER_INT(SELF, sigmund_data_offset) = (t_CKINT) bcdata;
}


// implementation for the destructor
CK_DLL_DTOR(sigmund_dtor)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // check it
  if( bcdata )
    {
      // clean up
      delete bcdata;
      OBJ_MEMBER_INT(SELF, sigmund_data_offset) = 0;
      bcdata = NULL;
    }
}


// implementation for tick function
CK_DLL_TICK(sigmund_tick)
{
  // get our c++ class pointer
  Sigmund * c = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  
  // invoke our tick function; store in the magical out variable
  if(c) *out = c->tick(in);
  
  // yes
  return TRUE;
}

// example implementation for getter
CK_DLL_MFUN(sigmund_getFreq)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->getFreq();
}

// example implementation for getter
CK_DLL_MFUN(sigmund_getPower)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->getPower();
}

// example implementation for getter
CK_DLL_MFUN(sigmund_getPeaks)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_object = bcdata->getPeaks();
}

// example implementation for getter
CK_DLL_MFUN(sigmund_getTracks)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_object = bcdata->getTracks();
}

CK_DLL_MFUN(sigmund_setNpts)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_int = bcdata->setNpts(GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(sigmund_setNpeak)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_int = bcdata->setNpeak(GET_NEXT_INT(ARGS));
}

CK_DLL_MFUN(sigmund_setMaxfreq)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setMaxfreq(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(sigmund_setMinpower)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setMinpower(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(sigmund_setParam1)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setParam1(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(sigmund_setParam2)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setParam2(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(sigmund_setParam3)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_float = bcdata->setParam3(GET_NEXT_FLOAT(ARGS));
}

CK_DLL_MFUN(sigmund_setStabletime)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  RETURN->v_dur = bcdata->setStabletime(GET_NEXT_DUR(ARGS));
}

CK_DLL_MFUN(sigmund_clear)
{
  // get our c++ class pointer
  Sigmund * bcdata = (Sigmund *) OBJ_MEMBER_INT(SELF, sigmund_data_offset);
  // set the return value
  bcdata->clear();
}

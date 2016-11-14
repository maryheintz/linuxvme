bool parity(unsigned long word);

// The sampleset class provited a buffer which stores event data and allows
// the user to retrieve it.  It could be optimized away in the future, the
// data just being left in the S-Link's buffer space to be retrieved from
// there, especially in normal mode.
// The Check()  function just checks the existing sampleset for errors and
// throws exceptions.  The parameter tells the sampleset its index within the
// vector of samplesets corresponding to an event, so if an error is found
// it can throw that information back out with the exception.

class sampleset {
public:
  sampleset() { }
  sampleset(const unsigned long *buffer, int numsamples);
  sampleset(const sampleset& inSet);
  sampleset& operator=(const sampleset& inSet); // Don't assign me
  ~sampleset();
  unsigned long header;
  unsigned long *data;
  unsigned long vertparity;
  void Check(int i);
private:
  int ns;
};

// Exceptions for S-Link errors:  An error in the S-Link library throws an
// exception which is just a wrapper for return code; the PrintErrorMessage()
// function prints the library's error message.

class SLINK_error {
public:
  virtual void PrintErrorMessage(ostream& ost) { }
};

class SLINK_Library_error {
public:
  SLINK_Library_error(int inCode);
  int code;
  virtual void PrintErrorMessage(ostream& ost);
};

class SLINK_Timeout_error {
public:
  virtual void PrintErrorMessage(ostream& ost);
};

// Exceptions for errors in the data.  Tells what type of error, which sample
// set, and which error.  Convention: sample = 0 means header word, sample =
// 1..n means data word, and sample -1 is vertical parity.

enum errtype {
  INDIC_ERROR, PARITY_ERROR, READ_ERROR, TEC_ERROR, SINGLE_ERROR, DOUBLE_ERROR
};

class Data_error {
public:
  Data_error(errtype inType, int setnum, int samplenum);
  errtype type;
  int set;
  int sample;
};

// Auxiliary parity-checking function

bool parity(unsigned long word) {
  bool parcheck = false;
  for (int shift = 0; shift < 32; ++shift) {
    if ((word >> shift) & 0x1) {
      parcheck = !parcheck;
    }
  }
  return parcheck;
}

// Sampleset functions: error checking happens here.

sampleset::sampleset(const unsigned long *buffer, int numsamples) :
  header(buffer[0]),
  data(new unsigned long[numsamples]),
  vertparity(buffer[numsamples+1]),
  ns(numsamples) {
  for (int i = 0; i < ns; ++i) {
    data[i] = buffer[i+1];
  }
}

sampleset::sampleset(const sampleset& inSet) :
  header(inSet.header),
  data(new unsigned long[inSet.ns]),
  vertparity(inSet.vertparity),
  ns(inSet.ns) {
  for (int i = 0; i < ns; ++i) {
    data[i] = inSet.data[i];
  }
}

sampleset& sampleset::operator=(const sampleset& inSet) {
  // Cleanup
  delete[] data;
  // Copy
  header = inSet.header;
  data = new unsigned long[inSet.ns];
  vertparity = inSet.vertparity;
  ns = inSet.ns;
  for (int i = 0; i < ns; ++i) {
    data[i] = inSet.data[i];
  }
}  

sampleset::~sampleset() {
  delete[] data;
}

void sampleset::Check(int set) {
  // Word to accumulate veertical parity
  unsigned long vertcheck = header;
  // Header word: check for error bits, as well as overall parity
  if (((header >> 31) & 0x1)  == 0) {
    throw Data_error(INDIC_ERROR, set, 0);
  }
  if ((header >> 29) & 0x1) {
    throw Data_error(PARITY_ERROR, set, -2);
  }
  if ((header >> 28) & 0x1) {
    throw Data_error(READ_ERROR, set, 0);
  }
  if ((header >> 25) & 0x7) {
    throw Data_error(TEC_ERROR, set, 0);
  }
  if ((header >> 24) & 0x1) {
    throw Data_error(DOUBLE_ERROR, set, 0);
  }
  if ((header >> 23) & 0x1) {
    throw Data_error(SINGLE_ERROR, set, 0);
  }
  if (parity(header)) {
    throw Data_error(PARITY_ERROR, set, 0);
  }
  // Data words: check for parity
  for (int i = 1; i <= ns; ++i) {
    if (parity(data[i-1])) {
      throw Data_error(PARITY_ERROR, set, i);
    }
    vertcheck ^= data[i-1];
  }
  // vertical parity check
  vertcheck ^= vertparity;
  if (vertcheck != 0xffffffff) {
    // throw Data_error(PARITY_ERROR, set, -1);
  }
}

// Exception constructors

Data_error::Data_error(errtype inType, int setnum, int samplenum) :
  type(inType),
  set(setnum),
  sample(samplenum) { }

SLINK_Library_error::SLINK_Library_error(int inCode) : code(inCode) { }

void SLINK_Library_error::PrintErrorMessage(ostream& ost) {
  ost << SLINK_GetErrorMessage(code) << endl;
}

void SLINK_Timeout_error::PrintErrorMessage(ostream& ost) {
  ost << "SLINK timed out waiting packet to be sent" << endl;
}

class up { }

void f() {
  // ...
  throw up();
}

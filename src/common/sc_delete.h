#ifndef __SCYTHE_DELETE_H__
#define __SCYTHE_DELETE_H__

#define SC_SAFE_DELETE(x) if (x) { delete x; x = nullptr; }
#define SC_SAFE_DELETE_ARRAY(x) if (x) { delete[] x; x = nullptr; }
#define SC_SAFE_RELEASE(x) if (x) { x->Release(); x = nullptr; }

#endif
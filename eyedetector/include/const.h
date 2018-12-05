#ifndef CONST_H_
#define CONST_H_
#define CONNECTPIPE(NAME, mode) \
    CreateFileW(NAME, mode, 0, NULL, OPEN_EXISTING, 0, NULL);

#define MATNAME L"\\\\.\\pipe\\frame"
#define MATSIZENAME L"\\\\.\\pipe\\framesize"
#define EYENAME L"\\\\.\\pipe\\eyes"
#define ROWNAME L"\\\\.\\pipe\\rows"
#define COLNAME L"\\\\.\\pipe\\cols"
#define TYPENAME L"\\\\.\\pipe\\type"
#endif
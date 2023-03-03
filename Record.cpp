#include "Record.h"

Record::Record(int tconst, int16_t avgrating, int numofvotes)
{
    SetRecord(tconst, avgrating, numofvotes);
}

void Record::SetRecord(int tconst,int16_t avgrating,int numofvotes)
{
    R_tconst = tconst;
    R_avgrating = avgrating;
    R_numofvotes = numofvotes;
}
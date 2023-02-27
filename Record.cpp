#include "Record.h"

Record::Record(int name, int16_t avgrating, int numofvotes)
{
    SetRecord(name, avgrating, numofvotes);
}

void Record::SetRecord(int name,int16_t avgrating,int numofvotes)
{
    R_name = name;
    R_avgrating = avgrating;
    R_numofvotes = numofvotes;
}
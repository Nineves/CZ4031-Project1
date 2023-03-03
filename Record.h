#ifndef DATE_H
#define DATE_H
#include <cstdint>
#include <string>
#pragma pack(1)
using std::string;
using std::__cxx11::to_string;
class Record {
  private:
    int R_tconst;
    int16_t R_avgrating;
    int R_numofvotes;
  public:
    Record(int R_tconst, int16_t R_avgrating, int R_numofvotes);
    void SetRecord(int tconst, int16_t avgrating, int numofvotes);
    int getName() { return R_tconst; }
    int16_t getAvgRating() { return R_avgrating; }
    int getNumOfVotes()  { return R_numofvotes; }
    string getTconst() { return "tt" + string(7 - to_string(R_tconst).length(), '0') + to_string(R_tconst);}
    double rating2Dec() {return (double)R_avgrating / 10;}
};

#endif

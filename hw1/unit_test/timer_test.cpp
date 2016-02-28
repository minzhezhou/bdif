#include "myutil.h"
#include "timer.h"
#include <iostream>
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;
using string = std::string;

ptime const time_t_epoch(date(2001, 1, 1));
long long boost_timer (string& s) {
    int pdel = 0;
    try{
    while (s[pdel]!=':') {
        pdel++;
    }
	if (pdel != 8 && s.length() < 19)
		return 0;
	string ds = s.substr(0,8);
	string ts = s.substr(9);
	if (ds.length() == 0 || ts.length() == 0) {
		return 0;
	}
	date d(from_undelimited_string(ds));
	ts = to_simple_string(d) + " " + ts;
	ptime t(time_from_string(ts));
	time_duration diff = t - time_t_epoch;
	return diff.total_microseconds();
    } catch (std::exception& e) {
        //MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        //std::cout << s << " at rank" << rank << " !!!!!!!!!!!!!\n";
        return 0;
    }
}
int main() {
    string ds1 = "20140813";
    string ds2 = "20140723";
    timer mt;
    while(1) {
        string ds;
        std::cin >> ds;
        long long delta = mt.get_time_stamp(ds) - boost_timer (ds);
        std::cout << "mt:" << mt.get_time_stamp(ds) << " bt:" <<boost_timer (ds) << " delta:" << delta << std::endl;
    }
    return 0;

}

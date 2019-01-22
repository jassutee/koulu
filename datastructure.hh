// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#ifndef DATASTRUCTURE_HH
#define DATASTRUCTURE_HH

#include <string>
#include <map>

const std::string NO_ROUTE = "Error: No route!";
const std::string NO_SUCH_STOP = "Error: No such stop!";

class Datastructure
{
public:
    Datastructure();

    ~Datastructure();

    // Add a new bus stop
    void add_stop(int stop_id, std::string stop_name);

    // Add a new bus route
    void add_route(int route_id, std::string route_name);

    // Add a new stop to a route (both the route and the stop have already been added
    // using add_route and add_stop)
    void add_stop_to_route(int route_id, int stop_id, unsigned int minutes);

    // Add a new bus driving a route (the route has already been added using add_route) leaving at a certain
    // time from the terminus (first stop on route)
    void add_bus(int bus_id, int route_id, unsigned int start_hours, unsigned int start_minutes);

    // Empty the datastructure
    void empty();

    // Print information on a bus stop
    void print_stop(int stop_id);

    // Print buses leaving from a stop
    void print_buses(int stop_id);

    // Print the number of stops, bus routes, and buses
    void print_statistics();

    // Print out the fastest journey leaving from stop start_stop to stop end_stop. Journey starts at the given time
    // and the whole journey must be done within the same day (before midnight).
    void print_fastest_journey(int start_stop, int end_stop, unsigned int hours, unsigned int minutes);

    // Copy constructor is forbidden
    Datastructure(const Datastructure&) = delete;
    // Assignment operator is forbidden
    Datastructure& operator=(const Datastructure&) = delete;

private:

    // luodaan tietue aika, jotta voidaan käsitellä helposti
    // luokassa aikoja
    struct aika {
        unsigned int tunnit;
        unsigned int minuutit;

        friend aika operator+(aika a, const aika& b) {
            a.tunnit += b.tunnit;
            a.minuutit += b.minuutit;
            if (a.minuutit >= 60) {
                a.tunnit++;
                a.minuutit -= 60;
            }
            return a;
        }
        friend aika operator-(aika a, const aika& b) {
            if (a.minuutit < b.minuutit) {
                a.tunnit--;
                a.minuutit += 60;
            }
            a.tunnit -= b.tunnit;
            a.minuutit -= b.minuutit;
            return a;
        }
        friend bool operator<(const aika& a, const aika& b) {
            if (a.tunnit == b.tunnit) {
                return a.minuutit < b.minuutit;
            } else {
                return a.tunnit < b.tunnit;
            }
        }
    };

    struct linja;
    struct bussi; // esittelyt tässä toimivuuden vuoksi

    // GRAAFIN SOLMUT
    struct pysakki {
        int id;
        std::string nimi;
        // map pitää linjat id:n mukaisessa järjestyksessä
        // map<linjan id, linjapointer>
        std::map<int, linja*> lahtevat;

        // reitinhakualgoritmia varten apumuuttujia
        // väriarvo voi olla 'v'=valkoinen, 'h'=harmaa tai 'm'=musta
        char vari;
        // etäisyys on tässä tapauksessa aika, jossa kyseiselle
        // pysäkille pääsee
        aika etaisyys;
        // edellinen osoittaa edelliseen pysäkkiin
        pysakki* edellinen;
        // tultiin taas kertoo linjan, jolla tälle pysäkille tultiin
        linja* tultiin;

        friend bool operator<(const pysakki &a, const pysakki &b) {
            return a.etaisyys > b.etaisyys;
        }
    };

    // GRAAFIN KAARET
    struct linja {
        int id;
        std::string nimi;
        // multimap siksi, että samalla minuutilla voisi olla
        // enemmänkin kuin yksi pysäkki. Minuutit ovat avaimia
        // siksi, että multimap pitäisi aikajärjestyksen.
        // multimap<minuutit, pysakkipointer>
        std::multimap<aika, pysakki*> pysakit;
        // nämä täytetään vain kysyttäessä
        std::string lahto;
        std::string kohde;
    };

    // tallennetaan graafin alkiot mappeihin id:n mukaan
    std::map<int, pysakki> bpysakit;
    std::map<int, linja> blinjat;

    // GRAAFISSA KULKEVAT BUSSIT
    struct bussi {
        int id;
        //int linja_id;
        aika blahtoaika;
    };

    // busseille on oma säiliö
    // multimap<linjan id, bussitietue>
    std::multimap<int, bussi> bussit;

    void relax(const pysakki*& mista, pysakki*& mihin, const aika& vali, const linja*& bussi);

};

#endif // DATASTRUCTURE_HH

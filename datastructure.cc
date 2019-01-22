// Definition of Datastructure class, hw3 of TIE-20100/TIE-20106

#include "datastructure.hh"

#include <string>
#include <map>
#include <iostream>
#include <queue> //priority_queue
#include <functional>
#include <algorithm> //min_element


Datastructure::Datastructure(): bpysakit{}, blinjat{}, bussit{}
{
}

Datastructure::~Datastructure()
{
    empty();
}

void Datastructure::add_stop(int stop_id, std::string stop_name)
{
    std::map<int, linja*> lahtevat_linjat{};
    aika etaisyys{24, 0};
    pysakki uusi{stop_id, stop_name, lahtevat_linjat, 'v', etaisyys, nullptr, nullptr};
    bpysakit[stop_id] = uusi;
}

void Datastructure::add_route(int route_id, std::string route_name)
{
    std::multimap<unsigned int, pysakki*> pysakit{};
    linja uusi{route_id, route_name, pysakit, "", ""};
    blinjat[route_id] = uusi;
}

void Datastructure::add_stop_to_route(int route_id, int stop_id, unsigned int minutes)
{
    aika minuutit{0,minutes};
    if (minuutit.minuutit >= 60) {
        int n = minuutit.minuutit / 60;
        minuutit.tunnit = n;
        minuutit.minuutit = minuutit.minuutit % 60;
    }
    // sijoitetaan uusi pysäkkitieto linjalle
    blinjat[route_id].pysakit.insert(std::pair<aika, pysakki*>(minuutit,&bpysakit[stop_id]));
    // ja linjatieto pysäkille
    bpysakit[stop_id].lahtevat[route_id] = &blinjat[route_id];
}

void Datastructure::add_bus(int bus_id, int route_id, unsigned int start_hours, unsigned int start_minutes)
{
    aika lahto{start_hours, start_minutes};
    bussi uusi{bus_id, lahto};
    bussit.insert(std::pair<int, bussi>(route_id, uusi));
}

void Datastructure::empty()
{
    bpysakit.clear();
    blinjat.clear();
    bussit.clear();
}

void Datastructure::print_stop(int stop_id)
{
    if (bpysakit.find(stop_id) == bpysakit.end()) {
        std::cout << NO_SUCH_STOP << std::endl;
    } else {
        std::cout << bpysakit[stop_id].nimi << std::endl;
    }
}

void Datastructure::print_buses(int stop_id)
{
    if (bpysakit.find(stop_id) == bpysakit.end()) {
        std::cout << NO_SUCH_STOP << std::endl;
    } else {
        for (auto linja: bpysakit[stop_id].lahtevat) {
            if (linja.second->lahto == "") {
                // haetaan lähtö- ja kohdepysäkkien tiedot iteraattorin avulla
                // ensin sijoitetaan iteraattori lähtöpysäkin kohdalle
                std::multimap<unsigned int, pysakki*>::iterator i{linja.second -> pysakit.begin()};
                linja.second -> lahto = (*i).second -> nimi;
                // siirretään iteraattori viimeisen pysäkin kohdalle
                i = linja.second->pysakit.end();
                --i;
                linja.second -> kohde = (*i).second -> nimi;
            }
            std::cout << linja.first << ": " << linja.second->lahto << " - ";
            std::cout << linja.second->kohde << std::endl;
        }
    }
}

void Datastructure::print_statistics()
{
    std::cout << bpysakit.size() << " stops, " <<
                 blinjat.size() << " routes, " <<
                 bussit.size() << " buses." << std::endl;
}

void Datastructure::print_fastest_journey(int start_stop, int end_stop, unsigned int hours, unsigned int minutes)
{
    if (bpysakit.find(start_stop) == bpysakit.end() or
            bpysakit.find(end_stop) == bpysakit.end()) {
        std::cout << NO_SUCH_STOP << std::endl;
        return;
    }
    // "siivotaan" edellisen haun jäljiltä
    for (auto pysakki: bpysakit) {
        if (pysakki.second.vari != 'v') {
            pysakki.second.vari = 'v';
            // "ääretön" etäisyys on hyvä asettaa 24 tunniksi, sillä matka tulee muutenkin
            // kulkea saman vuorokauden sisällä
            pysakki.second.etaisyys.tunnit = 24;
            pysakki.second.etaisyys.minuutit = 0;
            pysakki.second.edellinen = nullptr;
            pysakki.second.tultiin = nullptr;
        }
    }
    // luodaan prioriteettijono, josta valitaan aina ensimmäisenä nopeimpana oleva pysäkki
    std::priority_queue<pysakki*> pjono;
    // tallennetaan lähtöaika samaan muotoon kuin tietokannan ajat ovat
    aika lahtoaika{hours, minutes};

    // aloitetaan lähtöpysäkistä
    pysakki* nykyinen{&bpysakit[start_stop]};
    nykyinen->vari = 'h';
    nykyinen->etaisyys.tunnit = 0;
    nykyinen->etaisyys.minuutit = 0;
    pjono.push(nykyinen);

    while (!pjono.empty()) {
        nykyinen = pjono.top();
        pjono.pop();
        // tarkistetaan, ettei tehdä tarkasteluja monesti samalle pysäkille
        if (nykyinen->vari == 'm') continue;
        // tarkistetaan myös, löytyikö päätepysäkki
        if (nykyinen->id == end_stop) break;

        for (auto linja: nykyinen->lahtevat) {
            // iteroidaan nykyisen pysäkin kohdalle minuuttimapissa
            std::multimap<aika, pysakki*>::iterator p_i{linja.second->pysakit.begin()};
            while (p_i->second->id != nykyinen->id) {
                p_i++;
            }
            // määritellään mahdollinen odotusaika
            aika odotusaika{0, 0};
        //----------------vJOS EDELLINEN LINJA ON ERI KUIN SEURAAVA-------------
            if (linja->second != nykyinen->tultiin) {
                // haetaan linjan kuluttama aika päästä kyseiselle pysäkille lähtöpysäkiltä
                aika minuutit{p_i->first};

                // haetaan seuraava pysäkille tuleva linjan bussi
                std::vector<aika> aikaerot;
                std::multimap<int, bussi>::iterator b_i{bussit.find(linja.first)};
                // kirjataan ylös kaikki annetun ajan jälkeen tulevien bussien odotusajat
                while (b_i->first == linja.first) {
                    if (b_i->second.blahtoaika + minuutit >= lahtoaika) {
                        aikaerot.push_back(lahtoaika-(b_i->second.blahtoaika + minuutit));
                    }
                    b_i++;
                }
                // => saadaan selville tämän linjan odotusaika pysäkillä
                odotusaika = *std::min_element(aikaerot.begin(),aikaerot.end());
            }
        //----------------^JOS EDELLINEN LINJA ON ERI KUIN SEURAAVA-------------

            // haetaan linjan seuraava pysäkki ja merkataan se jonoon
            pysakki* seuraava{(++p_i)->second};
            if (seuraava->vari == 'v') {
                seuraava->vari = 'h';
                pjono.push(seuraava);
            }
            // tarkistetaan, tarvitseeko reittiä pysäkille muuttaa
            relax(nykyinen, seuraava, p_i->first+odotusaika, linja->second);
        }
        // merkitään nykyinen solmu läpikäydyksi
        nykyinen->vari = 'm';
    }
    if (nykyinen->id != end_stop) {
        std::cout << NO_ROUTE << std::endl;
        return;
    }
}

// Täällä lasketaan, että pitääkö reittiä mihin-pysäkille muuttaa
void Datastructure::relax(const pysakki *&mista, pysakki *&mihin, const aika &vali, const linja*& bussi)
{
    if (mihin->etaisyys > mista->etaisyys + vali) {
        mihin->etaisyys = mista->etaisyys + vali;
        mihin->edellinen = mista;
        mihin->tultiin = bussi;
    }
}

#include "datastructure.hh"

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

// Empty implementations for public interface, modify and add your own code here

//Alustetaan hajautustaulu 3000 kokoiseksi, sillä oletetaan täytön
//olevan noin 1000, eli 500-1500. Näin pidetään täyttöaste alle puolen.
Datastructure::Datastructure(): hajautustaulu(3000, nullptr),
    taytto{0}, maara{0}, kokomuutokset{0}
{

}

Datastructure::~Datastructure()
{

}

void Datastructure::add(const string &ID, const string &location,
                        unsigned int amount, const string &name)
{
    // Lasketaan karkille indeksi hajautusfunktiolla
    size_t avain{std::hash<std::string>()(ID) % hajautustaulu.size()};

    // Luodaan valmiiksi apumuuttujia
    karkki *x = hajautustaulu[avain];
    // Tätä tarvitaan, jos karkkia ei vielä ole varastossa
    karkki *uusi{new karkki{ID, amount, location, name, nullptr}};
    // Onko indeksi jo varattu?
    if (x != nullptr) {
        // Käydään läpi kaikki indeksissä olevat karkit
        // Luodaan apumuuttuja, joka kertoo, oliko karkki indeksissä
        bool loytynyt{false};
        if (x->id == ID) loytynyt = true;
        while (!loytynyt && x->seuraava != nullptr) {
            x = x->seuraava;
            if (x->id == ID) {
                loytynyt = true;
            }
        }
        if (loytynyt) {
            x->lkm += amount;
        }
        // Jos karkkia ei löytynyt, x osoittaa viimeiseen karkkiin.
        // Lisätään sen perään uusi karkki.
        else {
            x->seuraava = uusi;
            maara += 1;
        }
    }
    // Indeksiä ei oltu varattu, lisätään siis uusi karkki
    else {
        hajautustaulu[avain] = uusi;
        maara += 1;
        taytto += 1;
        //Täytyy tarkistaa, ettei täyttöaste kasva liian suureksi
        if (taytto/hajautustaulu.size() > MAX_TAYTTOASTE) {
            hajautustaulu.resize(hajautustaulu.size()*2, nullptr);
            kokomuutokset += 1;
        }
    }
}

void Datastructure::substract(const string &ID, unsigned int amount)
{
    size_t avain{std::hash<std::string>()(ID) % hajautustaulu.size()};
    karkki *x = hajautustaulu[avain];
    // Haetaan x:ää vastaava karkki, jossei löydy, x on nullpointer
    etsi(x, ID);
    if (x != nullptr) {
        if (x->lkm >= amount) {
            x->lkm -= amount;
            // Tarkistetaan, vähennettiinkö kaikki karkit
            if (x->lkm == 0) {
                // Siinä tapauksessa poistetaan karkki kokonaan tietokannasta
                poista(avain, x);
            }
        }
        else {
            std::cout << NOT_ENOUGH_CANDY << std::endl;
        }
        std::cout << "Amount: " << x->lkm << " ";
        std::cout << "Shelf: " << x->hylly << std::endl;
    }
    else {
        std::cout << NOT_AVAILABLE << std::endl;
    }
}

void Datastructure::find(const string &ID) const
{
    size_t avain{std::hash<std::string>()(ID) % hajautustaulu.size()};
    karkki *x = hajautustaulu[avain];
    etsi(x, ID);
    if (x != nullptr) {
        std::cout << x -> nimi << " ";
        std::cout << x -> lkm << " ";
        std::cout << x -> hylly << std::endl;
    }
    else {
        std::cout << NOT_AVAILABLE << std::endl;
    }
}

size_t Datastructure::count() const
{
    // Koko ajan ollaan pidetty kirjaa eri karkkien määrästä, joten tämä on helppo
    return maara;
}

void Datastructure::empty()
{
    // Käydään kaikki taulun alkiot läpi
    for (size_t i=0; i< hajautustaulu.size(); i++) {
        // Jos indeksissä on jotakin, kutsutaan poista-funktiota kunnes indeksi on tyhjä
        if (hajautustaulu[i] != nullptr) {
            while (hajautustaulu[i] != nullptr) {
                karkki *x = hajautustaulu[i];
                poista(i, x);
            }
        }
    }
}

void Datastructure::poista(const size_t& i, karkki* poistettava)
{
    // Jos poistettava on indeksin ensimmäinen karkki
    if (poistettava == hajautustaulu[i]) {
        hajautustaulu[i] = poistettava->seuraava;
        // Tarkistetaan, jäikö indeksi tyhjäksi
        if (hajautustaulu[i] == nullptr) {
            taytto -= 1;
        }
    }
    else {
        // Haetaan indeksistä poistettavaa edeltävä alkio, jotta osoittimet saadaan korjattua
        karkki *edellinen = hajautustaulu[i];
        while (edellinen->seuraava != poistettava) {
            edellinen = edellinen->seuraava;
        }
        edellinen->seuraava = poistettava->seuraava;
    }
    // Vapautetaan muisti
    delete poistettava;
    maara -= 1;
}

void Datastructure::etsi(karkki* &x, const std::string& ID) const
{
// Mielestäni toimiva koodinpätkä, mutta 30000 alkion testiajo ei aikatestauksessa
// jostain syystä tänne suostu edes menemään (joten jätin kommentiksi):
/*    //Jos taulua on tarvinnut suurentaa, täytyy avain laskea myös
    //edellisillä koilla ja etsiä niistä indekseistä
    for (int i = 0; i <= kokomuutokset; i++) {
        if (i>0) {
            // koska ei ole valmista potenssifunktiota...
            int p{2};
            while (i>1) {
                p *= 2;
                i--;
            }
            x = hajautustaulu[std::hash<std::string>()(ID) % (hajautustaulu.size()/p)];
        }*/
        // Etsitään indeksistä saman id:n alkio ja palautetaan se viiteosoittimena
        while (x != nullptr) {
            if (x->id == ID) {
                return;
            }
            x = x->seuraava;
        }
   // }
}

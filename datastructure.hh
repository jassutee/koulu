// Definition of Datastructure class for UDS/Tiraka homework 2 -


#ifndef DATASTRUCTURE_HH
#define DATASTRUCTURE_HH

#include <string>
using std::string;
#include <vector>

const std::string NOT_AVAILABLE = "Product not in warehouse.";
const std::string NOT_ENOUGH_CANDY = "Not enough candy available.";
// Määritellään maksimi-täyttöaste hajautustaululle
const float MAX_TAYTTOASTE{0.5};

class Datastructure
{
    public:

    Datastructure();

    ~Datastructure();

    // Adds candies to the datastructure (command "A")
    void add(const std::string& ID, const std::string& location,
            unsigned int amount, const std::string& name);
          
    // Removes candies from the datastructure (command "D")
    void substract(const std::string& ID, unsigned int amount);

    // Finds candy and prints its status (command "F")
    void find(const std::string& ID) const;

    // Returns how many different types of candy the datastructure contains (command "C")
    size_t count() const;

    // Empties the datastructure (command "E")
    void empty();
    
    // Copy constructor is forbidden (not important for this assignment)
    Datastructure(const Datastructure&) = delete;
    // Assignment operator is forbidden (not important for this assignment)
    Datastructure& operator=(const Datastructure&) = delete;
    
private:
    // tallennetaan muuttujaan taulun täyttö, auttaa täyttöasteen laskemisessa
    size_t taytto;
    // pidetään kirjaa myös erilaisten karkkien lukumäärästä
    int maara;
    // tämä kasvaa, kun taulun koko kaksinkertaistetaan
    int kokomuutokset;
  
    struct karkki {
        std::string id;
        unsigned int lkm;
        std::string hylly;
        std::string nimi;
        karkki *seuraava;
    };

    std::vector<karkki*> hajautustaulu;

    // poistofunktio empty()- ja substract()-metodeja varten
    void poista(const size_t& i, karkki* poistettava);

    // hakufunktio, siirtää x:n osoittamaan karkkiin, jonka id on kysytty.
    // jos ei löyty, x muuttuu nullptr:ksi
    void etsi(karkki* &x, const std::string& ID) const;
 
};

#endif

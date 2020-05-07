#include <bits/stdc++.h>

using namespace std;

ifstream f ("input.txt");
ofstream g ("output.txt");

const int Dim = 1e4 + 5;

const int Base1 = 67;
const int Base2 = 71;
const int Mod1 = 666013;
const int Mod2 = 666019;

map <int, int> c;
vector <int> part[Dim];

class DFA {
    set <int> Q, F;
    set <char> sigma;
    int q0;
    map <pair <int, char>, int> delta;

public:
    DFA () { q0 = 0; }
    DFA (set <int> Q, set <char> sigma, map <pair <int, char>, int> delta, int q0, set <int> F) {
        this -> Q = Q;
        this -> sigma = sigma;
        this -> delta = delta;
        this -> q0 = q0;
        this -> F = F;
    }

    set <int> getQ() { return Q; }
    set <int> getF() { return F; }
    set <char> getSigma() { return sigma; }
    map <pair <int, char>, int> getDelta() { return delta; }
    int getQ0() { return q0; }
    friend istream& operator >> (istream&, DFA&);
};

istream& operator >> (istream &f, DFA &M) {
    /// citesc AFD - ul
    /// Citesc numarul de stari, apoi starile

    int n; f >> n;
    for (int i = 1; i <= n; ++ i) {
        int x; f >> x;
        M.Q.insert(x);
    }

    /// Citesc numarul de litere din alfabet, dupa care alfabetul
    f >> n;
    for (int i = 1; i <= n; ++ i) {
        char lit; f >> lit;
        M.sigma.insert (lit);
    }

    /// Citesc numarul de tranzitii, dupa care tranzitiile
    f >> n;
    for (int i = 1; i <= n; ++ i) {
        int s, d;
        char lit;

        f >> s >> lit >> d;
        M.delta[{s, lit}] = d;
    }

    /// Citestc starea initiala
    f >> n;
    M.q0 = n;

    /// Citesc starile finale
    f >> n;
    for (int i = 1; i <= n; ++ i) {
        int x; f >> x;
        M.F.insert (x);
    }

    return f;
}

pair <int, int> makeHash (int q, DFA &M){
    int h1 = 0, h2 = 0;
    for (auto lit: M.getSigma()) {
        h1 = (h1 * Base1 + c[M.getDelta()[{q, lit}]]) % Mod1;
        h2 = (h2 * Base2 + c[M.getDelta()[{q, lit}]]) % Mod2;
    }

    return make_pair(h1, h2);
}

int main() {
    DFA M;
    f >> M;
    f.close();

    /// Partitionez starile in doua: cele finale si cele nefinale
    /// In c voi retine pentru fiecare stare din ce partitie face parte
    /// In part voi retine pentru o partitie, starile care fac parte din ea
    set <int> F = M.getF();
    for (auto it: M.getQ()) {
        if (F.find(it) != F.end()) {
            c[it] = 2;
            part[2].push_back (it);
        }

        else {
            c[it] = 1;
            part[1].push_back (it);
        }
    }

    int nrPart = 2;

    bool ok;
    do {
        ok = true;

        int nrPartNou = 0;
        map <int, int> newC;
        vector <int> newPart[Dim];

        /// Pentru fiecare partitie, vad daca exista stari separabile
        for (int i = 1; i <= nrPart; ++ i) {
            map <pair <int, int>, int> Hash;

            for (int j = 0; j < (int)part[i].size(); ++ j) {
                /// Pentru fiecare stare dintr-o partitie si fiecare literea din alfabet, ma uit la partitia din care face parte delta[stare, litera]
                /// Folosesc hash - uri, ca atunci cand ma uit daca exista vreo alta stare din aceeasi partitie care sa duca toate literele in aceleasi partitii, sa nu parcurg toate starile
                /// Calculez hash - urile modulo 2 valori, pentru a nu exista coliziuni

                pair <int, int> hs = makeHash (part[i][j], M);

                /// Daca am mai intalnit valoarea hs (partitiile din care fac parte delta[stare, litere], pentru fiecare litera), atunci starea curenta va face parte din aceeasi partitie cu starea care avea acelasi hash
                if (Hash.find(hs) != Hash.end()) {
                    newC[part[i][j]] = newC[part[i][Hash[hs]]];
                    newPart[newC[part[i][j]]].push_back (part[i][j]);
                }

                /// Altfel, starea curenta va face parte dintr-o noua partitie
                else {
                    if (j) ok = false;
                    Hash[hs] = j;
                    newC[part[i][j]] = ++nrPartNou;
                    newPart[nrPartNou].push_back (part[i][j]);
                }
            }
        }

        /// Actualizez part si c
        for (int i = 1; i <= nrPart; ++ i) {
            part[i].clear();
        }

        for (int i = 1; i <= nrPartNou; ++ i) {
            for (auto it: newPart[i]) {
                part[i].push_back (it);
            }
        }

        c.clear();
        for (auto it: M.getQ()) {
            c[it] = newC[it];
        }

        nrPart = nrPartNou;

    } while (!ok);

    /// Afisez AFD - ul obtinut
    g << nrPart << '\n';
    for (int i = 1; i <= nrPart; ++ i) {
        g << i << " ";
    }

    g << '\n' << M.getSigma().size() << '\n';
    for (auto lit: M.getSigma()) {
        g << lit << " ";
    }
    g << '\n' << M.getSigma().size() * nrPart << '\n';
    for (int i = 1; i <= nrPart; ++ i) {
        for (auto lit: M.getSigma()) {
            g << i << " " << lit << " " << c[M.getDelta()[{part[i][0], lit}]] << '\n';
        }
    }


    int q0 = 0;
    for (int i = 1; i <= nrPart; ++ i) {
        for (auto it: part[i]) {
            if (it == M.getQ0()) {
                q0 = i;
                break;
            }
        }
    }

    g << q0 << '\n';

    int nrF = 0;
    for (int i = 1; i <= nrPart; ++ i) {
        for (auto it: part[i]) {
            if (F.find(it) != F.end()) {
                ++ nrF;
                break;
            }
        }
    }

    g << nrF << '\n';
    for (int i = 1; i <= nrPart; ++ i) {
        for (auto it: part[i]) {
            if (F.find(it) != F.end()) {
                g << i << " ";
                break;
            }
        }
    }

    return 0;
}

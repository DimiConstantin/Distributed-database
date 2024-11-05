**Nume:**
Constantin Dimitrie
**Grupă:**
315CA

## Titlu + numar temă
Tema 2 - Distributed Database
### Descriere:
    Tema are scopul de a intelege conceptul de LRU Caching si cum se distribuie documentele pe un server. LRU Cache , spre deosebire de cache-ul normal, prioritizeaza elementele cel mai recent accesate, functionand pe ideea ca acestea vor fi accesate iarasi in viitorul apropiat. Acest tip de cache este alcatuit dintr o lista dublu inlantuita, care reprezinta ordinea elementelor, si un hashtable, care retine pointeri la nodurile din lista. Hashtable-ul imbunatateste semnificativ eficienta programului, elementele din lista fiind accesate in O(log n), fata de O(n), in cazul in care s- ar fi iterat prin lista pana la gasirea documentului dorit.

    Cand vrem sa adaugam un document in cache, intai verificam daca este plin. In cazul acesta, se elimina cel mai vechi accesat document (head-ul listei) si se adauga la final documentul dorit. In caz contrar, se verifica daca exista deja documentul cerut. Daca exista, se actualizeaza continutul si se muta la final. Daca nu exista, introducem documentul in cache.

### Disclaimer
    Mutarea nodurilor in lista se face manual, tinandu se cont de edge case-uri

    Serverul contine, in mod evident, un LRU Cache, un hashtable care are functionalitate de database (retine documente) si o coada in care se retin requesturile de tip EDIT. Cand requestul este de tip GET, intai se executa toate EDIT-urile , iar apoi GET.

    La comanda de tip EDIT, intai se verifica daca exista documentul in cache. Daca exista, se actualizeaza continutul in cache. Daca nu, se verifica daca exista in database-ul serverului. In acest caz, se adauga documentul in cache si se actualizeaza continutul. Altfel, documentul nu exista nicaieri, deci este creat si adaugat in database si cache. Daca este cache-ul plin, se elimina o intrare din cache si se muta in database.

    La comanda de tip GET, daca exista documentul in cache, se returneaza continutul lui. Daca nu exista nici in database, se returneaza NULL, iar altfel se returneaza continutul si se adauga documentul in cache.

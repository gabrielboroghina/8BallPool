Gabriel Boroghina, 333CB

                              8 Ball Pool Game

    Tacul, suprafata mesei si buzunarele au fost construite manual folosind 
vectori de puncte si indici. Pe masa, tac, cat si pe podea sunt aplicate 
texturi prin intermediul fragment shaderului.
    De asemenea, s-a aplicat un model de iluminare de tip Phong in fragment 
shader peste fiecare tip de obiect (colorat sau texturat).
------------------------------------------------------------------------------------

    Miscarea bilelor este simulata prin intermediul unei pozitii, a unei viteze
si a unei acceleratii negative determinate de forta de frecare. Viteza va scadea
intr-un timp t cu a*t.

    Tratarea coliziunilor bila-bila, bila-manta si bila-buzunar este facuta
pe baza ecuatiilor de miscare ale bilelor (pos_f = pos_i + v * t + a * t ^ 2 / 2).
Astfel, pentru fiecare interval de timp deltaTime (dintre 2 Update()-uri succesive),
se rezolva, in ordine cronologica, toate coliziunile care pot avea loc in acest 
interval. Acest lucru se realizeaza astfel: gasim prima coliziune (de orice tip)
din intervalul [0, deltaTime] folosind ecuatiile de miscare si tratam coliziunea.
Apoi, aducem toate bilele la momentul petrecerii coliziunii (le actualizam pozitiile
si vitezele), actualizam intervalul ramas de procesat la [0, deltaTime - moment_coliziune],
si repetam cautarea unei noi coliziuni, pana cand nu mai exista nicio coliziune 
de procesat in intervalul ramas.
------------------------------------------------------------------------------------

    La fiecare introducere a unei bile se afiseaza la stdout statisticile de joc.
Foul-urile sunt afisate cand se trece din modul de vizualizare top-down in modul
third-person (la apasarea tastei SPACE).
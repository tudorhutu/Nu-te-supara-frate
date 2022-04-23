proiect retele Hutu Tudor E4
tehnologii utilizate:
    -postgresql relational database
    -biblioteca pqxx
    
instructiuni de utilizare:
    -inainte de a rula orice altceva, rulati create_database_user_and_password.sh
    -rulati serverul cu comanda make run-server
    -inainte de a va conecta cu un client, alegeti daca doriti sa incarcati ultima sesiune salvata in baza de date
    -conectati clientii(minim 2, maxim 4 daca nu a fost facut un load la pasul antecedent)cu comanda make run-client
    -in client, alegeti fie pionul pe care doriti sa il mutati(1-4) fie optiunea de save and quit(5), fie optiunea de quit(6)
    -la momentul terminarii jocului, serverul se va inchide, si va afisa leaderboard-ul sub forma unui vector de lungime 4, fiecarui element ii corespunde un jucator, numerele afisate fiind ordinea terminarii

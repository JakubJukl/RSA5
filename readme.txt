Co to je RSA a k čemu slouží?

RSA je šifra s veřejným a soukromým klíčem. Text převedený do číselné formy se zašifruje 
pomocí veřejného klíče příjemce zprávy, který si ji posléze dešifruje svým soukromým klíčem.

--------------------------------------------------------------------------------------------
Krátké slovo úvodem k programu

Program vyžaduje knihovnu GMP (Multi‑Precision Arithmetic Library) pro výpočet, jelikož
jsem nepřišel na jednoduchý a efektivní způsob, jak zajistit výpočet např. 123^17 % 3233,
bez použití externí knihovny. (https://gmplib.org/)

Program je myšlen spíše jako funkční prototyp, než produkt, který by se dal reálně využívat
pro kryptografické účely. Slabinou mé implementace je, že šifruji samostatná písmena veřejným
klíčem, takže kdokoliv, kdo zná veřejný klíč, si může zašifrovat každé písmeno abecedy a dle
toho šifrovaný text prolomit. Mluvíme o sémantické bezpečnosti, kterou samostatná šifra RSA 
neobsahuje. (https://crypto.stackexchange.com/questions/1448/definition-of-textbook-rsa)

Uživatel by měl věnovat pozornost vygenerovaným klíčům. Někdy se stane, že hodnota 'd'
soukromého klíče je shodná s hodnotou 'e' veřejného klíče. V tom případě by si měl vygenerovat
jiný klíč. Platí, že čím vyšší jsou hodnoty 'e' a 'd', tím těžší je šifru prolomit
(nebereme-li v potaz výše zmíněnou sémantickou bezpečnost).

----------------------------------------------------------------------------------------------
Návod k použití

Do textového souboru "vstup.txt", který musí být uložen ve stejné složce, jako program, vložte
text k zašifrování nebo dešifrování. Čísla pište slovy a nepoužívejte diakritiku. Po spuštění
programu si zvolte, zda chcete šifrovat nebo dešifrovat. Při šifrování můžete zadat dříve
vygenerovaný veřejný klíč a nebo si ho nechat vygenerovat spolu se soukromým. Při dešifrování
zadejte soukromý klíč. Výstup je buď v souboru "desifrovane.txt" nebo "zasifrovane.txt", 
ve stejné složce jako je program.
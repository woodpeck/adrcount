adrcount
========

Counts who has created how many addresses in OSM.

Processes full history dump (initially) and then diffs. Keeps 
two databases - one with counts per user, and one with an "has
address" flag per object, so that users who modify an object that
already has an address don't get extra points. 

Includes a query program for the resulting database. Querying 
is fast enough to be run from a CGI, or whatever web platform you prefer.

Requires protobuf, osmium, Google sparsehash.

Rest of this README is currently in German.

-------

Also, anbei mal mein aktueller Stand. Es hat sich rausgestellt, dass das Auslesen aus dem User-File so schnell ist, dass man das problemlos direkt interaktiv auf einem Webserver laufen lassen kann, also auch ohne Umweg ueber eine Datenbank.

Derzeit sind es vier einzelne Programme, koennte man aber problemlos in eins zusammenfassen.

Stichwortartige Doku:

Erzeugen von Daten bzw. Update:

./adr <full-history-file oder osc-file> <directory>

Man startet einmal mit einem full history planet und haut dann taeglich die Diffs drauf, oder sogar stuendlich, oder in beliebigen anderen Zeitintervallen. Wenn sich da mal was ueberlappt, ist es kein Problem, die Zaehler kommen dadurch nicht durcheinander. Im angegebenen "directory" (kann "." sein) wird ein objects.dat und ein users.dat angelegt (und auch von dort eingelesen, wenn vorhanden). Das objects.dat hat ca. 150 MB, das users.dat hat ca. 1 MB. Im objects.dat sind die IDs aller Objekte mit housenumber-Tag, im users.dat die Counter pro User.

(Kleines Problem, das mir noch nicht ganz klar ist - das Programm schluckt probelmos .osm.pbf, .osm.bz2, oder unkomprimierte .osc, aber irgendwie gehen die .osc.gz nicht, da muss man vermutlich dem Osmosis extra sagen, was es fuer ein Filetype ist - ich dekomprimiere einfach immer die .osc.gz vorher, dann tuts.)

Zum Testen einfach mit einem beliebigen .osc oder .osm initialisieren; spaeter dann mit full history file.

Das Programm ueberschreibt immer die objects.dat und users.dat, eventuell sollte man da noch einen Schutz einbauen, damit bei Platte voll oder so nicht alles kaputt geht (also schreiben in separate Datei, danach ummoven oder so).

Die Idee ist, dass man sich die users.dat dann regelmaessig in ein archiv-directory kopiert (mit timestamp im Dateinamen).

Auslesen der Zaehlerstaende:

./adrdump users.dat

gibt eine einfache Liste mit userid-leerzeichen-counter auf stdout aus. Kann man natuerlich auch beliebig anders formatieren.

./adrdiff users1.dat users2.dat

macht das gleiche, aber Differenzen zwischen zwei Datenstaenden - so kann man leicht den Stand von jetzt mit dem archivierten Stand von frueher vergleichen. Mit einem geeigneten "|sort" muesste man eigentlich sofort die "top 100 im laufenden monat" usw. generieren koennen.

Auslesen eines einzelnen Users

./adrquery users.dat userid

gibt auf stdout nur die eine Zahl fuer diesen User aus. Da adrdiff, adrquery, adrdump jeweils nur die 1 MB grosse users.dat einlesen muessen, sind sie schoen flott. Das "adr"-Tool ist etwas langsamer, weil es mit der kompletten objects.dat hantieren muss.

Das Konzept "immer alles in Speicher lesen, dann auf Speicher arbeiten, dann alles rausschreiben" ist sicher arg simpel, und man koennte das noch verschiedentlich optimieren, aber ich denke, fuer diesen Zweck reichts. 

